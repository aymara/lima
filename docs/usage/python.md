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
contiguous sequences of tokens. The classes follow spaCy's API: tokens have
`text`, `lemma_`, `pos_`, `morph`, `dep_`, `head`, `children`, `subtree`…;
spans have `root`, `sents`, `ents`, `label_`…; documents have `sents`, `ents`,
`noun_chunks`, `char_span()`, `to_json()`…

```python
for token in doc:
    print(token.i, token.text, token.lemma_, token.pos_, token.dep_,
          token.head.text)

for sentence in doc.sents:
    print(sentence.text, sentence.root.text)

for entity in doc.ents:
    print(entity.text, entity.label_)
```

!!! warning "Differences with spaCy"
    - `token.i` starts at 1 (spaCy's starts at 0).
    - Since aymara 0.9.0, `token.head` is a `Token`, as in spaCy (the root's
      head is itself). Before, it was the 0-based index of the head in the
      document, now available as `token.head_i`.
    - `tag_` is empty: the models predict no language specific tags.
    - The dependency relations are the UD ones (`root`, not spaCy's English
      `ROOT`) and the entity labels are LIMA's (e.g. `Person.PERSON`).

Printing a `Doc` with `repr()` gives its CoNLL-U representation:

```pycon
>>> print(repr(nlp("Hello, World!")))
1       Hello   hello   INTJ    _       _               3       discourse       _       Pos=0|Len=5
2       ,       ,       PUNCT   _       _               1       punct   _       Pos=5|Len=1
3       World   World   PROPN   _       Number=Sing     0       root    _       Pos=7|Len=5
4       !       !       PUNCT   _       _               3       punct   _       Pos=12|Len=1
```

!!! warning "aymara 0.5.0: root heads in `repr()`"
    In aymara 0.5.0, `repr()` gives a negative head (instead of 0) to the root
    of every sentence after the first one of a document. `token.head` and
    `analyzeText()` are not affected. This is fixed in aymara 0.6.0.

`doc.to_conllu()` writes standard CoNLL-U, with `# sent_id` and `# text`
comments, `SpaceAfter=No` and the lines of multiword tokens.
`Doc.from_conllu(text, lang)` creates a document from CoNLL-U (for instance a
UD treebank's) without analyzing it. `analyzeText()` returns the raw CoNLL-U
string of LIMA instead of a `Doc`:

```python
print(nlp.analyzeText("The author wrote a novel.", lang="ud-eng"))
```

## Analyzing many texts

`nlp.pipe(texts)` yields one `Doc` per text, as in spaCy. With
`batch_size=n` (n > 1), the texts of a batch are analyzed together, which is
several times faster for short texts; the analysis of a text may then differ
slightly from `nlp(text)`, the sentence segmentation seeing the neighboring
texts. The default, `batch_size=1`, analyzes each text alone.

```python
for doc in nlp.pipe(texts, batch_size=64):
    ...
```

## Using LIMA with spaCy

Since aymara 0.9.0, LIMA's analyses can be used with spaCy's own tools
(displaCy, `Matcher`, `DocBin`, spaCy components…). Install the optional
dependency with `pip install "aymara[spacy]"`.

- `doc.to_spacy()` converts a LIMA document into a `spacy.tokens.Doc`, with
  its lemmas, parts of speech, morphology, dependency tree, sentences and
  named entities.
- `aymara.lima_spacy.spacy_pipeline()` creates a spaCy pipeline whose
  tokenizer is a LIMA analyzer, as spacy-stanza and spacy-udpipe do: spaCy
  components can be added after it.

```python
import aymara.lima_spacy

nlp = aymara.lima_spacy.spacy_pipeline("ud-eng")
doc = nlp("The author wrote a novel.")   # a spacy.tokens.Doc
print([(t.text, t.lemma_, t.pos_, t.dep_, t.head.text) for t in doc])
```

In a spaCy configuration file, the tokenizer is registered as
`aymara.LimaTokenizer.v1`:

```ini
[nlp.tokenizer]
@tokenizers = "aymara.LimaTokenizer.v1"
lang = "ud-eng"
```

The words of a multiword token that do not spell it (French "du" → "de" "le")
are separate spaCy tokens: the spaCy text is then "de le", and the original
text is in `doc.user_data["lima_text"]`.

## Matching phrases

Since aymara 0.10.0, `aymara.lima.PhraseMatcher` follows the API of spaCy's
`PhraseMatcher`, on LIMA's [ModEx](../reference/modex-rules.md) rules. The patterns are
compiled into rules that the analyzer applies itself, after the dependency
parsing and before its named entities rules. So the documents must be analyzed
after the patterns are added:

```python
import aymara.lima
nlp = aymara.lima.Lima("ud-eng")
# attr: "ORTH" (default), "LOWER", "LEMMA" or "POS"
matcher = aymara.lima.PhraseMatcher(nlp)
matcher.add("OBAMA", [nlp("Barack Obama")])       # or ["Barack Obama"]
doc = nlp("Barack Obama lifts America one last time in emotional farewell")
print(matcher(doc))                               # [('OBAMA', 0, 2)]
print(matcher(doc, as_spans=True))                # spans labelled "OBAMA"
```

`matcher.remove(key)` removes the patterns of a key, and
`add(key, patterns, on_match=callback)` calls `callback(matcher, doc, i,
matches)` for each match. The differences with spaCy:

- the matcher is created with the analyzer (`nlp`), not its vocabulary, and
  only the documents analyzed after `add` contain matches (adding the first
  pattern makes `nlp` use a new pipeline, with the matcher);
- the match ids are the keys, not their hashes;
- the matches do not overlap: the longest one starting first is kept;
- the matches are entities of the document (in `doc.ents`, labelled
  `PhraseMatcher<n>_<k>.<KEY>`). The named entities rules may include one in a
  larger entity (e.g. `Paris` in a `Location.CITY`), which then hides it in
  `doc.ents`, but `matcher(doc)` still finds it;
- `LOWER` matches the original, lowercase, uppercase, capitalized and title
  forms of the pattern's words; `LEMMA` matches their lemmas *and* parts of
  speech (take the patterns in sentences, so that their words get the POS they
  have in the texts).

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
