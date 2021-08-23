# Requirements

## Operating System Support

* We support:
  * [Windows](https://en.wikipedia.org/wiki/Microsoft_Windows).
  * [macOS](https://en.wikipedia.org/wiki/MacOS).
  * [Linux](https://en.wikipedia.org/wiki/Linux).
* Other systems might work.

## Language Requirements

* We require C++17 or newer.
* We require exception support. The `-fno-exceptions` option is not supported.
* We require RTTI support. The `-fno-rtti` option is not supported.

## Compiler Support

* We support:
  * [Visual Studio](https://en.wikipedia.org/wiki/Microsoft_Visual_Studio) version 2017 or newer.
  * [Xcode](https://en.wikipedia.org/wiki/Xcode) version 11 or newer.
  * [GCC](https://gcc.gnu.org/) version 8 or newer.
  * [Clang](https://clang.llvm.org/) version 8 or newer.
* Other compilers might work.

## Compiler Options Support

* We support the `/W4` option on Visual Studio.
* We support the `-Wpedantic`, `-Wall`, `-Wextra`, and `-Werror` options on [GCC](https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html) and [Clang](https://clang.llvm.org/docs/DiagnosticsReference.html).
* We support Clang's [`-fms-extensions`](https://clang.llvm.org/docs/MSVCCompatibility.html) option.

## Database Requirements

* We expect the database to use UTF-8 encoding.
* We expect the database connection to use [protocol version 3](https://www.postgresql.org/docs/current/protocol.html).

Copyright (c) 2021 Daniel Frey and Dr. Colin Hirsch
