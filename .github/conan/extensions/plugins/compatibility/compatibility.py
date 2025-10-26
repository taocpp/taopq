from cppstd_compat import cppstd_compat

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
                    ("compiler.runtime_version", "null")
                ]
            }
        ]
    configs = cppstd_compat(conanfile)
    return configs