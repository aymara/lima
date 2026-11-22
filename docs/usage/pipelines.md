# Pipelines

A pipeline is an ordered list of [process units](../reference/process-units/index.md),
each performing one analysis step on the result of the previous ones.
Pipelines are defined in the
[configuration files](../reference/configuration-files.md) and selected with
the `-p` option of [`analyzeText`](cli.md) or the `pipes`/`pipeline`
arguments of the [Python API](python.md).

## Neural pipelines (all languages)

These pipelines are defined in `lima-lp-ud.xml` and work for every language
for which [models](models.md) are installed.

| Pipeline | Input | Steps | Output |
| --- | --- | --- | --- |
| `deepud` | Plain text | Tokenization and sentence splitting, PoS tagging, morphological features and lemmatization, dependency parsing | CoNLL-U |
| `deepud-pretok` | CoNLL-U | PoS tagging, morphological features and lemmatization, dependency parsing | CoNLL-U |
| `deeplima` | Plain text | Tokenization and sentence splitting, PoS tagging, morphological features and lemmatization | CoNLL-U |

They use the neural process units `RnnTokenizer`, `RnnTokensAnalyzer` and
`RnnDependencyParser` (see [Neural units](../reference/process-units/neural.md)).
Steps without a model for the selected treebank are skipped. See
[Universal Dependencies analysis](../guides/ud-analysis.md) for details.

## English and French: neural analysis with named entities

The `ud-eng` and `ud-fra` media (configurations `lima-lp-ud-eng.xml` and
`lima-lp-ud-fra.xml`) run the same neural units, then the rule-based
[named entity recognizer](../guides/ner.md) (ModEx) in their `deepud` pipeline.
The deeplima model is selected with the `udlang` metadata:

```bash
analyzeText -l ud-eng -p deepud --meta udlang:eng-UD_English-MERGE file.txt
analyzeText -l ud-fra -p deepud --meta udlang:fra-UD_French-MERGE fichier.txt
```

The entities are written in the `MISC` column of the CoNLL-U output
(`NE=B-Person.PERSON`, `NE=I-Person.PERSON`...). For `ud-fra`, `deepud-pretok`
also runs the parser and the named entity recognizer on CoNLL-U input.

## Legacy pipelines (deprecated)

!!! warning "Deprecated"
    The legacy media `eng`, `fre` and `por` (configurations
    `lima-lp-eng.xml`, `lima-lp-fre.xml` and `lima-lp-por.xml`) are
    deprecated and are not built by default. Use `ud-eng`, `ud-fra` or the
    generic `ud` media instead. To build them, configure LIMA with
    `-DWITH_LEGACY_PIPELINES=ON` (or `gbuild.sh -r legacy`, see
    [Building from source](../install/source.md)).

The legacy configurations use a rule-based tokenizer, morphological
dictionaries, a statistical part-of-speech tagger and a rule-based syntactic
analyzer.

| Pipeline | Role |
| --- | --- |
| `main` | Full legacy analysis: tokenization, morphological analysis, idioms, named entities, PoS tagging, rule-based syntactic analysis |
| `ner-rules`, `ner-rules-pretok` | Rule-based [named entity recognition](../guides/ner.md) on raw text or CoNLL-U input |
| `ner-deep`, `ner-fusion` (and `-pretok`) | Kept for compatibility; currently identical to `ner-rules` (see [NER](../guides/ner.md)) |

The [linguistic processing steps](../reference/processing-steps.md) page
describes what each step of the legacy analysis does.

## Defining your own pipeline

A pipeline is a `ProcessUnitPipeline` group in the `Processors` module of a
`lima-lp-<lang>.xml` file:

```xml
<group name="my-pipeline" class="ProcessUnitPipeline">
  <list name="processUnitSequence">
    <item value="RnnTokenizer"/>
    <item value="RnnTokensAnalyzer"/>
    <item value="conllDumper"/>
  </list>
</group>
```

Copy the configuration file to your own configuration directory before editing
it (see [Configuring LIMA](configuration.md)). Dependencies between process
units are not checked automatically: removing a unit that a later one needs
can make LIMA fail.
