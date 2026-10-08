# Named entity recognition

LIMA includes predefined pipelines for named entity recognition (NER) in
English and French, available out of the box in the legacy `eng` and `fre`
configurations.

| Pipeline | Input | Output |
| --- | --- | --- |
| `ner-rules` | Plain text | CoNLL-03 |
| `ner-rules-pretok` | CoNLL-U | CoNLL-03 |

The output can be switched to CoNLL-U in the configuration of the
`conllDumperNer` process unit (in
[`lima-lp-eng.xml` and `lima-lp-fre.xml`](https://github.com/aymara/lima/tree/master/lima_linguisticprocessing/conf)).

Rule-based NER is implemented with [ModEx rules](../reference/modex-rules.md),
whose sources are in
[`lima_linguisticdata/SpecificEntities/<lang>`](https://github.com/aymara/lima/tree/master/lima_linguisticdata/SpecificEntities/).
You can add your own entity types by [writing a ModEx](writing-a-modex.md).

!!! info "Neural NER"
    LIMA previously also provided neural (`ner-deep`) and combined
    (`ner-fusion`) NER pipelines, based on TensorFlow. TensorFlow support has
    been removed; these pipeline names are kept for compatibility but
    currently run the rule-based recognizer only. A libtorch-based NER unit
    (`RnnNER`) exists but no models are published for it yet.

## Usage

```bash
analyzeText -l eng -p ner-rules input_file.txt
analyzeText -l fre -p ner-rules-pretok input_file.conllu
```

## Processing units

| | `ner-rules` | `ner-rules-pretok` |
| --- | :-: | :-: |
| **Input** | | |
| `flattokenizer` | ✓ | |
| `conllureader` | | ✓ |
| **Pre-processing** | | |
| `simpleWord` | ✓ | ✓ |
| `defaultProperties` | ✓ | ✓ |
| **Rule-based NER** | | |
| `SpecificEntitiesModex` | ✓ | ✓ |
| `sentenceBoundariesUpdater` | ✓ | ✓ |
| **Output** | | |
| `conllDumperNer` | ✓ | ✓ |

## Evaluation

The pipelines are evaluated on pre-tokenized input (`ner-rules-pretok`).

=== "English — CoNLL-03 (eng.testb)"

    ```text
    processed 46435 tokens with 5616 phrases; found: 4440 phrases; correct: 2984.
    accuracy:  92.45%; precision:  67.21%; recall:  53.13%; FB1:  59.35
                  LOC: precision:  64.31%; recall:  84.99%; FB1:  73.22  2202
                 MISC: precision:  90.32%; recall:   3.99%; FB1:   7.65  31
                  ORG: precision:  57.07%; recall:  20.10%; FB1:  29.73  580
                  PER: precision:  74.31%; recall:  75.47%; FB1:  74.88  1627
    ```

=== "English — WikiNER (aij-wikiner-en-wp2)"

    ```text
    processed 3499655 tokens with 296413 phrases; found: 211853 phrases; correct: 128203.
    accuracy:  92.12%; precision:  60.52%; recall:  43.25%; FB1:  50.45
                  LOC: precision:  64.89%; recall:  55.78%; FB1:  59.99  72577
                 MISC: precision:  57.84%; recall:   1.84%; FB1:   3.56  2144
                  ORG: precision:  41.70%; recall:  36.84%; FB1:  39.12  41009
                  PER: precision:  65.30%; recall:  64.00%; FB1:  64.64  96123
    ```

=== "French — WikiNER (aij-wikiner-fr-wp2)"

    ```text
    processed 3499679 tokens with 251726 phrases; found: 141976 phrases; correct: 102255.
    accuracy:  92.67%; precision:  72.02%; recall:  40.62%; FB1:  51.95
                  LOC: precision:  68.93%; recall:  45.60%; FB1:  54.89  74057
                 MISC: precision:  45.09%; recall:   4.74%; FB1:   8.57  4096
                  ORG: precision:  53.15%; recall:  33.15%; FB1:  40.84  15262
                  PER: precision:  84.94%; recall:  54.04%; FB1:  66.06  48561
    ```

The rule-based recognizer processes about 6,400 tokens per second on a single
thread.
