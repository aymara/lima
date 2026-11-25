# Accuracy and speed

This page reports the evaluation of LIMA's neural pipeline (`deepud`) with the **v0.3.0 models**, trained on **Universal Dependencies 2.18**, on the UD 2.18 test sets, and compares it with [spaCy](https://spacy.io/), [Stanza](https://stanfordnlp.github.io/stanza/) and [Trankit](https://trankit.readthedocs.io/). Every toolkit analyzes the raw text of the test sets, from tokenization to dependency parsing.

## Summary

On the 108 test sets of 47 languages that LIMA analyzes with both variants (macro-averages over the test sets, F1 scores of the CoNLL 2018 scorer, speeds in words per second on 8 CPU threads):

| Models | Tokens | Sentences | Words | UPOS | UFeats | Lemmas | UAS | LAS | Words/s (median) |
|:--|--:|--:|--:|--:|--:|--:|--:|--:|--:|
| default | 98.83 | 85.48 | 98.22 | 93.79 | 87.22 | 92.44 | 81.98 | 77.37 | 5 069 |
| accurate | 98.83 | 85.48 | 98.22 | 93.79 | 87.22 | 92.44 | 82.63 | 78.08 | 4 449 |

Lemmas, UAS and LAS are averaged over the test sets whose model has a lemmatizer and a parser. The **default** models are chosen for a balance of accuracy and speed; the **accurate** variants (`lima_models -i <lang> --accurate`, see [Language models](models.md)) are the most accurate models found during training. In this release, they differ from the default ones only by their dependency parser, so only UAS, LAS and speed change.

### Compared with other toolkits

Each comparison uses the test sets that both LIMA (default models) and the other toolkit analyze, keeping, for the other toolkit, the best of its models on each test set (e.g. the Stanza package of the treebank, or spaCy's transformer pipeline, when they score better). Averages are macro-averages; the speed ratio is the median of LIMA's speed divided by the other's, on the test sets where both timings are reliable.

| Toolkit | Test sets | UPOS<br>LIMA / other | Lemmas<br>LIMA / other | LAS<br>LIMA / other | LIMA better on LAS | Speed ratio |
|:--|--:|--:|--:|--:|--:|--:|
| Stanza 1.15.0 | 90 | 94.08 / 95.73 | 92.65 / 95.12 | 78.44 / 83.03 | 11 / 90 | 4.8× |
| Trankit 1.1.2 | 98 | 94.01 / 91.43 | 92.78 / 89.49 | 78.84 / 77.02 | 45 / 96 | 9.0× |
| spaCy 3.8.18 | 58 | 94.55 / 87.79 | 93.24 / 84.47 | 79.87 / 62.82 | 54 / 58 | 0.8× |

On average, LIMA's default models are 4.6 LAS points less accurate and 4.8 times faster than Stanza, 1.8 LAS points more accurate and 9.0 times faster than Trankit, and 17.1 LAS points more accurate and 1.3 times slower than spaCy. spaCy covers fewer languages, and not all its pipelines are trained on UD: the English ones are trained on OntoNotes, so their dependency labels differ from UD's.

!!! warning "Speeds are approximate"
    LIMA ran on Intel Xeon Gold 5317 (227 runs) and AMD EPYC 9554 (10 runs) nodes; the other toolkits on AMD EPYC 9554 (246 runs) nodes. All used 8 threads and no GPU. The speed ratios are therefore indicative only.

## How the evaluation was done

- **Test data**: the test sets of the UD 2.18 treebanks, given as raw text (`*-test.txt`) to every toolkit, and scored against the gold `*-test.conllu` with the [CoNLL 2018 shared task scorer](https://universaldependencies.org/conll18/evaluation.html). Scores are F1 percentages: the morphosyntactic and syntactic scores depend on the tokenization and sentence segmentation the toolkit produced.
- **LIMA**: `analyzeText -l ud -p deepud` with the v0.3.0 models, in the Docker image of LIMA's `master` branch at commit `81eae8fb`, except for Chinese Beginner, Chinese CFL, Finnish FTB, Finnish OOD, Finnish PUD, Finnish TDT, Korean GSD, Korean KSL, Korean Kaist, Korean LittlePrince, Korean PUD, analyzed with an earlier image of `master` (their scores agree within one point with those of the `deeplima` command on the same models). Only the models of the evaluated package are available to it.
- **Merged models**: for most languages, the default model is trained on all the treebanks of the language merged (`<code>-UD_<Language>-MERGE`). It is scored on the test set of each treebank, none of which was used for training or model selection, so its scores compare with per-treebank results.
- **Other toolkits**: spaCy 3.8.18, Stanza 1.15.0, Trankit 1.1.2. Stanza is run with its default package for the language and with the package of the test treebank when there is one; Trankit with the model of the treebank, or of the language; spaCy with each of its `sm`, `md`, `lg` and `trf` pipelines for the language. These models were trained on older UD releases, or not on UD at all (spaCy's English pipelines are trained on OntoNotes), so their training data may overlap the UD 2.18 test sets.
- **Speed**: gold words per second of analysis, model loading excluded, with 8 threads (`OMP_NUM_THREADS`, `torch.set_num_threads`) and no GPU. For LIMA, the time of a run on the whole test text minus that of a run on its first 1000 characters; for the Python toolkits, the time of the analysis call. Each LIMA test set is analyzed three times and the fastest run kept. When the analysis took less than one second (gold words divided by the measured speed), timing noise dominates and the speed is not reliable: it is shown as `n/r` and left out of the medians and ratios.
- **Artificial test sets**: some UD treebanks have only a test set. To train models for their language, their test set was split into training, development and test parts. Their scores, given [separately](#artificial-test-sets), are not comparable with published results on the full test set.
- **Missing components**: when a model has no lemmatizer or no parser, its Lemmas or UAS/LAS cells are left empty, not counted as 0.

## Results per test set

The best score of each row is in bold. The Stanza and spaCy columns show the best of the toolkit's models on the test set.

=== "LAS"

    | Language | Test set | LIMA | LIMA accurate | Stanza | Trankit | spaCy |
    |:--|:--|--:|--:|--:|--:|--:|
    | Afrikaans | AfriBooms | 84.24 | 84.10 | **86.40** | 83.28 |  |
    | Albanian | STAF | 40.97 | 40.97 | **60.57** |  |  |
    | Arabic | PADT | 68.72 | 68.88 |  | **83.46** |  |
    | Armenian | ArmTDP | 80.35 | 81.33 | **83.32** | 80.02 |  |
    | Armenian | BSUT | 81.25 | 82.71 | 82.23 | **83.74** |  |
    | Basque | BDT | 81.80 | 82.06 | **82.72** | 78.98 |  |
    | Bulgarian | BTB | 89.56 | 89.56 | 90.77 | **93.41** |  |
    | Catalan | AnCora | 90.76 | 91.00 | **91.55** | 85.09 | 89.55 |
    | Czech | CAC | 90.30 | 90.19 | 89.34 | **91.32** |  |
    | Czech | CLTT | 71.09 | 72.16 | 79.99 | **87.66** |  |
    | Czech | FicTree | 86.46 | 86.31 | **90.66** | 89.78 |  |
    | Czech | PDTC | 90.56 | 91.14 | **91.41** | 85.14 |  |
    | Danish | DDT | 81.65 | 82.26 | **85.71** | 84.16 | 82.97 |
    | Dutch | Alpino | 85.45 | 85.74 | 88.23 | **90.77** | 81.75 |
    | Dutch | LassySmall | 85.02 | 85.54 | 88.24 | **88.61** | 81.53 |
    | English | Atis | 87.56 | 88.57 | **92.71** | 68.61 | 22.83 |
    | English | CHILDES | 87.74 | 87.75 | **90.50** | 82.28 | 50.46 |
    | English | ESLSpok | 89.28 | 89.72 | **90.91** | 88.92 | 50.21 |
    | English | EWT | 81.23 | 81.82 |  | **84.16** | 41.82 |
    | English | GUM | 83.16 | 84.13 | **89.78** | 85.37 | 41.26 |
    | English | LinES | 80.60 | 81.19 | 85.42 | **85.47** | 42.39 |
    | English | ParTUT | 81.74 | 82.50 | **87.88** | 87.87 | 40.18 |
    | Estonian | EDT | 83.34 | 84.05 | 84.92 | **87.81** |  |
    | Estonian | EWT | 76.79 | **77.99** | 74.32 | 71.45 |  |
    | Finnish | FTB | 74.32 | 75.86 | **86.30** | 81.84 | 59.20 |
    | Finnish | TDT | 77.90 | 80.92 | 88.08 | **91.93** | 79.65 |
    | French | ALTS | 73.91 | 74.57 | **81.95** | 63.97 | 59.02 |
    | French | GSD | 88.76 | 88.85 | **91.31** | 91.11 | 76.95 |
    | French | ParTUT | 88.16 | 88.70 | **90.29** | 87.75 | 81.78 |
    | French | ParisStories | 74.78 | 74.88 | **79.43** | 65.13 | 56.61 |
    | French | Rhapsodie | 80.43 | 81.03 | **83.09** | 75.66 | 67.14 |
    | French | Sequoia | 87.81 | 88.33 | **90.96** | 87.68 | 81.12 |
    | Galician | CTG | 78.43 | 78.75 | **82.55** | 73.84 |  |
    | German | GSD | 71.97 | 71.88 | 82.72 | **84.49** | 7.77 |
    | German | HDT | 93.88 | 94.08 | **94.53** | 93.05 | 7.05 |
    | Greek | Cretan | **71.40** | 70.59 | 63.08 |  | 50.20 |
    | Greek | GDT | 87.39 | 88.24 |  | **91.48** | 80.18 |
    | Greek | Messinian | **71.87** | 70.23 | 48.25 |  | 62.42 |
    | Hindi | HDTB | 90.22 | 90.42 | **92.30** | 90.75 |  |
    | Hungarian | Szeged | 75.90 | 76.79 | **77.57** | 74.72 |  |
    | Icelandic | GC | 57.83 | 58.09 | **75.12** |  |  |
    | Icelandic | IcePaHC | 79.88 | 80.72 | **83.09** |  |  |
    | Icelandic | Modern | 80.04 | **80.68** | 78.64 |  |  |
    | Irish | IDT | 77.77 | 77.91 | **81.44** | 72.03 |  |
    | Irish | TwittIrish | 64.39 | 64.83 | **67.11** | 44.39 |  |
    | Italian | ISDT | 88.98 | 89.73 | 91.88 | **93.89** | 75.82 |
    | Italian | MarkIT | 80.35 | 80.68 |  | **82.76** | 65.56 |
    | Italian | Old | 73.53 | **74.74** |  | 58.30 | 41.98 |
    | Italian | ParTUT | 89.99 | 90.46 |  | **93.35** | 75.71 |
    | Italian | PoSTWITA | 72.99 | 73.76 |  | **78.90** | 48.56 |
    | Italian | TWITTIRO | 73.73 | 74.13 |  | **78.30** | 55.83 |
    | Italian | VIT | 84.01 | 84.60 |  | **89.44** | 69.20 |
    | Japanese | GSD | 40.07 | 40.03 | **88.82** | 61.22 | 88.38 |
    | Japanese | GSDLUW | 68.31 | 68.45 | **85.34** | 44.98 | 45.89 |
    | Korean | GSD | 53.99 | 69.05 | **82.30** | 77.98 | 34.95 |
    | Korean | KSL | 70.33 | 83.25 | **86.72** | 61.81 | 48.28 |
    | Korean | Kaist | 74.27 | 73.23 | **85.37** | 81.65 | 78.15 |
    | Latin | CIRCSE | 36.56 | **36.88** |  |  |  |
    | Latin | ITTB | 83.52 | **83.97** |  | 75.14 |  |
    | Latin | LLCT | 90.79 | **91.39** |  | 49.54 |  |
    | Latin | PROIEL | 62.87 | 63.29 |  | **69.47** |  |
    | Latin | UDante | 61.90 | **61.93** |  | 58.71 |  |
    | Latvian | LVTB | 85.54 | 85.95 | 86.53 | **87.86** |  |
    | Lithuanian | ALKSNIS | 75.80 | 76.69 | 71.89 | **80.36** | 64.32 |
    | Lithuanian | HSE | 57.79 | **58.73** | 55.51 | 58.56 | 51.04 |
    | Maltese | MUDT | 75.67 | **75.91** |  |  |  |
    | Marathi | CMUPAN | 78.06 | 78.06 | **81.29** | 23.73 |  |
    | Marathi | UFAL | 55.00 | 55.00 | **55.49** | 41.90 |  |
    | Norwegian | Bokmaal | 90.04 | 90.25 | **91.73** | 86.57 |  |
    | Norwegian | Nynorsk | 88.80 | **89.28** | 77.25 |  |  |
    | Persian | PerDT | 89.27 | 89.48 | **90.37** | 71.04 |  |
    | Persian | Seraji | 84.82 | 85.63 | **87.85** | 82.37 |  |
    | Polish | LFG | 89.01 | 89.42 | **94.99** | 92.18 | 78.56 |
    | Polish | MPDT | 65.23 | 66.69 | 70.41 | **72.02** | 50.79 |
    | Polish | PDB | 86.53 | 87.08 | **92.28** | 91.45 | 81.07 |
    | Portuguese | Bosque | 84.14 | 84.85 | 88.34 | **94.66** | 78.10 |
    | Portuguese | CINTIL | 59.21 | 59.56 | **81.11** | 57.32 | 50.24 |
    | Portuguese | DANTEStocks | 81.59 | 82.15 | **87.51** | 51.64 | 43.01 |
    | Portuguese | GSD | 76.29 | 76.44 | **89.86** | 88.41 | 62.36 |
    | Portuguese | PetroGold | 89.43 | 89.95 | **93.68** | 84.19 | 68.47 |
    | Portuguese | Porttinari | 87.84 | 88.19 | **94.04** | 85.86 | 71.59 |
    | Romanian | Nonstandard | 79.84 | 80.17 | 84.45 | **84.99** | 50.73 |
    | Romanian | RRT | 85.17 | 85.81 | 87.06 | **88.46** | 83.25 |
    | Romanian | SiMoNERo | 89.42 | 90.17 | **90.83** | 86.89 | 83.74 |
    | Russian | GSD | 84.46 | 84.88 | 85.85 | **88.60** | 76.73 |
    | Russian | Poetry | 81.46 | **82.19** | 79.36 | 73.84 | 66.32 |
    | Russian | SynTagRus | 89.53 | 90.18 | **91.16** | 88.89 | 81.80 |
    | Russian | Taiga | 81.29 | **82.01** | 81.21 | 80.54 | 71.72 |
    | Scottish Gaelic | ARCOSG | 74.99 | 75.57 | **78.89** | 65.09 |  |
    | Serbian | SET | 85.80 | 85.51 | 88.78 | **89.74** |  |
    | Slovak | SNK | 83.75 | 84.47 | 86.46 | **92.65** |  |
    | Spanish | AnCora | 87.38 | 87.86 | **91.70** | 85.84 | 85.71 |
    | Spanish | GSD | 83.80 | 83.76 | **89.40** | 87.57 | 72.71 |
    | Swedish | LinES | 84.08 | 84.66 | **86.59** | 85.41 | 70.33 |
    | Swedish | Talbanken | 85.60 | 86.06 | 88.45 | **88.78** | 78.61 |
    | Tamil | TTB | 45.66 | 45.56 | 53.73 | **60.53** |  |
    | Telugu | MTG | 83.22 | 83.22 |  | **84.35** |  |
    | Thai | TUD | 50.31 | **50.62** |  |  |  |
    | Turkish | Atis | 55.08 | 58.86 | **88.31** |  |  |
    | Turkish | BOUN | 63.42 | **63.96** | 57.57 | 56.77 |  |
    | Turkish | FrameNet | 80.91 | 81.46 | **84.32** | 48.92 |  |
    | Turkish | IMST | 57.14 | 57.24 | 66.33 | **74.62** |  |
    | Turkish | Kenet | 69.27 | 69.24 | **70.12** | 42.45 |  |
    | Turkish | Penn | 70.62 | 70.97 | **72.57** | 42.89 |  |
    | Turkish | Tourism | 90.64 | 90.56 | **91.23** | 39.04 |  |
    | Urdu | UDTB | 81.03 | 81.35 | **81.93** | 81.12 |  |
    | Vietnamese | VTB | 40.02 | 40.38 |  | **53.41** |  |
    | Welsh | CCG | 79.40 | 79.27 | **80.85** |  |  |

=== "UAS"

    | Language | Test set | LIMA | LIMA accurate | Stanza | Trankit | spaCy |
    |:--|:--|--:|--:|--:|--:|--:|
    | Afrikaans | AfriBooms | 87.60 | 87.69 | **89.80** | 86.58 |  |
    | Albanian | STAF | 52.83 | 52.83 | **75.10** |  |  |
    | Arabic | PADT | 73.23 | 73.32 |  | **88.34** |  |
    | Armenian | ArmTDP | 84.61 | 85.47 | **87.80** | 84.62 |  |
    | Armenian | BSUT | 86.31 | 87.79 | 87.57 | **88.53** |  |
    | Basque | BDT | 85.33 | 85.68 | **86.42** | 82.48 |  |
    | Bulgarian | BTB | 92.84 | 92.84 | 93.73 | **95.80** |  |
    | Catalan | AnCora | 92.67 | 92.95 | **93.46** | 89.34 | 93.02 |
    | Czech | CAC | 92.70 | 92.55 | 91.42 | **94.15** |  |
    | Czech | CLTT | 72.98 | 74.18 | 83.99 | **91.43** |  |
    | Czech | FicTree | 89.27 | 88.97 | 93.02 | **93.68** |  |
    | Czech | PDTC | 92.35 | 92.88 | **93.05** | 88.53 |  |
    | Danish | DDT | 84.26 | 84.81 | **87.91** | 86.58 | 85.47 |
    | Dutch | Alpino | 88.08 | 88.45 | 90.89 | **93.66** | 85.87 |
    | Dutch | LassySmall | 87.63 | 88.22 | 90.86 | **92.06** | 85.79 |
    | English | Atis | 89.47 | 90.55 | **94.60** | 72.97 | 35.38 |
    | English | CHILDES | 91.11 | 91.23 | **93.52** | 85.86 | 66.18 |
    | English | ESLSpok | 92.15 | 92.59 | **93.25** | 91.39 | 63.89 |
    | English | EWT | 84.08 | 84.73 |  | **86.97** | 56.00 |
    | English | GUM | 85.76 | 86.80 | **91.64** | 88.73 | 55.64 |
    | English | LinES | 84.03 | 84.71 | **88.77** | 88.52 | 57.41 |
    | English | ParTUT | 86.39 | 86.98 | 90.55 | **90.95** | 56.69 |
    | Estonian | EDT | 86.12 | 86.81 | 87.59 | **90.20** |  |
    | Estonian | EWT | 80.73 | **81.88** | 78.03 | 76.31 |  |
    | Finnish | FTB | 77.91 | 79.52 | 88.88 | **91.66** | 65.32 |
    | Finnish | TDT | 80.92 | 83.79 | 90.30 | **93.47** | 82.87 |
    | French | ALTS | 78.36 | 79.04 | **85.56** | 71.85 | 66.28 |
    | French | GSD | 91.24 | 91.30 | **93.36** | 93.19 | 80.41 |
    | French | ParTUT | 90.66 | 91.09 | **92.86** | 89.21 | 84.79 |
    | French | ParisStories | 78.65 | 78.58 | **82.29** | 69.82 | 64.93 |
    | French | Rhapsodie | 83.89 | 84.41 | **86.33** | 79.86 | 73.29 |
    | French | Sequoia | 90.15 | 90.77 | **92.89** | 90.02 | 83.69 |
    | Galician | CTG | 82.07 | 82.38 | **86.09** | 80.08 |  |
    | German | GSD | 76.40 | 76.34 | 86.14 | **88.40** | 43.87 |
    | German | HDT | 95.29 | 95.47 | **96.00** | 94.91 | 45.93 |
    | Greek | Cretan | 78.30 | **79.11** | 71.81 |  | 60.44 |
    | Greek | GDT | 89.90 | 90.88 |  | **92.84** | 82.77 |
    | Greek | Messinian | **81.31** | 79.26 | 63.09 |  | 76.39 |
    | Hindi | HDTB | 93.41 | 93.62 | **95.22** | 95.02 |  |
    | Hungarian | Szeged | 80.67 | 81.35 | 83.23 | **86.59** |  |
    | Icelandic | GC | 66.45 | 66.84 | **80.18** |  |  |
    | Icelandic | IcePaHC | 84.35 | 85.14 | **87.00** |  |  |
    | Icelandic | Modern | 83.31 | **83.98** | 82.43 |  |  |
    | Irish | IDT | 83.70 | 84.04 | **86.95** | 82.10 |  |
    | Irish | TwittIrish | 72.72 | 72.86 | **75.44** | 53.55 |  |
    | Italian | ISDT | 91.00 | 91.83 | 93.55 | **95.37** | 78.16 |
    | Italian | MarkIT | 84.83 | 85.21 |  | **86.75** | 70.35 |
    | Italian | Old | 78.81 | **80.14** |  | 66.33 | 51.05 |
    | Italian | ParTUT | 92.90 | 93.26 |  | **95.38** | 78.74 |
    | Italian | PoSTWITA | 77.45 | 78.12 |  | **84.18** | 53.62 |
    | Italian | TWITTIRO | 78.53 | 79.17 |  | **83.44** | 61.15 |
    | Italian | VIT | 87.81 | 88.34 |  | **92.34** | 73.66 |
    | Japanese | GSD | 41.70 | 41.66 | 89.69 | 67.57 | **89.85** |
    | Japanese | GSDLUW | 70.56 | 70.65 | **86.18** | 49.65 | 46.73 |
    | Korean | GSD | 65.63 | 77.31 | **86.36** | 85.12 | 51.62 |
    | Korean | KSL | 82.27 | 87.79 | **89.79** | 78.85 | 67.97 |
    | Korean | Kaist | 81.69 | 81.91 | **87.54** | 86.87 | 83.59 |
    | Latin | CIRCSE | 41.42 | **42.06** |  |  |  |
    | Latin | ITTB | 86.53 | **86.97** |  | 84.25 |  |
    | Latin | LLCT | 92.01 | **92.52** |  | 64.15 |  |
    | Latin | PROIEL | 67.57 | 68.10 |  | **78.11** |  |
    | Latin | UDante | 69.19 | 69.47 |  | **69.83** |  |
    | Latvian | LVTB | 88.43 | 88.89 | 89.50 | **92.04** |  |
    | Lithuanian | ALKSNIS | 80.24 | 81.26 | 77.32 | **88.31** | 70.53 |
    | Lithuanian | HSE | 68.71 | **69.93** | 67.48 | 66.79 | 62.71 |
    | Maltese | MUDT | 80.79 | **80.97** |  |  |  |
    | Marathi | CMUPAN | 84.98 | 84.98 | **87.51** | 31.94 |  |
    | Marathi | UFAL | 66.00 | 66.00 | **67.08** | 46.77 |  |
    | Norwegian | Bokmaal | 91.69 | 91.91 | **93.22** | 91.61 |  |
    | Norwegian | Nynorsk | 90.66 | **91.16** | 82.18 |  |  |
    | Persian | PerDT | 91.59 | 91.86 | **92.69** | 76.68 |  |
    | Persian | Seraji | 88.01 | 88.79 | **90.90** | 86.96 |  |
    | Polish | LFG | 91.27 | 91.44 | **96.58** | 93.32 | 83.56 |
    | Polish | MPDT | 70.53 | 71.52 | 75.81 | **77.23** | 56.89 |
    | Polish | PDB | 89.21 | 89.87 | 94.06 | **94.97** | 85.33 |
    | Portuguese | Bosque | 87.66 | 88.20 | 91.03 | **95.57** | 80.08 |
    | Portuguese | CINTIL | 73.22 | 73.65 | **84.55** | 74.13 | 65.93 |
    | Portuguese | DANTEStocks | 84.42 | 84.87 | **90.17** | 59.87 | 49.53 |
    | Portuguese | GSD | 84.69 | 84.94 | **92.25** | 90.51 | 70.14 |
    | Portuguese | PetroGold | 91.03 | 91.47 | **94.85** | 88.11 | 71.77 |
    | Portuguese | Porttinari | 90.45 | 90.74 | **95.49** | 90.11 | 75.94 |
    | Romanian | Nonstandard | 84.83 | 85.15 | 88.65 | **90.11** | 63.87 |
    | Romanian | RRT | 88.65 | 89.21 | 90.53 | **93.03** | 87.86 |
    | Romanian | SiMoNERo | 91.73 | 92.31 | **92.85** | 90.27 | 86.74 |
    | Russian | GSD | 88.11 | 88.72 | 89.72 | **91.86** | 80.48 |
    | Russian | Poetry | 84.67 | **85.39** | 83.93 | 79.65 | 72.10 |
    | Russian | SynTagRus | 91.71 | 92.35 | 92.96 | **93.67** | 85.55 |
    | Russian | Taiga | 84.29 | **84.96** | 84.60 | 84.49 | 76.62 |
    | Scottish Gaelic | ARCOSG | 79.57 | 80.08 | **83.45** | 73.78 |  |
    | Serbian | SET | 89.32 | 89.14 | 91.80 | **95.27** |  |
    | Slovak | SNK | 86.56 | 87.29 | 88.86 | **95.26** |  |
    | Spanish | AnCora | 90.27 | 90.71 | **93.60** | 90.25 | 89.05 |
    | Spanish | GSD | 87.99 | 88.13 | **91.92** | 91.78 | 80.92 |
    | Swedish | LinES | 87.79 | 88.40 | **90.24** | 88.59 | 76.23 |
    | Swedish | Talbanken | 89.03 | 89.22 | **91.27** | 90.83 | 82.64 |
    | Tamil | TTB | 51.59 | 52.01 | 62.42 | **67.49** |  |
    | Telugu | MTG | 90.57 | 90.57 |  | **92.24** |  |
    | Thai | TUD | 57.96 | **58.05** |  |  |  |
    | Turkish | Atis | 56.34 | 60.71 | **90.16** |  |  |
    | Turkish | BOUN | 70.17 | **70.75** | 66.00 | 67.49 |  |
    | Turkish | FrameNet | 91.68 | 92.02 | **92.57** | 61.99 |  |
    | Turkish | IMST | 65.26 | 65.44 | 74.17 | **84.18** |  |
    | Turkish | Kenet | 82.32 | 82.47 | **83.13** | 55.68 |  |
    | Turkish | Penn | 83.49 | 83.98 | **85.39** | 52.70 |  |
    | Turkish | Tourism | 96.35 | 96.09 | **96.64** | 45.47 |  |
    | Urdu | UDTB | 86.32 | 86.79 | **87.31** | 87.16 |  |
    | Vietnamese | VTB | 46.89 | 47.73 |  | **70.21** |  |
    | Welsh | CCG | 85.08 | 85.06 | **87.05** |  |  |

=== "UPOS"

    | Language | Test set | LIMA | LIMA accurate | Stanza | Trankit | spaCy |
    |:--|:--|--:|--:|--:|--:|--:|
    | Afrikaans | AfriBooms | 97.96 | 97.96 | **98.60** | 98.17 |  |
    | Albanian | STAF | **88.41** | **88.41** | 88.29 |  |  |
    | Arabic | PADT | 88.27 | 88.27 |  | **94.48** |  |
    | Armenian | ArmTDP | 95.73 | 95.73 | **97.00** | 92.99 |  |
    | Armenian | BSUT | 97.00 | 97.00 | **97.14** | 95.11 |  |
    | Basque | BDT | 95.61 | 95.61 | **96.00** | 95.28 |  |
    | Bulgarian | BTB | 98.61 | 98.61 | **99.07** | 99.02 |  |
    | Catalan | AnCora | 98.53 | 98.53 | **98.76** | 93.86 | 95.52 |
    | Czech | CAC | 96.71 | 96.71 | 98.62 | **99.01** |  |
    | Czech | CLTT | 85.76 | 85.76 | 97.62 | **98.59** |  |
    | Czech | FicTree | 97.76 | 97.76 | 98.37 | **98.38** |  |
    | Czech | PDTC | **98.60** | **98.60** | 98.40 | 95.10 |  |
    | Danish | DDT | 96.70 | 96.70 | 98.04 | 97.87 | **98.09** |
    | Dutch | Alpino | 96.36 | 96.36 | **96.59** | 96.19 | 95.13 |
    | Dutch | LassySmall | 96.69 | 96.69 | **97.16** | 96.22 | 95.15 |
    | English | Atis | 98.10 | 98.10 | **99.15** | 91.60 | 91.72 |
    | English | CHILDES | 96.04 | 96.04 | **97.15** | 92.46 | 93.62 |
    | English | ESLSpok | 95.32 | 95.32 | **98.15** | 95.76 | 95.27 |
    | English | EWT | 93.49 | 93.49 |  | **95.12** | 92.16 |
    | English | GUM | 95.42 | 95.42 | **97.93** | 94.08 | 95.42 |
    | English | LinES | 93.22 | 93.22 | **97.45** | 96.85 | 93.80 |
    | English | ParTUT | 92.66 | 92.66 | **96.63** | 95.95 | 91.58 |
    | Estonian | EDT | 97.25 | 97.25 | 96.90 | **97.64** |  |
    | Estonian | EWT | **94.31** | **94.31** | 92.91 | 91.20 |  |
    | Finnish | FTB | 94.14 | 94.14 | **96.25** | 94.61 | 88.71 |
    | Finnish | TDT | 95.88 | 95.88 | 97.48 | **98.01** | 95.31 |
    | French | ALTS | 92.01 | 92.01 | **95.33** | 86.64 | 83.05 |
    | French | GSD | 97.23 | 97.23 | **97.93** | 97.44 | 91.77 |
    | French | ParTUT | 96.21 | 96.21 | **97.67** | 97.31 | 91.67 |
    | French | ParisStories | 97.18 | 97.18 | **98.18** | 93.92 | 90.13 |
    | French | Rhapsodie | 97.35 | 97.35 | **98.12** | 95.71 | 92.84 |
    | French | Sequoia | 97.72 | 97.72 | **98.64** | 98.08 | 92.75 |
    | Galician | CTG | 95.14 | 95.14 | **97.20** | 96.13 |  |
    | German | GSD | 90.41 | 90.41 | **96.58** | 93.81 | 89.31 |
    | German | HDT | 98.08 | 98.08 | **98.33** | 94.70 | 90.25 |
    | Greek | Cretan | **92.09** | **92.09** | 89.25 |  | 73.90 |
    | Greek | GDT | **97.62** | **97.62** |  | 96.53 | 91.70 |
    | Greek | Messinian | **91.17** | **91.17** | 78.76 |  | 78.44 |
    | Hindi | HDTB | 96.99 | 96.99 | **97.90** | 96.97 |  |
    | Hungarian | Szeged | 95.24 | 95.24 | 96.20 | **96.90** |  |
    | Icelandic | GC | 86.48 | 86.48 | **92.49** |  |  |
    | Icelandic | IcePaHC | 95.49 | 95.49 | **96.23** |  |  |
    | Icelandic | Modern | 96.28 | 96.28 | **96.92** |  |  |
    | Irish | IDT | 94.78 | 94.78 | **96.32** | 92.98 |  |
    | Irish | TwittIrish | 87.48 | 87.48 | **88.60** | 66.75 |  |
    | Italian | ISDT | 97.78 | 97.78 | 98.19 | **98.55** | 86.64 |
    | Italian | MarkIT | 95.76 | 95.76 |  | **96.94** | 84.61 |
    | Italian | Old | **93.64** | **93.64** |  | 86.57 | 75.56 |
    | Italian | ParTUT | 97.28 | 97.28 |  | **98.63** | 84.86 |
    | Italian | PoSTWITA | 94.33 | 94.33 |  | **95.65** | 73.32 |
    | Italian | TWITTIRO | 95.01 | 95.01 |  | **95.31** | 78.00 |
    | Italian | VIT | 96.94 | 96.94 |  | **97.63** | 86.62 |
    | Japanese | GSD | 67.90 | 67.90 | 96.46 | 81.01 | **96.54** |
    | Japanese | GSDLUW | 82.91 | 82.91 | **94.40** | 71.23 | 70.55 |
    | Korean | GSD | 89.77 | 89.77 | **95.85** | 95.45 | 70.49 |
    | Korean | KSL | 94.02 | 94.02 | **95.72** | 80.67 | 71.09 |
    | Korean | Kaist | 92.87 | 92.87 | **95.23** | 93.13 | 92.73 |
    | Latin | CIRCSE | **84.82** | **84.82** |  | 69.43 |  |
    | Latin | ITTB | **98.34** | **98.34** |  | 79.96 |  |
    | Latin | LLCT | **99.40** | **99.40** |  | 78.81 |  |
    | Latin | PROIEL | **94.91** | **94.91** |  | 92.60 |  |
    | Latin | UDante | **88.31** | **88.31** |  | 76.47 |  |
    | Latvian | LVTB | **97.50** | **97.50** | 97.13 | 94.83 |  |
    | Lithuanian | ALKSNIS | 95.71 | 95.71 | 92.93 | **96.99** | 93.30 |
    | Lithuanian | HSE | 88.19 | 88.19 | 85.58 | **90.46** | 85.03 |
    | Maltese | MUDT | **95.53** | **95.53** |  |  |  |
    | Marathi | CMUPAN | 91.28 | 91.28 | **94.28** | 49.26 |  |
    | Marathi | UFAL | 78.00 | 78.00 | **81.87** | 73.08 |  |
    | Norwegian | Bokmaal | 97.78 | 97.78 | **98.32** | 95.66 |  |
    | Norwegian | Nynorsk | **97.45** | **97.45** | 89.54 |  |  |
    | Persian | PerDT | 96.29 | 96.29 | **97.03** | 83.06 |  |
    | Persian | Seraji | 97.06 | 97.06 | **97.71** | 94.81 |  |
    | Polish | LFG | 97.17 | 97.17 | **98.88** | 97.66 | 94.85 |
    | Polish | MPDT | 91.12 | 91.12 | **93.72** | 90.80 | 82.62 |
    | Polish | PDB | 97.81 | 97.81 | **98.57** | 98.12 | 95.62 |
    | Portuguese | Bosque | 95.14 | 95.14 | 97.61 | **99.20** | 87.21 |
    | Portuguese | CINTIL | 87.82 | 87.82 | **97.60** | 85.94 | 82.24 |
    | Portuguese | DANTEStocks | 94.66 | 94.66 | **97.00** | 76.82 | 69.62 |
    | Portuguese | GSD | 94.44 | 94.44 | **97.50** | 95.34 | 82.13 |
    | Portuguese | PetroGold | 97.33 | 97.33 | **98.76** | 95.27 | 83.79 |
    | Portuguese | Porttinari | 96.59 | 96.59 | **98.99** | 95.82 | 85.36 |
    | Romanian | Nonstandard | 94.70 | 94.70 | 95.35 | **96.04** | 74.48 |
    | Romanian | RRT | 96.32 | 96.32 | **97.11** | 96.97 | 91.16 |
    | Romanian | SiMoNERo | 97.23 | 97.23 | **98.02** | 94.61 | 90.22 |
    | Russian | GSD | 95.69 | 95.69 | 97.80 | **97.95** | 89.99 |
    | Russian | Poetry | 96.63 | 96.63 | **96.75** | 90.46 | 90.24 |
    | Russian | SynTagRus | 97.52 | 97.52 | **98.15** | 96.50 | 93.33 |
    | Russian | Taiga | 94.97 | 94.97 | **96.09** | 94.54 | 88.70 |
    | Scottish Gaelic | ARCOSG | 93.91 | 93.91 | **94.79** | 86.24 |  |
    | Serbian | SET | 97.35 | 97.35 | **98.50** | 98.03 |  |
    | Slovak | SNK | 96.41 | 96.41 | 96.34 | **98.13** |  |
    | Spanish | AnCora | 97.88 | 97.88 | **99.03** | 95.52 | 95.09 |
    | Spanish | GSD | 94.62 | 94.62 | **97.15** | 96.77 | 88.59 |
    | Swedish | LinES | 96.96 | 96.96 | **97.90** | 97.36 | 93.35 |
    | Swedish | Talbanken | 97.69 | 97.69 | 98.40 | **98.45** | 96.55 |
    | Tamil | TTB | 73.87 | 73.87 | 78.97 | **82.12** |  |
    | Telugu | MTG | 92.79 | 92.79 |  | **94.32** |  |
    | Thai | TUD | **77.94** | **77.94** |  |  |  |
    | Turkish | Atis | 98.13 | 98.13 | **98.57** | 81.66 |  |
    | Turkish | BOUN | **88.89** | **88.89** | 84.33 | 82.72 |  |
    | Turkish | FrameNet | 94.89 | 94.89 | **95.64** | 87.53 |  |
    | Turkish | IMST | 85.53 | 85.53 | 92.97 | **94.73** |  |
    | Turkish | Kenet | 91.13 | 91.13 | **92.20** | 81.46 |  |
    | Turkish | Penn | 93.20 | 93.20 | **94.20** | 79.45 |  |
    | Turkish | Tourism | 97.89 | 97.89 | **98.20** | 77.83 |  |
    | Urdu | UDTB | 94.14 | 94.14 | **94.56** | 93.45 |  |
    | Vietnamese | VTB | **77.61** | **77.61** |  | 68.35 |  |
    | Welsh | CCG | 95.27 | 95.27 | **95.84** |  |  |

=== "UFeats"

    | Language | Test set | LIMA | LIMA accurate | Stanza | Trankit | spaCy |
    |:--|:--|--:|--:|--:|--:|--:|
    | Afrikaans | AfriBooms | 96.77 | 96.77 | **98.32** | 98.19 |  |
    | Albanian | STAF | 63.88 | 63.88 | **68.37** |  |  |
    | Arabic | PADT | 77.68 | 77.68 |  | **93.83** |  |
    | Armenian | ArmTDP | 87.20 | 87.20 | **93.31** | 78.50 |  |
    | Armenian | BSUT | 88.61 | 88.61 | **93.17** | 80.44 |  |
    | Basque | BDT | 89.59 | 89.59 | **92.78** | 77.90 |  |
    | Bulgarian | BTB | 96.04 | 96.04 | 97.65 | **98.10** |  |
    | Catalan | AnCora | 97.86 | 97.86 | **98.28** | 93.44 | 96.06 |
    | Czech | CAC | 89.82 | 89.82 | **92.92** | 63.46 |  |
    | Czech | CLTT | 68.98 | 68.98 | **88.15** | 63.42 |  |
    | Czech | FicTree | 75.25 | 75.25 | **95.80** | 75.44 |  |
    | Czech | PDTC | 94.41 | 94.41 | **95.34** | 61.01 |  |
    | Danish | DDT | 95.42 | 95.42 | 97.80 | 97.79 | **98.39** |
    | Dutch | Alpino | 95.21 | 95.21 | **96.45** | 75.72 | 74.92 |
    | Dutch | LassySmall | 95.77 | 95.77 | **97.31** | 76.90 | 75.52 |
    | English | Atis | 96.81 | 96.81 | **98.91** | 79.58 | 79.70 |
    | English | CHILDES | 99.13 | 99.13 | **99.97** | 33.24 | 35.46 |
    | English | ESLSpok | 83.41 | 83.41 | **100.00** | 39.25 | 39.21 |
    | English | EWT | **90.61** | **90.61** |  | 87.22 | 82.05 |
    | English | GUM | 91.67 | 91.67 | **97.75** | 82.77 | 81.09 |
    | English | LinES | 84.32 | 84.32 | **96.96** | 88.27 | 78.87 |
    | English | ParTUT | 78.65 | 78.65 | **95.78** | 94.95 | 77.40 |
    | Estonian | EDT | 95.16 | 95.16 | 95.34 | **95.73** |  |
    | Estonian | EWT | 92.53 | 92.53 | **92.65** | 88.11 |  |
    | Finnish | FTB | 92.85 | 92.85 | **96.68** | 95.26 | 75.73 |
    | Finnish | TDT | 91.69 | 91.69 | **95.89** | 92.96 | 89.91 |
    | French | ALTS | 88.27 | 88.27 | **95.24** | 39.79 | 38.55 |
    | French | GSD | 96.05 | 96.05 | **97.54** | 82.50 | 75.15 |
    | French | ParTUT | 85.00 | 85.00 | **95.63** | 94.70 | 82.68 |
    | French | ParisStories | 94.10 | 94.10 | **95.52** | 79.01 | 73.33 |
    | French | Rhapsodie | 95.09 | 95.09 | **96.07** | 80.80 | 74.87 |
    | French | Sequoia | 92.77 | 92.77 | **97.81** | 82.86 | 77.80 |
    | Galician | CTG | 97.16 | 97.16 | **98.85** | 78.23 |  |
    | German | GSD | 61.93 | 61.93 | **90.53** | 78.43 | 81.08 |
    | German | HDT | 89.26 | 89.26 | **92.47** | 47.93 | 61.27 |
    | Greek | Cretan | **80.12** | **80.12** | 76.27 |  | 60.44 |
    | Greek | GDT | 92.57 | 92.57 |  | **93.16** | 88.33 |
    | Greek | Messinian | **72.69** | **72.69** | 53.20 |  | 62.42 |
    | Hindi | HDTB | 91.43 | 91.43 | **94.37** | 94.04 |  |
    | Hungarian | Szeged | 91.66 | 91.66 | **93.48** | 93.02 |  |
    | Icelandic | GC | 33.81 | 33.81 | **81.12** |  |  |
    | Icelandic | IcePaHC | 88.52 | 88.52 | **90.01** |  |  |
    | Icelandic | Modern | **86.60** | **86.60** | 86.41 |  |  |
    | Irish | IDT | 85.04 | 85.04 | **89.73** | 65.43 |  |
    | Irish | TwittIrish | 97.89 | 97.89 | **98.52** | 31.04 |  |
    | Italian | ISDT | 96.33 | 96.33 | 97.89 | **98.15** | 86.38 |
    | Italian | MarkIT | **85.21** | **85.21** |  | 85.20 | 74.87 |
    | Italian | Old | **84.46** | **84.46** |  | 69.21 | 62.80 |
    | Italian | ParTUT | 95.71 | 95.71 |  | **98.16** | 84.72 |
    | Italian | PoSTWITA | 91.92 | 91.92 |  | **95.55** | 74.64 |
    | Italian | TWITTIRO | 92.99 | 92.99 |  | **93.83** | 80.21 |
    | Italian | VIT | **95.14** | **95.14** |  | 95.07 | 84.58 |
    | Japanese | GSD | 71.50 | 71.50 | 97.50 | 85.48 | **97.92** |
    | Japanese | GSDLUW | 87.77 | 87.77 | **95.53** | 74.73 | 71.41 |
    | Korean | GSD | 99.23 | 99.23 | **99.64** | 98.43 | 98.61 |
    | Korean | KSL | 99.43 | 99.43 | **99.44** | 92.78 | 95.05 |
    | Korean | Kaist | 99.98 | 99.98 | **100.00** | 98.79 | 99.91 |
    | Latin | CIRCSE | **57.39** | **57.39** |  | 15.77 |  |
    | Latin | ITTB | **94.25** | **94.25** |  | 37.90 |  |
    | Latin | LLCT | **95.79** | **95.79** |  | 34.38 |  |
    | Latin | PROIEL | 83.69 | 83.69 |  | **91.28** |  |
    | Latin | UDante | **78.31** | **78.31** |  | 31.61 |  |
    | Latvian | LVTB | 92.96 | 92.96 | **94.80** | 94.59 |  |
    | Lithuanian | ALKSNIS | 87.35 | 87.35 | 86.63 | **91.77** | 84.41 |
    | Lithuanian | HSE | 64.38 | 64.38 | 63.24 | **68.19** | 62.71 |
    | Maltese | MUDT | **99.74** | **99.74** |  |  |  |
    | Marathi | CMUPAN | 66.21 | 66.21 | **71.56** | 28.82 |  |
    | Marathi | UFAL | 45.25 | 45.25 | **70.78** | 56.76 |  |
    | Norwegian | Bokmaal | 95.07 | 95.07 | **96.94** | 93.39 |  |
    | Norwegian | Nynorsk | **94.85** | **94.85** | 59.66 |  |  |
    | Persian | PerDT | 96.52 | 96.52 | **97.31** | 69.63 |  |
    | Persian | Seraji | 96.18 | 96.18 | **97.77** | 95.87 |  |
    | Polish | LFG | 92.25 | 92.25 | **95.70** | 95.06 | 70.14 |
    | Polish | MPDT | 76.06 | 76.06 | **85.44** | 69.58 | 61.03 |
    | Polish | PDB | 89.29 | 89.29 | 94.76 | **96.13** | 89.68 |
    | Portuguese | Bosque | 88.92 | 88.92 | 96.62 | **98.20** | 86.42 |
    | Portuguese | CINTIL | 82.09 | 82.09 | **95.75** | 76.10 | 72.15 |
    | Portuguese | DANTEStocks | 91.70 | 91.70 | **96.92** | 64.85 | 56.96 |
    | Portuguese | GSD | 60.39 | 60.39 | **93.97** | 85.20 | 44.17 |
    | Portuguese | PetroGold | 95.09 | 95.09 | **98.69** | 95.08 | 84.18 |
    | Portuguese | Porttinari | 90.66 | 90.66 | **98.70** | 82.43 | 72.31 |
    | Romanian | Nonstandard | 86.56 | 86.56 | 89.43 | **90.18** | 55.08 |
    | Romanian | RRT | 93.09 | 93.09 | **96.67** | 93.95 | 93.55 |
    | Romanian | SiMoNERo | 95.58 | 95.58 | **97.26** | 61.90 | 61.19 |
    | Russian | GSD | 79.60 | 79.60 | 93.94 | **94.80** | 77.04 |
    | Russian | Poetry | 90.11 | 90.11 | **90.96** | 75.38 | 69.71 |
    | Russian | SynTagRus | 92.99 | 92.99 | **95.62** | 82.90 | 76.79 |
    | Russian | Taiga | 90.54 | 90.54 | **92.53** | 78.66 | 70.40 |
    | Scottish Gaelic | ARCOSG | 86.68 | 86.68 | **91.06** | 56.63 |  |
    | Serbian | SET | 88.50 | 88.50 | 94.60 | **95.89** |  |
    | Slovak | SNK | 85.58 | 85.58 | 90.87 | **94.24** |  |
    | Spanish | AnCora | 96.95 | 96.95 | **98.59** | 92.44 | 93.35 |
    | Spanish | GSD | 94.42 | 94.42 | **96.64** | 95.90 | 88.09 |
    | Swedish | LinES | 90.56 | 90.56 | **92.85** | 89.10 | 84.41 |
    | Swedish | Talbanken | 94.57 | 94.57 | **97.01** | 92.21 | 90.33 |
    | Tamil | TTB | 74.34 | 74.34 | 81.46 | **83.53** |  |
    | Telugu | MTG | **99.45** | **99.45** |  | 97.92 |  |
    | Thai | TUD | **87.59** | **87.59** |  |  |  |
    | Turkish | Atis | 97.94 | 97.94 | **98.38** | 13.74 |  |
    | Turkish | BOUN | **78.02** | **78.02** | 71.62 | 67.98 |  |
    | Turkish | FrameNet | **91.27** | **91.27** | 90.73 | 33.67 |  |
    | Turkish | IMST | 51.40 | 51.40 | 89.93 | **91.58** |  |
    | Turkish | Kenet | 86.68 | 86.68 | **89.80** | 31.18 |  |
    | Turkish | Penn | 92.28 | 92.28 | **93.09** | 31.16 |  |
    | Turkish | Tourism | **94.60** | **94.60** | 94.20 | 41.84 |  |
    | Urdu | UDTB | 82.22 | 82.22 | **84.13** | 83.03 |  |
    | Vietnamese | VTB | 86.83 | 86.83 |  | **92.13** |  |
    | Welsh | CCG | 88.57 | 88.57 | **90.44** |  |  |

=== "Lemmas"

    | Language | Test set | LIMA | LIMA accurate | Stanza | Trankit | spaCy |
    |:--|:--|--:|--:|--:|--:|--:|
    | Afrikaans | AfriBooms | 97.72 | 97.72 | **98.02** | 96.22 |  |
    | Albanian | STAF | 78.71 | 78.71 | **83.45** |  |  |
    | Arabic | PADT | 85.46 | 85.46 |  | **94.65** |  |
    | Armenian | ArmTDP | 92.30 | 92.30 | **96.32** | 88.44 |  |
    | Armenian | BSUT | 94.82 | 94.82 | **96.70** | 92.72 |  |
    | Basque | BDT | 92.97 | 92.97 | **96.19** | 91.28 |  |
    | Bulgarian | BTB | 94.99 | 94.99 | 97.29 | **97.36** |  |
    | Catalan | AnCora | 98.88 | 98.88 | **99.03** | 94.68 | 93.58 |
    | Czech | CAC | 95.65 | 95.65 | 97.41 | **97.66** |  |
    | Czech | CLTT | 85.39 | 85.39 | 96.66 | **97.99** |  |
    | Czech | FicTree | 96.56 | 96.56 | 98.38 | **98.45** |  |
    | Czech | PDTC | 98.39 | 98.39 | **99.04** | 96.82 |  |
    | Danish | DDT | 96.26 | 96.26 | 96.97 | **97.18** | 95.39 |
    | Dutch | Alpino | 93.94 | 93.94 | **94.65** | 90.32 | 90.85 |
    | Dutch | LassySmall | 94.82 | 94.82 | **95.46** | 91.54 | 91.18 |
    | English | Atis | 98.94 | 98.94 | **99.67** | 69.11 | 71.64 |
    | English | CHILDES | 97.24 | 97.24 | **98.30** | 94.81 | 96.56 |
    | English | ESLSpok | 99.87 | 99.87 | **100.00** | 99.60 | 99.14 |
    | English | EWT | **94.23** | **94.23** |  | 93.39 | 93.12 |
    | English | GUM | 96.43 | 96.43 | **98.48** | 94.57 | 96.43 |
    | English | LinES | 95.35 | 95.35 | **98.04** | 95.36 | 94.91 |
    | English | ParTUT | 94.01 | 94.01 | **97.37** | 97.30 | 92.88 |
    | Estonian | EDT | 92.04 | 92.04 | 95.60 | **95.89** |  |
    | Estonian | EWT | 91.38 | 91.38 | **93.42** | 85.46 |  |
    | Finnish | FTB | 88.29 | 88.29 | **96.40** | 95.88 | 79.92 |
    | Finnish | TDT | 86.92 | 86.92 | 94.88 | **95.40** | 80.74 |
    | French | ALTS | 89.75 | 89.75 | **92.83** | 73.34 | 64.68 |
    | French | GSD | 97.64 | 97.64 | **98.28** | 94.96 | 88.79 |
    | French | ParTUT | **97.13** | **97.13** | 97.02 | 94.16 | 90.42 |
    | French | ParisStories | 98.56 | 98.56 | **99.25** | 90.60 | 88.33 |
    | French | Rhapsodie | 98.61 | 98.61 | **99.01** | 91.90 | 90.14 |
    | French | Sequoia | 97.89 | 97.89 | **98.46** | 95.96 | 89.32 |
    | Galician | CTG | 95.38 | 95.38 | **97.91** | 97.90 |  |
    | German | GSD | 91.14 | 91.14 | **97.86** | 95.40 | 79.10 |
    | German | HDT | 97.98 | 97.98 | **98.52** | 77.49 | 80.55 |
    | Greek | Cretan | **84.18** | **84.18** | 81.74 |  | 64.66 |
    | Greek | GDT | 94.56 | 94.56 |  | **96.52** | 86.84 |
    | Greek | Messinian | **85.42** | **85.42** | 71.75 |  | 65.71 |
    | Hindi | HDTB | **98.67** | **98.67** | 96.84 | 83.53 |  |
    | Hungarian | Szeged | 92.65 | 92.65 | 94.06 | **94.15** |  |
    | Icelandic | GC | 86.79 | 86.79 | **89.48** |  |  |
    | Icelandic | IcePaHC | 94.81 | 94.81 | **95.83** |  |  |
    | Icelandic | Modern | 94.14 | 94.14 | **95.50** |  |  |
    | Irish | IDT | 93.13 | 93.13 | **95.34** | 90.51 |  |
    | Irish | TwittIrish | 87.37 | 87.37 | **87.98** | 67.34 |  |
    | Italian | ISDT | 97.76 | 97.76 | 98.06 | **98.29** | 86.36 |
    | Italian | MarkIT | 86.57 | 86.57 |  | **87.25** | 75.97 |
    | Italian | Old | **93.21** | **93.21** |  | 82.96 | 72.56 |
    | Italian | ParTUT | 97.63 | 97.63 |  | **97.94** | 85.06 |
    | Italian | PoSTWITA | 93.53 | 93.53 |  | **95.52** | 74.86 |
    | Italian | TWITTIRO | **93.53** | **93.53** |  | 92.99 | 79.10 |
    | Italian | VIT | 97.88 | 97.88 |  | **97.93** | 87.34 |
    | Japanese | GSD | 69.25 | 69.25 | **96.22** | 74.39 | 83.67 |
    | Japanese | GSDLUW | 84.83 | 84.83 | **94.04** | 66.70 | 64.04 |
    | Korean | GSD | 87.08 | 87.08 | **93.47** | 91.82 | 80.55 |
    | Korean | KSL | 92.06 | 92.06 | **96.09** | 85.13 | 78.98 |
    | Korean | Kaist | 89.43 | 89.43 | **94.15** | 89.74 | 87.10 |
    | Latin | CIRCSE | **80.60** | **80.60** |  | 66.41 |  |
    | Latin | ITTB | **98.28** | **98.28** |  | 93.81 |  |
    | Latin | LLCT | **97.36** | **97.36** |  | 67.98 |  |
    | Latin | PROIEL | 93.05 | 93.05 |  | **96.89** |  |
    | Latin | UDante | **81.35** | **81.35** |  | 72.80 |  |
    | Latvian | LVTB | 95.15 | 95.15 | **96.44** | 95.03 |  |
    | Lithuanian | ALKSNIS | 89.70 | 89.70 | 92.37 | **94.05** | 82.89 |
    | Lithuanian | HSE | 84.52 | 84.52 | **86.24** | 80.07 | 78.63 |
    | Maltese | MUDT |  |  |  |  |  |
    | Marathi | CMUPAN | 87.42 | 87.42 | **90.59** | 52.94 |  |
    | Marathi | UFAL | 72.75 | 72.75 | **82.12** | 71.62 |  |
    | Norwegian | Bokmaal | 97.05 | 97.05 | **98.27** | 96.21 |  |
    | Norwegian | Nynorsk | **93.73** | **93.73** | 80.10 |  |  |
    | Persian | PerDT | 98.22 | 98.22 | **98.48** | 81.76 |  |
    | Persian | Seraji | 98.05 | 98.05 | **98.33** | 89.83 |  |
    | Polish | LFG | 94.20 | 94.20 | **96.68** | 95.37 | 91.85 |
    | Polish | MPDT | 87.72 | 87.72 | **92.24** | 90.89 | 79.61 |
    | Polish | PDB | 95.31 | 95.31 | 97.19 | **97.44** | 93.14 |
    | Portuguese | Bosque | 94.01 | 94.01 | 98.08 | **99.01** | 87.04 |
    | Portuguese | CINTIL | 77.69 | 77.69 | **98.55** | 71.90 | 70.63 |
    | Portuguese | DANTEStocks | 94.18 | 94.18 | **97.13** | 78.67 | 74.00 |
    | Portuguese | GSD | **97.24** | **97.24** | 96.08 | 94.00 | 87.03 |
    | Portuguese | PetroGold | 97.42 | 97.42 | **99.09** | 96.68 | 84.37 |
    | Portuguese | Porttinari | 94.86 | 94.86 | **99.15** | 97.34 | 86.70 |
    | Romanian | Nonstandard | 92.89 | 92.89 | **94.81** | 94.57 | 68.95 |
    | Romanian | RRT | 95.28 | 95.28 | **97.21** | 97.11 | 95.17 |
    | Romanian | SiMoNERo | 97.05 | 97.05 | **98.67** | 96.70 | 94.43 |
    | Russian | GSD | 94.14 | 94.14 | 95.28 | **95.47** | 81.90 |
    | Russian | Poetry | 92.75 | 92.75 | **93.79** | 87.74 | 84.69 |
    | Russian | SynTagRus | 96.90 | 96.90 | **97.77** | 96.61 | 87.55 |
    | Russian | Taiga | 93.23 | 93.23 | **95.97** | 90.54 | 82.81 |
    | Scottish Gaelic | ARCOSG | 94.57 | 94.57 | **96.23** | 84.67 |  |
    | Serbian | SET | 94.12 | 94.12 | 96.36 | **96.90** |  |
    | Slovak | SNK | 90.36 | 90.36 | 94.38 | **94.81** |  |
    | Spanish | AnCora | 98.56 | 98.56 | **99.29** | 96.13 | 95.49 |
    | Spanish | GSD | 95.79 | 95.79 | **97.83** | 91.37 | 91.56 |
    | Swedish | LinES | 96.59 | 96.59 | **96.96** | 96.22 | 89.02 |
    | Swedish | Talbanken | 97.00 | 97.00 | **97.99** | 93.79 | 91.62 |
    | Tamil | TTB | 77.90 | 77.90 | 83.85 | **87.87** |  |
    | Telugu | MTG |  |  |  | 98.89 |  |
    | Thai | TUD | **87.59** | **87.59** |  |  |  |
    | Turkish | Atis | 98.77 | 98.77 | **98.86** | 60.90 |  |
    | Turkish | BOUN | 85.35 | 85.35 | 86.27 | **87.50** |  |
    | Turkish | FrameNet | 91.41 | 91.41 | **94.68** | 88.41 |  |
    | Turkish | IMST | 83.93 | 83.93 | 94.00 | **97.20** |  |
    | Turkish | Kenet | 89.43 | 89.43 | **93.32** | 85.43 |  |
    | Turkish | Penn | 91.72 | 91.72 | **93.70** | 79.67 |  |
    | Turkish | Tourism | 97.43 | 97.43 | **98.85** | 80.49 |  |
    | Urdu | UDTB | **97.20** | **97.20** | 95.49 | 84.20 |  |
    | Vietnamese | VTB | 85.01 | 85.01 |  | **89.19** |  |
    | Welsh | CCG | 93.67 | 93.67 | **94.85** |  |  |

=== "Words"

    | Language | Test set | LIMA | LIMA accurate | Stanza | Trankit | spaCy |
    |:--|:--|--:|--:|--:|--:|--:|
    | Afrikaans | AfriBooms | 99.96 | 99.96 | **99.99** | 99.72 |  |
    | Albanian | STAF | **98.92** | **98.92** | 98.52 |  |  |
    | Arabic | PADT | 93.11 | 93.11 |  | **99.22** |  |
    | Armenian | ArmTDP | 98.75 | 98.75 | **99.55** | 95.98 |  |
    | Armenian | BSUT | 99.62 | 99.62 | **99.87** | 98.13 |  |
    | Basque | BDT | **99.99** | **99.99** | 99.98 | 99.79 |  |
    | Bulgarian | BTB | 99.92 | 99.92 | **99.93** | 99.84 |  |
    | Catalan | AnCora | 99.80 | 99.80 | **99.95** | 95.60 | 96.73 |
    | Czech | CAC | 99.94 | 99.94 | 99.58 | **99.95** |  |
    | Czech | CLTT | 86.96 | 86.96 | **99.47** | 99.22 |  |
    | Czech | FicTree | 99.93 | 99.93 | **99.97** | **99.97** |  |
    | Czech | PDTC | 99.96 | 99.96 | **99.97** | 99.95 |  |
    | Danish | DDT | 99.94 | 99.94 | **99.97** | 99.79 | 99.74 |
    | Dutch | Alpino | 99.30 | 99.30 | 99.70 | 99.22 | **99.82** |
    | Dutch | LassySmall | 99.87 | 99.87 | **99.91** | 99.35 | 99.79 |
    | English | Atis | 99.95 | 99.95 | **100.00** | 99.93 | 99.50 |
    | English | CHILDES | 99.48 | 99.48 | **99.97** | 97.40 | 99.08 |
    | English | ESLSpok | 99.87 | 99.87 | **100.00** | 99.60 | 99.14 |
    | English | EWT | 98.17 | 98.17 |  | **98.41** | 97.41 |
    | English | GUM | 98.85 | 98.85 | **99.73** | 98.16 | 99.47 |
    | English | LinES | 98.67 | 98.67 | **99.96** | 99.55 | 98.70 |
    | English | ParTUT | 99.40 | 99.40 | 99.43 | 99.27 | **99.84** |
    | Estonian | EDT | 99.91 | 99.91 | **99.95** | 99.73 |  |
    | Estonian | EWT | **98.60** | **98.60** | 98.49 | 97.01 |  |
    | Finnish | FTB | 99.94 | 99.94 | **99.99** | 99.74 | 99.90 |
    | Finnish | TDT | 99.55 | 99.55 | **99.74** | 99.62 | 98.91 |
    | French | ALTS | 97.72 | 97.72 | **98.88** | 97.14 | 92.49 |
    | French | GSD | 99.47 | 99.47 | **99.59** | 99.58 | 94.64 |
    | French | ParTUT | 99.40 | 99.40 | 99.37 | **99.69** | 94.49 |
    | French | ParisStories | 99.52 | 99.52 | **99.72** | 99.31 | 97.74 |
    | French | Rhapsodie | 99.57 | 99.57 | **99.68** | 99.27 | 97.51 |
    | French | Sequoia | 99.44 | 99.44 | 99.52 | **99.67** | 94.05 |
    | Galician | CTG | 98.31 | 98.31 | **99.49** | 99.26 |  |
    | German | GSD | 94.63 | 94.63 | 99.76 | **99.78** | 95.51 |
    | German | HDT | 99.90 | 99.90 | **100.00** | 97.44 | 97.13 |
    | Greek | Cretan | **100.00** | **100.00** | **100.00** |  | 97.19 |
    | Greek | GDT | 99.69 | 99.69 |  | **99.75** | 96.26 |
    | Greek | Messinian | 98.15 | 98.15 | **99.38** |  | 98.15 |
    | Hindi | HDTB | 99.99 | 99.99 | **100.00** | 99.89 |  |
    | Hungarian | Szeged | 99.79 | 99.79 | **99.88** | 99.41 |  |
    | Icelandic | GC | 99.62 | 99.62 | **99.70** |  |  |
    | Icelandic | IcePaHC | 99.77 | 99.77 | **99.85** |  |  |
    | Icelandic | Modern | 99.02 | 99.02 | **99.55** |  |  |
    | Irish | IDT | 99.32 | 99.32 | **99.90** | 99.29 |  |
    | Irish | TwittIrish | 97.98 | 97.98 | **98.52** | 79.26 |  |
    | Italian | ISDT | 99.62 | 99.62 | 99.70 | **99.87** | 88.89 |
    | Italian | MarkIT | 99.32 | 99.32 |  | **99.83** | 88.66 |
    | Italian | Old | **98.21** | **98.21** |  | 96.49 | 90.09 |
    | Italian | ParTUT | 99.56 | 99.56 |  | **99.81** | 87.27 |
    | Italian | PoSTWITA | 98.45 | 98.45 |  | **99.02** | 85.23 |
    | Italian | TWITTIRO | **99.01** | **99.01** |  | **99.01** | 88.71 |
    | Italian | VIT | 99.52 | 99.52 |  | **99.65** | 90.30 |
    | Japanese | GSD | 71.63 | 71.63 | 97.52 | 87.85 | **97.96** |
    | Japanese | GSDLUW | 87.88 | 87.88 | **95.54** | 75.51 | 71.43 |
    | Korean | GSD | **99.90** | **99.90** | 99.86 | 98.63 | 99.28 |
    | Korean | KSL | 99.80 | 99.80 | **99.83** | 97.85 | 99.70 |
    | Korean | Kaist | 99.99 | 99.99 | **100.00** | 98.79 | 99.91 |
    | Latin | CIRCSE | **95.82** | **95.82** |  | 95.06 |  |
    | Latin | ITTB | 99.95 | 99.95 |  | **99.98** |  |
    | Latin | LLCT | **99.95** | **99.95** |  | 99.72 |  |
    | Latin | PROIEL | **99.96** | **99.96** |  | 99.82 |  |
    | Latin | UDante | 98.31 | 98.31 |  | **98.71** |  |
    | Latvian | LVTB | 99.75 | 99.75 | **99.82** | 99.73 |  |
    | Lithuanian | ALKSNIS | **99.97** | **99.97** | 99.96 | 99.82 | 99.70 |
    | Lithuanian | HSE | 99.29 | 99.29 | 98.96 | 98.22 | **99.44** |
    | Maltese | MUDT | **99.74** | **99.74** |  |  |  |
    | Marathi | CMUPAN | **100.00** | **100.00** | **100.00** | 72.28 |  |
    | Marathi | UFAL | 90.25 | 90.25 | **91.74** | 80.63 |  |
    | Norwegian | Bokmaal | 99.87 | 99.87 | 99.87 | **99.88** |  |
    | Norwegian | Nynorsk | 99.93 | 99.93 | **99.95** |  |  |
    | Persian | PerDT | 99.37 | 99.37 | **99.47** | 98.41 |  |
    | Persian | Seraji | 99.57 | 99.57 | **99.69** | 99.11 |  |
    | Polish | LFG | 98.87 | 98.87 | **99.94** | 98.32 | 97.54 |
    | Polish | MPDT | 97.18 | 97.18 | **98.27** | 97.61 | 93.08 |
    | Polish | PDB | 99.31 | 99.31 | 99.83 | **99.89** | 98.51 |
    | Portuguese | Bosque | 99.26 | 99.26 | 99.72 | **99.89** | 88.51 |
    | Portuguese | CINTIL | 92.40 | 92.40 | **99.92** | 91.97 | 88.38 |
    | Portuguese | DANTEStocks | 98.90 | 98.90 | **99.51** | 84.69 | 83.42 |
    | Portuguese | GSD | 99.41 | 99.41 | **99.74** | 97.72 | 88.88 |
    | Portuguese | PetroGold | 99.21 | 99.21 | **99.67** | 98.76 | 87.68 |
    | Portuguese | Porttinari | 99.48 | 99.48 | **99.89** | 99.73 | 89.91 |
    | Romanian | Nonstandard | 98.75 | 98.75 | **98.87** | 98.68 | 98.41 |
    | Romanian | RRT | 98.75 | 98.75 | 99.25 | **99.42** | 99.15 |
    | Romanian | SiMoNERo | 98.96 | 98.96 | **99.73** | 99.05 | 98.91 |
    | Russian | GSD | 98.85 | 98.85 | **99.71** | 99.63 | 94.66 |
    | Russian | Poetry | 99.70 | 99.70 | **99.82** | 94.99 | 98.35 |
    | Russian | SynTagRus | 99.61 | 99.61 | **99.75** | 99.74 | 98.38 |
    | Russian | Taiga | 97.80 | 97.80 | **99.14** | 99.07 | 96.85 |
    | Scottish Gaelic | ARCOSG | 98.67 | 98.67 | **98.89** | 93.64 |  |
    | Serbian | SET | 99.99 | 99.99 | **100.00** | 99.91 |  |
    | Slovak | SNK | 99.95 | 99.95 | **99.98** | 99.86 |  |
    | Spanish | AnCora | 99.75 | 99.75 | **99.97** | 97.09 | 96.60 |
    | Spanish | GSD | 99.49 | 99.49 | **99.85** | 99.82 | 96.47 |
    | Swedish | LinES | 99.86 | 99.86 | **99.94** | 99.74 | 99.82 |
    | Swedish | Talbanken | 99.95 | 99.95 | **99.98** | 99.87 | 99.57 |
    | Tamil | TTB | 84.98 | 84.98 | 91.32 | **93.47** |  |
    | Telugu | MTG | **100.00** | **100.00** |  | 98.89 |  |
    | Thai | TUD | **87.59** | **87.59** |  |  |  |
    | Turkish | Atis | **100.00** | **100.00** | **100.00** | 94.07 |  |
    | Turkish | BOUN | **96.37** | **96.37** | 95.83 | 96.24 |  |
    | Turkish | FrameNet | **100.00** | **100.00** | **100.00** | 97.15 |  |
    | Turkish | IMST | 94.21 | 94.21 | 97.75 | **98.93** |  |
    | Turkish | Kenet | **100.00** | **100.00** | 99.99 | 96.19 |  |
    | Turkish | Penn | 99.84 | 99.84 | **100.00** | 93.00 |  |
    | Turkish | Tourism | **100.00** | **100.00** | **100.00** | 86.75 |  |
    | Urdu | UDTB | **100.00** | **100.00** | **100.00** | 99.66 |  |
    | Vietnamese | VTB | 86.83 | 86.83 |  | **95.94** |  |
    | Welsh | CCG | 99.41 | 99.41 | **99.66** |  |  |

=== "Sentences"

    | Language | Test set | LIMA | LIMA accurate | Stanza | Trankit | spaCy |
    |:--|:--|--:|--:|--:|--:|--:|
    | Afrikaans | AfriBooms | 99.65 | 99.65 | 99.65 | **100.00** |  |
    | Albanian | STAF | 85.71 | 85.71 | **92.68** |  |  |
    | Arabic | PADT | 79.09 | 79.09 |  | **96.59** |  |
    | Armenian | ArmTDP | 99.25 | 99.25 | **100.00** | 99.75 |  |
    | Armenian | BSUT | 97.71 | 97.71 | 97.10 | **97.96** |  |
    | Basque | BDT | 99.75 | 99.75 | **100.00** | 99.83 |  |
    | Bulgarian | BTB | 95.40 | 95.40 | 96.27 | **98.25** |  |
    | Catalan | AnCora | 99.57 | 99.57 | 99.16 | **99.89** | 99.40 |
    | Czech | CAC | 99.28 | 99.28 | **100.00** | **100.00** |  |
    | Czech | CLTT | 61.88 | 61.88 | **99.56** | 99.11 |  |
    | Czech | FicTree | 83.66 | 83.66 | 99.07 | **99.38** |  |
    | Czech | PDTC | 92.84 | 92.84 | 95.23 | **97.14** |  |
    | Danish | DDT | 90.91 | 90.91 | 91.90 | **95.19** | 91.27 |
    | Dutch | Alpino | **90.01** | **90.01** | 90.00 | 89.88 | 84.42 |
    | Dutch | LassySmall | 82.87 | 82.87 | 87.12 | **94.12** | 81.20 |
    | English | Atis | 64.24 | 64.24 | **85.25** | 45.51 | 2.74 |
    | English | CHILDES | 99.83 | 99.83 | **100.00** | 98.12 | 99.08 |
    | English | ESLSpok | 87.53 | 87.53 | **91.70** | 91.23 | 88.55 |
    | English | EWT | 84.83 | 84.83 |  | **91.02** | 85.14 |
    | English | GUM | 93.54 | 93.54 | **96.77** | 95.49 | 94.13 |
    | English | LinES | 87.62 | 87.62 | 89.46 | **93.14** | 87.33 |
    | English | ParTUT | 98.04 | 98.04 | **100.00** | **100.00** | 94.81 |
    | Estonian | EDT | 90.15 | 90.15 | 92.62 | **96.56** |  |
    | Estonian | EWT | **86.67** | **86.67** | 68.51 | 76.45 |  |
    | Finnish | FTB | 87.49 | 87.49 | 90.18 | **95.83** | 81.50 |
    | Finnish | TDT | 90.22 | 90.22 | 92.47 | **95.98** | 85.96 |
    | French | ALTS | 66.47 | 66.47 | **80.50** | 69.29 | 49.24 |
    | French | GSD | 94.67 | 94.67 | 95.07 | **96.63** | 85.45 |
    | French | ParTUT | 94.64 | 94.64 | **100.00** | 98.63 | 89.96 |
    | French | ParisStories | 93.82 | 93.82 | **98.28** | 86.57 | 89.78 |
    | French | Rhapsodie | 98.39 | 98.39 | **99.64** | 86.30 | 88.42 |
    | French | Sequoia | 80.52 | 80.52 | 89.71 | **94.36** | 88.75 |
    | Galician | CTG | 94.83 | 94.83 | **99.48** | 98.09 |  |
    | German | GSD | 74.29 | 74.29 | 85.21 | **92.72** | 90.51 |
    | German | HDT | 87.66 | 87.66 | 96.44 | **99.50** | 86.92 |
    | Greek | Cretan | 95.08 | 95.08 | **100.00** |  | 76.92 |
    | Greek | GDT | 91.11 | 91.11 |  | **93.57** | 88.77 |
    | Greek | Messinian | **100.00** | **100.00** | 85.25 |  | 94.92 |
    | Hindi | HDTB | 98.99 | 98.99 | 99.29 | **99.64** |  |
    | Hungarian | Szeged | 95.96 | 95.96 | **98.77** | 98.00 |  |
    | Icelandic | GC | 94.54 | 94.54 | **95.92** |  |  |
    | Icelandic | IcePaHC | 91.61 | 91.61 | **93.36** |  |  |
    | Icelandic | Modern | 86.47 | 86.47 | **96.21** |  |  |
    | Irish | IDT | 88.24 | 88.24 | **97.24** | **97.24** |  |
    | Irish | TwittIrish | 37.02 | 37.02 | **54.52** | 3.29 |  |
    | Italian | ISDT | 98.75 | 98.75 | **98.76** | **98.76** | 95.60 |
    | Italian | MarkIT | 97.78 | 97.78 |  | **99.12** | 93.66 |
    | Italian | Old | **97.20** | **97.20** |  | 34.35 | 32.05 |
    | Italian | ParTUT | **100.00** | **100.00** |  | **100.00** | 97.09 |
    | Italian | PoSTWITA | 26.28 | 26.28 |  | **69.84** | 2.99 |
    | Italian | TWITTIRO | 19.89 | 19.89 |  | **56.00** | 2.05 |
    | Italian | VIT | 93.59 | 93.59 |  | **94.47** | 76.11 |
    | Japanese | GSD | 96.43 | 96.43 | 99.36 | **99.45** | 92.57 |
    | Japanese | GSDLUW | 96.43 | 96.43 | **99.45** | **99.45** | 92.57 |
    | Korean | GSD | 87.22 | 87.22 | 95.82 | **97.67** | 68.93 |
    | Korean | KSL | 98.92 | 98.92 | **99.31** | 98.46 | 98.35 |
    | Korean | Kaist | **100.00** | **100.00** | 99.93 | 99.14 | 98.67 |
    | Latin | CIRCSE | **0.00** | **0.00** |  | **0.00** |  |
    | Latin | ITTB | 85.89 | 85.89 |  | **94.29** |  |
    | Latin | LLCT | 96.50 | 96.50 |  | **97.79** |  |
    | Latin | PROIEL | 6.12 | 6.12 |  | **58.10** |  |
    | Latin | UDante | 47.58 | 47.58 |  | **52.94** |  |
    | Latvian | LVTB | 98.20 | 98.20 | 98.16 | **98.35** |  |
    | Lithuanian | ALKSNIS | 87.10 | 87.10 | 88.89 | **95.10** | 81.18 |
    | Lithuanian | HSE | 94.64 | 94.64 | **96.36** | 94.55 | 95.58 |
    | Maltese | MUDT | **83.27** | **83.27** |  |  |  |
    | Marathi | CMUPAN | 88.92 | 88.92 | **91.08** | 75.49 |  |
    | Marathi | UFAL | 82.47 | 82.47 | **88.17** | 60.87 |  |
    | Norwegian | Bokmaal | 98.01 | 98.01 | 98.01 | **99.20** |  |
    | Norwegian | Nynorsk | 93.86 | 93.86 | **93.94** |  |  |
    | Persian | PerDT | 99.52 | 99.52 | 99.62 | **99.73** |  |
    | Persian | Seraji | 96.11 | 96.11 | **99.25** | **99.25** |  |
    | Polish | LFG | 98.87 | 98.87 | **99.91** | **99.91** | 98.18 |
    | Polish | MPDT | 80.86 | 80.86 | **92.71** | 89.20 | 74.40 |
    | Polish | PDB | 95.99 | 95.99 | 98.14 | **98.53** | 93.86 |
    | Portuguese | Bosque | 91.86 | 91.86 | 91.93 | **98.11** | 90.94 |
    | Portuguese | CINTIL | 85.53 | 85.53 | 83.95 | **93.38** | 82.08 |
    | Portuguese | DANTEStocks | 39.97 | 39.97 | **82.60** | 6.48 | 2.55 |
    | Portuguese | GSD | 92.31 | 92.31 | **97.71** | 97.21 | 82.35 |
    | Portuguese | PetroGold | 88.38 | 88.38 | **96.32** | 91.90 | 75.63 |
    | Portuguese | Porttinari | 98.32 | 98.32 | **100.00** | 98.40 | 90.47 |
    | Romanian | Nonstandard | 96.81 | 96.81 | 97.81 | **98.57** | 84.43 |
    | Romanian | RRT | 91.13 | 91.13 | 95.69 | **97.60** | 96.30 |
    | Romanian | SiMoNERo | 99.69 | 99.69 | **100.00** | 99.39 | 93.93 |
    | Russian | GSD | 94.70 | 94.70 | 96.98 | **98.25** | 88.18 |
    | Russian | Poetry | 82.61 | 82.61 | **97.66** | 85.23 | 93.19 |
    | Russian | SynTagRus | 97.53 | 97.53 | 98.61 | **98.73** | 91.70 |
    | Russian | Taiga | 87.44 | 87.44 | 87.36 | **91.24** | 81.95 |
    | Scottish Gaelic | ARCOSG | 58.01 | 58.01 | **66.28** | 54.15 |  |
    | Serbian | SET | 91.45 | 91.45 | 99.33 | **99.71** |  |
    | Slovak | SNK | 75.72 | 75.72 | 85.67 | **95.49** |  |
    | Spanish | AnCora | 97.85 | 97.85 | 98.95 | **99.39** | 98.78 |
    | Spanish | GSD | 93.32 | 93.32 | 96.10 | **99.30** | 89.90 |
    | Swedish | LinES | 87.16 | 87.16 | 88.69 | **90.80** | 76.93 |
    | Swedish | Talbanken | 93.90 | 93.90 | 98.28 | **99.38** | 93.52 |
    | Tamil | TTB | 88.00 | 88.00 | **100.00** | **100.00** |  |
    | Telugu | MTG | 97.93 | 97.93 |  | **98.62** |  |
    | Thai | TUD | **8.71** | **8.71** |  |  |  |
    | Turkish | Atis | 10.17 | 10.17 | **87.97** | 0.00 |  |
    | Turkish | BOUN | **80.42** | **80.42** | 79.36 | 79.32 |  |
    | Turkish | FrameNet | **100.00** | **100.00** | **100.00** | **100.00** |  |
    | Turkish | IMST | 91.57 | 91.57 | 97.56 | **98.05** |  |
    | Turkish | Kenet | 93.71 | 93.71 | **98.63** | 93.16 |  |
    | Turkish | Penn | 84.27 | 84.27 | **94.13** | 80.63 |  |
    | Turkish | Tourism | **100.00** | **100.00** | **100.00** | 99.93 |  |
    | Urdu | UDTB | 98.32 | 98.32 | **98.60** | 98.32 |  |
    | Vietnamese | VTB | 98.69 | 98.69 |  | **99.06** |  |
    | Welsh | CCG | 97.73 | 97.73 | **98.26** |  |  |

=== "Tokens"

    | Language | Test set | LIMA | LIMA accurate | Stanza | Trankit | spaCy |
    |:--|:--|--:|--:|--:|--:|--:|
    | Afrikaans | AfriBooms | 99.96 | 99.96 | **99.99** | 99.72 |  |
    | Albanian | STAF | 99.59 | 99.59 | **100.00** |  |  |
    | Arabic | PADT | **99.99** | **99.99** |  | 99.93 |  |
    | Armenian | ArmTDP | 99.55 | 99.55 | **99.67** | 96.59 |  |
    | Armenian | BSUT | 99.87 | 99.87 | **99.91** | 98.25 |  |
    | Basque | BDT | **99.99** | **99.99** | 99.98 | 99.79 |  |
    | Bulgarian | BTB | 99.92 | 99.92 | **99.93** | 99.84 |  |
    | Catalan | AnCora | **99.99** | **99.99** | 99.96 | 99.68 | 95.34 |
    | Czech | CAC | 99.94 | 99.94 | 99.83 | **99.96** |  |
    | Czech | CLTT | 86.96 | 86.96 | **99.47** | 99.32 |  |
    | Czech | FicTree | **99.98** | **99.98** | **99.98** | 99.97 |  |
    | Czech | PDTC | 99.96 | 99.96 | **99.98** | 99.95 |  |
    | Danish | DDT | 99.94 | 99.94 | **99.97** | 99.79 | 99.74 |
    | Dutch | Alpino | 99.30 | 99.30 | 99.70 | 99.22 | **99.82** |
    | Dutch | LassySmall | 99.87 | 99.87 | **99.91** | 99.35 | 99.79 |
    | English | Atis | 99.95 | 99.95 | **100.00** | 98.88 | 98.40 |
    | English | CHILDES | 99.98 | 99.98 | **99.99** | 93.28 | 91.37 |
    | English | ESLSpok | 99.87 | 99.87 | **100.00** | 99.60 | 99.14 |
    | English | EWT | **98.59** | **98.59** |  | 97.78 | 95.64 |
    | English | GUM | 99.22 | 99.22 | **99.74** | 97.00 | 97.34 |
    | English | LinES | 98.91 | 98.91 | **99.97** | 98.58 | 96.99 |
    | English | ParTUT | 99.53 | 99.53 | **99.57** | 98.94 | 99.13 |
    | Estonian | EDT | 99.91 | 99.91 | **99.95** | 99.73 |  |
    | Estonian | EWT | **98.60** | **98.60** | 98.49 | 97.01 |  |
    | Finnish | FTB | 99.96 | 99.96 | **100.00** | 99.75 | 99.71 |
    | Finnish | TDT | 99.69 | 99.69 | **99.77** | 99.62 | 98.90 |
    | French | ALTS | 99.00 | 99.00 | **99.70** | 98.82 | 94.91 |
    | French | GSD | 99.79 | 99.79 | **99.85** | 99.62 | 98.85 |
    | French | ParTUT | 99.62 | 99.62 | **99.94** | 99.74 | 99.70 |
    | French | ParisStories | 99.77 | 99.77 | **99.84** | 99.38 | 99.41 |
    | French | Rhapsodie | 99.84 | 99.84 | **99.92** | 99.36 | 99.22 |
    | French | Sequoia | 99.72 | 99.72 | **99.85** | 99.67 | 98.70 |
    | Galician | CTG | 99.75 | 99.75 | **99.89** | 99.76 |  |
    | German | GSD | 94.57 | 94.57 | 99.77 | **99.78** | 98.03 |
    | German | HDT | 99.92 | 99.92 | **100.00** | 99.90 | 99.59 |
    | Greek | Cretan | **100.00** | **100.00** | **100.00** |  | 97.19 |
    | Greek | GDT | 99.68 | 99.68 |  | 99.75 | **99.81** |
    | Greek | Messinian | 98.15 | 98.15 | **99.38** |  | 98.15 |
    | Hindi | HDTB | 99.99 | 99.99 | **100.00** | 99.89 |  |
    | Hungarian | Szeged | 99.79 | 99.79 | **99.88** | 99.41 |  |
    | Icelandic | GC | 99.65 | 99.65 | **99.70** |  |  |
    | Icelandic | IcePaHC | 99.85 | 99.85 | **99.90** |  |  |
    | Icelandic | Modern | 99.02 | 99.02 | **99.55** |  |  |
    | Irish | IDT | 99.32 | 99.32 | **99.90** | 99.29 |  |
    | Irish | TwittIrish | 97.98 | 97.98 | **98.52** | 79.26 |  |
    | Italian | ISDT | **99.88** | **99.88** | 99.85 | **99.88** | **99.88** |
    | Italian | MarkIT | **99.90** | **99.90** |  | **99.90** | 95.76 |
    | Italian | Old | **99.65** | **99.65** |  | 97.80 | 93.60 |
    | Italian | ParTUT | 99.66 | 99.66 |  | **99.82** | 99.66 |
    | Italian | PoSTWITA | 98.98 | 98.98 |  | **99.23** | 91.53 |
    | Italian | TWITTIRO | **99.44** | **99.44** |  | 99.22 | 93.63 |
    | Italian | VIT | **99.93** | **99.93** |  | 99.84 | 99.91 |
    | Japanese | GSD | 71.63 | 71.63 | 97.52 | 87.85 | **97.96** |
    | Japanese | GSDLUW | 87.88 | 87.88 | **95.54** | 75.51 | 71.43 |
    | Korean | GSD | **99.90** | **99.90** | 99.86 | 98.63 | 99.28 |
    | Korean | KSL | 99.80 | 99.80 | **99.83** | 97.85 | 99.70 |
    | Korean | Kaist | 99.99 | 99.99 | **100.00** | 98.79 | 99.91 |
    | Latin | CIRCSE | **100.00** | **100.00** |  | 99.15 |  |
    | Latin | ITTB | 99.96 | 99.96 |  | **99.99** |  |
    | Latin | LLCT | **99.96** | **99.96** |  | 99.74 |  |
    | Latin | PROIEL | **99.96** | **99.96** |  | 99.82 |  |
    | Latin | UDante | 99.11 | 99.11 |  | **99.44** |  |
    | Latvian | LVTB | 99.75 | 99.75 | **99.82** | 99.73 |  |
    | Lithuanian | ALKSNIS | **99.97** | **99.97** | 99.96 | 99.82 | 99.70 |
    | Lithuanian | HSE | 99.29 | 99.29 | 98.96 | 98.22 | **99.44** |
    | Maltese | MUDT | **99.74** | **99.74** |  |  |  |
    | Marathi | CMUPAN | **100.00** | **100.00** | **100.00** | 94.85 |  |
    | Marathi | UFAL | **99.60** | **99.60** | 97.59 | 99.20 |  |
    | Norwegian | Bokmaal | 99.87 | 99.87 | 99.87 | **99.88** |  |
    | Norwegian | Nynorsk | 99.93 | 99.93 | **99.95** |  |  |
    | Persian | PerDT | 99.91 | 99.91 | **99.94** | 98.61 |  |
    | Persian | Seraji | 99.99 | 99.99 | **100.00** | 99.22 |  |
    | Polish | LFG | 98.00 | 98.00 | **99.94** | 98.32 | 97.54 |
    | Polish | MPDT | **98.90** | **98.90** | 98.86 | 98.68 | 95.21 |
    | Polish | PDB | 99.74 | 99.74 | 99.85 | **99.90** | 99.65 |
    | Portuguese | Bosque | 99.80 | 99.80 | 99.81 | **99.92** | 99.87 |
    | Portuguese | CINTIL | 99.86 | 99.86 | **100.00** | 99.45 | 99.93 |
    | Portuguese | DANTEStocks | 98.96 | 98.96 | **99.60** | 84.08 | 89.70 |
    | Portuguese | GSD | 99.84 | 99.84 | **99.89** | 99.57 | 99.68 |
    | Portuguese | PetroGold | 99.58 | 99.58 | **99.72** | 98.85 | 99.02 |
    | Portuguese | Porttinari | 99.94 | 99.94 | **99.96** | 99.87 | 99.72 |
    | Romanian | Nonstandard | 98.75 | 98.75 | **98.87** | 98.68 | 98.41 |
    | Romanian | RRT | 98.75 | 98.75 | 99.25 | **99.42** | 99.15 |
    | Romanian | SiMoNERo | 98.96 | 98.96 | **99.73** | 99.05 | 98.91 |
    | Russian | GSD | 98.85 | 98.85 | **99.71** | 99.63 | 94.66 |
    | Russian | Poetry | 99.70 | 99.70 | **99.82** | 94.99 | 98.35 |
    | Russian | SynTagRus | 99.61 | 99.61 | **99.75** | 99.74 | 98.38 |
    | Russian | Taiga | 97.80 | 97.80 | **99.14** | 99.07 | 96.85 |
    | Scottish Gaelic | ARCOSG | **99.72** | **99.72** | 99.70 | 99.03 |  |
    | Serbian | SET | 99.99 | 99.99 | **100.00** | 99.91 |  |
    | Slovak | SNK | 99.95 | 99.95 | **99.98** | 99.91 |  |
    | Spanish | AnCora | 99.98 | 99.98 | **99.99** | 99.87 | 99.90 |
    | Spanish | GSD | 99.89 | 99.89 | **99.93** | 99.91 | 99.85 |
    | Swedish | LinES | 99.86 | 99.86 | **99.94** | 99.74 | 99.82 |
    | Swedish | Talbanken | 99.95 | 99.95 | **99.98** | 99.87 | 99.57 |
    | Tamil | TTB | 98.23 | 98.23 | **99.63** | 98.10 |  |
    | Telugu | MTG | **100.00** | **100.00** |  | 98.89 |  |
    | Thai | TUD | **87.59** | **87.59** |  |  |  |
    | Turkish | Atis | **100.00** | **100.00** | **100.00** | 99.03 |  |
    | Turkish | BOUN | 98.71 | 98.71 | 98.94 | **99.18** |  |
    | Turkish | FrameNet | **100.00** | **100.00** | **100.00** | 99.28 |  |
    | Turkish | IMST | 98.37 | 98.37 | **99.90** | 99.75 |  |
    | Turkish | Kenet | **100.00** | **100.00** | 99.99 | 99.71 |  |
    | Turkish | Penn | 99.84 | 99.84 | **100.00** | 97.44 |  |
    | Turkish | Tourism | **100.00** | **100.00** | **100.00** | 99.87 |  |
    | Urdu | UDTB | **100.00** | **100.00** | **100.00** | 99.66 |  |
    | Vietnamese | VTB | 86.83 | 86.83 |  | **95.94** |  |
    | Welsh | CCG | 99.84 | 99.84 | **99.91** |  |  |

=== "Speed (words/s)"

    | Language | Test set | LIMA | LIMA accurate | Stanza | Trankit | spaCy |
    |:--|:--|--:|--:|--:|--:|--:|
    | Afrikaans | AfriBooms | n/r | 7 630 | 960 | 634 |  |
    | Albanian | STAF | n/r | n/r | n/r |  |  |
    | Arabic | PADT | 7 729 | 4 596 |  | 242 |  |
    | Armenian | ArmTDP | n/r | 2 281 | 1 683 | 191 |  |
    | Armenian | BSUT | n/r | 4 475 | 1 651 | 193 |  |
    | Basque | BDT | 2 575 | 2 148 | 2 087 | 540 |  |
    | Bulgarian | BTB | n/r | n/r | 911 | 497 |  |
    | Catalan | AnCora | 4 598 | 3 569 | 2 040 | 620 | 1 745 |
    | Czech | CAC | n/r | n/r | 1 791 | 519 |  |
    | Czech | CLTT | n/r | 2 927 | 1 471 | 309 |  |
    | Czech | FicTree | n/r | 5 253 | 1 948 | 413 |  |
    | Czech | PDTC | 4 438 | 3 043 | 2 240 | 348 |  |
    | Danish | DDT | 2 729 | 1 654 | 868 | 592 | 1 867 |
    | Dutch | Alpino | n/r | n/r | 882 | 570 | n/r |
    | Dutch | LassySmall | n/r | n/r | 929 | 547 | 15 862 |
    | English | Atis | n/r | n/r | 982 | 549 | 1 744 |
    | English | CHILDES | 7 241 | 4 607 | 1 109 | 641 | 2 114 |
    | English | ESLSpok | n/r | n/r | 690 | 641 | 2 143 |
    | English | EWT | 8 102 | 7 634 |  | 158 | 799 |
    | English | GUM | 9 911 | 6 140 | 871 | 256 | 1 010 |
    | English | LinES | n/r | 18 235 | 979 | 677 | 2 021 |
    | English | ParTUT | n/r | n/r | 734 | 688 | 2 120 |
    | Estonian | EDT | 2 736 | 2 256 | 1 910 | 504 |  |
    | Estonian | EWT | 5 034 | 2 534 | 1 496 | 485 |  |
    | Finnish | FTB | 5 379 | 2 128 | 719 | 462 | 10 509 |
    | Finnish | TDT | 4 260 | 2 308 | 718 | 469 | 10 563 |
    | French | ALTS | 6 907 | 5 079 | 969 | 600 | 1 391 |
    | French | GSD | n/r | 4 800 | 903 | 633 | 1 702 |
    | French | ParTUT | n/r | n/r | 817 | 733 | 2 040 |
    | French | ParisStories | n/r | 5 301 | 1 088 | 680 | 1 750 |
    | French | Rhapsodie | 11 900 | 4 014 | 951 | 621 | 1 749 |
    | French | Sequoia | n/r | n/r | 825 | 612 | 1 745 |
    | Galician | CTG | 7 774 | 5 530 | 1 868 | 747 |  |
    | German | GSD | 3 457 | 2 347 | 915 | 612 | 1 646 |
    | German | HDT | 2 058 | 1 271 | 782 | 569 | 1 467 |
    | Greek | Cretan | n/r | n/r | n/r |  | n/r |
    | Greek | GDT | n/r | 6 072 |  | 531 | n/r |
    | Greek | Messinian | n/r | n/r | n/r |  | n/r |
    | Hindi | HDTB | 15 756 | 7 346 | 1 112 | 692 |  |
    | Hungarian | Szeged | 5 987 | n/r | 1 597 | 572 |  |
    | Icelandic | GC | 8 837 | 6 406 | 1 844 |  |  |
    | Icelandic | IcePaHC | 6 427 | 3 095 | 1 987 |  |  |
    | Icelandic | Modern | n/r | n/r | 1 825 |  |  |
    | Irish | IDT | 5 791 | n/r | 1 590 | 467 |  |
    | Irish | TwittIrish | 5 839 | 6 522 | 1 803 | 370 |  |
    | Italian | ISDT | 4 681 | n/r | 757 | 631 | n/r |
    | Italian | MarkIT | 6 553 | 4 241 |  | 729 | n/r |
    | Italian | Old | n/r | 2 765 |  | 647 | n/r |
    | Italian | ParTUT | 1 669 | n/r |  | 689 | n/r |
    | Italian | PoSTWITA | 3 104 | 4 895 |  | 565 | n/r |
    | Italian | TWITTIRO | n/r | n/r |  | 531 | n/r |
    | Italian | VIT | 2 849 | 3 105 |  | 660 | 17 999 |
    | Japanese | GSD | 5 770 | 5 007 | 859 | 121 | 602 |
    | Japanese | GSDLUW | n/r | 3 826 | 736 | 96 | 485 |
    | Korean | GSD | 5 821 | 1 329 | 1 902 | 426 | n/r |
    | Korean | KSL | 3 116 | 1 198 | 2 343 | 493 | 12 786 |
    | Korean | Kaist | 3 231 | 1 292 | 2 148 | 489 | 12 346 |
    | Latin | CIRCSE | 4 354 | 1 449 |  | 574 |  |
    | Latin | ITTB | 1 495 | 1 055 |  | 555 |  |
    | Latin | LLCT | 1 452 | 1 309 |  | 407 |  |
    | Latin | PROIEL | 1 709 | 1 496 |  | 473 |  |
    | Latin | UDante | 1 657 | 1 413 |  | 541 |  |
    | Latvian | LVTB | 2 405 | 1 940 | 2 060 | 213 |  |
    | Lithuanian | ALKSNIS | 4 592 | 2 806 | 1 344 | 448 | n/r |
    | Lithuanian | HSE | n/r | n/r | n/r | 165 | n/r |
    | Maltese | MUDT | n/r | 3 354 |  |  |  |
    | Marathi | CMUPAN | 8 701 | 8 701 | 890 | 443 |  |
    | Marathi | UFAL | n/r | n/r | n/r | 352 |  |
    | Norwegian | Bokmaal | 26 902 | 12 106 | 1 041 | 416 |  |
    | Norwegian | Nynorsk | 8 631 | 17 351 | 973 |  |  |
    | Persian | PerDT | 3 145 | 3 370 | 1 106 | 631 |  |
    | Persian | Seraji | 3 145 | 4 449 | 1 041 | 681 |  |
    | Polish | LFG | 4 422 | 1 756 | 674 | 546 | 8 422 |
    | Polish | MPDT | n/r | 5 388 | 640 | 562 | 8 828 |
    | Polish | PDB | 6 928 | 2 517 | 629 | 554 | 8 519 |
    | Portuguese | Bosque | 4 704 | 6 137 | 665 | 638 | 16 932 |
    | Portuguese | CINTIL | 5 885 | 4 100 | 655 | 651 | 17 520 |
    | Portuguese | DANTEStocks | n/r | n/r | 583 | 378 | n/r |
    | Portuguese | GSD | 4 700 | 5 579 | 632 | 727 | 16 961 |
    | Portuguese | PetroGold | 5 259 | 4 454 | 604 | 667 | 17 135 |
    | Portuguese | Porttinari | n/r | 4 688 | 693 | 756 | 16 871 |
    | Romanian | Nonstandard | 3 121 | 5 413 | 1 943 | 588 | 14 677 |
    | Romanian | RRT | 8 072 | 3 564 | 1 888 | 547 | 14 077 |
    | Romanian | SiMoNERo | 3 362 | 6 030 | 1 814 | 541 | 14 013 |
    | Russian | GSD | 5 635 | n/r | 711 | 491 | 6 187 |
    | Russian | Poetry | 6 198 | 2 893 | 877 | 138 | 6 366 |
    | Russian | SynTagRus | 4 626 | 4 397 | 846 | 589 | 6 604 |
    | Russian | Taiga | n/r | n/r | 876 | 184 | 6 683 |
    | Scottish Gaelic | ARCOSG | 4 876 | 5 042 | 1 760 | 483 |  |
    | Serbian | SET | n/r | 6 721 | 2 018 | 545 |  |
    | Slovak | SNK | 6 624 | 6 373 | 1 685 | 519 |  |
    | Spanish | AnCora | 12 286 | 5 459 | 1 029 | 625 | 1 721 |
    | Spanish | GSD | 6 218 | 8 642 | 885 | 699 | 1 881 |
    | Swedish | LinES | n/r | 5 560 | 931 | 582 | 15 475 |
    | Swedish | Talbanken | 6 538 | 3 117 | 898 | 277 | 15 789 |
    | Tamil | TTB | 1 623 | n/r | 1 197 | 402 |  |
    | Telugu | MTG | n/r | n/r |  | 381 |  |
    | Thai | TUD | n/r | n/r |  |  |  |
    | Turkish | Atis | n/r | n/r | 675 | 587 |  |
    | Turkish | BOUN | n/r | n/r | 727 | 520 |  |
    | Turkish | FrameNet | n/r | n/r | 750 | 499 |  |
    | Turkish | IMST | 4 401 | n/r | 717 | 425 |  |
    | Turkish | Kenet | 8 168 | 7 860 | 835 | 552 |  |
    | Turkish | Penn | 5 104 | n/r | 888 | 610 |  |
    | Turkish | Tourism | n/r | 9 588 | 840 | 536 |  |
    | Urdu | UDTB | 5 489 | n/r | 2 187 | 619 |  |
    | Vietnamese | VTB | n/r | n/r |  | 712 |  |
    | Welsh | CCG | n/r | 6 606 | 2 068 |  |  |

### Treebank models

Some treebanks have their own model, whose tagger and parser (sometimes tokenizer) were trained on that treebank only, because they analyze text like it better than the merged model (e.g. Middle French for UD_French-ALTS). They are installed by name, e.g. `lima_models -i fra-UD_French-ALTS`.

| Language | Test set | Model | UPOS | Lemmas | LAS | Merged model LAS | Words/s |
|:--|:--|:--|--:|--:|--:|--:|--:|
| Arabic | PADT | `ara-UD_Arabic-PADT` | 88.73 | 85.86 | 69.67 | 68.72 | 17 573 |
| Czech | FicTree | `ces-UD_Czech-FicTree` | 98.09 | 96.32 | 88.29 | 86.46 | 9 124 |
| English | Atis | `eng-UD_English-Atis` | 98.94 | 99.27 | 88.83 | 87.56 | n/r |
| English | EWT | `eng-UD_English-EWT` | 94.56 | 95.18 | 82.70 | 81.23 | 7 427 |
| English | LinES | `eng-UD_English-LinES` | 95.80 | 96.48 | 80.38 | 80.60 | n/r |
| French | ALTS | `fra-UD_French-ALTS` | 93.66 | 91.36 | 76.58 | 73.91 | 8 196 |
| German | GSD | `deu-UD_German-GSD` | 95.11 | 95.99 | 76.14 | 71.97 | 3 625 |
| Icelandic | GC | `isl-UD_Icelandic-GC` | 93.26 | 90.13 | 73.81 | 57.83 | n/r |
| Italian | Old | `ita-UD_Italian-Old` | 94.85 | 94.36 | 74.50 | 73.53 | n/r |
| Italian | PoSTWITA | `ita-UD_Italian-PoSTWITA` | 94.98 | 94.02 | 75.33 | 72.99 | 7 876 |
| Japanese | GSD | `jpn-UD_Japanese-GSD` | 92.14 | 91.58 | 80.25 | 40.07 | 12 744 |
| Japanese | GSDLUW | `jpn-UD_Japanese-GSDLUW` | 91.02 | 91.90 | 80.91 | 68.31 | n/r |
| Latin | LLCT | `lat-UD_Latin-LLCT` | 99.51 | 97.43 | 91.62 | 90.79 | 2 084 |
| Latin | PROIEL | `lat-UD_Latin-PROIEL` | 95.70 | 93.44 | 67.62 | 62.87 | 2 428 |
| Polish | LFG | `pol-UD_Polish-LFG` | 98.27 | 95.18 | 92.45 | 89.01 | 5 494 |
| Portuguese | CINTIL | `por-UD_Portuguese-CINTIL` | 96.43 | 87.07 | 77.08 | 59.21 | 5 477 |
| Portuguese | DANTEStocks | `por-UD_Portuguese-DANTEStocks` | 95.58 | 94.81 | 84.33 | 81.59 | n/r |
| Portuguese | GSD | `por-UD_Portuguese-GSD` | 95.96 | 97.08 | 85.56 | 76.29 | 4 563 |
| Portuguese | PetroGold | `por-UD_Portuguese-PetroGold` | 97.44 | 97.48 | 90.26 | 89.43 | 5 711 |
| Russian | GSD | `rus-UD_Russian-GSD` | 96.76 | 94.26 | 82.19 | 84.46 | 1 365 |
| Spanish | AnCora | `spa-UD_Spanish-AnCora` | 98.53 | 98.78 | 88.95 | 87.38 | 7 202 |

### Artificial test sets

Treebanks whose only test set was split to train a model (see above). All toolkits are scored on the part kept for testing, but the other toolkits' models may have been trained on the rest of the original test set, or on all of it.

??? note "Scores on the artificial test sets"

    | Language | Test set | Model | Words | UPOS | Lemmas | LAS | LIMA accurate LAS | Stanza LAS | Trankit LAS | spaCy LAS |
    |:--|:--|:--|--:|--:|--:|--:|--:|--:|--:|--:|
    | Albanian | TSA | `sqi-UD_Albanian-MERGE` | 99.26 | 90.69 | 76.81 | 53.18 | 53.18 | 78.47 |  |  |
    | Amharic | ATT | `amh-UD_Amharic-ATT` | 49.35 | 40.70 | 48.26 | 15.08 | 14.71 |  |  |  |
    | Arabic | PUD | `ara-UD_Arabic-MERGE` | 90.72 | 70.32 | 70.84 | 52.65 | 52.32 |  | 62.78 |  |
    | Chinese | Beginner | `zho-UD_Chinese-MERGE` | 82.99 | 75.33 | 82.32 | 42.17 |  | 31.30 | 32.05 | 33.70 |
    | Chinese | CFL | `zho-UD_Chinese-MERGE` | 80.31 | 69.20 | 79.36 | 34.03 |  | 35.87 | 34.64 | 37.92 |
    | Czech | PUD | `ces-UD_Czech-MERGE` | 99.63 | 94.69 | 95.63 | 87.23 | 87.32 | 87.17 | 87.14 |  |
    | Czech | Poetry | `ces-UD_Czech-MERGE` | 99.61 | 92.62 | 89.37 | 66.17 | 67.24 | 69.43 | 72.69 |  |
    | English | CTeTex | `eng-UD_English-MERGE` | 97.33 | 87.64 | 87.98 | 67.04 | 67.04 | 68.44 | 71.75 | 35.99 |
    | English | GENTLE | `eng-UD_English-MERGE` | 94.17 | 85.82 | 84.38 | 67.72 | 69.09 | 74.41 | 74.75 | 36.35 |
    | English | LittlePrince | `eng-UD_English-MERGE` | 100.00 | 97.40 | 97.17 | 81.83 | 82.34 | 82.53 | 82.34 | 49.16 |
    | English | PUD | `eng-UD_English-MERGE` | 99.31 | 95.65 | 96.37 | 82.44 | 83.01 | 90.29 | 89.41 | 43.57 |
    | English | Pronouns | `eng-UD_English-MERGE` | 98.70 | 93.68 | 82.60 | 70.48 | 70.30 | 95.17 | 80.69 | 52.29 |
    | Finnish | OOD | `fin-UD_Finnish-MERGE` | 99.28 | 95.18 | 85.59 | 71.53 | 75.68 | 82.92 | 86.81 | 74.09 |
    | Finnish | PUD | `fin-UD_Finnish-MERGE` | 99.65 | 97.52 | 87.56 | 81.18 | 84.27 | 89.76 | 92.44 | 82.16 |
    | French | FQB | `fra-UD_French-MERGE` | 99.73 | 97.82 | 98.37 | 94.07 | 94.30 | 89.49 | 87.57 | 70.85 |
    | French | PUD | `fra-UD_French-MERGE` | 99.54 | 96.82 | 98.19 | 88.54 | 88.72 | 90.36 | 91.04 | 80.35 |
    | French | PoitevinDIVITAL | `fra-UD_French-MERGE` | 98.33 | 72.97 | 75.63 | 42.98 | 45.41 | 19.37 | 31.25 | 36.58 |
    | Galician | PUD | `glg-UD_Galician-MERGE` | 97.78 | 93.15 | 92.88 | 74.93 | 75.31 | 69.20 | 64.86 |  |
    | Galician | TreeGal | `glg-UD_Galician-MERGE` | 97.91 | 92.89 | 93.85 | 70.74 | 71.11 | 71.76 | 81.85 |  |
    | German | LIT | `deu-UD_German-MERGE` | 98.37 | 92.89 | 93.86 | 75.16 | 75.75 | 78.11 | 78.69 | 4.23 |
    | German | PUD | `deu-UD_German-MERGE` | 97.11 | 91.58 | 93.29 | 78.81 | 79.17 | 83.96 | 85.01 | 6.38 |
    | Greek | GLCII | `ell-UD_Greek-MERGE` | 98.77 | 92.44 | 88.65 | 78.18 | 78.98 |  |  | 65.52 |
    | Greek | GUD | `ell-UD_Greek-MERGE` | 99.96 | 97.78 | 94.05 | 87.41 | 88.04 | 83.69 |  | 69.47 |
    | Greek | Lesbian | `ell-UD_Greek-MERGE` | 97.10 | 85.03 | 49.21 | 64.59 | 63.42 |  |  | 29.33 |
    | Hindi | PUD | `hin-UD_Hindi-MERGE` | 99.92 | 89.61 | 96.99 | 67.86 | 68.96 | 54.60 | 57.98 |  |
    | Icelandic | PUD | `isl-UD_Icelandic-MERGE` | 99.60 | 90.95 | 88.57 | 57.93 | 58.02 | 58.19 |  |  |
    | Irish | Cadhan | `gle-UD_Irish-MERGE` | 94.71 | 76.26 | 60.62 | 49.79 | 49.73 | 52.54 | 46.34 |  |
    | Italian | KIParlaForest | `ita-UD_Italian-MERGE` | 98.73 | 88.19 | 92.83 | 52.75 | 58.22 |  |  | 49.14 |
    | Italian | PUD | `ita-UD_Italian-MERGE` | 99.29 | 94.58 | 96.51 | 87.66 | 87.93 |  | 89.92 | 75.77 |
    | Italian | ParlaMint | `ita-UD_Italian-MERGE` | 99.53 | 97.73 | 97.64 | 86.17 | 86.51 |  | 90.33 | 73.44 |
    | Italian | Valico | `ita-UD_Italian-MERGE` | 98.66 | 96.22 | 88.72 | 83.90 | 85.16 |  | 87.76 | 76.45 |
    | Japanese | PUD | `jpn-UD_Japanese-MERGE` | 70.34 | 66.94 | 68.99 | 37.96 | 38.06 | 90.82 | 62.67 | 90.69 |
    | Japanese | PUDLUW | `jpn-UD_Japanese-MERGE` | 91.07 | 85.31 | 88.89 | 76.40 | 76.56 | 39.49 | 41.71 | 39.02 |
    | Korean | LittlePrince | `kor-UD_Korean-MERGE` | 99.64 | 92.14 | 59.80 | 72.26 | 70.70 | 78.92 | 46.48 | 74.18 |
    | Korean | PUD | `kor-UD_Korean-MERGE` | 78.03 | 54.29 | 51.60 | 37.91 |  | 38.59 | 32.39 | 36.13 |
    | Latin | Perseus | `lat-UD_Latin-MERGE` | 97.27 | 87.33 | 78.87 | 63.36 | 63.29 |  | 58.59 |  |
    | Latvian | Cairo | `lav-UD_Latvian-MERGE` | 100.00 | 100.00 | 100.00 | 93.06 | 98.61 | 93.06 | 90.28 |  |
    | Polish | PUD | `pol-UD_Polish-MERGE` | 99.46 | 96.80 | 94.57 | 87.68 | 87.75 | 91.98 | 90.37 | 83.18 |
    | Portuguese | PUD | `por-UD_Portuguese-MERGE` | 99.19 | 91.74 | 92.82 | 80.31 | 80.56 | 81.17 | 81.41 | 68.67 |
    | Romanian | ArT | `ron-UD_Romanian-MERGE` | 93.46 | 59.08 | 35.35 | 31.48 | 31.96 | 28.85 | 37.07 | 29.98 |
    | Romanian | MolDoRo | `ron-UD_Romanian-MERGE` | 94.87 | 38.46 | 51.28 | 6.41 | 8.97 | 0.00 | 16.00 | 6.49 |
    | Romanian | TueCL | `ron-UD_Romanian-MERGE` | 97.34 | 87.76 | 81.70 | 66.49 | 65.52 | 65.49 | 72.59 | 60.11 |
    | Russian | PUD | `rus-UD_Russian-MERGE` | 99.71 | 96.59 | 95.19 | 87.41 | 87.41 | 87.58 | 84.83 | 85.66 |
    | Spanish | COSER | `spa-UD_Spanish-MERGE` | 99.21 | 92.82 | 94.91 | 63.47 | 64.54 | 65.33 | 61.29 | 64.54 |
    | Spanish | PUD | `spa-UD_Spanish-MERGE` | 99.30 | 92.75 | 95.65 | 82.57 | 82.86 | 84.59 | 76.95 | 76.89 |
    | Swedish | Old | `swe-UD_Swedish-MERGE` | 100.00 | 55.62 | 39.33 | 15.73 | 19.66 | 8.99 | 30.99 | 19.10 |
    | Swedish | PUD | `swe-UD_Swedish-MERGE` | 99.72 | 96.30 | 93.87 | 82.86 | 82.78 | 81.70 | 84.41 | 71.27 |
    | Swedish | SweLL | `swe-UD_Swedish-MERGE` | 99.89 | 96.53 | 95.30 | 83.45 | 83.52 | 84.37 | 83.88 | 77.80 |
    | Tamil | MWTT | `tam-UD_Tamil-MERGE` | 94.95 | 89.49 | 83.02 | 62.46 | 64.28 | 44.88 | 57.35 |  |
    | Thai | PUD | `tha-UD_Thai-MERGE` | 80.88 | 63.84 | 80.84 | 31.51 | 31.16 |  |  |  |
    | Turkish | GB | `tur-UD_Turkish-MERGE` | 97.21 | 86.73 | 82.49 | 65.57 | 65.79 | 60.10 | 56.87 |  |
    | Turkish | PUD | `tur-UD_Turkish-MERGE` | 96.40 | 81.11 | 79.75 | 54.42 | 54.53 | 54.13 | 55.39 |  |
    | Turkish | TueCL | `tur-UD_Turkish-MERGE` | 97.18 | 85.24 | 84.91 | 73.30 | 75.29 | 58.82 | 65.26 |  |
    | Vietnamese | TueCL | `vie-UD_Vietnamese-MERGE` | 87.23 | 73.95 | 86.49 | 40.00 | 39.85 |  | 57.96 |  |

## Test sets without LIMA scores

- **Text withheld**: `UD_Arabic-NYUAD`, `UD_English-GUMReddit`, `UD_Japanese-BCCWJ`, `UD_Japanese-BCCWJLUW` are distributed without their word forms (they must be obtained separately), so no toolkit can be evaluated on them.

- **Analysis errors**: LIMA's output on `UD_Bengali-BRU`, `UD_Chinese-GSD`, `UD_Chinese-GSDSimp` could not be scored; this is being investigated.

- **Re-evaluation pending**: `UD_Chinese-HK`, `UD_Chinese-PUD`, `UD_Chinese-PatentChar`, `UD_Slovenian-SSJ`, `UD_Slovenian-SST`, `UD_Ukrainian-IU`, `UD_Ukrainian-ParlaMint` were analyzed with an older LIMA version, whose results are not reported.

Some other toolkits also failed on a few test sets (Stanza on `UD_English-EWT`; Stanza on `UD_Japanese-BCCWJ`; Stanza on `UD_Japanese-BCCWJLUW`; Trankit on `UD_Japanese-BCCWJ`; Trankit on `UD_Japanese-BCCWJLUW`).

## Reproducing

The models' own scores, per model and test set, are also listed in the [model card](https://huggingface.co/aymaralima/deeplima/tree/v0.3.0) of the release. The test sets are those of the [UD 2.18 release](https://universaldependencies.org/), and the scorer is the CoNLL 2018 one, so the results can be reproduced with `analyzeText -l ud --meta udlang:<model> -p deepud -o text:stdout <test>.txt` and the scorer.

