# Installation

## Using CMake

Since CMake 3.11, the feature [FetchContent➚](https://cmake.org/cmake/help/latest/module/FetchContent.html) can be used to download and build project dependencies.
This mechanism makes our development much easier, but it lacks in terms of reproducibility, so be careful if you are using it for production. Also, we will use `FetchContent_MakeAvailable` which is available since CMake 3.14:

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

The CMake client will download taoPQ source files based on the `main` branch, but is highly recommended using a commit or tag to keep the reproducibility.
Besides that, PostgreSQL (libpq) is a pre-requirement. You can extend the `CMakeLists.txt` to download and build libpq too, or just consume from your system.
When executing the build step, taoPQ will be built first, as its target is required by our application, after that, the example application will be built and linked to both libpq and taoPQ.

---

This document is part of [taoPQ](https://github.com/taocpp/taopq).

Copyright (c) 2021-2025 Daniel Frey and Dr. Colin Hirsch<br>
Distributed under the Boost Software License, Version 1.0<br>
See accompanying file [LICENSE_1_0.txt](../LICENSE_1_0.txt) or copy at https://www.boost.org/LICENSE_1_0.txt
