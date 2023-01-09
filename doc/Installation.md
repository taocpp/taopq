# Installation

**TODO**

### From old documentation

## Using Conan package manager

[Conan➚](https://conan.io) is a dedicated C/C++ package manager which supports many popular projects, including Taocpp.
To install install taoPQ using Conan, first we need to create a file named ``conanfile.txt`` which points taoPQ as our dependency:

```ini
[requires]
taocpp-taopq/20210727

[generators]
cmake_find_package
```

To check which the latest taoPQ packaged version available for download, visit: https://conan.io/center/taocpp-taopq.
Or, you can simply search by using the Conan client command:

    conan search -r conancenter taocpp-taopq

Now, to install taoPQ and its dependencies, run:

    conan install .

Where `.` indicates the folder where `conanfile.txt` is installed. You also can pass the file path instead.

However, if you do not want to keep a new file, you can simply install the package directly, the result will be the same:

    conan install -r conan-center taocpp-taopq/20200222@ -g cmake_find_package

Conan will generate the files `FindPostgreSQL.cmake`, `FindZLIB.cmake` and `Findtaopq.cmake` which can be linked directly our `CMakeLists.txt` file:


```cmake
cmake_minimum_required(VERSION 3.1)
project(example CXX)

find_package(ZLIB REQUIRED)
find_package(PostgreSQL REQUIRED)
find_package(taopq REQUIRED)

add_library(example main.cpp)
target_link_libraries(example taocpp::taopq)
set_target_properties(example PROPERTIES CXX_STANDARD 17)
```

Now, we just need to configure and build our project:

    cmake .
    cmake --build .

Once done, your project should be built correctly and linked to taoPQ, libpq and libz.


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
set_property(TARGET example PROPERTY CXX_STANDARD 17)
```

Now, we just need to execute CMake as usual:

```sh
cmake .
cmake --build .
```

The CMake client will download taoPQ source files based on the `main` branch, but is highly recommended using a commit to keep the reproducibility.
Besides that, PostgreSQL (libpq) is a pre-requirement. You can extend the `CMakeLists.txt` to download and build libpq too, or just consume from your system.
When executing the build step, taoPQ will be built first, as its target is required by our application, after that, the example application will be built and linked to both libpq and taoPQ.

If you want to use `libpq` from your system, there are few ways to install it:

* Windows
   * See PostgreSQL installation [page➚](https://www.postgresql.org/docs/current/install-win32.html)

* macOS
   * `$ brew install libpq`

* Linux
    * Debian/Ubuntu: `$ sudo apt-get install libpq-dev postgresql-server-dev-all`
    * Fedora: `$ sudo dnf install libpq-devel`
    * Arch: `$ sudo pacman -S postgresql-libs`

---

This document is part of [taoPQ](https://github.com/taocpp/taopq).

Copyright (c) 2021-2023 Daniel Frey and Dr. Colin Hirsch<br>
Distributed under the Boost Software License, Version 1.0<br>
See accompanying file [LICENSE_1_0.txt](../LICENSE_1_0.txt) or copy at https://www.boost.org/LICENSE_1_0.txt
