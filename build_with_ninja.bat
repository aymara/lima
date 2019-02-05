set VERBOSE=1

set QTDIR=C:\Qt\5.11.2\msvc2017_64
set TFDIR=C:\Program^ Files\tensorflow 1.9.0
set WINKITDIR="C:\Program Files (x86)\Windows Kits\10\bin\10.0.17134.0\x64"
set MSYSDIR=c:\msys64\usr\bin
set NINJADIR=c:\soft
set BOOST_ROOT=C:\Boost
set NLTK_PTB_DP_DIR=%HOMEDRIVE%\%HOMEPATH%\Application^ Data\nltk_data\corpora\dependency_treebank
set LIMA_SRC=c:\lima

set BLD_DIR=c:\\b
set LIMA_DIST=c:\\d

rmdir /S /Q %LIMA_DIST%
mkdir %LIMA_DIST%

rmdir /S /Q %BLD_DIR%
mkdir %BLD_DIR%
cd %BLD_DIR%

set BOOST_LIBRARYDIR=%BOOST_ROOT%\lib
set LIMA_EXTERNALS=%LIMA_DIST%
set LIMA_CONF=%LIMA_DIST%\share\config\lima
set LIMA_RESOURCES=%LIMA_DIST%\share\apps\lima\resources
set PATH=%NINJADIR%;%TFDIR%;%QTDIR%\bin;%LIMA_DIST%\bin;%LIMA_DIST%\share\apps\lima\scripts;%BOOST_LIBRARYDIR%;%MSYSDIR%;%WINKITDIR%;%PATH%
set LIMA_DISABLE_FSW_TESTING=true
set NLTK_PTB_DP_FILE=%NLTK_PTB_DP_DIR%\nltk-ptb.dp
set TFINCLUDE=%TFDIR%\include
set LIMA_VERSION_RELEASE=20181218114529-d9467701

cmake -G "Ninja" "-DCMAKE_PREFIX_PATH=%QTDIR%;%BOOST_ROOT%" "-DLIMA_RESOURCES=build" "-DLIMA_VERSION_RELEASE:STRING=%LIMA_VERSION_RELEASE%" "-DTF_SOURCES_PATH=%TFINCLUDE%" -DCMAKE_BUILD_TYPE=Release "-DCMAKE_INSTALL_PREFIX=%LIMA_DIST%" -DCMAKE_IGNORE_PATH=%MSYSDIR% %LIMA_SRC%
cmake --build . --config Release -- -v -j 4

cd %LIMA_SRC%