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

## Training code verification (2026-06-22)

Reviewed `deeplima-train-dp` end to end. **The training code only supports the ARC task;
the REL (deprel/label) task is absent at every layer** — which structurally forces LAS≈0
and almost certainly explains the "very low scores" of the cluster models. The cluster
script itself confirms it: `deeplima-training-scripts/dependency-parsing/dp-train.py`
runs `deeplima-train-dp ... --tasks="(arc)+" -w 256` (arc only).

Evidence (rel task missing everywhere):
1. **Gold data has no labels.** `graph_dp/train/conllu_file_iterator.cpp`: the gold matrix
   is `TorchMatrix<int64_t>(total_timepoints, 1)` — one column (`:114`); `vectorize_bucket_gold`
   sets only `dst.set(t, 0, head._first)` (`:171`); the deprel read is commented out
   (`:161 // ... (*it).deprel();`).
2. **Model built arc-only.** `train_graph_dp.cpp:101` builds a single
   `deep_biaffine_attention_descr_t(128)` (arc decoder); no label-decoder descriptor/member.
   The imported `deep_biaffine_attention_label_decoder` is never instantiated, and the
   `stanza_models_depparse_model` is not used by training at all.
3. **Train call hardcodes arc.** `train_graph_dp.cpp:151` `model->train(params, { "arc" }, …)`
   ignores `params.m_tasks_string` (default is also `"arc"`, `train_params_graph_dp.h:35`).
4. **Loss/eval track only "arc"** (`birnn_and_deep_biaffine_attention.cpp:224-232`).

Arc-quality issues (would hurt UAS even once rel is added):
5. **No padding/root masking in the loss.** `birnn_seq_classifier.cpp:351`
   `nll_loss(o, this_task_target)` has no `ignore_index`; accuracy counts all timepoints
   incl. padding (`m_items += this_task_target.size(0)`), so the reported metric is over
   padded positions too — diluted gradient and misleading accuracy.
6. **Head/root index encoding** has commented-out adjustments
   (`conllu_file_iterator.cpp:171-179`) — audit for off-by-one / root handling.
7. Reported loss normalization mixes per-batch mean with per-item division (cosmetic).
8. Small default model (`m_rnn_hidden_dims={64}`; cluster used a single 256-dim layer).

**Conclusion:** re-running the cluster scripts as-is cannot produce a labeled parser — the
training code must first be extended to the rel task. Required work, in order:
(a) read deprel into a 2nd gold column + build a deprel dict;
(b) instantiate the label decoder in the model and produce rel scores (conditioned on gold
   head at train time, predicted head at inference);
(c) train tasks {arc, rel} with a rel loss and track LAS;
(d) add `ignore_index` masking for padding (arc and rel) and audit root indexing;
(e) then the matching inference side (Eigen label decoder, per the deep-dive section).

## Increment 1 implemented: deprel gold + root masking (2026-06-22, branch finish-dependency-parsing)

Done and compiling (deeplima-train-dp + deeplima-train-tag link clean):
- **Deprel gold column + dictionary.** `train_graph_dp.cpp` builds a deprel→id map from
  the training data (shared with dev for consistent ids) and passes it to the dataset.
  `conllu_file_iterator.{h,cpp}`: gold matrix widened to 2 columns (col0=head, col1=deprel
  id), `vectorize_bucket_gold` fills the rel column. Verified at runtime: prints
  "Built deprel dictionary: 46 labels" on UD_French-GSD. The rel column is inert until the
  label decoder lands (arc-only training slices col 0; the DP train/eval reshapes use
  dynamic dims so the 2-col gold is safe).
- **Root masking (the corrected "padding masking").** There is no padding in training
  (sentences are length-bucketed), so the real issue was the synthetic <ROOT> token being
  counted in the arc loss/accuracy. The synthetic root's gold is now `IGNORE_INDEX` (-100)
  in both columns; `birnn_seq_classifier.cpp` passes `ignore_index(-100)` to `nll_loss` and
  excludes ignored positions from accuracy. Harmless to tag/segmentation (their targets are
  never -100). NB: the earlier "head/root off-by-one" worry was unfounded — with <ROOT> at
  position 0 the CoNLL-U HEAD value maps directly to the timepoint position (the commented
  `-1` code would have been wrong).

**Separately surfaced (pre-existing, NOT from these changes):** training crashes on *raw*
UD data with `c10::IndexError` in `EmbeddingImpl::forward` — the synthetic <ROOT> token's
UPOS "ROOT" (and other synthetic feature values) are absent from the morph-feature dicts
built from the corpus, so the input embedding index is out of range. The same crash occurs
with the old (pre-change) binary. The cluster pipeline avoids it because its
`ud-treebanks-v2.10-trainable` data is preprocessed; raw UD is not. This blocks local
end-to-end validation and is a real robustness bug (unseen categorical feature values need
an UNK slot), to fix before/independently of the label-decoder work.

## Training-code bugs found & fixed — arc training now works (2026-06-22)

Smoke-testing the trainer (deeplima-train-dp on UD_French-GSD slices) revealed that the
DP training was fundamentally broken (it crashed before completing a step), which is the
likely real cause of the "very low scores". Three pre-existing bugs were fixed:

1. **Double `std::move(tag_dh)`** in `train_graph_dp.cpp` — the dict holder was moved into
   both the embedding-dicts arg and the output-classes arg; argument evaluation order is
   unspecified, so one ended up moved-from/empty (UB that behaves differently per build —
   why it "worked" on the cluster but crashed locally). Now passes two distinct holders.

2. **Embedding dict-index misalignment (the main crash).** The generated script numbers
   each input embedding `dict=<position in embd_descr>`, but the model was given the *full*
   tag dict holder while `embd_descr` is *filtered* (features with an empty dict are
   dropped) and has `raw` prepended. After the first dropped feature, every embedding read
   the wrong dict (often a size-0 one) → `c10::IndexError` in `EmbeddingImpl::forward`.
   Fixed by passing a dict holder built **parallel to `embd_descr`** (placeholder for the
   `raw` slot + the non-empty feature dicts in order; new `get_embd_feature_dicts()`), and
   sizing each embedding with `max(dict.size(), 1)` as belt-and-braces.
   This triggers whenever any morph feature has no values in the corpus (common, e.g.
   Emph/ExtPos/NumType in French), so it affected real training, not just tiny samples.

3. Plus the increment-1 changes (deprel gold column + dict, root masking via ignore_index).

**Result:** training now runs and learns. On a 5k-sentence French slice, dev **arc accuracy
rises 0.78 → 0.89 over 10 epochs** (was: immediate crash). The arc path is sound.
Remaining for a usable *labeled* parser: the label decoder (rel) — train+inference (step b).

## Step (b) scope finding: the imported label decoder is a STUB (2026-06-22)

Starting (b), the label decoder turns out not to be reusable as-is:
`deep_biaffine_attention_label_decoder.cpp::forward` returns `{r, r_label}` where BOTH are
`Wx + b` — `r_label` (line 131) is a verbatim copy of the arc score `r` (line 87). The
"# scorer (LABELS)" part is only comments (describing Dozat's separate rel MLPs + a bilinear
producing `[batch, dep, head, num_labels]`, conditioned on the head). No label scoring is
implemented, and `DeepBiaffineAttentionLabelDecoderImpl`'s constructor takes no `num_labels`.

So (b) is a from-scratch implementation, not an integration:
1. Implement the real biaffine label scorer: dep/head rel MLPs + `U` tensor of shape
   `[hidden, num_labels, hidden]` → label logits `[batch, dep, head, num_labels]`; add a
   `num_labels` constructor arg and parameters.
2. Integrate into the model: the static-graph script has no label-decoder module type, no
   `num_labels` plumbing, and no head-conditioned gather. Either extend the static graph
   (add module type + a gather-by-head op fed gold/predicted heads) or move DP to a
   hand-written forward (cf. the also-unfinished `stanza_models_depparse_model`).
3. Training: gather label logits at the gold head per token → log_softmax → nll_loss vs the
   deprel gold column (already added), track LAS.
4. Inference: a matching Eigen `Op` for the label decoder + the gather, then emit DEPREL in
   `RnnDependencyParser`.

This is a multi-day implementation. The arc trainer fix (this session) stands on its own and
should be committed first.

## Step (b) piece 1 done: real biaffine label scorer + unit test (2026-06-22)

Replaced the stub `DeepBiaffineAttentionLabelDecoder` with a real affine biaffine label
scorer (Dozat & Manning 2017):
- Constructor now takes `num_labels`. Dep/head rel MLPs (Linear+ELU), and a single
  augmented bilinear `U` of shape `[num_labels, hidden+1, hidden+1]` (the +1 folds in the
  linear terms and bias). `forward(input[seq,batch,dim])` →
  `einsum("bxi,lij,byj->bxyl")` = label logits `[batch, dep, head, num_labels]`.
- `input_includes_root`: when false, a learned <ROOT> row is prepended on the head axis
  (head = seq+1), matching DeepBiaffineAttentionDecoder's convention.
- Fixed a latent bug: the header's include guard was identical to the arc decoder's.
- Unit test `deeplima/tests/test_label_decoder.cpp` (+ CMake wiring, run in standalone
  deeplima builds): checks output shape, root handling, finiteness and gradient flow.
  Verified passing (compiled against the built torch_modules + libtorch).

Remaining for labeled parsing: piece 2 (integrate into the model — instantiate the label
decoder, size it with the deprel dict, produce label logits as a model output), piece 3
(training: gather logits at the gold head → log_softmax → nll_loss vs the deprel gold
column, track LAS), piece 4 (Eigen inference Op + emit DEPREL in RnnDependencyParser).

## Step (b) piece 2 done: label decoder integrated into the model (2026-06-22)

The label decoder is now a first-class static-graph module:
- `static_graph.{h,cpp}`: new module type `deep_biaffine_attention_label_decoder`, a
  `create_submodule_DeepBiaffineAttentionLabelDecoder` (parses input_dim/hidden_dim/
  num_labels/input_includes_root), an execute case, a `to(device)` entry, and the
  `DeepBiaffineAttentionLabelDecoder` parse-script branch.
- `generate_script` takes `num_labels`; when > 0 it emits `label_decoder_0` and a
  `rel_logits = forward module=label_decoder_0 input=<rnn_out>` producing
  `[batch, dep, head, num_labels]`.
- The model constructor takes `num_labels`; `train_graph_dp.cpp` passes the deprel dict
  size (here 58 for the French slice).

Verified: the model constructs with the label decoder and training runs unchanged for arc
(no regression; rel_logits is produced but not yet consumed). Next, piece 3 — consume
rel_logits in training: gather at the gold head, log_softmax over labels, nll_loss vs the
deprel gold column, and report LAS.

## Step (b) piece 3 done: labeled parsing trains (arc + rel) (2026-06-22)

Custom training/eval in BiRnnAndDeepBiaffineAttention consumes rel_logits:
- train_batch / evaluate now forward {arc, rel_logits}, compute the arc loss as before,
  and for rel gather the label logits at the gold head
  (rel_logits.gather(dim=2, gold_head)) -> log_softmax over labels -> nll_loss vs the
  deprel gold column, both with ignore_index (root/padding). A single combined backward
  + optimizer step. Per-task stats for arc and rel; train() computes accuracy/loss for
  every task (was hardcoded to "arc").
- train_graph_dp trains tasks {arc, rel} when a deprel dict exists.

Verified on the 5k French slice (-w 128, cpu): both tasks learn —
dev arc(UAS) 0.79 -> 0.87 and dev rel(label acc at gold head) 0.93 -> 0.956 over 5 epochs.
So the label decoder learns deprels. (Reported rel acc is conditioned on the gold head;
true LAS = head & label both correct, computed at inference once piece 4 lands.)

Remaining: piece 4 — Eigen inference Op for the label decoder + the head-gather, then emit
DEPREL in RnnDependencyParser (and a model that converts to the eigen engine).

## Step (b) piece 4a done: Eigen label-decoder op + conversion + test (2026-06-22)

The CPU-inference (Eigen) side of the label decoder:
- `deeplima/include/deeplima/eigen_wrp/deep_biaffine_attn_label_decoder.h`: new
  `params_deep_biaffine_attn_label_decoder_t` (per-label `U` as a vector of matrices) and
  `Op_DeepBiaffineAttnLabelDecoder` with `compute_logits()` (per-(dep,head) label logits,
  mirroring the torch einsum) and `predict_labels()` (gather at given heads -> argmax rel).
- `convert_from_torch.h`: a `convert_module_from_torch` overload reading the torch label
  decoder's mlp_dep/mlp_head/U (per label)/root into the eigen params.
- The torch label decoder's members are made public (like the arc decoder) + an
  `input_includes_root()` accessor, so the converter can read them.
- Unit test `deeplima/tests/test_eigen_label_decoder.cpp`: builds the torch decoder, copies
  its params into the eigen op, and checks the label logits match for both <ROOT>
  conventions. **Verified: max|torch-eigen| ~ 1e-6.**

Remaining piece 4: wire the label op into the eigen `predict()` (run after the arc op,
gather at the predicted head, emit a rel column), expose rel in
`DependencyParser::TokenIterator`, make `dumper_conllu` emit the predicted deprel, and have
`RnnDependencyParser` write real labels (+ true LAS).

## Step (b) piece 4 remaining: predict() wiring scope (2026-06-22)

Tracing the inference chain to wire predict() surfaced that it is not a localized edit and
has a training-side prerequisite:

1. **Deprel vocab is not persisted in the model.** The model save/load only writes
   SERIALIZATION_KEY_TASKS / INPUT_FEATURES(_NAMES) / EMBD_FN — there is NO rel/deprel
   class vocabulary. So a trained model has the label-decoder weights but no id->deprel
   string map; inference can produce rel ids but not deprel strings. Fixing this is a
   TRAINING-side change (save the deprel dict, e.g. SERIALIZATION_KEY_REL_CLASSES) + load
   + convert into the eigen inference. Models must then be (re)saved/trained.
2. **2-column output through the generic inference pool.** The arc head column is produced
   into GraphDependencyParser's shared StdMatrix<uint32_t> output (1 column); adding a rel
   column means threading a 2-wide output through the generic templated inference-pool /
   callback machinery (deeplima::graph_dp::impl::GraphDependencyParser, BiRnnInferenceBase).
3. predict() (graph_dp_eigen_inference_impl.h): after the arc op, run
   Op_DeepBiaffineAttnLabelDecoder.predict_labels(encoder_out, heads) -> output col 1.
   convert_from_torch (graph_dp_model.cpp): populate the eigen label params.
4. DependencyParser::TokenIterator::rel() reading col 1; map id->deprel via the loaded vocab.
5. dumper_conllu emit the deprel; RnnDependencyParser write real labels (+ true LAS).

So piece 4a (the Eigen op + conversion + test, done & validated) is the isolated core; the
rest is a deep, multi-file chain plus the vocab-persistence prerequisite, best done as a
focused effort.
