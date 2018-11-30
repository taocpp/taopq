#!/usr/bin/env python
# -*- coding: utf-8 -*-
from conans import ConanFile, CMake, tools
from conans.model.version import Version
from conans.errors import ConanInvalidConfiguration

class TaopqConan(ConanFile):
    name = "taopq"
    description = "C++ client library for PostgreSQL"
    homepage = "https://github.com/taocpp/taopq"
    url = homepage
    license = "MIT"
    author = "taocpp@icemx.net"
    settings = "os", "build_type", "compiler", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}
    exports = "LICENSE"
    exports_sources = "include/*", "src/*", "CMakeLists.txt", "cmake/*"
    generators = "cmake"
    requires = "libpq/9.6.9@bincrafters/stable"

    def config_options(self):
        if self.settings.os == "Windows":
            self.options.remove("fPIC")

    def configure(self):
        compiler_version = Version(self.settings.compiler.version.value)
        if (self.settings.compiler == "gcc" and compiler_version < "7") or \
           (self.settings.compiler == "clang" and compiler_version < "5.0") or \
           (self.settings.compiler == "clang" and compiler_version == "5.0" and \
            self.settings.compiler.libcxx == "libc++") or \
           (self.settings.compiler == "apple-clang" and compiler_version < "10") or \
           (self.settings.compiler == "Visual Studio" and compiler_version < "15"):
            raise ConanInvalidConfiguration("taocpp/taopq requires C++17 support")

    def _configure_cmake(self):
        cmake = CMake(self)
        cmake.definitions["TAOPQ_BUILD_TESTS"] = False
        cmake.definitions["TAOPQ_INSTALL_DOC_DIR"] = "licenses"
        if self.settings.os == 'Windows' and self.settings.compiler == 'Visual Studio':
            cmake.definitions["CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS"] = self.options.shared
        cmake.configure()
        return cmake

    def build(self):
        cmake = self._configure_cmake()
        cmake.build()

    def package(self):
        cmake = self._configure_cmake()
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = tools.collect_libs(self)
