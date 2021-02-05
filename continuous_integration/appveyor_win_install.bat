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

rem #ps: (new-object net.webclient).DownloadFile('http://www.nltk.org/nltk_data/packages/corpora/dependency_treebank.zip', 'c:\dependency_treebank.zip')
mkdir c:\dependency_treebank
unzip c:\dependency_treebank.zip -d c:\
cd c:\
c:\msys64\usr\bin\cat /c/dependency_treebank/wsj_*.dp | c:\msys64\usr\bin\grep -v "^$" > c:\dependency_treebank\nltk-ptb.dp


