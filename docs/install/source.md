# Build from source

LIMA is developed under GNU/Linux. The build is orchestrated by the
`gbuild.sh` script, which relies on environment variables defined by
`setenv-lima.sh`: it is **not** a plain `cmake && make`.

## Dependencies

**Build tools:** CMake, Ninja, a C++ compiler (GCC or Clang), gawk, Python 3
and NLTK.

**Libraries:** Boost, Qt 6, ICU, Eigen and libtorch (the PyTorch C++
distribution, downloaded by a script, see below).

**Optional:**

- tre: approximate string matching module;
- enchant: spelling correction;
- [qhttpserver](https://github.com/aymara/qhttpserver/releases): `limaserver`
  HTTP/JSON API;
- [svmtool++](https://github.com/aymara/svmtool-cpp/releases): SVM-based
  part-of-speech tagger for the legacy pipelines.

Under Ubuntu 22.04, most of them are installed with:

```bash
sudo apt-get update && sudo apt-get install -y \
    build-essential gcc g++ cmake ninja-build git curl wget unzip gawk \
    python3 python3-pip python-is-python3 python3-nltk python3-requests python3-tqdm \
    qt6-base-dev qt6-base-dev-tools qt6-tools-dev qt6-declarative-dev \
    qt6-declarative-dev-tools qt6-multimedia-dev libqt6concurrent6 qml6-module-qtqml \
    libboost-all-dev libicu-dev libeigen3-dev libtre-dev nodejs npm dos2unix
```

The Dockerfiles in
[`continuous_integration/`](https://github.com/aymara/lima/tree/master/continuous_integration)
are the up-to-date reference for Debian 12 and Ubuntu 22.04: check them if the
instructions on this page fail.

### English training data { #english-training-data }

As there is no Free part-of-speech tagged English corpus, the legacy English
pipeline uses an extract of the Penn treebank that is freely available for fair
use in the [NLTK data](https://www.nltk.org/data.html). Download it and prepare
it:

```bash
python3 -m nltk.downloader -d ~/nltk_data dependency_treebank
cat ~/nltk_data/corpora/dependency_treebank/wsj_*.dp | grep -v "^$" \
    > ~/nltk_data/corpora/dependency_treebank/nltk-ptb.dp
```

`setenv-lima.sh` points `NLTK_PTB_DP_FILE` to this file. It is only needed to
build the legacy English resources: the neural pipelines do not use it.

## Building

Clone the repository with its submodules and download libtorch:

```bash
git clone https://github.com/aymara/lima.git
cd lima
git submodule update --init
(cd extern && ./download_libtorch.sh)
```

Set up the environment, then build and install. The `-m` mode given to
`setenv-lima.sh` must match the one given to `gbuild.sh`:

```bash
source ./setenv-lima.sh -m release   # every new shell; check its values first
./gbuild.sh -m Release -d ON
```

This builds LIMA in release mode, with debug messages available (`-d ON`).
The neural modules are built whenever libtorch and Eigen are found.

`setenv-lima.sh` defines the following variables, where `LIMA_ROOT` is the
directory containing your clone:

| Variable | Default | Role |
| --- | --- | --- |
| `LIMA_BUILD_DIR` | `$LIMA_ROOT/Builds` | Out-of-tree build trees, per branch and mode |
| `LIMA_DIST` | `$LIMA_ROOT/Dist/<branch>/<mode>` | Install prefix; its `bin/` and `lib/` are added to `PATH` and `LD_LIBRARY_PATH` |
| `LIMA_CONF` | `$LIMA_DIST/share/config/lima` | Configuration files |
| `LIMA_RESOURCES` | `$LIMA_DIST/share/apps/lima/resources` | Compiled linguistic resources |

Useful `gbuild.sh` options (`./gbuild.sh -h` lists them all):

| Option | Meaning |
| --- | --- |
| `-m <mode>` | `Debug` (default), `Release` or `RelWithDebInfo` |
| `-d ON` | Keep debug messages in release mode |
| `-t ON` | Run the unit tests after the build |
| `-p ON` | Build packages |
| `-g OFF` | Do not build the Qt graphical interface |
| `-r build\|precompiled\|deeplima\|none` | Linguistic resources: build them all (default), use precompiled ones, build only those needed by the neural pipelines, or skip them |
| `-G <generator>` | CMake generator (Ninja by default) |
| `-j <n>` | Parallelism |

To report a bug, build in debug mode: omit `-m release` and `-m Release`.

## After the build

[Install language models](../usage/models.md) for at least one language:

```bash
lima_models.py -l eng-UD_English-EWT
analyzeText -l eng-UD_English-EWT -p deepud my-text.txt
```

To run the tests, see [Testing](../developers/testing.md).

## Troubleshooting

- If you use your own Boost build alongside the system one and CMake finds your
  headers but links the system libraries, add `set(Boost_NO_SYSTEM_PATHS ON)` at
  the beginning of the root `CMakeLists.txt` of each subproject.
- If some packages are not found at configure time, double-check the installed
  dependencies. If they are all there, we probably forgot to document one:
  please [open an issue](https://github.com/aymara/lima/issues) or a pull
  request.
