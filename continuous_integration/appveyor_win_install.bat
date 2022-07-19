rem # Prepend chosen Python to the PATH of this build
set PATH=%PYTHON%;%PYTHON%\\Scripts;%PATH%

rem # Check that we have the expected version and architecture for Python
python --version
python -c "import struct; print(struct.calcsize('P') * 8)"

rem ############################################################################
rem # All external dependencies are installed in C:\externals
rem ############################################################################
mkdir C:\externals
cd C:\externals

rem ############################################################################
rem # Install Ninja
rem ############################################################################
appveyor DownloadFile https://github.com/ninja-build/ninja/releases/download/v1.8.2/ninja-win.zip -FileName ninja.zip
7z x ninja.zip -oC:\externals\ninja > nul
set PATH=C:\externals\ninja;%PATH%
ninja --version

appveyor DownloadFile https://github.com/aymara/tensorflow/releases/download/v1.9.0-lima/tensorflow-1.9.0-raw.zip -FileName tensorflow-1.9.0-raw.zip
7z x tensorflow-1.9.0-raw.zip -oC:\externals\

appveyor DownloadFile https://download.pytorch.org/libtorch/cpu/libtorch-win-shared-with-deps-1.9.0%%2Bcpu.zip -FileName libtorch.zip
7z x libtorch.zip -oC:\projects\lima\extern\

appveyor DownloadFile https://gitlab.com/libeigen/eigen/-/archive/3.4.0/eigen-3.4.0.zip -FileName eigen.zip
7z x eigen.zip -oC:\externals\
mv C:\externals\eigen-3.4.0\ C:\externals\eigen3\

appveyor DownloadFile https://github.com/unicode-org/icu/releases/download/release-67-1/icu4c-67_1-Win64-MSVC2017.zip -FileName icu4c.zip
7z x icu4c.zip -oc:\externals\icu4c\

rem #ps: (new-object net.webclient).DownloadFile('http://www.nltk.org/nltk_data/packages/corpora/dependency_treebank.zip', 'c:\dependency_treebank.zip')
mkdir c:\dependency_treebank
unzip c:\dependency_treebank.zip -d c:\
cd c:\
c:\msys64\usr\bin\cat /c/dependency_treebank/wsj_*.dp | c:\msys64\usr\bin\grep -v "^$" > c:\dependency_treebank\nltk-ptb.dp

