# Using LIMA from Python

The `aymara.lima` module gives access to LIMA's main features with an API
inspired by [spaCy](https://spacy.io/). Install it with
[pip](../install/python.md).

## Analyzing text

Create a `Lima` analyzer once (initialization loads the configuration and the
models, which takes some time) and call it on texts. Reuse it: an analyzer with
a neural pipeline uses about 3 GB of memory.

!!! note "Memory of deleted analyzers"
    Up to aymara 0.7.1, this memory is not released when the analyzer is
    deleted, so creating several in one process quickly exhausts the memory.
    Since aymara 0.8.0, deleting the last analyzer of a language frees its
    models, and a new analyzer of that language replaces them instead of adding
    new ones. The memory goes back to the C library allocator: the process size
    may not shrink until it is reused (or `malloc_trim` is called).

```python
import aymara.lima

nlp = aymara.lima.Lima("ud-eng", pipes="deepud",
                       meta={"udlang": "eng-UD_English-MERGE"})
doc = nlp("The author wrote a novel. It was a success.")
```

Without `udlang`, `aymara.lima.Lima("ud-eng")` uses the default model that
`lima_models` installed for the language (e.g. after `lima_models -i eng`).

The constructor takes:

- `langs`: comma-separated list of languages to initialize (see
  [Language models](models.md#selecting-a-model-at-analysis-time));
- `pipes`: comma-separated list of pipelines to initialize (see
  [Pipelines](pipelines.md));
- `meta`: metadata, e.g. `{"udlang": "eng-UD_English-MERGE"}` to select the
  treebank of the models;
- `user_config_path` and `user_resources_path`: directories searched before
  the configuration and resources of the package (see
  [Configuring LIMA](configuration.md)).

When several languages or pipelines are initialized, choose them at analysis
time with the `lang` and `pipeline` arguments, e.g.
`nlp(text, lang="ud-eng", pipeline="deepud-pretok")`. Without them, the first
initialized language and pipeline are used. The models of the neural pipelines
are loaded when the analyzer is created, so create one analyzer per treebank.

The `deepud` pipeline of `ud-eng` and `ud-fra` also recognizes named entities,
available as `doc.ents`. The deprecated legacy language codes `eng`, `fre` and
`por` and pipelines (`main`...) select `ud-eng`, `ud-fra`, `ud-por` and
`deepud`, with a `DeprecationWarning`.

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
1       Hello   hello   INTJ    _       _               3       discourse       _       Pos=0|Len=5
2       ,       ,       PUNCT   _       _               1       punct   _       Pos=5|Len=1
3       World   World   PROPN   _       Number:Sing     0       root    _       Pos=7|Len=5
4       !       !       PUNCT   _       _               3       punct   _       Pos=12|Len=1
```

!!! warning "aymara 0.5.0: root heads in `repr()`"
    In aymara 0.5.0, `repr()` gives a negative head (instead of 0) to the root
    of every sentence after the first one of a document. `token.head` and
    `analyzeText()` are not affected. This is fixed in aymara 0.6.0.

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
