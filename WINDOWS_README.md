How to build lima under Windows (Windows 10):

1. install msys2 (x86_64 version) from msys2.org
   From now on we assume that you've installed msys2 into c:\msys64.

2. Add c:\msys64\usr\bin and c:\msys64\usr\lib to user-wide Path variable (necessary to invoke unix commands from any other console).

3. [from any console] pacman -S git (necessary to clone lima)

4. [from any console] pacman -S gawk patch (used in lima build process)

5. install cmake from cmake.org . Don't use cmake from msys (i.e. don't do "pacman -S cmake" - this doesn't work)

6. install Python 3.7 from python.org
   From now on we assume that you've installed Python into c:\Python37 (attention: this isn't a default path!)

7. install Qt from qt.io

8. install NSIS from sourceforge.net/projects/nsis

9. add path to NSIS binaries (by default "C:\Program Files (x86)\NSIS\Bin") to user-wide Path variable

10. install Microsoft Visual Studio 2017 Community Edition ("Desktop Development with C++" is the only component required)

11. download and compile Boost (takes time)
    [from vs2017 x64 native console] bootstrap.bat
    [from vs2017 x64 native console] b2 --build-type=complete install

12. install nltk
    [from any console] pip3 install nltk
    [from any console] python
    [from python interactive console] import nltk
    [from python interactive console] nltk.download()
    [from python interactive console] d dependency_treebank
    [from python interactive console] quit()
    [from any console] cd %HOMEDRIVE%\%HOMEPATH%\"Application Data"\nltk_data\corpora\dependency_treebank
    [from any console] cat wsj_*.dp | grep -v "^$" > nltk-ptb.dp

13. [from any console] git clone https://github.com/aymara/lima/

14. Download ninja from ninja-build.org and make it available with Path variable

15. Install "TensorFlow for lima" release from github.com/aymara/tensorflow/releases

16. Update build_lima_with_ninja.bat according to your paths. Following variables are to be verified and/or updated:
    - MSYSDIR
    - QTDIR
    - TFDIR
    - WINKITDIR
    - NINJADIR
    - BOOST_ROOT
    - NLTK_PTB_DP_DIR
    - LIMA_SRC

17. (Optionally) update LIMA_VERSION_RELEASE variable in build_lima_with_ninja.bat

17. compile lima
    [from vs2017 x64 native console inside LIMA_SRC directory] build_lima_with_ninja.bat
