# Windows

LIMA builds and runs natively on Windows 10/11 (x64) with Microsoft Visual
Studio 2022. Its output is identical to the Linux one. To use LIMA from
Python, install the [Python package](python.md), which has a Windows x64 wheel
since aymara 0.8.0. Otherwise, build LIMA as described below, or use the
[Docker image](docker.md) (with Docker Desktop or WSL 2).

## Building under Windows

The `build-win.yml` GitHub workflow automates these steps.

### Prerequisites

Install with `winget` (or from the projects' sites):

```powershell
winget install Git.Git Kitware.CMake Ninja-build.Ninja Python.Python.3.12 7zip.7zip
winget install Microsoft.VisualStudio.2022.BuildTools --override "--quiet --wait --add Microsoft.VisualStudio.Workload.VCTools --includeRecommended"
```

Then the libraries (the examples install them in `C:\dev`):

- **Qt 6** for MSVC 2022 x64, with the `qtbase` and `qtdeclarative`
  modules, e.g. from the [Qt online installer](https://www.qt.io/download-qt-installer).
  Use the Qt version of the Python bindings (6.12) if you will build them.
- **Boost and ICU** with [vcpkg](https://vcpkg.io), release only:

    ```powershell
    vcpkg install --triplet x64-windows-release boost-regex boost-filesystem `
      boost-program-options boost-system boost-thread boost-test boost-iostreams `
      boost-serialization boost-spirit boost-graph boost-tuple boost-algorithm `
      boost-tokenizer boost-fusion boost-format boost-foreach boost-process `
      boost-any boost-functional boost-lexical-cast boost-range `
      boost-concept-check boost-bind boost-smart-ptr icu
    ```

- **Eigen 3.4** (vcpkg's Eigen 5 is not supported yet): extract the
  [3.4.0 archive](https://gitlab.com/libeigen/eigen/-/archive/3.4.0/eigen-3.4.0.tar.gz)
  and install it with `cmake -S eigen-3.4.0 -B eigen-build -DCMAKE_INSTALL_PREFIX=C:/dev/eigen3`
  then `cmake --install eigen-build`.
- **libtorch** (CPU, release), the version of `extern/download_libtorch.sh`:
  [libtorch-win-shared-with-deps-2.1.0+cpu.zip](https://download.pytorch.org/libtorch/cpu/libtorch-win-shared-with-deps-2.1.0%2Bcpu.zip).

### Build

From a *x64 Native Tools Command Prompt for VS 2022*, with the DLL directories
of the dependencies in the `PATH` (the LIMA tools run during the build need
them to compile the linguistic resources):

```bat
set PATH=C:\dev\Qt\6.12.0\msvc2022_64\bin;C:\dev\vcpkg\installed\x64-windows-release\bin;C:\dev\libtorch\lib;%PATH%
git clone --recurse-submodules https://github.com/aymara/lima
cmake -G Ninja -S lima -B build ^
  -DCMAKE_BUILD_TYPE=Release ^
  -DCMAKE_TOOLCHAIN_FILE=C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake ^
  -DVCPKG_TARGET_TRIPLET=x64-windows-release ^
  "-DCMAKE_PREFIX_PATH=C:/dev/Qt/6.12.0/msvc2022_64;C:/dev/libtorch" ^
  -DEigen3_DIR=C:/dev/eigen3/share/eigen3/cmake ^
  -DCMAKE_INSTALL_PREFIX=C:/dev/lima-dist ^
  -DWITH_LIMA_RESOURCES=ON -DWITH_GUI=OFF
cmake --build build
ctest --test-dir build
cmake --install build
```

`cd build && cpack -G ZIP` makes a ZIP archive of the installation.

The installation directory is self-contained: its `bin` holds the
third-party DLLs too (Qt, Boost, ICU, libtorch...), and LIMA finds its
configuration and resources relative to it, without environment variables.

The knowledge-based semantic role labeler, which embeds Python, is not built
on Windows by default (`-DWITH_PYTHON_SRL=ON` to build it).

## Running

Install a model (e.g. `python C:\dev\lima-dist\bin\lima_models.py -i eng`;
models go to `%USERPROFILE%\.local\share\lima\resources`), then:

```bat
C:\dev\lima-dist\bin\analyzeText.exe -l ud-eng -p deepud --meta udlang:eng-UD_English-MERGE my-text.txt
```
