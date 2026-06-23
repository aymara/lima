#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

# -*- coding: utf-8 -*-

"""
Install LIMA deep-learning (libtorch/deeplima) models.

Models are distributed as one zip per UD treebank in the lima-models
``v0.2.0-beta`` release. Each zip already has the on-disk layout LIMA expects
(``RnnTokenizer/ud/``, ``RnnTagger/ud/``, ``embd/`` …), so installing is just
download + extract into the resources directory.

There are usually several treebanks per language. Given a language code or name
(e.g. ``fra`` or ``french``) the best treebank is chosen automatically using the
scores in ``best_models.json`` (POS accuracy for tagging, falling back to token
segmentation). An explicit treebank (e.g. ``fra-UD_French-GSD``) overrides the
automatic choice.

NB: this used to download the now-retired TensorFlow models (.deb). It will be
relocated out of TensorFlowUnits/ when those modules are removed.
"""

import argparse
import os
import sys
import tempfile
import urllib.request
import zipfile
from os import listdir
from os.path import isdir, isfile, join

import requests
from tqdm import tqdm

VERSION = "v0.2.0-beta"
RELEASE_URL = (
    "https://github.com/aymara/lima-models/releases/download/" + VERSION + "/"
)
API_URL = (
    "https://api.github.com/repos/aymara/lima-models/releases/tags/" + VERSION
)
BEST_MODELS_URL = RELEASE_URL + "best_models.json"
C2LC_URL = (
    "https://raw.githubusercontent.com/aymara/lima-models/" + VERSION + "/c2lc.txt"
)

# Caches populated lazily.
_ASSET_INDEX = None  # code -> { corpus: stem } where stem == "<code>-<corpus>"
_BEST_MODELS = None  # parsed best_models.json
_C2LC = {"lang2code": {}, "code2lang": {}}


def get_target_dir(dest=None):
    if dest:
        return dest
    if "XDG_DATA_HOME" in os.environ and len(os.environ["XDG_DATA_HOME"]) > 0:
        target_dir_prefix = os.environ["XDG_DATA_HOME"]
    else:
        target_dir_prefix = os.path.join(os.path.expanduser("~"), ".local", "share")
    return os.path.join(target_dir_prefix, "lima", "resources")


def get_asset_index():
    """Build {code: {corpus: stem}} from the release assets.

    Asset names look like ``fra-UD_French-GSD.zip``; the language code is the part
    before the first '-', the corpus is the remainder and the stem (used as the
    ``udlang`` value passed to lima, e.g. ``analyzeText -l fra-UD_French-GSD``) is
    the whole name without the ``.zip`` suffix.
    """
    global _ASSET_INDEX
    if _ASSET_INDEX is not None:
        return _ASSET_INDEX
    index = {}
    response = requests.get(API_URL)
    response.raise_for_status()
    for asset in response.json().get("assets", []):
        name = asset.get("name", "")
        if not name.endswith(".zip") or "-" not in name:
            continue
        stem = name[: -len(".zip")]
        code, corpus = stem.split("-", 1)
        index.setdefault(code, {})[corpus] = stem
    _ASSET_INDEX = index
    return index


def get_best_models():
    global _BEST_MODELS
    if _BEST_MODELS is None:
        response = requests.get(BEST_MODELS_URL)
        response.raise_for_status()
        _BEST_MODELS = response.json()
    return _BEST_MODELS


def load_c2lc():
    if len(_C2LC["lang2code"]) > 0:
        return
    with urllib.request.urlopen(C2LC_URL) as f:
        for line in f.read().decode("utf-8").split("\n"):
            line = line.strip()
            if not line or " " not in line:
                continue
            corpus, code = line.rsplit(" ", 1)
            lang = corpus.split("-")[0].lower()
            _C2LC["lang2code"][lang] = code
            _C2LC["code2lang"][code] = lang


def corpus_score(corpus):
    """A sortable quality score for a treebank, higher is better.

    Prefer POS tagging accuracy (the treebank must have a tagger to be usable in
    the deeplima pipeline); fall back to token segmentation accuracy.
    """
    best = get_best_models()
    tagging = best.get("tagging", {}).get(corpus)
    if tagging and "pos" in tagging and tagging["pos"].get("values"):
        # +1 keeps any usable tagger ranked above segmentation-only treebanks.
        return 1.0 + tagging["pos"]["values"][0]
    seg = best.get("segmentation", {}).get(corpus)
    if seg and "tokens" in seg and seg["tokens"].get("values"):
        return seg["tokens"]["values"][0]
    return 0.0


def resolve_treebank(language):
    """Resolve a user-supplied language to an installable treebank stem.

    Accepts an explicit treebank stem (``fra-UD_French-GSD``), a language code
    (``fra``) or a language name (``french``). For a code/name, the best-scoring
    treebank is selected from best_models.json.
    """
    index = get_asset_index()
    query = language.strip()

    # 1. Explicit treebank stem (case-insensitive match on the asset name).
    for code in index:
        for stem in index[code].values():
            if stem.lower() == query.lower():
                return code, stem

    # 2. A language code, or a language name resolved through c2lc.
    code = query.lower()
    if code not in index:
        load_c2lc()
        if code in _C2LC["lang2code"]:
            code = _C2LC["lang2code"][code]
    if code not in index:
        return None, None

    corpora = index[code]
    best_corpus = max(corpora, key=corpus_score)
    return code, corpora[best_corpus]


def download_zip(stem, directory):
    url = RELEASE_URL + stem + ".zip"
    local_filename = os.path.join(directory, stem + ".zip")
    response = requests.get(url, stream=True)
    response.raise_for_status()
    total = int(response.headers.get("content-length", 0))
    progress = tqdm(total=total, unit="iB", unit_scale=True)
    with open(local_filename, "wb") as f:
        for chunk in response.iter_content(chunk_size=8192):
            if chunk:
                progress.update(len(chunk))
                f.write(chunk)
    progress.close()
    return local_filename


def install_language(language, dest=None, force=False):
    target_dir = get_target_dir(dest)
    code, stem = resolve_treebank(language)
    if stem is None:
        print(
            "ERROR: no model found for '{}' in lima-models {}. "
            "Use -i to list available languages.".format(language, VERSION),
            file=sys.stderr,
        )
        return False

    if not force and is_installed(target_dir, stem):
        print("Model '{}' is already installed (use -f to reinstall).".format(stem))
        return False

    print("Language code: {}, treebank: {}".format(code, stem))
    print("Installation dir: {}".format(target_dir))
    os.makedirs(target_dir, exist_ok=True)
    with tempfile.TemporaryDirectory() as tmpdirname:
        print("Downloading {}".format(RELEASE_URL + stem + ".zip"))
        archive = download_zip(stem, tmpdirname)
        print("Extracting into {}".format(target_dir))
        with zipfile.ZipFile(archive) as zf:
            zf.extractall(target_dir)
    print(
        "Done. Analyze with e.g.: analyzeText -l {} -p deeplima <file>".format(stem)
    )
    return True


def is_installed(target_dir, stem):
    return isfile(join(target_dir, "RnnTokenizer", "ud", "tokenizer-" + stem + ".pt"))


def list_installed_models(dest=None):
    target_dir = get_target_dir(dest)
    found = {}
    for module, prefix in (
        ("RnnTokenizer", "tokenizer-"),
        ("RnnTagger", "tagger-"),
        ("RnnLemmatizer", "lemmatizer-"),
        ("RnnDependencyParser", "dependencyParser-"),
    ):
        moddir = join(target_dir, module, "ud")
        if not isdir(moddir):
            continue
        for f in listdir(moddir):
            if f.startswith(prefix) and f.endswith(".pt"):
                stem = f[len(prefix): -len(".pt")]
                found.setdefault(stem, set()).add(module)

    if not found:
        print("No models installed in {}".format(target_dir))
        return
    print("Installed models in {}:".format(target_dir))
    print("{:<32} tok tag lem dep".format("treebank"))
    for stem in sorted(found):
        mods = found[stem]
        mark = lambda m: " X " if m in mods else " . "  # noqa: E731
        print(
            "{:<32}{}{}{}{}".format(
                stem,
                mark("RnnTokenizer"),
                mark("RnnTagger"),
                mark("RnnLemmatizer"),
                mark("RnnDependencyParser"),
            )
        )


def info():
    index = get_asset_index()
    print("Available languages in lima-models {} (code: treebanks):".format(VERSION))
    for code in sorted(index):
        corpora = sorted(index[code])
        print("{:<8}\t{}".format(code, ", ".join(corpora)))


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "-i", "--info", action="store_true",
        help="print the list of available languages/treebanks and exit",
    )
    parser.add_argument(
        "-l", "--lang", type=str,
        help="language code, language name or explicit treebank to install "
        "(e.g. 'fra', 'french' or 'fra-UD_French-GSD')",
    )
    parser.add_argument("-d", "--dest", type=str, help="destination directory")
    parser.add_argument(
        "-f", "--force", action="store_true",
        help="force reinstallation of existing files",
    )
    parser.add_argument(
        "-L", "--list", action="store_true", help="list installed models",
    )
    args = parser.parse_args()

    if args.info:
        info()
        sys.exit(0)
    elif args.list:
        list_installed_models(args.dest)
        sys.exit(0)
    elif args.lang is None:
        print(
            "ERROR: the following argument is required: -l/--lang\n",
            file=sys.stderr,
        )
        parser.print_help()
        sys.exit(1)
    else:
        ok = install_language(args.lang, args.dest, args.force)
        sys.exit(0 if ok else 1)
