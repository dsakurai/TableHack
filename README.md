# qt-conan2

## Conan Qt6 recipe

This project includes a `conanfile.py` that requests Qt6 with `widgets` and optionally `qtwebengine` enabled. WebEngine requires shared Qt builds and additional system build tools (nodejs, ninja, bison/flex on some platforms).

## Prerequisite

We assume that C++ tools like the compiler and linker are installed externally by you.

Set the `CONAN_HOME` env var to some folder without a whitespace (because libiconv fails to build otherwise).
For a global config of the env var in PowerShell:
```powershell
[Environment]::SetEnvironmentVariable("CONAN_HOME", "C:\NoWhitespace\conan-cache", "User")
```

CMake and Conan (and Python) can be installed using `uv`.

## On Windows

Make sure you have enabled long paths in Windows Settings.
This project is tested using the Visual Studio compiler:
```
//Generator instance identifier.
CMAKE_GENERATOR_INSTANCE:INTERNAL=C:/Program Files/Microsoft Visual Studio/18/Community
```

On the command pallette, activate `CMake: Set Build Target` and choose `ALL_BUILD` or some proper target you want to build. Somehow, CMakeTool tends to default to `all`, which is invalid for the Visual Studio project. This issue started to appear only since the following commit: `da901a5b591bd9ce2405320dd7b77e57710ee721` with the commit message `Rename the preset.` on 2026-09-28. So, maybe there's a fix to this. But it's possibly a bug. Not sure whether I should fix it.

## Auto build on VSCode 

Press F5.
This also initializes and configures the project using commands as defined in `.vscode/tasks.json`

You *could* use the `Build` button of the CMakeTool extension, but, for the first time build, you have to press F5 to invoke the build task in `tasks.json` directly. This is because the CMakeTool extension assumes a CMake preset, which becomes valid only after Conan install is run.

## Setting Up The Project Manually

```powershell
.\venv\Scripts\activate # Assuming you did `uv sync`
mkdir build
cd build
#
.\.venv\Scripts\conan.exe install .. --build=missing -s build_type=Release
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

## On Wasm

Web assembly is not supported by this project. It requires a specifically built Qt, which is not made officially available by Conan.

For example, Qt 6.11 explains the process here: https://doc.qt.io/qt-6/wasm.html.
On Windows, it is probably the simplest to use the Qt's official installer to install Qt for wasm.

Regarding how to use wasm with CMake, check the Qt documentation. An example project (albeit without Qt) is available here: https://github.com/dsakurai/webassembly-cmake.