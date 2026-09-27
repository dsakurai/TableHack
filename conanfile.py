from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout, CMakeDeps, CMakeToolchain


class QtConan(ConanFile):
    name = "qt-conan2"
    version = "0.1.0"
    settings = "os", "compiler", "build_type", "arch"
    
    exports_sources = "CMakeLists.txt"

    requires = ["qt/[>=6.11 <12]",
                ]
    default_options = {
        "qt/*:shared": True,
        "qt/*:gui": True,
        "qt/*:widgets": True,
        "qt/*:qtwebengine": True,
        "qt/*:qtdeclarative": True, # used for QtWebEngine
        "qt/*:qtwebchannel": True, # used for QtWebEngine
        "qt/*:with_pq": False,
        "libiconv/*:tools.env.virtualenv:powershell": False, # Work around for a bug in libiconv's PowerShell virtualenv activation script that causes the build to fail
    }

    def layout(self):
        cmake_layout(self)
        # Keep Debug/Release outputs side by side instead of sharing one "build" folder.
        self.folders.build = f"build-{self.settings.build_type}"
        self.folders.generators = f"{self.folders.build}/generators"

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()

        tc = CMakeToolchain(self)
        # Distinct preset names per build folder, avoids "conan-default" collision in CMakeUserPresets.json.
        tc.presets_prefix = f"conan-{str(self.settings.build_type).lower()}"
        tc.generate()

        # VirtualRunEnv(self).generate() # Not needed if we pass the option to generate the "dotenv" file from the command line.

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
