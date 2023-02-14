add_rules("mode.debug", "mode.release")

target("ifs")
    set_kind("binary")
    set_languages("c17","c++20")
    add_includedirs("src")
    add_files("src/**.cpp")
