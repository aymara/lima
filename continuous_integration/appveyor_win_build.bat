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
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" amd64
cmake -G "Ninja" "-DCMAKE_PREFIX_PATH=%QTDIR%;c:/projects/lima/extern/libtorch/;c:/externals/eigen3/cmake;c:/externals/icu4c/;c:/externals/tensorflow-1.9.0/" "-DLIMA_RESOURCES=build" "-DLIMA_VERSION_RELEASE:STRING=%LIMA_VERSION_RELEASE%" "-DCMAKE_BUILD_TYPE=RelWithDebInfo" "-DCMAKE_INSTALL_PREFIX=c:\d" "-DBoost_DEBUG=ON" "-DBoost_COMPILER=-vc142" "-DBOOST_LIBRARYDIR:STRING=%BOOST_LIBRARYDIR%" -DCMAKE_IGNORE_PATH="C:/msys64/usr/bin" -DWITH_DEBUG_MESSAGES=ON -DWITH_ARCH=OFF -DWITH_ASAN=OFF -DSHORTEN_POR_CORPUS_FOR_SVMLEARN=ON -DWITH_GUI=ON c:/projects/lima
cmake --build . --config Release

