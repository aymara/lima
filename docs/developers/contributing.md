# Contributing

Contributions are welcome: bug reports, suggestions, documentation, bug
fixes, linguistic resources, models…

## Reporting issues

Open an issue on [GitHub](https://github.com/aymara/lima/issues), with the
LIMA version (`analyzeText -v`), how you installed it, the command or code you
ran and its output. For crashes, a stack trace from a
[debug build](../install/source.md) helps a lot.

## Proposing changes

1. Fork the repository and create a branch from `master`.
2. Make your changes, following the style of the surrounding code. Python
   code is formatted with black and isort and checked with flake8 (line
   length 88); a pre-commit configuration is provided.
3. Build and run the [tests](testing.md).
4. Update the documentation in `docs/` if your change affects users (see
   [Writing documentation](documentation.md)).
5. Open a pull request against `master`. The continuous integration builds
   LIMA in Docker images and runs the tests.

## Contributor agreement

LIMA is published under the [MIT license](../project/license.md) by CEA LIST.
Before merging significant contributions of code or linguistic resources, the
maintainers may ask you to sign a contributor agreement.
