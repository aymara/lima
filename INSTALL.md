# LIMA - Libre Multilingual Analyzer

LIMA is mutliplatform. It has been developed under GNU/Linux and ported to MS
Windows. Its build procedure under Linux is described bellow.
Build instructions under Windows are still to be written but can be inferred
from the [Appveyor CI configuration file](https://github.com/aymara/lima/blob/master/appveyor.yml). 
This CI configuration fils contains also pointers to (hopefuly) up to date instructions 
to build under latest stable Debian and Ubuntu. Check them if instructions below fail.

LIMA has been occasionally built on MacOS but there is no standard procedure to
do so.

## Install

Build dependencies:
- Tools: cmake, ninja, C++ (tested with gcc and clang), gawk, NLTK,
- Libraries and development packages for : boost , Qt5 and Qwt.

Optional dependencies:
- python3:
- enchant: for orthographic correction;
- qhttpserver: lima http/json API;
- svmtool++: for SVM-based PoS tagger;
- TensorFlow, Eigen and Protobuf: for neural network-based modules (currently
Named Entity Recognition and soon parsing too);
- tre: for approximate string matcher module;


Under Ubuntu, most of these dependencies are installed with the following packages:
```

$ sudo apt-get clean && sudo apt-get update && sudo apt-get install -y locales unzip bash \
coreutils apt-utils lsb-release git gcc g++ build-essential make cmake cmake-data \
curl python3-nltk gawk wget python3 python3-pip ninja-build qt5-default qtbase5-dev-tools \
libqt5xmlpatterns5-dev libqt5qml5 qtdeclarative5-dev qml-module-qtquick-extras \
qml-module-qtquick-controls qml-module-qtquick-layouts qml-module-qtquick2 libenchant-dev \
libtre-dev libboost-all-dev nodejs npm qml-module-qtquick-controls2 qml-module-qtquick-dialogs \
qml-module-qtquick-privatewidgets qml-module-qtquick-scene3d qml-module-qtquick-templates2 \
qml-module-qtquick-virtualkeyboard qml-module-qtquick-window2 qml-module-qtquick-xmllistmodel \
libicu-dev libeigen3-dev -qq
```

qhttpserver can be downloaded and installed from
https://github.com/aymara/qhttpserver/releases

svmtool++ can be downloaded and installed from https://github.com/aymara/svmtool-cpp/releases

To compile SVMTool models, you also need svm_light:
```bash
$ mkdir svm_light && cd svm_light
$ wget http://osmot.cs.cornell.edu/svm_light/current/svm_light.tar.gz
$ tar xvzf svm_light.tar.gz
$ make
$ sudo cp svm_classify svm_learn /usr/bin
```

For TensorFlow, we use a specially compiled version. It can be installed with
our ppa in Ubuntu versions starting from 18.04:

```bash
$ sudo add-apt-repository ppa:limapublisher/ppa
$ sudo apt-get update
$ sudo apt install libtensorflow-for-lima-dev
```

Modified sources of TensorFlow are [here](https://github.com/aymara/tensorflow/tree/r1.9).

As we were not able to find a Free part of speech tagged English corpus, LIMA
depends for analyzing English on freely available but not Free data that you
will have to download and prepare yourself. This data is an extract of the Penn
treebank corpus available for fair use in the NLTK data. To install, please
refer to http://nltk.org/data.html. Under Ubuntu this can be  done like that:

```bash
$ sudo apt-get  install python-nltk
$ python
>>> import nltk
>>> nltk.download()
d dependency_treebank
```

Then prepare the data for use with LIMA by running the following commands:

```bash
$ cd $HOME/nltk_data/corpora/dependency_treebank
$ cat wsj_*.dp | grep -v "^$" > nltk-ptb.dp
```

:warning: **If you havn't already downloaded LIMA git repository (source code),
please [do it now](https://github.com/aymara/lima.git)**:
```bash
$ cd $HOME
$ git clone https://github.com/aymara/lima.git
```


Move to the root of the LIMA  git repository and clone submodules, e.g.:
```bash
$ cd $HOME/lima
$ git submodule init
$ git submodule update
```

You need to set up a few environment variables. For this purpose, you can
source the setenv-lima.sh script from the root of **the LIMA git repository**
(please check
values before):
```bash
$ source ./setenv-lima.sh -m release
```

Finally, from the LIMA repository root, run:
```bash
$ ./gbuild.sh -m Release -d ON
```

By default LIMA is built with neural network-based modules (i.e. with
TensorFlow). To build LIMA without neural network-based modules use -T option:

```bash
$ ./gbuild.sh -m Release -T
```

This builds LIMA in release mode, assuring the best performance. To report bugs
for example, you should build LIMA in debug mode. To do so, just omit the
`-m Release` option when invoking `setenv-lima.sh` and `gbuild.sh`. You can also use
the `-h` option of `gbuild.sh` to see the other possibilities (deactivate packages 
generation, optimize for your computer, etc.)

After the installation of LIMA, if you have built the neural network-based
modules (the default, see above), you must
[install the models](https://github.com/aymara/lima/wiki/UD-pipelines#installation-of-language-models)
for one of the 60+ supported languages.

## Build troubleshoutings

* If you use your own compiled boost libraries alongside system boost libraries
AND cmake fails on lima_linguisticprocessings indicating it found your boost
version headers but it uses the system libraries, add the following definition
at the beginning of the root CMakeLists.txt of each subproject :
set(Boost_NO_SYSTEM_PATHS ON)
* If some packages are not found at configure time (when running cmake), double
check the dependencies packages you have installed. If it's OK, maybe we missed
to indicate a dependency. Then, don't hesitate to open an issue. Or submit a
merge request that solves the problem.

