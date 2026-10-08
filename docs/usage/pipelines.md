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

## Legacy pipelines (English and French)

The legacy configurations `lima-lp-eng.xml` and `lima-lp-fre.xml` (languages
`eng` and `fre`) use dictionaries, rules and statistical models. They are
useful for resource development and rule-based extraction.

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
