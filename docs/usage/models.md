# Language models

LIMA's neural pipelines need models, one set per
[Universal Dependencies](https://universaldependencies.org/) treebank. They are
not installed with LIMA: download the ones for the languages you need.

Models are published in the
[aymara/lima-models](https://github.com/aymara/lima-models) repository, for
more than 60 languages. Each archive contains, depending on the language:

| Model | Directory in the resources | Used by |
| --- | --- | --- |
| Tokenizer and sentence splitter | `RnnTokenizer/ud/tokenizer-<treebank>.pt` | `RnnTokenizer` |
| PoS tagger and morphological features | `RnnTagger/ud/tagger-<treebank>.pt` | `RnnTokensAnalyzer` |
| Lemmatizer (optional) | `RnnLemmatizer/ud/lemmatizer-<treebank>.pt` | `RnnTokensAnalyzer` |
| Dependency parser (optional) | `RnnDependencyParser/ud/dependencyParser-<treebank>.pt` | `RnnDependencyParser` |
| Word embeddings | `embd/*.ftz` | all of the above |

When an optional model is missing, the corresponding unit does nothing: the
LEMMA, HEAD or DEPREL columns of the output are left empty.

## Installing models with `lima_models.py`

LIMA (Docker image, packages or source build) provides the `lima_models.py`
command:

```text
usage: lima_models.py [-h] [-i] [-l LANG] [-d DEST] [-f] [-L]

  -i, --info            print the list of available languages/treebanks and exit
  -l LANG, --lang LANG  language code, language name or explicit treebank to install
                        (e.g. 'fra', 'french' or 'fra-UD_French-GSD')
  -d DEST, --dest DEST  destination directory
  -f, --force           force reinstallation of existing files
  -L, --list            list installed models
```

For example:

```bash
lima_models.py -i                     # list the available languages and treebanks
lima_models.py -l fra                 # install the best French treebank
lima_models.py -l fra-UD_French-GSD   # install a specific treebank
lima_models.py -L                     # list the installed models
```

There are usually several treebanks per language. Given a language code or
name, `lima_models.py` picks the treebank with the best scores; give an
explicit treebank name to override this choice. At the end of the
installation, it prints the exact command line to use.

Models are installed in `$XDG_DATA_HOME/lima/resources`, or in
`~/.local/share/lima/resources` if `XDG_DATA_HOME` is not defined, where LIMA
looks for them automatically. Use `-d` to install them elsewhere and add that
directory to `LIMA_RESOURCES` (see [Configuring LIMA](configuration.md)).

## Selecting a model at analysis time

The models are selected by the **treebank identifier**, the
`<language code>-<treebank>` stem of the installed files, e.g.
`eng-UD_English-EWT` or `fra-UD_French-GSD`. There are two equivalent ways to
pass it:

```bash
# Use the treebank identifier as the language
analyzeText -l fra-UD_French-GSD -p deepud my-text.txt

# Or use the generic "ud" language and give the treebank as metadata
analyzeText -l ud --meta udlang:fra-UD_French-GSD -p deepud my-text.txt
```

Each language is also registered as `ud-<code>` (`ud-eng`, `ud-fra`,
`ud-spa`…). With these names, the treebank must be given explicitly:

```bash
analyzeText -l ud-eng --meta udlang:eng-UD_English-EWT -p deepud my-text.txt
```

From Python, pass the same information when creating the analyzer:

```python
import aymara.lima
nlp = aymara.lima.Lima("ud", pipes="deepud",
                       meta={"udlang": "fra-UD_French-GSD"})
```

## Models of the Python package

--8<-- "pypi-release-note.md"

With the PyPI package, use:

- `lima_models.py -l <lang>` (or `lima_models -i <lang>`, depending on the
  version) to install the legacy models used by the `ud-eng`/`ud-fra`
  pipelines;
- `deeplima_models` to install the libtorch models from
  [Hugging Face](https://huggingface.co/aymaralima/deeplima): `deeplima_models -a`
  lists the available treebanks, `deeplima_models -i UD_English-EWT` installs
  one and `deeplima_models -l` lists the installed ones.

## Embeddings and memory

The taggers and parsers use [fastText](https://fasttext.cc/) word embeddings.
The original fastText files are about 7 GB per language; lima-models
distributes quantized versions (around 600 MB per language), which slightly
lower the analysis quality. Depending on the language, analysis may need
several GB of RAM.
