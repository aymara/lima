# Python API

This reference is generated from the docstrings of the
[`aymara.lima`](https://github.com/aymara/lima-python/blob/master/aymara/lima.py)
module. For an introduction, see [Using LIMA from Python](../usage/python.md).

::: aymara.lima
    options:
      show_root_heading: false
      members:
        - Lima
        - Doc
        - Span
        - Token
        - PhraseMatcher
        - LimaInternalError

## spaCy interoperability

The [`aymara.lima_spacy`](https://github.com/aymara/lima-python/blob/master/aymara/lima_spacy.py)
module (aymara 0.9.0 and later, with `pip install "aymara[spacy]"`) converts
LIMA documents into spaCy ones and makes LIMA the tokenizer of a spaCy pipeline.

::: aymara.lima_spacy
    options:
      show_root_heading: false
      members:
        - spacy_pipeline
        - LimaTokenizer
        - doc_to_spacy
