# Using LIMA from Python

The `aymara.lima` module gives access to LIMA's main features with an API
inspired by [spaCy](https://spacy.io/). Install it with
[pip](../install/python.md).

--8<-- "pypi-release-note.md"

## Analyzing text

Create a `Lima` analyzer once (initialization loads the configuration and the
models, which takes some time) and call it on texts:

```python
import aymara.lima

nlp = aymara.lima.Lima("ud-eng", pipes="deepud",
                       meta={"udlang": "eng-UD_English-EWT"})
doc = nlp("The author wrote a novel. It was a success.")
```

With the 0.5.0b6 PyPI release, `aymara.lima.Lima("ud-eng")` is enough: that
version selects the English models by itself.

The constructor takes:

- `langs`: comma-separated list of languages to initialize (see
  [Language models](models.md#selecting-a-model-at-analysis-time));
- `pipes`: comma-separated list of pipelines to initialize (see
  [Pipelines](pipelines.md));
- `meta`: metadata, e.g. `{"udlang": "eng-UD_English-EWT"}` to select the
  treebank of the models;
- `user_config_path` and `user_resources_path`: directories searched before
  the configuration and resources of the package (see
  [Configuring LIMA](configuration.md)).

When several languages or pipelines are initialized, choose them at analysis
time with the `lang` and `pipeline` arguments, e.g.
`nlp(text, lang="eng", pipeline="main")`. Without them, the first initialized
language and pipeline are used. The models of the neural pipelines are loaded
when the analyzer is created, so create one analyzer per treebank.

## Documents, sentences and tokens

A `Doc` is a sequence of `Token`s. Sentences and named entities are `Span`s,
contiguous sequences of tokens.

```python
for token in doc:
    print(token.i, token.text, token.lemma, token.pos, token.dep, token.head)

for sentence in doc.sents:
    print(sentence)

for entity in doc.ents:
    print(entity.text, entity.label)
```

Printing a `Doc` with `repr()` gives its CoNLL-U representation:

```pycon
>>> print(repr(nlp("Hello, World!")))
1       Hello   hello   INTJ    _       _               0       root    _       Pos=0|Len=5
2       ,       ,       PUNCT   _       _               1       punct   _       Pos=5|Len=1
3       World   World   PROPN   _       Number:Sing     1       vocative        _       Pos=7|Len=5
4       !       !       PUNCT   _       _               1       punct   _       Pos=12|Len=1
```

`analyzeText()` returns the raw CoNLL-U string instead of a `Doc`:

```python
print(nlp.analyzeText("The author wrote a novel.", lang="ud-eng"))
```

## Customizing the configuration

`Lima.export_system_conf()` copies the configuration files of the package to a
directory, so that you can edit them and pass that directory as
`user_config_path`. `Lima.add_pipeline_unit()` adds a process unit to a
pipeline at run time.

The full API is described in the [Python API reference](../reference/python-api.md).

## The `lima` command

The package also installs a `lima` command analyzing files and writing CoNLL-U
on the standard output:

```bash
lima my-text.txt
```

Run `lima --help` for its options.

!!! note
    Some messages may be displayed on the console when the analyzer is
    created. If you get a valid result, you can ignore them.
