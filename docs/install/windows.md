# Windows

!!! warning "Windows support is currently broken"
    LIMA has been ported to Microsoft Windows and Windows installers were
    published in the past, but the Windows build is currently broken and no
    recent installer is available. Use the [Docker image](docker.md) (with
    Docker Desktop or WSL 2) to run LIMA under Windows. Contributions to restore
    the Windows build are welcome.

## Building under Windows

The following procedure was used to build LIMA under Windows 10. It is kept
as a starting point; versions are outdated and the procedure needs updating.

1. Install [MSYS2](https://www.msys2.org/) (x86_64), e.g. in `C:\msys64`, and
   add `C:\msys64\usr\bin` and `C:\msys64\usr\lib` to the user `Path` so that
   Unix commands can be called from any console.
2. From any console, install git, gawk and patch: `pacman -S git gawk patch`.
3. Install [CMake](https://cmake.org/) from cmake.org (not the MSYS2 one, which
   does not work).
4. Install Python 3 from python.org.
5. Install Qt from [qt.io](https://www.qt.io/).
6. Install [NSIS](https://sourceforge.net/projects/nsis/) and add its `Bin`
   folder to the user `Path`.
7. Install Microsoft Visual Studio Community Edition (only the "Desktop
   Development with C++" component is required).
8. Download and build Boost from a Visual Studio x64 native console:
   `bootstrap.bat`, then `b2 --build-type=complete install`.
9. Install NLTK and the Penn treebank extract (see
   [Build from source](source.md#english-training-data)).
10. Clone the LIMA repository: `git clone https://github.com/aymara/lima`.
11. Download [Ninja](https://ninja-build.org/) and make it available in the
    `Path`.
12. Download libtorch (the PyTorch C++ distribution) into `extern/` (see
    `extern/download_libtorch.sh` for the URL and version used).
13. Update `build_with_ninja.bat` with your paths (`MSYSDIR`, `QTDIR`,
    `WINKITDIR`, `NINJADIR`, `BOOST_ROOT`, `NLTK_PTB_DP_DIR`, `LIMA_SRC`) and,
    optionally, `LIMA_VERSION_RELEASE`.
14. From a Visual Studio x64 native console in the LIMA source directory, run
    `build_with_ninja.bat`.
