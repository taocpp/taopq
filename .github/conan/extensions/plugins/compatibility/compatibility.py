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
    elif conanfile.settings.compiler == "apple-clang" and conanfile.settings.compiler.version in ("17", "17.0"):
        results.extend([{"settings": [("compiler.version", v)]}
                for v in ("16.0", "16", "15.0", "15", "14.0", "14", "13.0", "13")])
    return results