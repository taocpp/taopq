from conan import ConanFile
from conan.tools.cmake import CMakeDeps, CMakeToolchain, cmake_layout

class TaopqRequirements(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    default_options = {
        "libpq/*:with_openssl": False,
        "libpq/*:with_icu": False,
        "libpq/*:with_zlib": False,
        "libpq/*:with_zstd": False,
        "libpq/*:with_libxml2": False,
        "libpq/*:with_lz4": False,
        "libpq/*:with_xslt": False,
        "libpq/*:with_readline": False
    }

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
