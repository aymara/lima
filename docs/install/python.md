# Python package

LIMA's Python bindings are published on PyPI as the
[`aymara`](https://pypi.org/project/aymara/) package. They bundle the LIMA C++
libraries, so nothing else needs to be installed.

**Requirements:** Linux x86_64 and Python ≥ 3.7. An up-to-date `pip` is
required to select the right wheel.

--8<-- "pypi-install.md"

The package provides:

- the `aymara.lima` Python module (see [Using LIMA from Python](../usage/python.md)
  and the [Python API reference](../reference/python-api.md));
- a `lima` command to analyze files from the shell;
- the model installer `lima_models` (`deeplima_models` is a synonym; see
  [Language models](../usage/models.md)).

The sources of the bindings are in the
[aymara/lima-python](https://github.com/aymara/lima-python) repository, which
also explains how to build the wheel.
