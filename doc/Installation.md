# Installation

## Using CMake

Since CMake 3.11, the feature [FetchContent➚](https://cmake.org/cmake/help/latest/module/FetchContent.html) can be used to download and build project dependencies.
This mechanism simplifies development, but it has a downside: downloaded dependencies may change between builds, which can make builds non-reproducible. Be careful when using it in production.
Also, this example uses `FetchContent_MakeAvailable`, which is available since CMake 3.14:

```cmake
cmake_minimum_required(VERSION 3.14)
project(example CXX)

include(FetchContent)
find_package(PostgreSQL REQUIRED)

FetchContent_Declare(
   taocpp-taopq
   GIT_REPOSITORY https://github.com/taocpp/taopq
   GIT_TAG main
)
FetchContent_MakeAvailable(taocpp-taopq)


add_library(example main.cpp)
target_link_libraries(example taocpp::taopq)
set_property(TARGET example PROPERTY CXX_STANDARD 20)
```

Now, we just need to execute CMake as usual:

```sh
cmake .
cmake --build .
```

The CMake build will download the taoPQ source files from the `main` branch. For reproducibility, we recommend pinning to a specific commit or tag instead.
PostgreSQL (libpq) is a prerequisite; you can either install it from your system or extend the `CMakeLists.txt` to download and build it as well.
During the build, taoPQ is built first, as it is a dependency of the example application. The example is then built and linked to both libpq and taoPQ.

---

## Using Conan

You can install a pre-built binary for taoPQ or build it from source using [Conan](https://conan.io/). Use the following command:

```bash
conan install --requires="taocpp-taopq/[*]" --build=missing
```

The taoPQ Conan recipe is maintained by the Conan community. If the version is out of date, please [create an issue or pull request](https://github.com/conan-io/conan-center-index) on the ConanCenterIndex repository.


This document is part of [taoPQ](https://github.com/taocpp/taopq).

Copyright (c) 2021-2026 Daniel Frey and Dr. Colin Hirsch<br>
Distributed under the Boost Software License, Version 1.0<br>
See accompanying file [LICENSE_1_0.txt](../LICENSE_1_0.txt) or copy at https://www.boost.org/LICENSE_1_0.txt
