#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os
from conans import ConanFile, CMake


class TestPackageConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake"

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def test(self):
        # Search for LICENSE file
        path = os.path.join(self.deps_cpp_info["taopq"].rootpath, "licenses", "LICENSE")
        assert os.path.isfile(path)
        # Validate package import
        bin_path = os.path.join("bin", "test_package")
        self.run(bin_path, run_environment=True)
