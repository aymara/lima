# Output formats

## CoNLL-U

The default output of the pipelines is the
[CoNLL-U Plus format](https://universaldependencies.org/ext-format.html): one
token per line, sentences separated by blank lines, ten tab-separated columns.

| # | Field | Content in LIMA |
| --- | --- | --- |
| 1 | ID | Word index, starting at 1 in each sentence; a range for multiword tokens |
| 2 | FORM | Word form or punctuation symbol |
| 3 | LEMMA | Lemma, or `_` if not available |
| 4 | UPOS | Universal part-of-speech tag (legacy pipelines use LIMA's own tagset) |
| 5 | XPOS | Language-specific part-of-speech tag; `_` with the neural pipelines |
| 6 | FEATS | Morphological features from the universal feature inventory |
| 7 | HEAD | Head of the word (an ID, or 0 for the root) |
| 8 | DEPREL | Dependency relation to the head |
| 9 | DEPS | Enhanced dependencies; always `_` (not produced by LIMA) |
| 10 | MISC | Pipe-separated `key=value` annotations |

LIMA always writes `Pos` (character offset in the text) and `Len` (length) in
the MISC column, plus `SpaceAfter=No` when the token is not followed by a
space. Named entities are annotated with the `NE` key, whose value is the
entity type.

```text
# sent_id = 1
# text = The author wrote a novel.
1  The     the     DET    _  Definite=Def|PronType=Art         2  det    _  Len=3|Pos=1
2  author  author  NOUN   _  Number=Sing                       3  nsubj  _  Len=6|Pos=5
3  wrote   write   VERB   _  Mood=Ind|Tense=Past|VerbForm=Fin  0  root   _  Len=5|Pos=12
4  a       a       DET    _  Definite=Ind|PronType=Art         5  det    _  Len=1|Pos=18
5  novel   novel   NOUN   _  Number=Sing                       3  obj    _  Len=5|Pos=20|SpaceAfter=No
6  .       .       PUNCT  _  _                                 3  punct  _  Len=1|Pos=25
```

The NER pipelines can produce CoNLL-03 instead (see the `conllDumperNer`
configuration and the [ConllDumper](../reference/process-units/dumpers.md#conlldumper)
reference).

## Bag of words { #bag-of-words }

The legacy pipelines can produce a binary "bag of words" (BoW)
representation of the text, containing the lemmas of the content words
(nouns, adjectives, verbs) and the named entities. Replace `conllDumper` by
`bowDumper` in your copy of `lima-lp-<lang>.xml` and pass `-d bow` to
`analyzeText`. Print the result with `readBowFile`:

```bash
readBowFile file.txt.bin
```

For the text "On 4th April 2011, Bill Williams looked at Paris from his home
window.", the output is:

```text
(4th_April_2011-8192-4)->[*(4th-8192-4)(April-16384-8)(2011-8192-14)]:DateTime.DATE:date=2011-04-04;value=4th April 2011
(Bill_Williams-16384-20)->[*(Bill-16384-20)(Williams-16384-25)]:Person.PERSON:firstname=Bill;lastname=Williams;value=Bill Williams
(look-49152-34)
(Paris-16384-44)->[*(Paris-16384-44)]:Location.LOCATION:value=Paris
(home-8192-59)
(window-8192-64)
```

Each line is a term: simple (the verb "to look") or complex (the named entities
"Paris", "Bill Williams" and "4th April 2011"). Each term is described by its
normalized form, the numerical value of its category and its position in the
text, followed by its structure. This text form is meant for inspection only;
use the C++ BoW API to process bags of words in programs.

## Other formats

LIMA has many other [dumpers](../reference/process-units/dumpers.md) (XML,
annotation graphs, Brat, dependency triples…) and
[loggers](../reference/process-units/loggers.md) producing intermediate
results for debugging.
