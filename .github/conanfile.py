from conan import ConanFile
from conan.tools.cmake import CMakeDeps, CMakeToolchain, cmake_layout
from conan.tools.apple import is_apple_os

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

    def compatibility(self):
        if is_apple_os(self) and self.settings.compiler == "apple-clang" and self.settings.compiler.version == "17.0":
            return [{"settings": [("compiler.version", v)]}
                    for v in ("16.0", "15.0", "14.0", "13.0")]
