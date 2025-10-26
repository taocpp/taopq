def compatibility(conanfile):
    results = []
    if conanfile.settings.compiler == "clang" and conanfile.settings.os == "Windows":
        results.append(
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
        )
    return results