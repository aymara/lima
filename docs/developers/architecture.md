# Architecture

LIMA is written mostly in C++ (with Qt and Boost), with Python tooling and
bindings. It is a **plugin and factory** system: the analysis is assembled at
run time from configuration files, not hard-wired in the code.

## Subprojects

The repository is made of subprojects, built in this order (see the root
`CMakeLists.txt`):

| Subproject | Content |
| --- | --- |
| `lima_common` | Foundations: factory and plugin framework, configuration, common data types |
| `lima_linguisticprocessing` | The analysis engine, the process units, the clients and tools, and the configuration files (`conf/`) |
| `lima_linguisticdata` | Sources of the linguistic data (dictionaries, rules, disambiguation matrices) compiled into the runtime resources |
| `deeplima` | Standalone C++ deep learning library on libtorch: inference for tokenization, tagging, lemmatization and dependency parsing, and the training applications |
| `lima_pelf` | Evaluation and benchmarking tools |
| `lima_gui` | The Qt graphical interface |
| `extern` | Third-party dependencies (fastText submodule, libtorch) |

## Factories and plugins

Code: `lima_common/src/common/AbstractFactoryPattern/`.

Components register themselves in factories (`SimpleFactory`,
`RegistrableFactory`, `InitializableObjectFactory`) under a class identifier,
the one used in the `class` attribute of the configuration groups. Shared
libraries containing components are loaded at startup by the
`AmosePluginsManager` and the `DynamicLibrariesManager`, so new components can
be added without recompiling LIMA. See [Plugins and components](components.md).

## Process units and pipelines

Code: `lima_common/src/common/ProcessUnitFramework/`.

A **process unit** (`AbstractProcessUnit`, `MediaProcessUnit`) performs one
analysis step. A **pipeline** (`ProcessUnitPipeline`) is an ordered list of
process units. An `AnalysisContent` is passed along the pipeline as a
blackboard: each unit reads the `AnalysisData` it needs (e.g. `AnalysisGraph`,
`PosGraph`, `SyntacticData`) and adds its own.

Adding a processing step means implementing a process unit, registering its
factory, and referencing it in a pipeline of the configuration.

## Global data

Code: `lima_common/src/common/MediaticData/`.

`MediaticData` holds the global run-time data: the languages ("media"), the
entity types and the loaded resources.

## Processing modules

Code: `lima_linguisticprocessing/src/linguisticProcessing/core/`, one
directory per stage, e.g. `FlatTokenizer`, `MorphologicAnalysis`,
`PosTagger`, `SyntacticAnalysis`, `SpecificEntities`, `ConlluReader`,
`AnalysisDumpers`, `Automaton`, and `DeepLimaUnits` for the neural units built
on `deeplima`. They are documented in the
[process units reference](../reference/process-units/index.md).

## Clients and tools

- `lima_linguisticprocessing/src/linguisticProcessing/client/`: the analysis
  client API (`AbstractLinguisticProcessingClient`) and `lima.cpp`, the
  `LimaAnalyzer` used by the Python bindings.
- `lima_linguisticprocessing/test/`: `analyzeText` and `limaserver`.
- `lima_linguisticprocessing/tools/`: `compile-rules` (the automaton compiler),
  `tva` (the test-driver analyzer used in the unit tests) and other tools.

## Configuration drives behavior

Pipelines and module wiring are defined in XML, in
`lima_linguisticprocessing/conf/` (e.g. `lima-lp-ud.xml` defines the `deepud`,
`deepud-pretok` and `deeplima` pipelines; the `*-modex.xml` files define
rule-based modules). To change which steps run or in which order, edit the
configuration, not the C++. See [Configuration files](../reference/configuration-files.md).

## Linguistic data

ModEx rules and dictionaries are written as sources in
`lima_linguisticdata/` (`rules-idiom`, `analysisDictionary`,
`syntacticAnalysis`, `SpecificEntities`, `disambiguisationMatrices`) and
compiled during the build into the binary resources loaded at run time. See
[Linguistic resources](../reference/linguistic-resources.md) and
[ModEx rules format](../reference/modex-rules.md).
