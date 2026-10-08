# Training neural models

The neural models used by LIMA are trained with the applications of the
in-tree `deeplima` library, on
[Universal Dependencies](https://universaldependencies.org/) treebanks and
[fastText](https://fasttext.cc/) word embeddings. Trained models are published
in [aymara/lima-models](https://github.com/aymara/lima-models).

## Training applications

Built and installed with LIMA (sources in `deeplima/apps/`):

| Application | Trains or produces |
| --- | --- |
| `deeplima-train-segm` | Tokenizer and sentence splitter |
| `deeplima-train-tag` | PoS tagger and morphological features (UPOS, XPOS, FEATS) |
| `deeplima-train-lemmatization` | Lemmatizer |
| `deeplima-gen-lemm-dict` | Lemma dictionary cache used alongside the lemmatizer |
| `deeplima-mwt-dict` | Multiword token dictionary |
| `deeplima-train-dp` | Labeled dependency parser |
| `deeplima` | Standalone analyzer running the trained models, outside LIMA's pipelines |

Run each application with `--help` for its options.
`deeplima/scripts/train_ud_tagging.sh` is an example of tagger training on a UD
treebank.

## Using a trained model in LIMA

Copy the model to the resources directory, named after the treebank
identifier used at analysis time:

| Model | Destination |
| --- | --- |
| Tokenizer | `RnnTokenizer/ud/tokenizer-<treebank>.pt` |
| Tagger | `RnnTagger/ud/tagger-<treebank>.pt` |
| Lemmatizer | `RnnLemmatizer/ud/lemmatizer-<treebank>.pt` (and optionally `.dic`) |
| Dependency parser | `RnnDependencyParser/ud/dependencyParser-<treebank>.pt` |

where `<treebank>` is e.g. `fra-UD_French-GSD`. Then analyze with
`analyzeText -l <treebank> -p deepud` (see [Language models](../usage/models.md)).
