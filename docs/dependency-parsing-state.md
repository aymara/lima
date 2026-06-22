# State of libtorch/deeplima dependency parsing in LIMA

Investigated 2026-06-22. Summary: **the in-tree (master) dependency parser is an
incomplete, experimental work-in-progress with no trained models.** A separate,
unmerged branch (`deeplima-stanza-dp`) started importing Stanford NLP's **Stanza**
parser (Apache-2.0, license-compatible with LIMA's MIT) and is more advanced, but
also unfinished.

This contrasts with lemmatization, which is code-complete and only lacked packaged
models (see `docs/tf-removal-plan.md` and the lemmatization notes).

## What exists on `master`

Code locations:
- `deeplima/libs/tasks/graph_dp/` — `model/` (`birnn_and_deep_biaffine_attention`,
  the Dozat & Manning graph-based biaffine parser) and `train/`. **No `inference/`**
  subdir (lemmatization/segmentation/tag each have one).
- `deeplima/include/deeplima/dependency_parser.h` — the inference wrapper used by LIMA.
- `deeplima/include/deeplima/graph_dp/impl/arborescence.h` — MST decoding scaffolding.
- `deeplima/include/deeplima/eigen_wrp/graph_dp_eigen_inference_impl.h`.
- `lima_linguisticprocessing/.../core/DeepLimaUnits/RnnDependencyParser/` — the LIMA
  process unit (`RnnDependencyParser`, class id `RnnDependencyParser`).

### Why it is not usable as-is
1. **No label (deprel) prediction.** `dependency_parser.h` has only `head()` on its
   token iterator and the comment `// TODO where is stored the rel type ???`. The arc
   *labels* (UD deprels) are never produced.
2. **Head wiring is a placeholder.** In `dependency_parser.h` (~lines 356-360):
   `token.m_head_idx = iter.position();` (head = the token's own position) with
   `// TODO lines below were commented out to avoid crash. Does it really work?`.
3. **LIMA unit is debug-grade.** `RnnDependencyParser.cpp` uses `LERROR <<` as print
   statements and writes arcs with a hardcoded relation:
   `syntacticData->addRelationNoChain(1, curToken, head)` — i.e. a constant label, not
   the predicted deprel. `insertDependencies()` collects only `ti.head()`.
4. **Commented out of every pipeline** in the `conf/lima-lp-*.xml` files.
5. **No models.** `best_models.json` (lima-models) contains only segmentation, tagging
   and lemming tasks — there is no parsing task and no published or trained DP model
   (the per-corpus JSON has `"dp": null`).

Training scaffolding does exist: `deeplima-train-dp` (apps) and
`deeplima-training-scripts/dependency-parsing/` (`dp-train.py`, `dp-train.slurm`,
`eval_dependency_parsing.py`).

## The `deeplima-stanza-dp` branch (the "import from elsewhere" attempt)

Tip `a1222073 Update Stanza impl`, **2023-02-28, NOT merged into master** (~16 months
stale; predates the TF removal and the modern-toolchain build fixes).

It ports **Stanza** (Stanford NLP, Apache-2.0) at the NN/model level. `git diff
master...deeplima-stanza-dp` touches (deeplima only):
- `nn/torch_modules/stanza_models_depparse_model.{cpp,h}` (NEW, ~880 lines) — Stanza
  depparse model port.
- `nn/torch_modules/deep_biaffine_attention_label_decoder.{cpp,h}` (NEW) — the
  **missing label/deprel decoder**.
- `nn/torch_modules/deep_biaffine_attention_decoder.{cpp,h}`, `static_graph.{cpp,h}`
  (large rework), `convert/from_torch/graph_dp_model.cpp`, `train/train_graph_dp.cpp`.

What it does NOT do:
- It does not touch `dependency_parser.h` or the LIMA unit `RnnDependencyParser.cpp`
  (unchanged from master), so the LIMA-side integration (emitting real heads+deprels
  into the CoNLL-U output) is still unfinished.
- No trained models.
- Stale license header: the new label-decoder file carries an **AGPL** header
  (leftover from before LIMA's MIT relicensing) — must be corrected to MIT.

## Licensing options for importing

All compatible with LIMA's MIT license:
- **Stanza** (Stanford NLP) — Apache-2.0. Already the basis of `deeplima-stanza-dp`.
- **Trankit** — Apache-2.0.
- **spaCy** — MIT.

## Recommended path to finish

Building on the existing Stanza work is the most efficient route:

1. **Assess `deeplima-stanza-dp`** (in progress): build it (will need the modern-toolchain
   fixes from the TF-removal Phase 0 — Boost.System, `<cstdint>` for fastText — rebased),
   and run the deeplima CLI DP inference to see whether the model layer produces correct
   heads **and** labels independently of the LIMA wiring.
2. **Rebase** the branch onto current master (post-TF-removal).
3. **Finish the LIMA integration**: extend `dependency_parser.h` to expose the predicted
   deprel, and update `RnnDependencyParser.cpp` to map and write real labels (replace the
   hardcoded `addRelationNoChain(1, …)`), remove debug `LERROR` spam, and add the unit
   back to the `deepud` pipeline.
4. **Fix the AGPL header** → MIT on the imported files.
5. **Train models** (`deeplima-train-dp` / `dp-train.py`) and add a `parsing`/`dp` task to
   the lima-models packaging so `RnnDependencyParser/<lang>/dependencyParser-$udlang.pt`
   ships and `lima_models.py` installs it (it already extracts whatever is in the zip).

If the branch's model layer turns out to be incomplete/incorrect, the fallback is a fresh
port from Stanza/Trankit/spaCy using the same biaffine architecture already present.

## Worktree assessment of `deeplima-stanza-dp` (2026-06-22)

Checked out the branch in a git worktree (`/home/gael/Projets/Lima/lima-stanza-dp`),
symlinked the already-downloaded `extern/libtorch` and `extern/fastText`, cherry-picked the
modern-toolchain build fixes (Boost.System, fastText `<cstdint>`) and added a deeplima-wide
`-include cstdint` (the stale branch lacks the GCC-13+ include hygiene), then built the DP
targets `deeplima-train-dp` and `deeplima-eigen`.

**Findings:**
- **Build feasibility: GOOD.** All deeplima C++ compiled, *including the whole Stanza DP
  stack* — `deep_biaffine_attention_label_decoder.cpp`, `deep_biaffine_attention_decoder.cpp`,
  `graph_dp`, `convert_from_torch`, `conllu`. So the imported Stanza model code is
  compatible with the current toolchain (libtorch 1.13.1 / GCC 15.2) after only trivial
  `<cstdint>` additions. The build then failed at *link* time on **fastText** (undefined
  `boost::iostreams::lzma_error::check` / `zlib_error::check`) — a Boost.IOStreams
  lzma/zlib backend linking issue, orthogonal to dependency parsing (master links fastText
  differently and builds fine).
- **Functional completeness: still incomplete (same gap as master).** On this branch too,
  `deeplima/include/deeplima/dependency_parser.h` keeps the placeholder
  `token.m_head_idx = iter.position();` and `m_rel_type = 0; // TODO where is stored the
  rel type ???`, and `dumper_conllu.h` assigns deprels heuristically (`"root"`/`"dep"`),
  not from the model. The deeplima CLI does expose `--dp-model` and a "with dependency
  parsing" code path, and `feat_extractors.h` knows `token.deprel()` — but the predicted
  **labels are not threaded from the model back into the output**. The LIMA unit
  `RnnDependencyParser.cpp` is unchanged from master (heads only, hardcoded label `1`).

**Conclusion:** the Stanza import is the right base and is build-compatible, but it is a
*model/training-layer* contribution whose **inference→output wiring (labels, and verifying
heads) was never finished**, on master or on the branch, and **no models were ever trained**.
Finishing it is a focused but real piece of work (wire labels through
`dependency_parser.h` + `RnnDependencyParser`, fix the fastText link, rebase, fix the AGPL
header, then train models) — not a quick "uncomment and ship".

Worktree left in place for follow-up; remove with
`git worktree remove ../lima-stanza-dp --force` (it has local assessment-only edits).

## Deep-dive findings on branch `finish-dependency-parsing` (2026-06-22)

Created off current master, ported the DP code from `deeplima-stanza-dp` (clean 3-way
patch — master's deeplima barely diverged since the branch base) and **relicensed the
whole DP subsystem AGPL-3.0 → MIT** (confirmed CEA LIST own work; ~20 files:
`deeplima/libs/tasks/graph_dp/**`, the biaffine decoders, `train-dp.cpp`).

**The hard blocker for labeled parsing is the Eigen inference engine**, not just the LIMA
wiring. LIMA's `RnnDependencyParser` runs the hand-written Eigen inference
(`graph_dp_eigen_inference_impl.h`), whose `predict()` executes only:
encoder (BiLSTM, `m_ops[0]`) → one arc decoder (`Op_DeepBiaffineAttnDecoder`, `m_ops[1]`)
→ `(*output)[0]` (heads). There is:
- **no Eigen label decoder** (`deep_biaffine_attention_label_decoder` is a *torch training*
  module only; no `Op_*` Eigen counterpart, not converted by `convert/from_torch`, not in
  `predict()`), so deprels are never produced at inference;
- **arborescence/MST tree decoding commented out** (line ~131), so even heads are raw
  argmax (possible cycles / invalid trees).

Consequently the earlier "just wire labels" estimate is wrong. Real remaining work:
1. Implement an Eigen `Op_DeepBiaffineAttnLabelDecoder` (forward pass mirroring the torch
   label decoder) and add it as `m_ops[2]` → `(*output)[1]` (rel column).
2. Extend `convert/from_torch/graph_dp_model.cpp` to convert the label-decoder weights.
3. Re-enable/finish arborescence so heads form valid trees.
4. Expose `rel()`/label in `DependencyParser::TokenIterator` (read column 1) + map the rel
   index to the UD deprel string via the model's rel dict.
5. In `RnnDependencyParser.cpp`: collect labels, map rel → LIMA syntactic relation type
   (replace the hardcoded `addRelationNoChain(1, …)`), remove debug `LERROR` spam, add the
   unit to the `deepud` pipeline.
6. **Train a model** with `deeplima-train-dp --tasks arc,rel` on a UD treebank + the
   fastText embeddings (no model exists; needs compute, ideally the cluster).

This is a multi-day implementation effort (the Eigen label-decoder + tree decoding are the
bulk), and it cannot be end-to-end verified until a model is trained. The torch *training*
path (with label decoder) exists, so the architecture is there; the fast CPU *inference*
path is the missing half.

**Status of this branch:** code ported + relicensed to MIT + builds (validates the port).
Inference label path and a trained model are the remaining work.
