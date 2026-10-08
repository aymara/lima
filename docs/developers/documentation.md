# Writing documentation

This site is built with [MkDocs](https://www.mkdocs.org/) and the
[Material](https://squidfunk.github.io/mkdocs-material/) theme from the
`docs/` directory of the repository. It is deployed to
<https://aymara.github.io/lima/> by the `docs` GitHub Actions workflow on each
push to `master`, and built (without deployment) for each pull request.

## Previewing locally

```bash
source .venv/bin/activate        # or create one: uv venv && source .venv/bin/activate
uv pip install -r docs/requirements.txt
mkdocs serve
```

then open <http://127.0.0.1:8000/>. The [Python API](../reference/python-api.md)
page is generated from the sources of
[aymara/lima-python](https://github.com/aymara/lima-python): clone it next to
this repository (`../lima-python`), or point `LIMA_PYTHON_SRC` to a clone.

!!! tip
    Make sure the `mkdocs` you run is the virtual environment's: a system
    `mkdocs` (e.g. `/usr/bin/mkdocs`) does not see the Material theme and fails
    with *Unrecognised theme name: 'material'*.

Before submitting, check that the strict build passes, as in the CI:

```bash
mkdocs build --strict
```

The C++ API pages are generated separately with `doxygen docs/Doxyfile`.

## Conventions

- One topic per page; put the page in the section matching what the reader
  wants to do: *Install*, *Using LIMA* (tasks), *Guides* (in-depth
  walkthroughs), *Reference* (exhaustive descriptions) or *Developers*.
- Add new pages to the `nav` of `mkdocs.yml`.
- Use relative links to other pages (`../usage/models.md`), never links to the
  published site.
- Content repeated on several pages lives in `docs/includes/` and is included
  with `--8<-- "file.md"`. In particular, `pypi-install.md` holds the pip
  command and the version of the `aymara` package: update it on each Python
  release.
- `docs/internals/` holds design notes and work-in-progress documents. They are
  kept in the repository but not published.
