# Testing

The tests run with CTest. The simplest way to build LIMA and run the whole
suite is:

```bash
source ./setenv-lima.sh -m release
./gbuild.sh -m Release -t ON
```

On failure, `gbuild.sh` re-runs the failed tests and shows their output.

To run tests by hand, go to the build tree of a subproject:

```bash
cd "$LIMA_BUILD_DIR/<branch>/<mode>-OFF/<subproject>"
ctest --output-on-failure              # all tests
ctest -R <regex>                       # tests whose name matches
ctest --rerun-failed --output-on-failure
```

For example, `$LIMA_BUILD_DIR/master/release-OFF/lima_linguisticprocessing`.

Some tests exercise the neural pipelines and need models: install them first,
e.g. `lima_models.py -l eng` (see [Language models](../usage/models.md)).

## Analysis regression tests

Linguistic regression tests are written as XML files describing a text, the
pipeline to apply and XPath assertions on the result. They are run by `tva`,
the test-driver analyzer (`lima_linguisticprocessing/tools/tva/`); the test
files are in `lima_linguisticprocessing/data/test-<lang>.*.xml`:

```xml
<testcase id="eng.abbrev.1" type="bloquant">
  <call-parameters>
    <param key="text" value="My sister's house is nice."/>
    <param key="language" value="eng"/>
    <list key="pipelines"><item value="indexer"/></list>
  </call-parameters>
  <expl>Possessive handling</expl>
  <test id="eng.abbrev.1.1" trace=".tokenizer.xml"
        comment="the token sister's exists"
        left="XPATH#//data_structure/vertex/token[position=4][length=8]"
        operator="exists" right=""/>
</testcase>
```

## Debugging

Use a debug build (`setenv-lima.sh` and `gbuild.sh` without `-m`) to
investigate crashes; release builds lack the symbols. Debug messages are
controlled by the `log4cpp.properties` files (see
[Configuration files](../reference/configuration-files.md)).
