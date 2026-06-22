# Plan: remove the TensorFlow 1.9 dependency, consolidate on libtorch

Status: **Phase 2 complete — TensorFlow fully removed.** Direction chosen: **complete
the libtorch (deeplima) migration and drop TensorFlow entirely** (rather than porting
to TensorFlow 2.x). See the "Phase 0/1/2 results" sections at the end.

## Why

LIMA's neural modules currently depend on a *modified* TensorFlow 1.9 that can no
longer be installed. A libtorch (PyTorch C++) replacement stack already exists
in-tree (`deeplima/` + `DeepLimaUnits/`) and is the project's intended successor
(see branches `deeplima-inference-refactoring`, `deeplima-lemmatizer-dev`,
`deeplima-stanza-dp`). Porting to TF2 would be a near-total rewrite of the
inference code, keep TF's painful C++ distribution, and leave two NN backends.

## Current wiring (as of master)

TensorFlow 1.9 is isolated to two plugins, both gated by `find_package(TensorFlow)`:

- `lima_linguisticprocessing/.../core/TensorFlowUnits/` → process units
  `cpptftokenizer` (`CppUppsalaTensorFlowTokenizer`), `tfmorphosyntax`
  (`TensorFlowMorphoSyntax`), `tflemmatizer` (`TensorFlowLemmatizer`).
- `lima_linguisticprocessing/.../core/TensorflowSpecificEntities/` → neural NER.

These are the units in the default `deepud` / `deepud-pretok` pipelines
(`lima_linguisticprocessing/conf/lima-lp-ud.xml`).

The libtorch replacements already exist (`core/DeepLimaUnits/`), linking the
`deeplima` task libs (`tag`, `lemmatization_inference`, `segmentation_inference`):

| TF unit                      | libtorch replacement      | notes |
|------------------------------|---------------------------|-------|
| `cpptftokenizer`             | `RnnTokenizer`            | model `RnnTokenizer/ud/tokenizer-$udlang.pt` |
| `tfmorphosyntax` + `tflemmatizer` | `RnnTokensAnalyzer`  | one unit: writes UPOS/MACRO/MICRO + UD features into `MorphoSyntacticData` **and** lemma. Tagger model required, lemmatizer model optional (falls back to empty lemma — see `RnnTokensAnalyzer.cpp:308-311`) |
| `TensorflowSpecificEntities` | `RnnNER`                  | model `ner-$udlang` |
| (none)                       | `RnnDependencyParser`     | new; `dependencyParser-$udlang.pt`; currently commented out of pipelines |

Build gating (already parallel and clean):
- TF: `core/CMakeLists.txt:36-38` — `if (TensorFlow_FOUND AND Eigen_FOUND)`
- libtorch: `core/CMakeLists.txt:41-42` — `if (EIGEN3_FOUND AND TORCH_FOUND)`
- deeplima lib: root `CMakeLists.txt:510-515`; Torch found at root `CMakeLists.txt:413`.

## Model situation (the real critical path)

Models are distributed via the `aymara/lima-models` GitHub repo.

- Old TF models: `.deb` packages (e.g. release `v0.1.5`, ~69 assets). Installed by
  `TensorFlowUnits/scripts/lima_models.py` into `TensorFlowTokenizer/ud`,
  `TensorFlowLemmatizer/ud`, plus `morphosyntax-*` + `fasttext-*.bin`.
- **New libtorch models: release `v0.2.0-beta`** — one `.zip` per UD language (60
  languages). Confirmed layout (example `fra-UD_French-GSD.zip`):

  ```
  embd/<lang>-...-.ftz                 fastText embeddings
  RnnTokenizer/ud/tokenizer-<treebank>.pt
  RnnTagger/ud/tagger-<treebank>.pt
  RnnLemmatizer/ud/                    EMPTY  ← no neural lemmatizer model yet
  RnnDependencyParser/ud/              EMPTY  ← no parser model yet
  <treebank>.json                      config
  ```

This layout maps **exactly** onto the paths the Rnn units request, e.g.
`findFileInPaths(resources, "/RnnTokenizer/%1/%2.pt".arg("ud", "tokenizer-fra-UD_French-GSD"))`.
So installation is essentially **download + unzip into `$LIMA_RESOURCES`**.

**Coverage with currently published models:** tokenization ✅, PoS tagging +
morphosyntactic features ✅. **Not covered:** neural lemmatization, dependency
parsing, NER (those dirs are empty / absent). Decisions needed (see below).

## Phased plan

### Phase 0 — De-risk on one language (English or French)
1. Build with `-T` (no TF), confirm `deeplima`/`DeepLimaUnits` compile against
   `extern/libtorch`.
2. Download one `v0.2.0-beta` language zip, unzip into `$LIMA_RESOURCES`.
3. Run a Rnn-only pipeline on sample text; diff CoNLL-U vs the TF `deepud` output
   for tokenization + UPOS + features. **Gate:** acceptable → proceed.

### Phase 1 — Make libtorch the default path (no deletion yet)
4. Edit `conf/lima-lp-ud.xml`: create a fully-Rnn pipeline. The existing
   `deepud-future` is close but still calls `tfmorphosyntax`/`tflemmatizer`;
   replace those with `RnnTokensAnalyzer`. Target sequence:
   `RnnTokenizer → RnnTokensAnalyzer → [SpecificEntities modex] → [RnnDependencyParser] → conllDumper`.
5. Rework `lima_models.py` (and rename/relocate out of `TensorFlowUnits/scripts/`):
   download `v0.2.0-beta` `.zip` per language and extract into the resource tree
   (`RnnTokenizer/`, `RnnTagger/`, `embd/`). Keep the `c2lc.txt` lang→treebank map.
6. Keep TF building in parallel for A/B comparison.

### Phase 2 — Remove TensorFlow
7. Delete `core/TensorFlowUnits/` and `core/TensorflowSpecificEntities/`; remove
   `add_subdirectory` lines at `core/CMakeLists.txt:36-38`.
8. Remove TF from root `CMakeLists.txt`: `find_package(TensorFlow)` + blocks at
   `:111-152`, `:399`, and the `lima-tf` vs `lima` package-name switch at `:558-562`.
9. Strip TF from tooling/docs: `gbuild.sh` (`-T`/`-P` options, the Docker
   `aymara/manylinux_..._tensorflow_for_lima_1_9` pull at `:243-256`,
   `-DTF_SOURCES_PATH`), `setenv-lima.sh`, `INSTALL.md`, `WINDOWS_README.md`,
   `continuous_integration/*tensorflow*`, `CLAUDE.md`.
10. Make the Rnn pipeline the default `deepud`/`deepud-pretok`; delete the TF unit
    `<group>`s from `conf/lima-lp-ud.xml`.

### Phase 3 — Tidy
11. Promote libtorch to a required dependency in docs; ensure
    `extern/download_libtorch.sh` is part of the standard setup.
12. Update CLAUDE.md architecture section to drop the dual-backend description.

## Open decisions (carried over)

1. **Lemmatization**: `RnnLemmatizer` models are not published. Options: (a) accept
   degraded/empty neural lemmas for now (`RnnTokensAnalyzer` already tolerates a
   missing lemmatizer model), (b) fall back to LIMA's dictionary-based
   lemmatization, or (c) train models via `deeplima/apps/train-lemmatization`.
2. **Dependency parsing & NER**: no published models either. Decide whether they
   are in scope for "remove TF" or deferred (tokenizer + tagger is the minimum
   viable replacement of the default pipeline).
3. **Languages**: validate on English/French first, then roll out to all 60.

## Phase 0 results (2026-06-22, French)

Validated the libtorch path end to end on French; the decision gate is **passed**.

**Build environment fixes required (committed to LIMA-tracked files):**
- `CMakeLists.txt` + `AbstractFactoryPattern/CMakeLists.txt`: Boost.System is header-only
  in Boost ≥1.69 and ships no component config in Boost 1.90, so `system` was removed
  from `find_package(Boost ... COMPONENTS ...)` and the `Boost::system` link replaced
  with `Boost::headers`.
- `extern/CMakeLists.txt`: force-include `<cstdint>` for the fastText targets
  (`add_compile_options(-include cstdint)`) — GCC 13+ no longer pulls it in transitively.
- System dev packages needed: `libboost-all-dev libicu-dev libtre-dev`.

**Key result: libtorch 1.13.1 (extern/libtorch, CPU) compiles cleanly against GCC 15.2** —
deeplima + DeepLimaUnits build with only warnings. Built with `./gbuild.sh -m Release -T -g OFF`.

**Models:** unzip `fra-UD_French-GSD.zip` (lima-models v0.2.0-beta) directly into
`$LIMA_RESOURCES` — its internal layout matches the unit paths as-is.

**Invocation recipe (important):** the model files embed the *full treebank name*
(`tagger-fra-UD_French-GSD.pt`). `fix_lang_codes` (LangCodeHelpers.h) + MediaticData's
auto-mapping mean you must pass the full name as the language so `udlang` resolves to it:
`analyzeText -l fra-UD_French-GSD -p <pipeline> file.txt`. Passing `ud-fra` yields
`udlang=fra` and the model is not found.

**Pipeline:** no shipped pipeline is TF-free, so a pure-Rnn pipeline was added (for the
test, to the *installed* configs only — must be ported to source in Phase 1):
- `lima-lp-ud.xml`: new `<group name="rnn">` = `RnnTokenizer → RnnTokensAnalyzer → conllDumper`.
- `lima-analysis.xml`: new `<map name="rnn"><entry key="ud" value="rnn"/></map>` in the
  `pipelines` group (pipelines must be registered here per media, keyed by map name).

**Output quality:** correct tokenization, UPOS, and morphological features (Gender,
Number, Tense, Mood, Person, Definite, PronType, VerbForm). `LEMMA` and `HEAD/DEPREL`
empty — expected, since v0.2.0-beta ships no lemmatizer/parser models (open decision #1/#2).

## Phase 1 results (2026-06-22)

- **Pipeline ported to source.** `lima_linguisticprocessing/conf/lima-lp-ud.xml`: the
  `deeplima` pipeline is now pure libtorch (`RnnTokenizer → RnnTokensAnalyzer →
  conllDumper`; the redundant `tfmorphosyntax` was removed — RnnTokensAnalyzer already
  does PoS+features+lemma). No `lima-analysis.xml` change needed: its `deeplima` map
  already registers media `ud`. Rebuilt (`-T`) and re-verified French from a clean
  install with `analyzeText -l fra-UD_French-GSD -p deeplima`.
- **`lima_models.py` reworked** (still under TensorFlowUnits/scripts/ for now; relocate
  in Phase 2) to install the v0.2.0-beta torch `.zip` models instead of TF `.deb`:
  - Drops `arpy`/`.deb`; uses stdlib `zipfile` (requirements.txt: `tqdm`, `requests`).
  - Resolves a language code/name **or** explicit treebank; auto-picks the best
    treebank from `best_models.json` (POS accuracy, falling back to token segmentation),
    overridable by naming a treebank. Verified: `fra`/`french` → `fra-UD_French-GSD`,
    `eng` → `eng-UD_English-GUM`, `deu` → `deu-UD_German-HDT`; explicit override works.
  - Asset index built from the GitHub release API (code is the asset-name prefix);
    `-i` lists languages/treebanks (119 codes, 220 treebanks), `-L` lists installed.
  - Note: the lima-models `best_models.json` has a `lemming` task for 9 languages
    (incl. French) but the French GSD zip ships an *empty* `RnnLemmatizer/` — packaging
    gap to confirm upstream.

**Carried-forward enhancement (not done):** let users pass `-l fra` to `analyzeText`
(not the full `fra-UD_French-GSD`). Needs a C++ change in LangCodeHelpers/MediaticData
+ a c2lc short-code→treebank map; deferred.

## Phase 2 results (2026-06-22) — TensorFlow removed

Verified: clean build from scratch with TF gone, no TF plugins installed, and the
**default `deepud` pipeline now runs on libtorch** (correct French output).

Removed/changed:
- **Deleted** `core/TensorFlowUnits/` and `core/TensorflowSpecificEntities/`, plus
  `cmake/Modules/FindTensorFlow.cmake`. `lima_models.py` was `git mv`'d to
  `core/DeepLimaUnits/scripts/` and wired into `DeepLimaUnits/CMakeLists.txt`.
- **CMake**: dropped the `find_package(TensorFlow)`/Eigen/Protobuf TF block and the
  `TF_SOURCES_PATH` message from the root `CMakeLists.txt`; package name is now
  always `lima` (was `lima-tf` when TF present); removed the TF `add_subdirectory`
  in `core/CMakeLists.txt` and the `tensorflowSpecificEntities` templating in
  `conf/CMakeLists.txt`.
- **gbuild.sh**: removed `-T`/`-P` options, `USE_TF`/`TF_SOURCES_PATH`, the
  `aymara/manylinux_..._tensorflow_for_lima` docker pull, and `-DTF_SOURCES_PATH`.
- **Configs**: removed all TF unit groups/items. `deepud` and `deepud-pretok` are now
  libtorch (`lima-lp-ud.xml`, `lima-lp-ud-eng.xml`). In the legacy `lima-lp-eng.xml`,
  `lima-lp-fre.xml` and the automatonCompiler test conf, `cpptftokenizer` → classic
  `flattokenizer` and the TF NER (`tensorflowSpecificEntities*`) → rule-based
  `SpecificEntitiesModex`. **These legacy non-UD configs were edited mechanically but
  NOT runtime-validated** (no eng/fre resources/models in this environment).
- **Docs**: INSTALL.md, WINDOWS_README.md, CLAUDE.md updated; the three
  `continuous_integration/*tensorflow_for_lima*` files deleted.

Remaining (future): lemmatization models (open #1), NER + dependency-parser models
(open #2), and the `-l fra` short-code convenience. Note `deepud` dropped the
rule-based NER/compounds that the old TF `deepud` had — re-add once validated /
once parser models exist.
