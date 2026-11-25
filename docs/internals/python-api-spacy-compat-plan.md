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

### Debugging status (2026-11-26, to continue on another machine)

Established (aymara 0.8.1 wheel, models v0.3.0, clean `XDG_DATA_HOME`):

- The `ud-eng` `deepud` pipeline (wheel `config/lima-lp-ud-eng.xml`, l. 41-52)
  is `RnnTokenizer → RnnTokensAnalyzer → RnnDependencyParser →
  SpecificEntitiesModexAfterPos → conllDumper`: it does run the ModEx NER
  (unlike the generic `deepud` of `lima-lp-ud.xml`).
- `Apple's CEO.`: `deeplima` pipeline (tokenizer + tagger/lemmatizer, no parser,
  no NER) rc=0 ×3; `deepud` segfault ×3. The crash needs the NER (or the parser,
  also absent from `deeplima`; not yet separated).
- In the `deeplima` output, the split clitic `'s` has a **wrong offset**:

  ```
  1  Apple  Apple  PROPN  ...  Pos=0|Len=5
  2  's     's     PART   ...  Pos=0|Len=2   <- should be Pos=5
  3  CEO    ceo    NOUN   ...  Pos=8|Len=3
  ```

Hypothesis (not verified): `RnnTokenizer` gives the token split from a word
(`Apple's` → `Apple` + `'s`) the offset of the word start; ModEx, when building
an entity on `Apple`, mishandles the overlapping token (StringsPool id out of
bounds, then segfault). It would explain why only recognized entities crash
(`Apple`, `Microsoft`, `Paris`) and not `John's` / `IBM's` (not tagged). Even
without the crash, such offsets make `Token.idx` and `Span.text` wrong in the
Python API.

Not checked on current master: the local `Dist/master/release` (June 2026)
fails earlier with `InvalidConfiguration` (`NumForm`), a stale-build/config
mismatch.

Next steps, with a fresh debug+ASAN build of master:

1. `analyzeText -l ud-eng -p deeplima --meta=udlang:eng-UD_English-MERGE` on
   `Apple's CEO.`: is the `'s` offset still 0? If yes, find where `RnnTokenizer`
   (DeepLimaUnits) computes the position of tokens split from one word.
2. Same with `-p deepud`, under ASAN/GDB: the stack of the crash.
3. Separate parser and NER: a pipeline with the parser but without
   `SpecificEntitiesModexAfterPos`.
4. Check whether `ud-fra` has the same offset problem with split tokens
   (`du` → `de le`, `l'`…).
5. Regression tests: offsets of split tokens (lima), and `Apple's CEO.` in
   lima-python `tests/test_aymara.py`.

Repro setup used here: a venv with `aymara==0.8.1` from PyPI,
`XDG_DATA_HOME=<empty dir> lima_models -i eng`, then
`python -c "import aymara.lima; aymara.lima.Lima('ud-eng')(\"Apple's CEO.\")"`.
Warning: a developer's `~/.local/share/lima` may hold stale `config/` and
compiled resources that override the wheel's; always test with an isolated
`XDG_DATA_HOME`.

Every real English corpus contains this pattern, so this alone rules out
replacing spaCy.

## P1: bugs in the existing API

| Bug | Where | Fix |
| --- | --- | --- |
| `doc.ents` spans have empty `.text`: `start` is the 1-based `Token.i`, `end` the 0-based inclusive index, used as exclusive. An entity ending on the last token is dropped. | `_DocEntitiesIterator` | Compute `[start, end)` from 0-based indices. Flush the last entity. |
| `doc.ents` is a one-shot iterator: no `len()`, no indexing, consumed by the first loop | `Doc.ents` | Return a tuple of `Span`s (as spaCy does). |
| `doc[:3]`, `doc[5:]` raise `TypeError` (`None` bounds) | `Doc.__getitem__` | Normalize the slice (`slice.indices(len(self))`). The same for `Span.__getitem__`, whose `stop=-1` default for `None` is also wrong. |
| `Token.i` is 1-based (spaCy: 0-based) | `Token.i` | Make it 0-based. **Breaking change**: `Token.__repr__` and `Span.__repr__` rely on the +1. Add a CHANGES note. |
| FEATS printed as `Number:Sing` in `repr` (CoNLL-U: `Number=Sing`) | `Token.__repr__` | Use `=`. |
| `is_sent_start` is True only for token 0 | `Token.is_sent_start` | Needs the token's doc (see `token.doc`). |
| `Span.label` is a string (spaCy: int hash, string in `label_`) | `Span` | Add `label_`, keep `label` as is (no hashes in LIMA). |
| The docstrings copied from spaCy assert things that are false (`label == 346`, `label_ == "PERSON"`, `span.sents`...) | everywhere | Make them true and run them as doctests in CI. |

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
2. P1 fixes + doctests (one minor release; `Token.i` change announced).
3. P2a naming/navigation layer, with a test suite that runs the same idioms on
   spaCy and LIMA (the probe used for this note as the starting point).
4. P2b `pipe`, then P2c `to_spacy` / spaCy factory.
5. P3 alongside.

## spaCy usage today

PyPI downloads, last month (pypistats.org, 2026-11-25): transformers 122 M,
nltk 49 M, spacy 25.5 M, textblob 3.1 M, gensim 2.9 M, stanza 0.94 M, flair
62 k, spacy-udpipe 12 k, aymara 0.7 k. NLTK and spaCy counts are inflated by
transitive dependencies and CI. transformers is a model library, not a
linguistic pipeline. spaCy (3.8.x line, still released in 2026) remains the
most used dedicated linguistic analyzer; Stanza, the closest UD-based
competitor (the most similar to LIMA), is ~27× less downloaded.
