class << Daedalus
  def host_triple
    Rubinius::BUILD_CONFIG[:host]
  end

  def config_flags
    "--build=#{host_triple} --host=#{host_triple} "
  end
end

Daedalus.blueprint do |i|
  gcc = i.gcc!

  gcc.cflags << "-Ivm -Ivm/test/cxxtest -I. -I/usr/local/include -I/opt/local/include "
  gcc.cflags << "-pipe -Wall -fno-omit-frame-pointer"
  gcc.cflags << "-ggdb3 -Werror"
  gcc.cflags << "-DRBX_PROFILER -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS"
  gcc.cflags << "-DLLVM_VERSION=#{(llvm_version.to_f*10).round}"

  if ENV['DEV']
    gcc.cflags << "-O0"
  else
    gcc.cflags << "-O2"
  end

  # This is necessary for the gcc sync prims to fully work
  if Rubinius::BUILD_CONFIG[:x86_32]
    gcc.cflags << "-march=i686"
  end

  Rubinius::BUILD_CONFIG[:defines].each do |flag|
    gcc.cflags << "-D#{flag}"
  end

  if RUBY_PLATFORM =~ /darwin/i
    if `sw_vers` =~ /10\.4/
      gcc.cflags << "-DHAVE_STRLCAT -DHAVE_STRLCPY"
    end

    # This flag makes the executable non-relocatable (and
    # slightly faster), but 4.3 does not support it.
    # TODO: Look for workarounds.
    unless `gcc -v 2>&1` =~ /gcc version 4\.3/i
      gcc.cflags << "-mdynamic-no-pic"
    end
  end

  gcc.ldflags << "-lstdc++"
  gcc.ldflags << "-L/usr/local/lib -L/opt/local/lib"

  # TODO: Fix with Platform object
  case RUBY_PLATFORM
  when /linux/i
    gcc.ldflags << '-Wl,--export-dynamic' << "-lrt" << "-lcrypt"
  when /openbsd/i
    gcc.ldflags << '-lcrypto' << '-pthread' << '-lssl' << "-ldl" << "-rdynamic"
  when /haiku/i
    gcc.ldflags << "-ldl" << "-lnetwork"
  when /bsd/i
    gcc.ldflags << "-ldl" << "-lcrypt" << "-rdynamic"
  when /mingw|win32/i
    gcc.ldflags << "-Wl,--export-all-symbols"
    gcc.ldflags << "-Wl,--out-implib,vm/librubinius-#{Rubinius::BUILD_CONFIG[:version]}.a"
    gcc.ldflags << "-lws2_32"
  else
    gcc.ldflags << "-ldl"
  end

  if RUBY_PLATFORM =~ /bsd/ and
      Rubinius::BUILD_CONFIG[:defines].include?('HAS_EXECINFO')
    gcc.ldflags << "-lexecinfo"
  end

  # Files
  subdirs = %w[ builtin capi util instruments gc llvm missing ].map do |x|
    "vm/#{x}/*.{cpp,c}"
  end

  files = i.source_files "vm/*.{cpp,c}", *subdirs

  perl = Rubinius::BUILD_CONFIG[:build_perl] || "perl"

  # Libraries
  case Rubinius::BUILD_CONFIG[:llvm]
  when :prebuilt, :svn
    llvm = i.external_lib "vm/external_libs/llvm" do |l|
      conf = llvm_configure
      flags = `#{perl} #{conf} --cflags`.strip.split(/\s+/)
      flags.delete_if { |x| x.index("-O") == 0 || x.index("-I") == 0 }
      flags << "-Ivm/external_libs/llvm/include" << "-DENABLE_LLVM"
      l.cflags = flags

      ldflags = `#{perl} #{conf} --ldflags`.strip
      objects = `#{perl} #{conf} --libfiles`.strip.split(/\s+/)

      if Rubinius::BUILD_CONFIG[:windows]
        ldflags = ldflags.sub(%r[-L/([a-zA-Z])/], '-L\1:/')

        objects.select do |f|
          f.sub!(%r[^/([a-zA-Z])/], '\1:/')
          File.file? f
        end
      end

      l.ldflags = [ldflags]
      l.objects = objects
    end

    gcc.add_library llvm
    files << llvm
  when :config
    conf = Rubinius::BUILD_CONFIG[:llvm_configure]
    flags = `#{perl} #{conf} --cflags`.strip.split(/\s+/)
    flags.delete_if { |x| x.index("-O") == 0 }
    flags << "-DENABLE_LLVM"
    gcc.cflags.concat flags
    gcc.ldflags.concat `#{perl} #{conf} --ldflags --libfiles`.strip.split(/\s+/)
  when :no
    # nothing, not using LLVM
  else
    STDERR.puts "Unsupported LLVM configuration: #{Rubinius::BUILD_CONFIG[:llvm]}"
    raise "get out"
  end

  ltm = i.external_lib "vm/external_libs/libtommath" do |l|
    l.cflags = ["-Ivm/external_libs/libtommath"]
    l.objects = [l.file("libtommath.a")]
    l.to_build do |x|
      x.command "make"
    end
  end

  onig = i.external_lib "vm/external_libs/onig" do |l|
    l.cflags = ["-Ivm/external_libs/onig"]
    l.objects = [l.file(".libs/libonig.a")]
    l.to_build do |x|
      x.command "./configure #{config_flags}" unless File.exists?("Makefile")
      x.command "make"
    end
  end

  gdtoa = i.external_lib "vm/external_libs/libgdtoa" do |l|
    l.cflags = ["-Ivm/external_libs/libgdtoa"]
    l.objects = [l.file("libgdtoa.a")]
    l.to_build do |x|
      x.command "make"
    end
  end

  ffi = i.external_lib "vm/external_libs/libffi" do |l|
    l.cflags = ["-Ivm/external_libs/libffi/include",
      "-Ivm/external_libs/libffi/#{Rubinius::BUILD_CONFIG[:host]}/include"]
    l.objects = [l.file("#{Rubinius::BUILD_CONFIG[:host]}/.libs/libffi.a")]
    l.to_build do |x|
      x.command "./configure #{config_flags}" unless File.exists?("Makefile")
      x.command "make"
    end
  end

  udis = i.external_lib "vm/external_libs/udis86" do |l|
    l.cflags = ["-Ivm/external_libs/udis86"]
    l.objects = [l.file("libudis86/.libs/libudis86.a")]
    l.to_build do |x|
      x.command "./configure #{config_flags}" unless File.exists?("Makefile")
      x.command "make"
    end
  end


  gcc.add_library udis
  gcc.add_library ffi
  gcc.add_library gdtoa
  gcc.add_library onig
  gcc.add_library ltm

  if Rubinius::BUILD_CONFIG[:windows]
    winp = i.external_lib "vm/external_libs/winpthreads" do |l|
      l.cflags = ["-Ivm/external_libs/winpthreads/include"]
      l.objects = [l.file("libpthread.a")]
      l.to_build do |x|
        x.command "./configure #{config_flags}" unless File.exists?("Makefile")
        x.command "make"
      end
    end

    gcc.add_library winp

    files << winp
  end


  files << udis
  files << ffi
  files << gdtoa
  files << onig
  files << ltm

  cli = files.dup
  cli << i.source_file("vm/drivers/cli.cpp")

  exe = RUBY_PLATFORM =~ /mingw|mswin/ ? 'vm/vm.exe' : 'vm/vm'
  i.program exe, *cli

  test_files = files.dup
  test_files << i.source_file("vm/test/runner.cpp") { |f|
    tests = Dir["vm/test/**/test_*.hpp"].sort

    f.depends_on tests

    f.autogenerate do |x|
      x.command("#{perl} vm/test/cxxtest/cxxtestgen.pl --error-printer --have-eh " +
        "--abort-on-fail -include=string.h -include=stdlib.h " +
        "-include=vm/test/test_setup.h -o vm/test/runner.cpp " +
        tests.join(' '))
    end
  }

  i.program "vm/test/runner", *test_files
end
