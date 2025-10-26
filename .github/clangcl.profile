[settings]
os=Windows
arch=x86_64
build_type=Release
compiler=clang
compiler.version=19
compiler.cppstd=20
compiler.runtime=dynamic
compiler.runtime_type=Release
compiler.runtime_version=v144

[options]
libpq/*:with_readline=False
libpq/*:with_openssl=False
libpq/*:with_icu=False
libpq/*:with_zlib=False
libpq/*:with_zstd=False
libpq/*:with_libxml2=False
libpq/*:with_lz4=False
libpq/*:with_xslt=False

[conf]
tools.cmake.cmaketoolchain:generator=Ninja
tools.build:cflags=["/std:c11"]
tools.meson.mesontoolchain:backend=ninja
tools.build:compiler_executables={"c": "clang-cl.exe", "cpp": "clang-cl.exe"}
