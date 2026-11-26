# LIMA - Libre Multilingual Analyzer

![LIMA logo](https://raw.githubusercontent.com/aymara/lima/master/pics/lima-logo.png)

[![Build](https://github.com/aymara/lima/actions/workflows/build.yml/badge.svg)](https://github.com/aymara/lima/actions/workflows/build.yml)
[![Documentation](https://github.com/aymara/lima/actions/workflows/docs.yml/badge.svg)](https://aymara.github.io/lima/)
[![LIMA Python Downloads](https://static.pepy.tech/personalized-badge/aymara?period=total&units=international_system&left_color=black&right_color=brightgreen&left_text=LIMA%20Python%20Downloads)](https://pepy.tech/project/aymara)

LIMA is a multilingual natural language processing analyzer developed by
[CEA LIST](https://list.cea.fr/en/) (LASTI laboratory). It is free software,
available under the MIT license.

- **Neural modules** (trained with libtorch, run on the CPU with Eigen) for tokenization, part-of-speech tagging and
  morphological features, lemmatization and dependency parsing, with
  [models for more than 60 languages](https://huggingface.co/aymaralima/deeplima).
- **ModEx**, a powerful rule-based mechanism to extract entities, relations
  and events in domains where no annotated data exists.

## Why LIMA?

- **Accuracy versus speed**: on the UD 2.18 test sets, with the v0.3.0 models,
  8 CPU threads and no GPU, LIMA analyzes about 5,000 words per second from
  raw text to labeled dependencies. It is about 5× faster than Stanza (which
  is 4.6 LAS points more accurate), 9× faster and 1.8 LAS points more
  accurate than Trankit, and 17 LAS points more accurate than spaCy, which is
  1.3× faster (speeds are indicative: not all measured on the same machines).
  [Details](https://aymara.github.io/lima/usage/performance/).
- **Many languages, one pipeline**, with a uniform CoNLL-U output.
- **Rules where there is no annotated data**, with ModEx, on top of the neural
  analysis.
- **Embeddable**: C++ library, command line, HTTP server and Python package;
  pipelines configured without changing code; MIT license.

It is less accurate than the most accurate toolkits, such as Stanza, has no
GPU support, and its named entity recognition is limited to English and
French.
See [Why LIMA?](https://aymara.github.io/lima/why-lima/) for when to choose
it, or another tool.

## Documentation

**<https://aymara.github.io/lima/>**: installation, user guide, guides,
reference and developer documentation.

## Quick start

With the Python package (Linux x86_64, Windows x64 or macOS on Apple Silicon,
Python ≥ 3.9 on Linux, ≥ 3.10 elsewhere):

```bash
pip install --upgrade pip
pip install aymara==0.8.1
lima_models -i eng
```

```python
import aymara.lima
nlp = aymara.lima.Lima("ud-eng")
doc = nlp("Hello, World!")
print(repr(doc))
```

Other options (Docker image, building from source) are described in the
[installation guide](https://aymara.github.io/lima/install/).

## Contributing

Bug reports and contributions are welcome: see the
[contributing guide](https://aymara.github.io/lima/developers/contributing/).
The documentation sources are in [`docs/`](docs/).
