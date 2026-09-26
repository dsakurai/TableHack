# qt-conan2

## Conan Qt6 recipe

This project includes a `conanfile.py` that requests Qt6 with `widgets` and optionally `qtwebengine` enabled. WebEngine requires shared Qt builds and additional system build tools (nodejs, ninja, bison/flex on some platforms).

### Notes on Build Tools

We assume that C++ tools like the compiler and linker are installed externally by you.

Set the `CONAN_HOME` env var to some folder without a whitespace (because libiconv fails to build otherwise).

CMake and Conan (and Python) can be installed using `uv`.

## On Windows

Make sure you have enabled long paths in Windows Settings.
This project is tested using the Visual Studio compiler:
```
//Generator instance identifier.
CMAKE_GENERATOR_INSTANCE:INTERNAL=C:/Program Files/Microsoft Visual Studio/18/Community
```

### Auto build on VSCode 

Press F5.
This also initializes and configures the project using commands as defined in `.vscode/tasks.json`

You *could* use the `Build` and `Debug` buttons of the CMakeTool extension, but, for the first time build, you have to press F5. This is because the CMakeTool extension assumes a CMake preset, which becomes valid only after Conan install is run.

### Setting Up The Project Manually

```powershell
.\venv\Scripts\activate # Assuming you did `uv sync`
mkdir build
cd build
#
.\.venv\Scripts\conan.exe install .. --build=missing -s build_type=Release
```

Or, for a global config of the env var in PowerShell:
```powershell
[Environment]::SetEnvironmentVariable("CONAN_HOME", "C:\NoWhitespace\conan-cache", "User")
```

Configure
```
cmake --preset conan-default
```

Build
```
cmake --build --preset conan-release
```

Run
```
& "build\generators\conanrun.ps1"
build\Debug\example.exe
# or `build\Release\example.exe`, depending on your configuration.
```
