# Process units

The core of LIMA is the execution of process units in a
[pipeline](../../usage/pipelines.md). This section is the technical
documentation of the process units of the standard LIMA distribution. Thanks to
its [plugin mechanism](../../developers/components.md), LIMA can be extended
with new units, which have their own documentation.

For each process unit, we describe:

- **class**: the identifier used to instantiate the corresponding C++ class
  (the `class` attribute of its configuration group);
- **role**: what the unit does;
- **inputs**: the state of the LIMA data structures needed to run the unit, and
  the parameters that change its behavior;
- **outputs**: the data written to files or to the standard output;
- **preconditions**: the state the data structures must have reached before
  running the unit;
- **effects**: the changes made to the LIMA data structures by the unit.

| Family | Units |
| --- | --- |
| [Neural units](neural.md) | RnnTokenizer, ConlluReader, RnnTokensAnalyzer, RnnDependencyParser, RnnNER |
| [Tokenization](tokenization.md) | FlatTokenizer, SentenceBoundariesFinder |
| [Morphology](morphology.md) | SimpleWord, HyphenWordAlternatives, EnchantSpellingAlternatives, RegexMatcher, DefaultProperties, SimpleDefaultProperties |
| [Entities and rules](entities.md) | ApplyRecognizer, GeoEntitiesTagger |
| [PoS tagging](pos-tagging.md) | ViterbiPosTagger, SvmToolPosTagger, DynamicSvmToolPosTagger |
| [Syntactic analysis](syntax.md) | SyntacticAnalyzerChains, SyntacticAnalyzerDeps and related units |
| [Semantics](semantics.md) | CoreferencesSolving, WordSenseDisambiguation |
| [Loggers and debugging](loggers.md) | StatusLogger, XML loggers, graph writers… |
| [Dumpers](dumpers.md) | ConllDumper, BowDumper, TextDumper, XML dumpers… |

Run `analyzeText --availableUnits` to list the units known to your
installation.
