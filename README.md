# LIMA - Libre Multilingual Analyzer

![LIMA logo](https://raw.githubusercontent.com/aymara/lima/master/pics/lima-logo.png)

[![Build](https://github.com/aymara/lima/actions/workflows/build.yml/badge.svg)](https://github.com/aymara/lima/actions/workflows/build.yml)
[![Documentation](https://github.com/aymara/lima/actions/workflows/docs.yml/badge.svg)](https://aymara.github.io/lima/)
[![LIMA Python Downloads](https://static.pepy.tech/personalized-badge/aymara?period=total&units=international_system&left_color=black&right_color=brightgreen&left_text=LIMA%20Python%20Downloads)](https://pepy.tech/project/aymara)

LIMA is a multilingual natural language processing analyzer developed by
[CEA LIST](https://list.cea.fr/en/) (LASTI laboratory). It is free software,
available under the MIT license.

- **Neural modules** (libtorch) for tokenization, part-of-speech tagging and
  morphological features, lemmatization and dependency parsing, with
  [models for more than 60 languages](https://github.com/aymara/lima-models).
- **ModEx**, a powerful rule-based mechanism to extract entities, relations
  and events in domains where no annotated data exists.

## Documentation

**<https://aymara.github.io/lima/>**: installation, user guide, guides,
reference and developer documentation.

## Quick start

With the Python package (Linux x86_64, Python ≥ 3.7):

```bash
pip install --upgrade pip
pip install aymara==0.5.0b6
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
