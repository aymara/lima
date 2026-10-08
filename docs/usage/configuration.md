# Configuring LIMA

LIMA's behavior is driven by XML configuration files and by compiled
linguistic resources. To customize them, do not edit the installed files:
copy the ones you want to change to your own directories and put these
directories first in the search paths.

## Search paths

Configuration files are searched in this order:

1. directories given with `--config-dir`;
2. directories of the colon-separated `LIMA_CONF` environment variable;
3. `$XDG_DATA_HOME` (or `~/.local/share/` if not defined);
4. `$LIMA_DIST/config`;
5. `/usr/share/config/lima`.

Resources (dictionaries, compiled rules, models) are searched the same way
with `--resources-dir` and `LIMA_RESOURCES`.

For example:

```bash
install -d ~/MyLima/conf ~/MyLima/resources
export LIMA_CONF=~/MyLima/conf:/usr/share/config/lima
export LIMA_RESOURCES=~/MyLima/resources:/usr/share/apps/lima/resources
```

## Example: removing a step

Suppose you do not need the named entities extracted by the legacy English
pipeline. Copy `lima-lp-eng.xml` to `~/MyLima/conf` and comment out this line
in the `main` group of the `Processors` module:

```xml
<item value="SpecificEntitiesModex"/>
```

Note that dependencies between process units are not checked: deactivating a
unit needed by a later one can make the analysis fail. For a one-off test, the
`--inactive-units` option of [`analyzeText`](cli.md) does the same without
editing any file.

## Troubleshooting

If your configuration files seem to be ignored, another file is probably found
first. Set the `LIMA_SHOW_CONFIG_PATH` environment variable to a non-empty
value to print the list of searched directories:

```bash
LIMA_SHOW_CONFIG_PATH=1 analyzeText -l eng -p main file.txt
```

## Going further

- The [configuration files reference](../reference/configuration-files.md)
  describes all the files and their structure.
- The [process units reference](../reference/process-units/index.md) lists the
  parameters of each unit.
- [Writing a ModEx](../guides/writing-a-modex.md) shows how to add your own
  extraction rules.
