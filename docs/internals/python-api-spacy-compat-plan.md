# Python API: spaCy compatibility, state and plan (2026-11-25)

The `aymara.lima` API (lima-python, `aymara/lima.py`) was designed as a
drop-in replacement for spaCy (`Lima` ≈ `nlp`, `Doc`, `Span`, `Token`). This
note checks whether that is still true and plans the changes.

## How it was checked

- aymara 0.8.1 from PyPI, spaCy 3.8.18 + `en_core_web_sm`, fresh venv.
- An isolated `XDG_DATA_HOME` with only `lima_models -i eng` (models v0.3.0,
  `eng-UD_English-MERGE`). Note: a developer's `~/.local/share/lima` can hold
  stale `config/` and compiled resources that override the wheel's.
- Each of 56 common spaCy idioms evaluated on both libraries (each in its own
  process) with the text "Mr. Best flew to New York on Saturday morning. He met
  the CEO of Apple there!".

Result: **2 / 56 idioms behave the same.** Most others raise `AttributeError`,
a few return wrong results. Code written for spaCy does not run unchanged
anymore (and mostly never did: the spaCy `_`-suffixed string attributes were
never provided).

## P0: crash on a named entity followed by `'s`

Deterministic with `Lima("ud-eng")`, deepud pipeline: a segfault (rc 139) or
`LimaInternalError: stringspool(size = 42144): Out of bounds (<garbage>)`.

| Crashes | Works |
| --- | --- |
| `Apple's CEO.` | `IBM's CEO.` |
| `Microsoft's CEO.` | `John's car is red.` |
| `Apple's.` | `The dog's bone.` |
| `Paris's mayor.` | `It's raining.` / `Apple CEO.` |

### Cause and fix (2026-11-26)

The crash is in `ConllDumper::process` (gdb on the 0.8.1 wheel), not in ModEx.
`Apple's` is a multiword token of the English model (`.pt.mwt` dictionary from
GUM: `Apple's` -> `Apple` `'s`), and:

- `RnnTokenizer` gave all the words of a multiword token the surface's
  position (hence `'s` at `Pos=0`), the first one carrying the surface as
  orthographic alternative.
- To write the `N-M` line, `ConllDumper` took the token of a **PosGraph**
  vertex from the **AnalysisGraph**. Both graphs share vertex ids in the
  deeplima pipelines, except the entity vertex ModEx adds to the PosGraph:
  its id is beyond the AnalysisGraph, hence the garbage StringsPool index and
  the segfault. Only entities followed by a multiword token crash (`Apple's`,
  not `John's`, `IBM's` not being an entity, `Apple 's` not being split).
- The multiword token ids were also counted one vertex off (from the sentence
  begin vertex): no `N-M` line was ever written (`du` -> `de le` included).

Fixed (lima branch `conll-mwt-entity-crash`):

- `ConllDumper` collects the multiword tokens while it writes the lines: a
  word carrying a surface, then the next words starting inside it; it writes
  the `N-M` line before them. No lookup across graphs.
- `RnnTokenizer` gives the words that spell the surface their own positions
  (`Apple` 0-5, `'s` 5-7); the others (`du` -> `de le`) still share the
  surface's position.

Every real English corpus contains this pattern, so this alone rules out
replacing spaCy.

## P1: bugs in the existing API

| Bug | Where | Fix |
| --- | --- | --- |
| `doc.ents` spans have empty `.text`: `start` is the 1-based `Token.i`, `end` the 0-based inclusive index, used as exclusive. An entity ending on the last token is dropped. | `_DocEntitiesIterator` | Compute `[start, end)` from 0-based indices. Flush the last entity. |
| `doc.ents` is a one-shot iterator: no `len()`, no indexing, consumed by the first loop | `Doc.ents` | Return a tuple of `Span`s (as spaCy does). |
| `doc[:3]`, `doc[5:]` raise `TypeError` (`None` bounds) | `Doc.__getitem__` | Normalize the slice (`slice.indices(len(self))`). The same for `Span.__getitem__`, whose `stop=-1` default for `None` is also wrong. |
| FEATS printed as `Number:Sing` in `repr` (CoNLL-U: `Number=Sing`) | `Token.__repr__` | Use `=`. |
| `is_sent_start` is True only for token 0 | `Token.is_sent_start` | Needs the token's doc (see `token.doc`). |
| `Span.label` is a string (spaCy: int hash, string in `label_`) | `Span` | Add `label_`, keep `label` as is (no hashes in LIMA). |
| The docstrings copied from spaCy assert things that are false (`label == 346`, `label_ == "PERSON"`, `span.sents`...) | everywhere | Make them true and run them as doctests in CI. |

`Token.i` is 1-based, as the UD word IDs (CoNLL-U `ID`), where spaCy's is
0-based. That is the UD convention, not a bug: keep it, and provide 0-based
indices as a spaCy compatibility option (in the P2a layer and in `to_spacy`).
`doc[k]` stays 0-based, as Python sequences. Note that `Token.i` counts in the
whole document, where the CoNLL-U `ID` restarts at 1 in each sentence
(`repr(sentence)` renumbers): a per-sentence UD id could be added too.

Not API bugs, but they hurt drop-in use: with the MERGE model, `Mr.` is split
into `Mr` + `.` and opens a sentence by itself, and `Best` is lemmatized `good`.
Those are tokenizer/lemmatizer model issues (abbreviations), to report on the
deeplima side.

## P2: missing spaCy surface, by value / cost

### a. Naming and navigation layer (pure Python, cheap, highest value)

All computable from what the C++ `Doc`/`Token` already give (text, idx, len,
head, dep, features, sentences):

- `Token`: `text`, `lemma_`, `pos_`, `tag_` (= XPOS, `_` today), `dep_`,
  `ent_type_`, `ent_iob_`, `morph` (a small `MorphAnalysis`: `str()`,
  `get()`, `to_dict()`), `whitespace_`, `text_with_ws`, `lower_`, `norm_`,
  `shape_`, `like_num`, `is_stop` (from a stop list per language), `nbor()`.
- `Token.head` returning a `Token` (root's head is itself, as in spaCy),
  `token.doc`, `token.sent`, `children`, `lefts`, `rights`, `n_lefts`,
  `n_rights`, `subtree`, `ancestors`, `is_ancestor()`.
  To do that, `Token` must keep a reference to its `Doc`.
- `Span`: `root`, `sent`, `sents`, `label_`, `lemma_`, `ents`, `text_with_ws`,
  `as_doc()`, iteration consistent with `Doc`.
- `Doc`: `text_with_ws`, `lang_`, `ents` as a tuple, `char_span()`,
  `has_annotation()`, `to_json()`, `noun_chunks` (rule over UD deps: NOUN/PROPN/
  PRON heads with det/amod/compound/nmod:poss/nummod dependents).

Naming: spaCy's int attributes (`lemma`, `pos`, `dep`... as hashes) have no
LIMA equivalent. Keep the current string-valued ones and add the `_` ones;
document the difference.

### b. Pipeline-level API

- `nlp.pipe(texts, batch_size=..., n_process=1)` — at least a sequential
  generator, then batch through the C++ analyzer.
- `nlp.pipe_names`, `nlp.lang`, `nlp.meta`.
- `Doc.from_conllu` / `to_conllu` (the current `repr`, made explicit).

### c. Interoperability (strategic, exact compatibility)

Instead of mirroring all of spaCy by hand, provide conversion to spaCy's own
objects, as spacy-stanza and spacy-udpipe do:

- `Doc.to_spacy(vocab)`: builds a `spacy.tokens.Doc` with words, spaces,
  lemmas, POS, morph, heads, deps, sentence starts and entities.
- `spacy.blank("en")` + a LIMA tokenizer/pipe registered as a spaCy factory
  (`aymara.lima.spacy_pipeline("ud-eng")`), so that displaCy, `Matcher`,
  `DocBin`... work unchanged.

spaCy remains an optional dependency (extra `aymara[spacy]`). The two approaches
are complementary: (a) for users without spaCy, (c) for exact compatibility.

## P3: housekeeping

- `pydantic.parse_obj_as` is deprecated in pydantic v2: replace by a plain
  `isinstance` check, and drop the pydantic dependency.
- stderr noise for each analyzer (`classifier doesn't provide required feature`,
  `QSocketNotifier`, `getSyntacticRelationId NOT found obl:unmarked`).
- `print(..., file=sys.stderr)` in library code (`add_pipeline_unit`,
  `analyzeText`, `Span.__len__`): use `logging`/exceptions.
- Mutable default arguments (`meta: Dict = {}`).

## Suggested order

1. P0 crash (lima, C++), with a regression test in lima-python.
2. P1 fixes + doctests (one minor release).
3. P2a naming/navigation layer, with a test suite that runs the same idioms on
   spaCy and LIMA (the probe used for this note as the starting point).
4. P2b `pipe`, then P2c `to_spacy` / spaCy factory.
5. P3 alongside.

## Status (2026-11-26)

| Item | State |
| --- | --- |
| P0 crash | Done: aymara/lima#229 (ConllDumper multiword tokens, positions) |
| P1 | Done: aymara/lima-python#37, except running the docstrings as doctests in CI |
| P3 | Done in aymara/lima-python#37, except the C++ stderr noise |
| P2a | lima-python branch `spacy-compat-p2a`. `Token.head` is now a `Token` (breaking); the int is `Token.head_i`. Not yet: `is_stop` (needs stop lists), `Span.as_doc()` |
| P2b | `Lima.pipe` (sequential) and `Lima.lang` in `spacy-compat-p2a`. Not yet: batching through the C++ analyzer, `pipe_names`, `meta`, `Doc.from_conllu` |
| P2c | Not started |

Found while doing P2a: with the English MERGE model, `Mr.` makes a sentence of
its own, and `morning` in `Mr. Best flew to New York on Saturday morning.` gets
no relation (`dep` `_`, LIMA logs `LimaAnalyzerPrivate::getConllRelName target 0
not found in segmentation mapping`). The parser's head is apparently in another
sentence, which lima-python's `Doc` building (`lima.cpp`) cannot map. lima-python
now treats such a token as its own head; the LIMA side remains to be checked.

## spaCy usage today

PyPI downloads, last month (pypistats.org, 2026-11-25): transformers 122 M,
nltk 49 M, spacy 25.5 M, textblob 3.1 M, gensim 2.9 M, stanza 0.94 M, flair
62 k, spacy-udpipe 12 k, aymara 0.7 k. NLTK and spaCy counts are inflated by
transitive dependencies and CI. transformers is a model library, not a
linguistic pipeline. spaCy (3.8.x line, still released in 2026) remains the
most used dedicated linguistic analyzer; Stanza, the closest UD-based
competitor (the most similar to LIMA), is ~27× less downloaded.
