# Named entity recognition

LIMA recognizes named entities (persons, locations, organizations, dates,
times, numbers and amounts, events, products) with
[ModEx rules](../reference/modex-rules.md). In English and French, the
`deepud` pipeline of the `ud-eng` and `ud-fra` media runs them after the
neural tokenization, tagging, lemmatization and parsing:

```bash
analyzeText -l ud-eng -p deepud --meta udlang:eng-UD_English-MERGE input_file.txt
analyzeText -l ud-fra -p deepud --meta udlang:fra-UD_French-MERGE input_file.txt
```

The generic `ud` media, used when the language is given as a model name
(`-l fra-UD_French-MERGE`), does not run them: use `-l ud-eng` or `-l ud-fra`
with the `udlang` metadata.

The entities are written in the `MISC` column of the CoNLL-U output, with
their normalized values:

```text
7	12	12	NUM	...	NE=B-DateTime.DATE|NE-value=2021-01-12|...
8	janvier	janvier	NOUN	...	NE=I-DateTime.DATE|NE-value=2021-01-12|...
```

The rule sources are in
[`lima_linguisticdata/SpecificEntities/<media>`](https://github.com/aymara/lima/tree/master/lima_linguisticdata/SpecificEntities/)
(`ud-eng`, `ud-fra`). Rules matching a lemma (for instance `jeu$NOUN`) need a
lemmatizer model for the language. You can add your own entity types by
[writing a ModEx](writing-a-modex.md).

!!! info "Neural NER"
    LIMA previously also provided neural (`ner-deep`) and combined
    (`ner-fusion`) NER pipelines, based on TensorFlow. TensorFlow support has
    been removed; these pipeline names are kept for compatibility but
    currently run the rule-based recognizer only. A libtorch-based NER unit
    (`RnnNER`) exists but no models are published for it yet.

## Legacy NER pipelines (deprecated)

!!! warning "Deprecated"
    The `ner-rules` pipelines below belong to the legacy `eng` and `fre`
    media, which are deprecated and only built with
    `-DWITH_LEGACY_PIPELINES=ON` (see [Pipelines](../usage/pipelines.md)).
    The evaluation figures of this page were obtained with them.

| Pipeline | Input | Output |
| --- | --- | --- |
| `ner-rules` | Plain text | CoNLL-03 |
| `ner-rules-pretok` | CoNLL-U | CoNLL-03 |

The output can be switched to CoNLL-U in the configuration of the
`conllDumperNer` process unit (in
[`lima-lp-eng.xml` and `lima-lp-fre.xml`](https://github.com/aymara/lima/tree/master/lima_linguisticprocessing/conf)).

```bash
analyzeText -l eng -p ner-rules input_file.txt
analyzeText -l fre -p ner-rules-pretok input_file.conllu
```

### Processing units

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

### Evaluation

The legacy pipelines are evaluated on pre-tokenized input (`ner-rules-pretok`).

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
