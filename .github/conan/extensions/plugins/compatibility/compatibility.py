def compatibility(conanfile):
    if conanfile.settings.compiler == "clang" and conanfile.settings.os == "Windows":
        return [
            {
                "settings": [
                    ("compiler", "msvc"),
                    ("compiler.version", "194"),
                    ("compiler.cppstd", "20"),
                    ("compiler.runtime", "dynamic"),
                    ("compiler.runtime_type", "Release"),
                ]
            }
        ]
