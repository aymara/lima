pushd c:\\projects\\lima
git submodule init
git submodule update
qhttpserver.exe /S /NCRC /D=c:\d
popd
mkdir c:\b
cd c:\b
echo %BOOST_ROOT%
echo %BOOST_LIBRARYDIR%
echo %PATH%
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" amd64
cmake -G "Ninja" "-DCMAKE_PREFIX_PATH=%QTDIR%;c:/projects/lima/extern/libtorch/;c:/externals/eigen-3.4.0/;c:/externals/icu4c/" "-DLIMA_RESOURCES=build" "-DLIMA_VERSION_RELEASE:STRING=%LIMA_VERSION_RELEASE%" "-DCMAKE_BUILD_TYPE=RelWithDebInfo" "-DCMAKE_INSTALL_PREFIX=c:\d" "-DBoost_DEBUG=ON" "-DBoost_COMPILER=-vc141" "-DBOOST_LIBRARYDIR:STRING=%BOOST_LIBRARYDIR%" -DCMAKE_IGNORE_PATH="C:/msys64/usr/bin" c:/projects/lima
cmake --build . --config Release

