# Linux packages

!!! warning "No up-to-date binary packages are published"
    The packages attached to the
    [GitHub releases](https://github.com/aymara/lima/releases) date from LIMA
    2.1 (2019, Ubuntu 16.04/18.04 and Debian 9) and are obsolete. To get a
    current LIMA, use the [Docker image](docker.md), or build the packages
    yourself as described below.

## Building Debian packages

The build script can produce `.deb` packages for the distribution it runs on.
Follow the [build instructions](source.md) and add `-p ON` to the `gbuild.sh`
command:

```bash
source ./setenv-lima.sh -m release
./gbuild.sh -m Release -p ON
```

The packages are copied to `$LIMA_DIST/share/apps/lima/packages/`. Their
dependencies are computed
automatically from the shared libraries LIMA links to, so `apt` installs
them for you:

```bash
sudo apt install ./lima-<version>-<distribution>.deb
```

The continuous integration builds LIMA on **Debian 12** and **Ubuntu 22.04**
(see the Dockerfiles in
[`continuous_integration/`](https://github.com/aymara/lima/tree/master/continuous_integration)),
so these are the best-tested distributions.

## Optional dependencies

Two optional dependencies are distributed by the LIMA project:

- [svmtool++](https://github.com/aymara/svmtool-cpp/releases/latest), for the
  SVM-based part-of-speech tagger of the legacy pipelines;
- [qhttpserver](https://github.com/aymara/qhttpserver/releases/latest), for the
  `limaserver` HTTP server.

## After installation

[Install language models](../usage/models.md) and analyze a text:

```bash
lima_models.py -l eng
analyzeText -l eng-UD_English-EWT -p deepud my-text.txt
```
