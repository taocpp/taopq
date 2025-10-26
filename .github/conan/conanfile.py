from conan import ConanFile
from conan.tools.cmake import CMakeDeps, CMakeToolchain, cmake_layout

class TaopqRequirements(ConanFile):
    settings = "os", "compiler", "build_type", "arch"

    def layout(self):
        cmake_layout(self)

    def requirements(self):
        self.requires("libpq/[*]")

    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()

        deps = CMakeDeps(self)
        deps.set_property("libpq", "cmake_file_name", "PostgreSQL")
        deps.set_property("libpq", "cmake_target_name", "PostgreSQL::PostgreSQL")
        deps.set_property("libpq", "cmake_additional_variables_prefixes", ["PostgreSQL",])
        deps.generate()
