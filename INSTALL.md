# LIMA - Libre Multilingual Analyzer

LIMA is mutliplatform. It has been developed under GNU/Linux and ported MS
Windows. Its build procedure under Linux is described bellow.
Build instructions under Windows are still to be written but can be inferred
from the [Appveyor CI configuration file](https://github.com/aymara/lima/blob/master/appveyor.yml).

## Install

Build dependencies:
- Tools: cmake, ninja, C++ (tested with gcc and clang), gawk, NLTK,
- Libraries and development packages for : boost , Qt5 and Qwt.

Optional dependencies:
- python3:
- enchant: for orthographic correction;
- qhttpserver: lima http/json API;
- svmtool++: for SVM-based PoS tagger;
- TensorFlow, Eigen and Protobuf: for neural network-based modules (currently Named Entity Recognition and soon parsing too);
- tre: for approximate string matcher module;


Under Ubuntu, most of these dependencies are installed with the following packages:
```
$ sudo apt-get install python-nltk gawk cmake ninja-build qt5-default libqt5xmlpatterns5 \
libqt5xmlpatterns5-dev qttools5-dev build-essential libboost-all-dev libenchant-dev \
mesa-common-dev libgl1-mesa-dev libglu1-mesa-dev libasan0 qml-module-qt-labs-folderlistmodel \
libqwt-qt5-dev qtscript5-dev qtxmlpatterns5-dev-tools \
qml-module-qt-labs-settings qtdeclarative5-dev python3-dev libenchant-dev libtre-dev
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

For TensorFlow, we use a specially compiled version. It can be installed with our ppa in Ubuntu versions starting from 18.04:

```bash
$ sudo add-apt-repository ppa:limapublisher/ppa
$ sudo apt-get update
$ sudo apt install libtensorflow-for-lima-dev
```

Modified sources of TensorFlow are [here](https://github.com/aymara/tensorflow/tree/r1.9).

As we were not able to find a Free part of speech tagged English corpus, LIMA
depends for analyzing English on freely available but not Free data that you
will have to download and prepare yourself. This data is an extract of the Peen
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

:warning: **If you havn't already downloaded LIMA git repository (source code), please [do it now](https://github.com/aymara/lima.git).**

Move to the root of the LIMA  git repository and clone submodules, e.g.:
```bash
$ cd $HOME/lima
$ git submodule init
$ git submodule update
```

You need to set up a few environment variables. For this purpose, you can
source the setenv-lima.sh script from the root of **the LIMA git repository** (please check
values before):
```bash
$ source ./setenv-lima.sh -m release
```

Finally, from the LIMA repository root, run:
```bash
$ ./gbuild.sh -m Release
```

By default LIMA is built without neural network-based modules (i.e. without TensorFlow). To build LIMA with neural network-based modules use -T option:

```bash
$ ./gbuild.sh -m Release -T
```

This builds LIMA in release mode, assuring the best performance. To report bugs
for example, you should build LIMA in debug mode. To do so, just omit the
`-m Release` option when invoking `setenv-lima.sh` and `gbuild.sh`.

Alternatively, you can

1. define the following environment variables manually:

`LIMA_DIST`             binaries and libraries
`LIMA_EXTERNALS`        dependencies
`LIMA_RESOURCES`        any kind of ressources (including training data)
`LIMA_CONF`             configuration folder
`LINGUISTIC_DATA_ROOT`  path to the lima_linguisticdata project root
`NLTK_PTB_DP_FILE`      path to the Penn treebank extract from NLTK (see below)

2. set `PATH` and `LD_LIBRARY_PATH`:

```
export PATH=$LIMA_DIST/bin:$LIMA_EXTERNALS/bin:$PATH
export LD_LIBRARY_PATH=$LIMA_EXTERNALS/lib:$LIMA_DIST/lib
```

3. run `gbuild.sh`

## Build troubleshoutings

* If you use your own compiled boost libraries alongside system boost libraries
AND cmake fails on lima_linguisticprocessings indicating it found your boost
version headers but it uses the system libraries, add the following definition
at the beginning of the root CMakeLists.txt of each subproject :
set(Boost_NO_SYSTEM_PATHS ON)
* If some packages are not found at configure time (when running cmake), double check the dependencies packages you have installed. If it's OK, maybe we missed to indicate a dependency. Then, don't hesitate to open an issue. Or submit a merge request that solves the problem.

