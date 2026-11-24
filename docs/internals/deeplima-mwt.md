# Multi-word token (MWT) expansion in deeplima

Merged in #184 (dictionary expansion), #185 (tokenizer predicts MWT) and #186
(expansion also on the tokenization-only path), 2026-06-29 → 2026-07-01. LIMA's
`RnnTokenizer` (the `deepud` pipelines) expands MWTs the same way since
2026-11, and #217 (2026-11-24) added the affix-rule fallback for flagged tokens
missing from the dictionary.

UD treebanks distinguish *surface tokens* from *syntactic words*: French `du` is
one token but two words, `de` + `le`, written in CoNLL-U as a range line followed
by the words:

```
3-4   du   _   _   ...
3     de   de  ADP ...
4     le   le  DET ...
```

Without expansion, deeplima tags and parses `du` as one word. Because the tagger and
parser were trained on `de`/`le`, `du` is out of distribution for them. On French
GSD this cost about 3.4 Words F1 and about 4 UAS/LAS points.

## Design

### Expand before tagging and parsing

Expansion happens between the tokenizer and the tagger (`apps/deeplima.cpp`). It
can't be done later, for example in the CoNLL-U dumper or in LIMA afterwards: by
then the tagger and parser have already worked on the wrong units. Splitting the
output afterwards would still leave wrong tags and heads.

### The dictionary says how, the tokenizer says whether

This is the approach Stanza uses:

- **The dictionary** gives the expansion strings (`du` → `de le`). It is extracted
  from the training treebank. For the closed-class, concatenative contractions of
  French, German, Italian, Spanish and Portuguese, a dictionary is enough.
- **The tokenizer** decides, in context, whether a given surface token is a
  multi-word token. This matters for ambiguous forms. French `des` is either the
  contraction `de les` or the indefinite article `des`, and blind dictionary
  lookup expands both.

### Affix rules for flagged tokens missing from the dictionary

A dictionary only knows the surfaces seen in training. In languages where MWTs
are productive (a stem plus a clitic), most test MWTs are unseen. For them, the
dictionary also holds **affix rules**, learned from its own entries:

- a **suffix rule** cuts an ending of the surface, rewrites the end of the
  remaining stem and adds fixed words after it: Polish `zrobiłem` →
  `zrobił` + `em`; in Tamil the clitic fuses with the stem's last vowel sign,
  which the rewrite restores;
- a **prefix rule** is the mirror: Arabic `و` + stem → `و` + stem.

Each dictionary entry gives one suffix rule (from its longest common prefix with
its first word) and one prefix rule (from its longest common suffix with its
last word). A rule is kept when at least 2 entries give it
(`--rule-min-support`), and only the best rule per affix is kept, scored by its
precision on the dictionary: support / (entries having the affix + 1).

Rules apply **only** to surfaces the tokenizer flags as multiword and the
dictionary doesn't know: applied to every token, they would split most words.
With a tokenizer trained without `--mwt` (no flags), only the dictionary is used.

A seq2seq fallback is still not implemented. It would be needed for the
non-concatenative MWTs that no affix rule covers (most unseen Amharic and Tamil
ones; Hebrew).

### Tag scheme and backward compatibility

`segm_tag_t` (`include/deeplima/segmentation/impl/segmentation_decoder.h`) gets 4
end tags, `E_MWT`, `S_MWT`, `E_EOS_MWT` and `S_EOS_MWT` (0x07–0x0A). They are
added after the base tags, so tokenizers trained without MWT keep their exact tag
values (7 classes, `max_segm_tag = 0x07`). The decoder sets
`token_flags_t::multiword` on the surface token when it predicts one of these tags.

`SegmentationImpl::predicts_mwt()` detects an MWT-aware model from its number of
output classes (`> max_segm_tag`). The model converter records that number, so no
extra metadata is needed:

| Tokenizer | Expansion behaviour |
|---|---|
| Trained with `--mwt` (11 classes) | Only surfaces the tokenizer flagged are expanded |
| Old 7-class model | Every dictionary match is expanded, and a one-line note goes to stderr |
| No `.mwt` dictionary | No expansion: identical to the behaviour before MWT |

## Code map

| Piece | Location |
|---|---|
| Dictionary and affix rules: data, learning, file format | `include/deeplima/mwt_lexicon.h` (`MwtLexicon`, header-only, shared by deeplima and LIMA) |
| Dictionary extraction | `libs/conllu/mwt_dict.{h,cpp}` (`extract_mwt_dict`), app `apps/mwt-dict.cpp` → `deeplima-mwt-dict` |
| Training gold | `libs/conllu/treebank.{h,cpp}`: `token_t::multiword` set from `N-M` lines; `vectorize_gold` emits the `_MWT` tags |
| Training option | `apps/train-segm.cpp` `--mwt` (`train_params.train_mwt`, implies `--sentence`) |
| Inference expansion | `include/deeplima/mwt_expander.h` (`MwtExpander`) |
| Wiring, dictionary auto-load | `apps/deeplima.cpp`: `init()` loads the dictionary, `parse_file()` gates expansion on `predicts_mwt()` |
| Metadata carried with tokens | `segmentation::token_pos::m_mwt_len`/`m_mwt_surface_pch`/`m_mwt_surface_len`; `impl::token_t::m_mwt_len`/`m_mwt_surface_idx` (StringIndex id, analyzer and parser path) |
| Range line output | `include/deeplima/dumper_conllu.h` (both the tokenization-only and analysis dumpers) |
| LIMA | `DeepLimaUnits/RnnTokenizer/RnnTokenizer.cpp` loads `<model>.pt.mwt` and expands; `ConllDumper` writes the `N-M` lines |

`MwtExpander` replaces a surface token with its words. The surface's leading
whitespace goes to the first word, and its sentence/paragraph break flag goes to
the last word. The first word also records the word count and the surface form,
which the dumper uses to write the `N-M surface` line. The expander owns the word
bytes in a `std::deque` (pointer-stable). They stay valid because the next stage
interns strings synchronously.

## Usage

```bash
# 1. Extract the expansion dictionary and its affix rules from the training
#    treebank, next to the tokenizer
deeplima-mwt-dict -i fr_gsd-ud-train.conllu -o tokenizer.pt.mwt
#    format: surface <TAB> count <TAB> word1 <TAB> word2 ...
#            #rule <TAB> suffix|prefix <TAB> affix <TAB> rewrite <TAB> support <TAB> applicable <TAB> word1 ...
#    (French GSD gives 13 entries: du, des, au, aux, auquel, ...; ambiguous
#     surfaces keep their most frequent expansion)
#    Older readers skip the "#rule" lines as comments and use the dictionary alone.

#    Add the rules to an existing dictionary (e.g. of an already packaged
#    tokenizer), without the treebank; in place is fine:
deeplima-mwt-dict -d tokenizer.pt.mwt -o tokenizer.pt.mwt
#    --rule-min-support N: entries needed to keep a rule (default 2, 0: no rules)

# 2. Train an MWT-aware tokenizer (recommended; without it expansion is blind)
deeplima-train-segm --mwt ...            # 11-class model

# 3. Run as usual: <tok-model>.mwt is loaded automatically
deeplima --tok-model tokenizer.pt --tag-model tagger.pt --dp-model dp.pt ...
#    stderr: "Loaded multiword-token dictionary (N entries, R affix rules) from ..."
#            "MWT: tokenizer predicts multiword tokens; expanding only flagged surfaces."
```

There is no CLI option for the dictionary. It is found as `<tok-model>.mwt`, the
same way as the `<stem>.txt` and lemmatizer dictionary files. Expansion also runs
when only a tokenizer is given (tokenization-only output), so segmentation output
and evaluation include UD MWT lines. With `--input-format conllu` there is no
tokenizer, so no expansion happens: the input's own tokenization is kept.

LIMA's `RnnTokenizer` loads the same `<tok-model>.mwt` file beside
`tokenizer-<model>.pt` and expands the same way, so `analyzeText -p deepud` and
the Python `Lima` analyzers give the same words as `deeplima`.

## Results

### Dictionary and tokenizer flags

French GSD test set, raw text → tokenizer + tagger + parser, conll18 scorer, same
tagger and parser models in every run:

| F1 | No MWT | Dictionary only | Tokenizer-gated |
|---|---|---|---|
| Words | 95.12 | 98.49 | **98.97** |
| UPOS | 91.73 | 95.22 | — |
| UAS | 76.25 | 80.11 | **81.01** |
| LAS | 72.94 | 77.36 | **78.27** |
| `N-M` ranges emitted (gold: 279) | 0 | 338 | 304 |
| … of which `des` (gold: 74) | 0 | 126 | 94 |
| … of which `au` / `aux` (gold: 68 / 21) | 0 | 68 / 21 | 68 / 21 |

The gated tokenizer here was a small CPU test model (hidden size 64, about 29
epochs). The remaining over-expansion (304 vs 279, mostly `des`) comes from its
imperfect recall, and a fully trained tokenizer should reduce it. Aligned UAS
also improves with MWT (80.16 → 81.34): even on correctly aligned words, the
parser does better when it sees the words it was trained on.

### Affix rules

2026 campaign models (`--mwt` tokenizers), test sets, Words F1 and LAS,
dictionary alone → dictionary + affix rules; `deeplima` and LIMA `deepud` give
the same results:

| Treebank | Words | LAS |
|---|---|---|
| Amharic-ATT | 49.4 → **64.2** | 15.3 → **23.6** |
| Tamil-TTB | 85.0 → **92.4** | 45.4 → **55.8** |
| Marathi-UFAL | 90.3 → **96.0** | 55.0 → **59.8** |
| Arabic-PADT | 93.1 → **95.6** | 69.1 → **71.9** |
| Polish-MPDT | 97.2 → **97.6** | +0.9 |
| Galician-TreeGal | 97.9 → **98.1** | +0.6 |
| French-GSD, Turkish-IMST | unchanged | unchanged |

French has no productive MWTs: its dictionary covers them. In Latin and Irish
(probably Turkish too), the missing words have another cause that no expansion
fixes: test-only treebanks split clitics that the training treebanks of the
merged model don't, so the tokenizer never learned to flag them.

## Limitations and next steps

- The tokenizers of the v0.3.0 models release are trained with `--mwt` and ship
  their `<tok-model>.mwt` dictionary (for the treebanks with MWTs), without
  affix rules: add them with `deeplima-mwt-dict -d`.
- `deeplima-train-segm` doesn't write the dictionary itself; run
  `deeplima-mwt-dict` separately.
- No seq2seq fallback for unseen non-concatenative MWTs (see above).
