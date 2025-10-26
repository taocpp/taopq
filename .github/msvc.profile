 [settings]
arch=x86_64
build_type=Release
compiler=msvc
compiler.cppstd=20
compiler.runtime=dynamic
compiler.runtime_type=Release
compiler.version=194
os=Windows

[conf]
tools.cmake.cmaketoolchain:generator=Ninja
tools.build:compiler_executables={"c": "cl.exe", "cpp": "cl.exe"}
