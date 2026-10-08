# Command line

`analyzeText` is the main command-line analyzer. It analyzes one or more text
files with a given language and pipeline.

```bash
analyzeText -l eng-UD_English-EWT -p deepud file1.txt file2.txt
```

By default, the result is written on standard output in
[CoNLL-U](output-formats.md). Input files must be UTF-8 encoded.

The language (`-l`) selects the language configuration and, for neural
pipelines, the models: see [Language models](models.md). The pipeline (`-p`)
selects the sequence of processing steps: see [Pipelines](pipelines.md).

## Options

| Option | Meaning |
| --- | --- |
| `-l`, `--language <lang>` | Language to initialize and use. Can be repeated |
| `-p`, `--pipeline <name>` | Analysis pipeline to use |
| `--meta <k1:v1,k2:v2>` | Metadata passed to the analysis, e.g. `udlang:eng-UD_English-EWT` |
| `-d`, `--dumper <name>` | Output handler to use (`text` by default; also `bow`, `bowh`, `fullxml`, `event`, `xmlbow`). The matching dumper must be in the pipeline |
| `-o`, `--output <dumper>:<dest>` | Where a dumper writes: `stdout` or a suffix appended to the input file name |
| `-s`, `--split-mode <mode>` | `none` (default), `lines` or `para`: analyze each line or paragraph independently |
| `--inactive-units <unit>` | Deactivate a process unit of the pipeline. Can be repeated |
| `--availableUnits` | List the known process units and exit |
| `--config-dir <dir>` | Directory containing the configuration files |
| `--resources-dir <dir>` | Directory containing the linguistic resources |
| `--common-config-file <file>` | Common configuration file (default `lima-common.xml`) |
| `--lp-config-file <file>` | Linguistic processing configuration file (default `lima-analysis.xml`) |
| `-c`, `--client <id>` | Linguistic processing client (default `lima-coreclient`) |
| `-v`, `--version` | Print the LIMA version |
| `-h`, `--help` | Print the help |

## Examples

Analyze raw text with the full neural pipeline:

```bash
analyzeText -l fra-UD_French-GSD -p deepud article.txt
```

Analyze an already tokenized CoNLL-U file:

```bash
analyzeText -l fra-UD_French-GSD -p deepud-pretok corpus.conllu
```

Run the rule-based named entity recognizer of the legacy English pipeline:

```bash
analyzeText -l eng -p ner-rules article.txt
```

Analyze a file line by line (one sentence or document per line):

```bash
analyzeText -l eng-UD_English-EWT -p deepud -s lines sentences.txt
```

## Other commands

| Command | Role |
| --- | --- |
| `lima` | The [graphical interface](gui.md) (C++ installation) or a simple file analyzer (Python package) |
| `limaserver` | An HTTP server exposing the analyzer (see [Docker](../install/docker.md#running-the-lima-server)) |
| `lima_models.py` | Install [language models](models.md) |
| `compile-rules` | Compile [ModEx rules](../guides/writing-a-modex.md) |
| `readBowFile` | Print the content of a binary bag-of-words file (see [Output formats](output-formats.md#bag-of-words)) |
| `deeplima` | The standalone neural analyzer of the deeplima library (see [Training neural models](../developers/training-models.md)) |
