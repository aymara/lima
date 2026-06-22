# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What LIMA is

LIMA (Libre Multilingual Analyzer) is a multilingual NLP analyzer from CEA LIST,
written mostly in C++ with Python tooling/bindings. It supports 60+ languages
through two complementary mechanisms:

- **Neural (deep-learning) modules** — tokenization, morphosyntax/PoS, lemmatization,
  NER, dependency parsing. Built on libtorch (the PyTorch C++ distribution) via the
  in-tree `deeplima` library. (LIMA previously also had TensorFlow-based modules;
  those have been removed — see `docs/tf-removal-plan.md`.)
- **ModEx (rule-based) modules** — finite-state automaton rules for extracting
  entities/relations/events in domains without annotated data.

Output is typically CoNLL-U. The default modern pipelines are named `deepud`
(raw text) and `deepud-pretok` (pre-tokenized CoNLL-U input).

## Build

The build is **not** a plain `cmake`/`make`. It is orchestrated by `gbuild.sh`,
which requires environment variables set by sourcing `setenv-lima.sh` first.

```bash
# One-time setup (from repo root)
git submodule update --init          # pulls extern/fastText
cd extern && ./download_libtorch.sh && cd ..

# Every session: source the env script BEFORE building. -m must match gbuild -m.
source ./setenv-lima.sh -m release   # or omit -m for debug

# Build + install (Release, with debug messages).
./gbuild.sh -m Release -d ON

# Run unit tests after install:
./gbuild.sh -m Release -t ON
```

The neural modules are built whenever libtorch and Eigen are found (libtorch comes
from `extern/download_libtorch.sh`).

`setenv-lima.sh` defines the variables everything depends on:
- `LIMA_BUILD_DIR` (`$LIMA_ROOT/Builds`) — out-of-tree build trees, per branch/mode.
- `LIMA_DIST` (`$LIMA_ROOT/Dist/<branch>/<mode>`) — install prefix; `bin/` and `lib/`
  are added to `PATH`/`LD_LIBRARY_PATH`.
- `LIMA_CONF`, `LIMA_RESOURCES` — runtime config and compiled linguistic resources
  (overridable at runtime).

Key `gbuild.sh` options (`-h` for all): `-m` mode (Debug/Release/RelWithDebInfo),
`-t ON` run tests, `-p ON` build packages, `-d ON` debug messages in release,
`-g OFF` skip the Qt GUI, `-r precompiled|build|none` for linguistic resources,
`-G` cmake generator (default Ninja), `-j N` parallelism.

After building you must install language models before analysis, e.g.
`lima_models.py -l fra` (auto-picks the best treebank). Then analyze with
`analyzeText -l <treebank> -p deeplima <file>` (e.g. `-l fra-UD_French-GSD`).

## Tests

Tests run through CTest (Ninja generator). After a build:

```bash
cd "$LIMA_BUILD_DIR/<branch>/<mode>-OFF/lima"   # the build tree for the project
ctest --output-on-failure                       # all tests
ctest -R <regex>                                 # single test / subset by name
ctest --rerun-failed --output-on-failure        # re-run failures (what gbuild does)
```

`./gbuild.sh -t ON` runs the full suite at the end of a build and, on failure,
re-runs failed tests with output.

## Python tooling

Python scripts (data prep, model management, former Perl scripts being converted)
are formatted/linted with black, isort, and flake8 (all `line-length = 88`,
config in `pyproject.toml` and `.flake8`). A `.pre-commit-config.yaml` runs
flake8 + isort.

The installable Python package is `aymara` (`import aymara.lima`), distributed via
PyPI for Linux x86_64; `pyproject.toml` here covers formatting config, not the
binding build.

## Architecture

LIMA is a **plugin + factory** system. Processing is assembled at runtime from
configuration, not hard-wired.

### Layers (top-level subprojects, built in this order — see root `CMakeLists.txt`)
- `lima_common` — foundation: factory/plugin framework, configuration, data types.
- `lima_linguisticprocessing` — the analysis engine, process units, and pipelines.
- `lima_linguisticdata` — source linguistic data (dictionaries, rules, matrices)
  that gets *compiled* into the runtime resources under `LIMA_RESOURCES`.
- `deeplima` — standalone C++ deep-learning library (training apps + inference libs)
  for tokenization, tagging, lemmatization, dependency parsing.
- `lima_pelf` — evaluation/benchmarking tools (PoS eval, resource tooling).
- `lima_gui` — Qt GUI. `lima_annoqt` — annotation tool (currently disabled in build).
- `extern` — third-party deps (fastText submodule, libtorch).

### Core concepts (where to look)
- **Factory / plugin framework**: `lima_common/src/common/AbstractFactoryPattern/`.
  Modules self-register via factories (`SimpleFactory`, `RegistrableFactory`,
  `InitializableObjectFactory`); `DynamicLibrariesManager`/`AmosePluginsManager`
  load shared-library plugins at startup. Adding a new processing component means
  implementing a process unit and registering its factory.
- **Process units & pipelines**: `lima_common/src/common/ProcessUnitFramework/`
  (`AbstractProcessUnit`, `ProcessUnitPipeline`, `AnalysisContent`). A pipeline is
  an ordered list of process units; `AnalysisContent` is the blackboard passed
  through them.
- **MediaticData**: `lima_common/src/common/MediaticData/` — global runtime state
  for languages ("media"), entity types, and loaded resources.
- **Processing modules**: `lima_linguisticprocessing/src/linguisticProcessing/core/`
  — one directory per stage, e.g. `FlatTokenizer`, `MorphologicAnalysis`,
  `PosTagger`, `SyntacticAnalysis`, `SpecificEntities`, `DeepLimaUnits` (libtorch
  neural units), `ConlluReader`, `AnalysisDumpers`, `Modex`, `Automaton`.
- **Client/entry**: `lima_linguisticprocessing/src/linguisticProcessing/client/`
  (`lima.cpp` is the main `lima` CLI). `tools/tva/tva.cpp` is the test-driver
  analyzer used in unit tests.

### Configuration drives behavior
Pipelines and module wiring live in XML, not code:
`lima_linguisticprocessing/conf/` (e.g. `lima-lp-ud.xml` defines the `deepud`,
`deepud-pretok`, `deeplima` pipelines as ordered `processUnitSequence` lists;
`*-modex.xml` files define rule-based modules). To change which steps run or in
what order, edit the pipeline config, not the C++.

### ModEx rules and linguistic data
ModEx rules and dictionaries are authored as source in `lima_linguisticdata/`
(`rules-idiom`, `analysisDictionary`, `syntacticAnalysis`, `SpecificEntities`,
`disambiguisationMatrices`) and compiled at build time (the `-r` option) into the
binary resources LIMA loads at runtime. The automaton compiler lives in
`lima_linguisticprocessing/tools/automatonCompiler/`.
