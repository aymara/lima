# Neural units

The neural process units are built on the in-tree `deeplima` library and
libtorch. They are used by the `deepud`, `deepud-pretok` and `deeplima`
[pipelines](../../usage/pipelines.md) and defined in
[`lima-lp-ud.xml`](https://github.com/aymara/lima/blob/master/lima_linguisticprocessing/conf/lima-lp-ud.xml).

Their model file names are built from a prefix in which `$udlang` is replaced
by the treebank identifier selected at analysis time (e.g.
`eng-UD_English-EWT`, see [Language models](../../usage/models.md)). Models
are searched in the `ud` subdirectory of the unit's directory in the
resources, e.g. `RnnTokenizer/ud/tokenizer-eng-UD_English-EWT.pt`.

All these units accept a `data` parameter naming the sentence boundaries data
they use or produce (default: `SentenceBoundaries`).

## RnnTokenizer

**Class:** RnnTokenizer

**Role:** splits the text into tokens and sentences with a neural model. It is
the first unit of the `deepud` and `deeplima` pipelines.

**Inputs:** an AnalysisContent containing the initial text.

| Parameter | Description |
| --- | --- |
| `model_prefix` | Model file name without extension, in `RnnTokenizer/ud/`. Default configuration: `tokenizer-$udlang` |

**Effects:** creates the `AnalysisGraph` (one vertex per token) and the
sentence boundaries.

## ConlluReader

**Class:** ConlluReader

**Role:** reads already tokenized text in CoNLL-U format instead of tokenizing
raw text. It is the first unit of the `deepud-pretok` pipeline.

| Parameter | Description |
| --- | --- |
| `boundaryMicro` | Category used for sentence boundaries (`SENT` in the default configuration) |

**Effects:** creates the `AnalysisGraph` and the sentence boundaries from the
CoNLL-U tokens and sentences.

## RnnTokensAnalyzer

**Class:** RnnTokensAnalyzer

**Role:** assigns to each token its universal part of speech and
morphological features and, when a lemmatizer model is available, its lemma.

**Preconditions:** the `AnalysisGraph` and the sentence boundaries exist.

| Parameter | Description |
| --- | --- |
| `tagger_model_prefix` | Tagger model name in `RnnTagger/ud/`. Default configuration: `tagger-$udlang` |
| `lemmatizer_model_prefix` | Lemmatizer model name in `RnnLemmatizer/ud/`. Default configuration: `lemmatizer-$udlang`. An optional `.dic` file with the same name is used as a lemma cache |

**Effects:** creates the `PosGraph` and the annotation data; makes the
tagging results available to the dependency parser.

If no lemmatizer model is installed for the treebank, the lemmas are left
empty.

## RnnDependencyParser

**Class:** RnnDependencyParser

**Role:** computes the labeled dependency tree of each sentence (heads and
relations), using the features predicted by the tagger.

**Preconditions:** `RnnTokensAnalyzer` has been run.

| Parameter | Description |
| --- | --- |
| `dependency_parser_model_prefix` | Parser model name in `RnnDependencyParser/ud/`. Default configuration: `dependencyParser-$udlang` |
| `tagger_model_prefix` | Name of the tagger model whose outputs feed the parser. Default configuration: `tagger-$udlang` |

**Effects:** creates the `SyntacticData` holding the dependency relations,
written in the HEAD and DEPREL columns by `conllDumper`.

If no parser model is installed for the treebank, the unit does nothing.

## RnnNER

**Class:** RnnNER

**Role:** neural named entity recognition.

| Parameter | Description |
| --- | --- |
| `model_prefix` | Model name. Default configuration: `ner-$udlang` |

!!! note
    This unit is defined in the configuration but not used by the default
    pipelines: no NER models are published yet. See
    [Named entity recognition](../../guides/ner.md).
