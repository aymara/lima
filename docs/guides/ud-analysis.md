# Universal Dependencies analysis

LIMA's neural pipelines analyze text following the
[Universal Dependencies](https://universaldependencies.org/) (UD) guidelines,
with models trained on UD treebanks for more than 60 languages. They are built
on the in-tree `deeplima` library and libtorch.

| Pipeline | Description | Input | Output |
| --- | --- | --- | --- |
| `deepud` | Full analysis, including tokenization and sentence splitting | Plain text | CoNLL-U |
| `deepud-pretok` | Analysis of already tokenized text | CoNLL-U | CoNLL-U |
| `deeplima` | Like `deepud`, without dependency parsing | Plain text | CoNLL-U |

## Installation

Install the [models](../usage/models.md) of the treebanks you need:

```bash
lima_models.py -a                    # available languages and models
lima_models.py -i fra                # the default French model (fra-UD_French-MERGE)
lima_models.py -i fra-UD_French-ALTS # or a specific one (Middle French)
```

## Usage

The examples below use `fra-UD_French-MERGE`; use the model identifier
printed by `lima_models.py` for the one you installed.

```bash
# Raw text
analyzeText -l fra-UD_French-MERGE -p deepud my-text.txt

# Pre-tokenized text
analyzeText -l fra-UD_French-MERGE -p deepud-pretok my-corpus.conllu
```

See [Language models](../usage/models.md#selecting-a-model-at-analysis-time)
for the other ways to select the treebank.

## Processing units

| | `deepud` | `deepud-pretok` | `deeplima` |
| --- | :-: | :-: | :-: |
| **Input** | | | |
| `RnnTokenizer`: tokenization and sentence splitting | ✓ | | ✓ |
| `conllureader`: reads CoNLL-U | | ✓ | |
| **Analysis** | | | |
| `RnnTokensAnalyzer`: UPOS, features, lemmas | ✓ | ✓ | ✓ |
| `RnnDependencyParser`: heads and relations | ✓ | ✓ | |
| **Output** | | | |
| `conllDumper` | ✓ | ✓ | ✓ |

The up-to-date definitions of these pipelines are in
[`lima_linguisticprocessing/conf/lima-lp-ud.xml`](https://github.com/aymara/lima/blob/master/lima_linguisticprocessing/conf/lima-lp-ud.xml).
The units are described in the [neural units reference](../reference/process-units/neural.md).

Each unit uses the model of the selected treebank. Lemmatizer and dependency
parser models are not available for every treebank yet: when one is missing,
the corresponding columns are left empty and the rest of the analysis runs
normally.

## Performance

The scores of the models for every treebank are reported in the
[models card on Hugging Face](https://huggingface.co/aymaralima/deeplima).

## Limitations

- **Memory:** depending on the language model and the size of its word
  embeddings, analysis can require several GB of RAM.
- **XPOS:** the XPOS column and the `Typo` and `Abbr` features are not
  produced.
- **Quantized embeddings:** the distributed fastText embeddings are
  quantized to reduce their size from about 7 GB to about 600 MB per
  language, which slightly lowers accuracy.
