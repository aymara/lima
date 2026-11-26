# Why LIMA?

Many free NLP toolkits analyze text into tokens, parts of speech, lemmas and
dependencies. This page explains what LIMA does well, where it is weaker, and
when another tool is a better choice. The figures come from the
[evaluation of the v0.3.0 models on Universal Dependencies 2.18](usage/performance.md).

## Accuracy versus speed

LIMA is designed for a good balance of accuracy and speed on ordinary CPUs.
Its neural models run in a C++ inference engine built on Eigen, without a GPU
and without Python. With 8 threads, the default models analyze about
5 000 words per second (median over the test sets), from raw text to labeled
dependencies.

Compared with the best models of each toolkit, on the test sets that both
analyze (LAS: labeled attachment score of the dependency parsing, in %):

| Toolkit | LAS, LIMA / other | LIMA's speed |
|:--|--:|--:|
| [Stanza](https://stanfordnlp.github.io/stanza/) | 78.4 / 83.0 | 4.8× faster |
| [Trankit](https://trankit.readthedocs.io/) | 78.8 / 77.0 | 9.0× faster |
| [spaCy](https://spacy.io/) | 79.9 / 62.8 | 1.3× slower |

In other words, LIMA is:

- **much faster than Stanza**, which is slightly more accurate: 1.7 points
  on parts of speech and 2.5 on lemmas, the main gap being on dependency
  parsing (4.6 LAS points);
- **faster and more accurate than Trankit**;
- **much more accurate than spaCy** on UD (17 LAS points), while spaCy is
  1.3× faster. spaCy covers fewer languages and not all its pipelines are
  trained on UD.

Within LIMA, you can also choose your own point on this curve: the
**accurate** model variants (`lima_models -i <lang> --accurate`) add about
0.7 LAS point on average for about 12 % less speed (4 450 instead of
5 070 words per second). The speeds are indicative: the toolkits did not all
run on the same machines. The details, per language and per test set, are in
[Accuracy and speed](usage/performance.md).

## Strengths

**Fast and accurate enough on the CPU**, as shown above: a good fit for large
volumes of text, for servers without GPUs, or for desktop applications.

**Many languages, one pipeline.** Models for more than 60 languages are
trained on [Universal Dependencies](https://universaldependencies.org/)
treebanks, with the same pipeline (`deepud`), the same command and the same
CoNLL-U output for all of them, from raw text or from pre-tokenized CoNLL-U.
For most languages, the default model is trained on all the treebanks of the
language merged, to be robust across genres.

**Rules where there is no annotated data.** With [ModEx](guides/writing-a-modex.md),
you write finite-state rules over the words, lemmas, parts of speech and
dependencies produced by the neural modules, to extract the entities,
relations or events of your domain, without training data. The named entity
recognition of English and French is written this way: you can read, correct
and extend its rules.

**Embeddable and configurable.** LIMA is a C++ library with a command line,
an HTTP server, a Python package and a graphical interface. The Python
package, `aymara`, covers the same uses as the other toolkits' packages, with
a spaCy-like API (`Doc`, `Token`, `Span`); LIMA's analyses can be converted to
spaCy documents, and a LIMA analyzer can be the tokenizer of a spaCy pipeline,
followed by spaCy components (in the next release of `aymara`). Pipelines are
assembled from [configuration files](usage/configuration.md) and
[plugins](developers/architecture.md): you can add, remove or reorder
processing steps, or add your own components, without changing LIMA's code.

**Easy to deploy.** Python wheels for Linux x86_64, Windows x64 and macOS on
Apple Silicon, a Docker image and Linux packages. Models are downloaded per
language, with smaller embedding variants when memory is limited.

**Free and maintained over the long term.** LIMA is under the MIT license and
has been developed at CEA LIST, and used in research and industrial projects,
for more than twenty years.

## Weaknesses

- **Not the most accurate.** Stanza, with its best packages, is slightly
  more accurate, mainly on dependency parsing (4.6 LAS points on average; LIMA
  is better on 11 of the 90 test sets that both analyze). LIMA trades this
  accuracy for speed and CPU-only inference.
- **Built-in named entities only for English and French.** There is no
  statistical NER model; the entities of English and French are recognized by
  ModEx rules. With the same mechanism, you can write rules for your own rich
  entities, relations and events.
- **No GPU support.** Inference runs on the CPU only.
- **A smaller community** than those of spaCy or Stanza.
- **A learning curve for advanced uses.** ModEx rules and the XML
  configuration are powerful but take time to learn.
- **Deeper analyses are legacy.** Coreference resolution and semantic analysis
  only exist in the deprecated rule-based pipelines for English and French,
  which are no longer built by default.

## When to choose LIMA

LIMA is a good choice when:

- you analyze **large volumes of text on CPUs** and need UD analyses of
  reasonable quality, quickly;
- you need **many languages** with a single tool and a uniform output;
- you want to **integrate the analysis in a C++ application** or a service,
  without a Python runtime;
- you need to **extract domain-specific information** (entities, relations,
  events) without annotated data, with rules you control;
- you need a **permissively licensed** tool you can embed and redistribute.

Another tool may suit you better when:

- **the best accuracy, especially in dependency parsing, matters more than
  speed**: Stanza is more accurate, even on the CPU, and faster with a GPU;
- you need **statistical named entity recognition** in many languages, without
  writing rules: spaCy, Stanza;
- you only process **English or a few major languages** in a Python
  application and want the fastest setup: spaCy's small pipelines are fast,
  though less accurate on UD and not all trained on UD.

## Learn more

- [Accuracy and speed](usage/performance.md): scores and speeds per language
  and test set.
- [Getting started](getting-started.md): install LIMA and analyze a first text.
