# Language models

LIMA's neural pipelines need models, trained on
[Universal Dependencies](https://universaldependencies.org/) treebanks. They are
not installed with LIMA: download the ones for the languages you need.

Models are published on the Hugging Face Hub, in the
[aymaralima/deeplima](https://huggingface.co/aymaralima/deeplima) repository,
one tag per release. Its model card lists the languages, the treebanks each
model was trained on, their licenses and the evaluation scores;
[Accuracy and speed](performance.md) compares them with other toolkits. Each
model contains, depending on the language:

| Model | Directory in the resources | Used by |
| --- | --- | --- |
| Tokenizer, sentence splitter, multiword-token dictionary (+ seq2seq fallback) and segmenter word list | `RnnTokenizer/ud/tokenizer-<model>.pt[.mwt[.s2s]][.lex]` | `RnnTokenizer` |
| PoS tagger and morphological features | `RnnTagger/ud/tagger-<model>.pt` | `RnnTokensAnalyzer` |
| Lemmatizer (optional) | `RnnLemmatizer/ud/lemmatizer-<model>.{pt,dic,rules}` | `RnnTokensAnalyzer` |
| Dependency parser (optional) | `RnnDependencyParser/ud/dependencyParser-<model>.pt` | `RnnDependencyParser` |
| Word embeddings | `embd/*.ftz` | the tagger and the parser |

When an optional model is missing, the corresponding unit does nothing: the
LEMMA, HEAD or DEPREL columns of the output are left empty.

Most languages have a **default model**, trained on all the treebanks of the
language (`<code>-UD_<Language>-MERGE`, e.g. `fra-UD_French-MERGE`), the one
to use for general text. Some languages also have **treebank models**, whose
tagger and parser (sometimes tokenizer) were trained on one treebank only,
because they analyze better the text of that treebank (e.g.
`fra-UD_French-ALTS`, for Middle French): use them only for such text. And
most models have a **more accurate variant**, slower.

## Installing models with `lima_models`

LIMA (Docker image, packages or source build) provides `lima_models.py`, and
the Python package the same command as `lima_models`:

```bash
lima_models -a                     # list the available languages and models
lima_models -i fra                 # install the default French model
lima_models -i french              # the same, by language name
lima_models -i fra-UD_French-ALTS  # install a given model
lima_models -i fra --accurate      # the more accurate variant
lima_models -L                     # list the installed models
lima_models -r fra                 # remove the French models (-f: without asking)
```

`lima_models.py -l fra` also installs (and `-l` alone lists the installed
models), as in former versions. Models are installed in
`$XDG_DATA_HOME/lima/resources`, or in `~/.local/share/lima/resources` if
`XDG_DATA_HOME` is not defined, where LIMA looks for them automatically. Use
`-d` to install them elsewhere and add that directory to `LIMA_RESOURCES` (see
[Configuring LIMA](configuration.md)). Downloads are checked against the
checksums of the release, and the embeddings, shared by the models of a
language, are downloaded once.

Each version of LIMA names the models release it was tested with: `v0.3.0`
for LIMA 3.0.0 and aymara 0.5.0 to 0.7.1. Set
`LIMA_MODELS_REVISION` to install another release, and `LIMA_MODELS_URL` to
install from a mirror, or from a local directory holding a release (its
`models.json` and files).

## Selecting a model at analysis time

The models are selected by their **identifier**, the
`<language code>-<corpus>` stem of the installed files, e.g.
`eng-UD_English-MERGE` or `fra-UD_French-ALTS`. There are two equivalent ways
to pass it:

```bash
# Use the model identifier as the language
analyzeText -l fra-UD_French-MERGE -p deepud my-text.txt

# Or use the generic "ud" language and give the model as metadata
analyzeText -l ud --meta udlang:fra-UD_French-MERGE -p deepud my-text.txt
```

Each language is also registered as `ud-<code>` (`ud-eng`, `ud-fra`,
`ud-spa`…). With these names, the model must be given explicitly:

```bash
analyzeText -l ud-eng --meta udlang:eng-UD_English-MERGE -p deepud my-text.txt
```

From Python, pass the same information when creating the analyzer, or give
only the language: the default model installed by `lima_models` for it is
then used:

```python
import aymara.lima
nlp = aymara.lima.Lima("ud", pipes="deepud",
                       meta={"udlang": "fra-UD_French-MERGE"})
nlp = aymara.lima.Lima("ud-fra", pipes="deepud")   # after lima_models -i fra
```

## Models of the Python package

The Python package installs the same models with the same command,
`lima_models` (`deeplima_models` is kept as a synonym).

## Embeddings and memory

The taggers and parsers use [fastText](https://fasttext.cc/) word embeddings,
trained on the [C4](https://huggingface.co/datasets/allenai/c4) corpus. The
original fastText files are several GB per language; the models use quantized
versions (from about 100 MB to 1 GB per language), which slightly lower the
analysis quality. Depending on the language, analysis may need several GB of
RAM.
