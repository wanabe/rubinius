#include "ruby.h"

#include "symbols.hpp"

#ifdef __cplusplus
extern "C" {
#endif

namespace melbourne {
  ID rb_sOpAnd;
  ID rb_sOpOr;

  ID rb_sAlias;
  ID rb_sAnd;
  ID rb_sArgs;
  ID rb_sArgsCat;
  ID rb_sArgsPush;
  ID rb_sArray;
  ID rb_sAttrAsgn;
  ID rb_sBackRef;
  ID rb_sBegin;
  ID rb_sBlock;
  ID rb_sBlockArg;
  ID rb_sBlockPass;
  ID rb_sBreak;
  ID rb_sCall;
  ID rb_sCase;
  ID rb_sCDecl;
  ID rb_sClass;
  ID rb_sColon2;
  ID rb_sColon3;
  ID rb_sConst;
  ID rb_sCVar;
  ID rb_sCVAsgn;
  ID rb_sCVDecl;
  ID rb_sData;
  ID rb_sDefined;
  ID rb_sDefn;
  ID rb_sDefs;
  ID rb_sDot2;
  ID rb_sDot3;
  ID rb_sDRegx;
  ID rb_sDRegxOnce;
  ID rb_sDStr;
  ID rb_sDSym;
  ID rb_sDXStr;
  ID rb_sEnsure;
  ID rb_sEvStr;
  ID rb_sFalse;
  ID rb_sFCall;
  ID rb_sFile;
  ID rb_sFixnum;
  ID rb_sFlip2;
  ID rb_sFlip3;
  ID rb_sFloat;
  ID rb_sFor;
  ID rb_sGAsgn;
  ID rb_sGVar;
  ID rb_sHash;
  ID rb_sIAsgn;
  ID rb_sIf;
  ID rb_sIter;
  ID rb_sIVar;
  ID rb_sLAsgn;
  ID rb_sLit;
  ID rb_sLVar;
  ID rb_sMAsgn;
  ID rb_sMatch;
  ID rb_sMatch2;
  ID rb_sMatch3;
  ID rb_sModule;
  ID rb_sNegate;
  ID rb_sNext;
  ID rb_sNil;
  ID rb_sNot;
  ID rb_sNthRef;
  ID rb_sNumber;
  ID rb_sOpAsgn1;
  ID rb_sOpAsgn2;
  ID rb_sOpAsgnAnd;
  ID rb_sOpAsgnOr;
  ID rb_sOr;
  ID rb_sPostExe;
  ID rb_sRedo;
  ID rb_sRegex;
  ID rb_sResbody;
  ID rb_sRescue;
  ID rb_sRetry;
  ID rb_sReturn;
  ID rb_sSClass;
  ID rb_sScope;
  ID rb_sSelf;
  ID rb_sSplat;
  ID rb_sStr;
  ID rb_sSuper;
  ID rb_sSValue;
  ID rb_sToAry;
  ID rb_sTrue;
  ID rb_sUndef;
  ID rb_sUntil;
  ID rb_sVAlias;
  ID rb_sVCall;
  ID rb_sWhen;
  ID rb_sWhile;
  ID rb_sXStr;
  ID rb_sYield;
  ID rb_sZArray;
  ID rb_sZSuper;

};

#ifdef __cplusplus
}  /* extern "C" { */
#endif

namespace melbourne {

  void init_symbols() {
    rb_sOpAnd     = rb_intern("and");
    rb_sOpOr      = rb_intern("or");

    rb_sAlias     = rb_intern("process_alias");
    rb_sAnd       = rb_intern("process_and");
    rb_sArgs      = rb_intern("process_args");
    rb_sArgsCat   = rb_intern("process_argscat");
    rb_sArgsPush  = rb_intern("process_argspush");
    rb_sArray     = rb_intern("process_array");
    rb_sAttrAsgn  = rb_intern("process_attrasgn");
    rb_sBackRef   = rb_intern("process_back_ref");
    rb_sBegin     = rb_intern("process_begin");
    rb_sBlock     = rb_intern("process_block");
    rb_sBlockArg  = rb_intern("process_block_arg");
    rb_sBlockPass = rb_intern("process_block_pass");
    rb_sBreak     = rb_intern("process_break");
    rb_sCall      = rb_intern("process_call");
    rb_sCase      = rb_intern("process_case");
    rb_sCDecl     = rb_intern("process_cdecl");
    rb_sClass     = rb_intern("process_class");
    rb_sColon2    = rb_intern("process_colon2");
    rb_sColon3    = rb_intern("process_colon3");
    rb_sConst     = rb_intern("process_const");
    rb_sCVar      = rb_intern("process_cvar");
    rb_sCVAsgn    = rb_intern("process_cvasgn");
    rb_sCVDecl    = rb_intern("process_cvdecl");
    rb_sData      = rb_intern("process_data");
    rb_sDefined   = rb_intern("process_defined");
    rb_sDefn      = rb_intern("process_defn");
    rb_sDefs      = rb_intern("process_defs");
    rb_sDot2      = rb_intern("process_dot2");
    rb_sDot3      = rb_intern("process_dot3");
    rb_sDRegx     = rb_intern("process_dregx");
    rb_sDRegxOnce = rb_intern("process_dregx_once");
    rb_sDStr      = rb_intern("process_dstr");
    rb_sDSym      = rb_intern("process_dsym");
    rb_sDXStr     = rb_intern("process_dxstr");
    rb_sEnsure    = rb_intern("process_ensure");
    rb_sEvStr     = rb_intern("process_evstr");
    rb_sFalse     = rb_intern("process_false");
    rb_sFCall     = rb_intern("process_fcall");
    rb_sFile      = rb_intern("process_file");
    rb_sFixnum    = rb_intern("process_fixnum");
    rb_sFlip2     = rb_intern("process_flip2");
    rb_sFlip3     = rb_intern("process_flip3");
    rb_sFloat     = rb_intern("process_float");
    rb_sFor       = rb_intern("process_for");
    rb_sGAsgn     = rb_intern("process_gasgn");
    rb_sGVar      = rb_intern("process_gvar");
    rb_sHash      = rb_intern("process_hash");
    rb_sIAsgn     = rb_intern("process_iasgn");
    rb_sIf        = rb_intern("process_if");
    rb_sIter      = rb_intern("process_iter");
    rb_sIVar      = rb_intern("process_ivar");
    rb_sLAsgn     = rb_intern("process_lasgn");
    rb_sLit       = rb_intern("process_lit");
    rb_sLVar      = rb_intern("process_lvar");
    rb_sMAsgn     = rb_intern("process_masgn");
    rb_sMatch     = rb_intern("process_match");
    rb_sMatch2    = rb_intern("process_match2");
    rb_sMatch3    = rb_intern("process_match3");
    rb_sModule    = rb_intern("process_module");
    rb_sNegate    = rb_intern("process_negate");
    rb_sNext      = rb_intern("process_next");
    rb_sNil       = rb_intern("process_nil");
    rb_sNot       = rb_intern("process_not");
    rb_sNthRef    = rb_intern("process_nth_ref");
    rb_sNumber    = rb_intern("process_number");
    rb_sOpAsgn1   = rb_intern("process_op_asgn1");
    rb_sOpAsgn2   = rb_intern("process_op_asgn2");
    rb_sOpAsgnAnd = rb_intern("process_op_asgn_and");
    rb_sOpAsgnOr  = rb_intern("process_op_asgn_or");
    rb_sOr        = rb_intern("process_or");
    rb_sPostExe   = rb_intern("process_postexe");
    rb_sRedo      = rb_intern("process_redo");
    rb_sRegex     = rb_intern("process_regex");
    rb_sResbody   = rb_intern("process_resbody");
    rb_sRescue    = rb_intern("process_rescue");
    rb_sRetry     = rb_intern("process_retry");
    rb_sReturn    = rb_intern("process_return");
    rb_sSClass    = rb_intern("process_sclass");
    rb_sScope     = rb_intern("process_scope");
    rb_sSelf      = rb_intern("process_self");
    rb_sSplat     = rb_intern("process_splat");
    rb_sStr       = rb_intern("process_str");
    rb_sSuper     = rb_intern("process_super");
    rb_sSValue    = rb_intern("process_svalue");
    rb_sToAry     = rb_intern("process_to_ary");
    rb_sTrue      = rb_intern("process_true");
    rb_sUndef     = rb_intern("process_undef");
    rb_sUntil     = rb_intern("process_until");
    rb_sVAlias    = rb_intern("process_valias");
    rb_sVCall     = rb_intern("process_vcall");
    rb_sWhen      = rb_intern("process_when");
    rb_sWhile     = rb_intern("process_while");
    rb_sXStr      = rb_intern("process_xstr");
    rb_sYield     = rb_intern("process_yield");
    rb_sZArray    = rb_intern("process_zarray");
    rb_sZSuper    = rb_intern("process_zsuper");
  }
};
