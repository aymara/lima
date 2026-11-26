# Getting started

This page takes you from nothing to a first analysis. Pick the path that suits
you; both produce [CoNLL-U](usage/output-formats.md) output.

--8<-- "versions.md"

=== "Python package"

    The quickest way to try LIMA, under **Linux x86_64**, **Windows x64** or
    **macOS on Apple Silicon** (Python ≥ 3.9 on Linux, ≥ 3.10 elsewhere):

    --8<-- "pypi-install.md"

    Install the English models, then analyze a text from Python:

    ```bash
    lima_models -i eng
    ```

    ```pycon
    >>> import aymara.lima
    >>> nlp = aymara.lima.Lima("ud-eng")
    >>> doc = nlp("Hello, World!")
    >>> print(doc[0].lemma)
    hello
    >>> print(repr(doc))
    1       Hello   hello   INTJ    _       _               3       discourse       _       Pos=0|Len=5
    2       ,       ,       PUNCT   _       _               1       punct   _       Pos=5|Len=1
    3       World   World   PROPN   _       Number:Sing     0       root    _       Pos=7|Len=5
    4       !       !       PUNCT   _       _               3       punct   _       Pos=12|Len=1
    ```

    The package also installs a `lima` command that analyzes files:

    ```bash
    lima my-text.txt
    ```

    Continue with [Using LIMA from Python](usage/python.md).

=== "Docker"

    The `latest` Docker image contains LIMA built from the `master` branch;
    use the `3.0.1` tag instead of `latest` to get the LIMA 3.0.1 release:

    ```bash
    docker pull aymara/lima-ubuntu22.04:latest
    docker run -it --rm -v "$PWD":/data aymara/lima-ubuntu22.04:latest bash
    ```

    Inside the container, install the models of a treebank and analyze a
    file:

    ```bash
    lima_models.py -i eng
    analyzeText -l eng-UD_English-MERGE -p deepud /data/my-text.txt
    ```

    `lima_models.py -i eng` installs the default English model,
    `eng-UD_English-MERGE`, and prints the identifier to use. Continue with [Language models](usage/models.md) and
    [the command line](usage/cli.md).

## What next?

- Learn how [language models](usage/models.md) are named and selected.
- Discover the available [pipelines](usage/pipelines.md).
- [Configure LIMA](usage/configuration.md) for your needs, or
  [write your own extraction rules](guides/writing-a-modex.md).
