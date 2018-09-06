#!/usr/bin/env python
# -*- coding: utf-8 -*-
from conans import ConanFile, CMake, tools


class PostgreSQLConan(ConanFile):
    name = "postgres"
    description = "C++11 client library for PostgreSQL"
    homepage = "https://github.com/taocpp/postgres"
    url = homepage
    license = "MIT"
    author = "taocpp@icemx.net"
    settings = "os", "build_type", "compiler", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = "shared=False", "fPIC=True"
    exports = "LICENSE"
    exports_sources = "include/*", "src/*", "CMakeLists.txt", "cmake/*"
    generators = "cmake"
    requires = "libpq/9.6.9@bincrafters/stable"

    def config_options(self):
        if self.settings.os == "Windows":
            self.options.remove("fPIC")

    def configure_cmake(self):
        cmake = CMake(self)
        cmake.definitions["TAOCPP_POSTGRES_BUILD_TESTS"] = False
        cmake.definitions["TAOCPP_POSTGRES_INSTALL_DOC_DIR"] = "licenses"
        cmake.configure()
        return cmake

    def build(self):
        cmake = self.configure_cmake()
        cmake.build()

    def package(self):
        cmake = self.configure_cmake()
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = tools.collect_libs(self)
