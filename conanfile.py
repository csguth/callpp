from conans import ConanFile, tools, CMake


class CallppConan(ConanFile):
    name = "callpp"
    version = "0.1"
    settings = "os", "compiler", "build_type", "arch", "arch_build"
    url = "https://csguth.com"
    license = "Proprietary"
    author = "Chrystian Guth"
    generators = "cmake"
    exports_sources = [
        "CMakeLists.txt",
        "apps/**"
    ]

    def requirements(self):
        self.requires("pjproject/2.10-dev")

    def build_env(self):
        cmake = CMake(self)
        cmake.configure()
        return cmake

    def build(self):
        self.build_env().build()
