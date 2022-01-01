# Requirements

## Operating System Support

* We support:
  * [Windows➚](https://en.wikipedia.org/wiki/Microsoft_Windows).
  * [macOS➚](https://en.wikipedia.org/wiki/MacOS).
  * [Linux➚](https://en.wikipedia.org/wiki/Linux).
* Other systems might work.

## Compiler Support

* We support:
  * [Visual Studio➚](https://en.wikipedia.org/wiki/Microsoft_Visual_Studio) version 2019 or newer.
  * [Xcode➚](https://en.wikipedia.org/wiki/Xcode) version 11 or newer.
  * [GCC➚](https://gcc.gnu.org/) version 8 or newer.
  * [Clang➚](https://clang.llvm.org/) version 8 or newer.
* Other compilers might work.

## Language Requirements

* We require [C++17➚](https://en.wikipedia.org/wiki/C%2B%2B17) or newer.
* We require exception support. The `-fno-exceptions` option is not supported.
* We require RTTI support. The `-fno-rtti` option is not supported.
* We support Clang's [`-fms-extensions`➚](https://clang.llvm.org/docs/MSVCCompatibility.html) option.

## Compiler Warnings

* We support the `/W4` option on [Visual Studio➚](https://docs.microsoft.com/en-us/cpp/build/reference/compiler-option-warning-level).
* We support the `-pedantic`, `-Wall`, and `-Wextra` options on [GCC➚](https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html) and [Clang➚](https://clang.llvm.org/docs/DiagnosticsReference.html).

:point_up: Note that we *support* these options, we don't *require* them to be used.
You can decide which options you want to use in your project, we just try to not get in the way by making sure that our code doesn't generate any of those warnings.

## Database Requirements

* We expect the database to use UTF-8 encoding.
* We expect the database to send `BYTEA` data in [`bytea` hex format➚](https://www.postgresql.org/docs/current/datatype-binary.html).
* We expect the database connection to use [protocol version 3➚](https://www.postgresql.org/docs/current/protocol.html).

---

This document is part of [taoPQ](https://github.com/taocpp/taopq).

Copyright (c) 2021-2022 Daniel Frey and Dr. Colin Hirsch<br>
Distributed under the Boost Software License, Version 1.0<br>
See accompanying file [LICENSE_1_0.txt](../LICENSE_1_0.txt) or copy at https://www.boost.org/LICENSE_1_0.txt
