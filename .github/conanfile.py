from conan import ConanFile
from conan.tools.cmake import CMakeDeps, CMakeToolchain, cmake_layout

class TaopqRequirements(ConanFile):
    settings = "os", "compiler", "build_type", "arch"

    def layout(self):
        cmake_layout(self)

    def requirements(self):
        self.requires("libpq/[>=14 <17]")

    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()

        deps = CMakeDeps(self)
        deps.set_property("libpq", "cmake_file_name", "PostgreSQL")
        deps.set_property("libpq", "cmake_target_name", "PostgreSQL::PostgreSQL")
        deps.set_property("libpq", "cmake_additional_variables_prefixes", ["PostgreSQL",])
        deps.generate()

    def compatibility(self):
        if self.settings.compiler == "apple-clang" and self.settings.compiler.version in ("17", "17.0"):
            return [{"settings": [("compiler.version", v)]}
                    for v in ("16.0", "16", "15.0", "15", "14.0", "14", "13.0", "13")]
