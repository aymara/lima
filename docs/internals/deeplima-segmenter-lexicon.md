# Lexicon feature of the deeplima segmenter

Branch `segm-lexicon` (2026-11-24). The segmenter can take an external word list as an extra input feature. This helps languages written without spaces, whose word boundaries the treebank alone teaches badly.

## Why

On Chinese, Japanese, Vietnamese and Thai, deeplima's word segmentation lags behind Stanza and Trankit. For example, on Chinese-GSDSimp the Words F1 is 88.8, against 94.9 for Stanza. The segmenter sees only character n-grams of length 1 to 3, learned from about 100,000 training words. A bigger model does not help: test Words F1 is 88.8, 88.4 and 87.3 at hidden sizes 32, 128 and 512. What it lacks is the vocabulary.

A PyTorch prototype with the same inputs as deeplima, plus dictionary flags per character, measured on Chinese-GSDSimp test (word F1, gold sentence boundaries):

| Lexicon | Word F1 |
|---|---|
| None | 88.7 to 89.5 |
| Training words (jackknifed) | 90.8 |
| CC-CEDICT alone, flags for words starting and ending at the character | 93.7 to 94.1 |
| CC-CEDICT alone, flags for words ending at the character only (this implementation) | 92.7 to 93.6 |
| Oracle: the test words added | 96.0 |

## Design

- **Feature.** For each character, a 6-bit mask. Bit k (k = 0 to 4) is set when a word of the list with length k + 2 ends at this character; bit 5 when one of length 7 or 8 does.
- **Why only word ends.** Word ends only look back at the last 8 characters, so the streaming reader computes the mask when it reads a character. Flagging word starts would need a 7-character lookahead, which the 3-character code point buffer of `Utf8Reader` cannot hold without breaking the existing n-grams. The segmenter's backward RNN carries the information to the word's start.
- **Plumbing.**
  - `Utf8Reader` has a fifth channel, `channel_lexicon`, which pushes 0 when there is no list.
  - The n-gram type `ngram_descr_t::lexicon_ngram`, with offset 0 and length 1, exposes it like the other n-grams.
  - The training dictionary maps the mask values to an embedding of size 4.
- **Word list.** `SegmLexicon` (`segmentation/impl/segm_lexicon.h`) holds the list: UTF-8, one word per line (the first tab-separated field), words of 2 to 8 characters.
- **Training.** `deeplima-train-segm --lexicon <file>` adds the n-gram and copies the file beside the model as `<name>.pt.lex`.
- **Inference.** `SegmentationImpl::load(<model>.pt)` reads `<model>.pt.lex` when the model has the lexicon n-gram, so deeplima and LIMA's `RnnTokenizer` need no other change. Loading fails when the file is missing.
- **Compatibility.** Models without the feature behave as before. A model with it needs a deeplima that has this feature: older ones do not know the n-gram type.

## Results

Chinese-GSDSimp test, segmenters trained with the campaign's Chinese hyperparameters (hidden size 32), with and without the CC-CEDICT word list (109,896 words, simplified headwords). `deeplima` tokenization only, and LIMA `deepud`, give the same scores:

| | Words F1 | Speed (deeplima, tokenization only) |
|---|---|---|
| No lexicon | 89.16 | 114,000 tokens/s |
| CC-CEDICT | **93.61** | 93,000 tokens/s |

## Word lists

`corpora/make_segm_lexicon.py` in deeplima-training-scripts builds them. For Chinese it takes both the traditional and the simplified headwords of CC-CEDICT, 182,546 words, because the Chinese treebanks mix both scripts. CC-CEDICT is under CC BY-SA 4.0, so a distributed `.pt.lex` built from it keeps that license and needs attribution.

No list is chosen yet for Japanese and Vietnamese. In the prototype, Thai gains almost nothing from a lexicon.
