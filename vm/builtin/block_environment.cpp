/* A BlockEnvironment is created when a block is created. Its primary
 * operation is call, which activates the code associated with the block. */

#include "config.h"

#include "builtin/object.hpp"
#include "builtin/symbol.hpp"

#include "builtin/block_environment.hpp"
#include "objectmemory.hpp"

#include "vm/object_utils.hpp"

#include "arguments.hpp"
#include "dispatch.hpp"
#include "call_frame.hpp"
#include "builtin/class.hpp"
#include "builtin/compiledmethod.hpp"
#include "builtin/fixnum.hpp"
#include "builtin/tuple.hpp"
#include "builtin/system.hpp"
#include "builtin/staticscope.hpp"
#include "builtin/location.hpp"
#include "builtin/nativemethod.hpp"

#include "instruments/profiler.hpp"
#include "configuration.hpp"
#include "on_stack.hpp"

#ifdef RBX_WINDOWS
#include <malloc.h>
#endif

#ifdef ENABLE_LLVM
#include "llvm/jit.hpp"
#endif

#include <iostream>

namespace rubinius {

  void BlockEnvironment::init(STATE) {
    GO(blokenv).set(state->new_class("BlockEnvironment", G(object),
                                     G(rubinius)));
    G(blokenv)->set_object_type(state, BlockEnvironmentType);
    G(blokenv)->name(state, state->symbol("Rubinius::BlockEnvironment"));
  }

  BlockEnvironment* BlockEnvironment::allocate(STATE) {
    BlockEnvironment* env = state->new_object<BlockEnvironment>(G(blokenv));
    return env;
  }

  VMMethod* BlockEnvironment::vmmethod(STATE) {
    return this->method_->internalize(state);
  }

  Object* BlockEnvironment::invoke(STATE, CallFrame* previous,
                            BlockEnvironment* const env, Arguments& args,
                            BlockInvocation& invocation)
  {

    VMMethod* vmm = env->vmmethod(state);
    if(!vmm) {
      Exception::internal_error(state, previous, "invalid bytecode method");
      return 0;
    }

#ifdef ENABLE_LLVM
    if(void* ptr = vmm->native_function()) {
      return (*((BlockExecutor)ptr))(state, previous, env, args, invocation);
    }
#endif

    return execute_interpreter(state, previous, env, args, invocation);
  }

  // Installed by default in BlockEnvironment::execute, it runs the bytecodes
  // for the block in the interpreter.
  //
  // Future code will detect hot blocks and queue them in the JIT, whereby the
  // JIT will install a newly minted machine function into ::execute.
  Object* BlockEnvironment::execute_interpreter(STATE, CallFrame* previous,
                            BlockEnvironment* const env, Arguments& args,
                            BlockInvocation& invocation)
  {
    VMMethod* const vmm = env->vmmethod(state);

    if(!vmm) {
      Exception::internal_error(state, previous, "invalid bytecode method");
      return 0;
    }

#ifdef ENABLE_LLVM
    if(vmm->call_count >= 0) {
      if(vmm->call_count >= state->shared.config.jit_call_til_compile) {
        LLVMState* ls = LLVMState::get(state);

        ls->compile_soon(state, env->method(), env);

      } else {
        vmm->call_count++;
      }
    }
#endif

    size_t scope_size = sizeof(StackVariables) +
                         (vmm->number_of_locals * sizeof(Object*));

    StackVariables* scope =
      reinterpret_cast<StackVariables*>(alloca(scope_size));

    Module* mod = invocation.module;
    if(!mod) mod = env->module();
    scope->initialize(invocation.self, env->top_scope_->block(),
                      mod, vmm->number_of_locals);
    scope->set_parent(env->scope_);

    InterpreterCallFrame* frame = ALLOCA_CALLFRAME(vmm->stack_size);
    frame->prepare(vmm->stack_size);

    frame->previous = previous;
    frame->static_scope_ = invocation.static_scope;

    frame->arguments = &args;
    frame->dispatch_data = reinterpret_cast<BlockEnvironment*>(env);
    frame->cm =       env->method_;
    frame->scope =    scope;
    frame->top_scope_ = env->top_scope_;
    frame->flags =    invocation.flags | CallFrame::cCustomStaticScope
                                       | CallFrame::cMultipleScopes
                                       | CallFrame::cBlock;

    // Check the stack and interrupts here rather than in the interpreter
    // loop itself.

    if(state->detect_stack_condition(frame)) {
      if(!state->check_interrupts(frame, frame)) return NULL;
    }

    if(unlikely(state->interrupts.check)) {
      state->interrupts.checked();
      if(state->interrupts.perform_gc) {
        state->interrupts.perform_gc = false;
        state->collect_maybe(frame);
      }
    }

    state->set_call_frame(frame);
    state->shared.checkpoint(state);

#ifdef RBX_PROFILER
    if(unlikely(state->shared.profiling())) {
      Module* mod = scope->module();
      if(MetaClass* mc = try_as<MetaClass>(mod)) {
        if(Module* ma = try_as<Module>(mc->attached_instance())) {
          mod = ma;
        }
      }

      profiler::MethodEntry method(state,
                              env->top_scope_->method()->name(),
                              mod, env->method_);
      return (*vmm->run)(state, vmm, frame);
    } else {
      return (*vmm->run)(state, vmm, frame);
    }
#else
    return (*vmm->run)(state, vmm, frame);
#endif
  }

  Object* BlockEnvironment::call(STATE, CallFrame* call_frame,
                                 Arguments& args, int flags)
  {
    BlockInvocation invocation(scope_->self(), method_->scope(), flags);
    return invoke(state, call_frame, this, args, invocation);
  }

  Object* BlockEnvironment::call_prim(STATE, CallFrame* call_frame,
                                      Executable* exec, Module* mod,
                                      Arguments& args)
  {
    return call(state, call_frame, args);
  }

  Object* BlockEnvironment::call_on_object(STATE, CallFrame* call_frame,
                                           Arguments& args, int flags)
  {
    if(args.total() < 1) {
      Exception* exc =
        Exception::make_argument_error(state, 1, args.total(),
                                       state->symbol("__block__"));
      exc->locations(state, Location::from_call_stack(state, call_frame));
      state->thread_state()->raise_exception(exc);
      return NULL;
    }

    Object* recv = args.shift(state);

    BlockInvocation invocation(recv, method_->scope(), flags);
    return invoke(state, call_frame, this, args, invocation);
  }

  Object* BlockEnvironment::call_under(STATE,CallFrame* call_frame,
                                       Executable* exec, Module* mod,
                                       Arguments& args)
  {
    if(args.total() < 2) {
      Exception* exc =
        Exception::make_argument_error(state, 2, args.total(),
                                       state->symbol("__block__"));
      exc->locations(state, Location::from_call_stack(state, call_frame));
      state->thread_state()->raise_exception(exc);
      return NULL;
    }

    Object* recv = args.shift(state);
    StaticScope* static_scope = as<StaticScope>(args.shift(state));

    BlockInvocation invocation(recv, static_scope, 0);
    return invoke(state, call_frame, this, args, invocation);
  }


  BlockEnvironment* BlockEnvironment::under_call_frame(STATE,
      CompiledMethod* cm, VMMethod* caller,
      CallFrame* call_frame, size_t index)
  {
    OnStack<1> os(state, cm);

    state->set_call_frame(call_frame);

    VMMethod* vmm = cm->internalize(state);
    if(!vmm) {
      Exception::internal_error(state, call_frame, "invalid bytecode method");
      return 0;
    }

    vmm->set_parent(caller);

    BlockEnvironment* be = state->new_object<BlockEnvironment>(G(blokenv));
    be->scope(state, call_frame->promote_scope(state));
    be->top_scope(state, call_frame->top_scope(state));
    be->method(state, cm);
    be->module(state, call_frame->module());
    be->local_count(state, cm->local_count());
    return be;
  }

  BlockEnvironment* BlockEnvironment::dup(STATE) {
    BlockEnvironment* be = state->new_object<BlockEnvironment>(G(blokenv));

    be->scope(state, scope_);
    be->top_scope(state, top_scope_);
    be->method(state, method_);
    be->local_count(state, local_count_);

    return be;
  }

  Object* BlockEnvironment::of_sender(STATE, CallFrame* call_frame) {
    NativeMethodEnvironment* nme = NativeMethodEnvironment::get();
    CallFrame* target = call_frame->previous;

    if(nme->current_call_frame() == target) {
      NativeMethodFrame* nmf = nme->current_native_frame();
      if(nmf) return nme->get_object(nmf->block());
    }

    if(target && target->scope) {
      return target->scope->block();
    }

    return Qnil;
  }

  void BlockEnvironment::Info::show(STATE, Object* self, int level) {
    BlockEnvironment* be = as<BlockEnvironment>(self);

    class_header(state, self);
    indent_attribute(level, "local_count");
      be->local_count()->show(state, level);
    indent_attribute(level, "method");
      be->method()->show(state, level);
    close_body(level);
  }
}
