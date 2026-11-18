#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2022-2026 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

"""Install, list and remove the LIMA deep learning models (tokenizers,
taggers, lemmatizers and dependency parsers trained on Universal Dependencies).

The models are published on the Hugging Face Hub, in the aymaralima/deeplima
repository, one git tag per release. A release holds a manifest, models.json,
listing for each language its default model and the other ones (a merged
corpus' model, its treebanks' own ones, the most accurate variants), and, for
each model, its zip, the fastText embeddings it loads (stored once, shared by
the models of a language), their sizes and sha256 sums, licenses and scores.

Installing a model unzips it into a LIMA resources directory (by default
$XDG_DATA_HOME/lima/resources, i.e. ~/.local/share/lima/resources) and copies
its embeddings into its embd/ subdirectory. What was installed is recorded in
<resources>/lima_models.json, which removal and the language defaults (the
model a "ud-<code>" analysis uses without an explicit udlang) rely on.

This file is shared, unchanged, by LIMA (lima_models.py) and the aymara Python
package (the lima_models command): it only needs the standard library (tqdm,
when installed, shows the download progress).

Command line, both conventions of the former scripts being understood:

    lima_models -i fra | -l fra | --install fra   install French's default model
    lima_models -i fra-UD_French-MERGE              install a given model
    lima_models -i fra --accurate                 its most accurate variant
    lima_models -a | -i                           available languages and models
    lima_models -l | -L                           installed models
    lima_models -r fra [-f]                       remove French's models

Environment: LIMA_MODELS_URL (default https://huggingface.co/aymaralima/deeplima;
a local directory holding models.json works too) and LIMA_MODELS_REVISION (the
release tag, default MODELS_REVISION below).
"""

import argparse
import hashlib
import json
import os
import shutil
import sys
import tempfile
import urllib.request
import zipfile
from pathlib import Path
from typing import List, Optional, Tuple

# The models release this version of LIMA / aymara is tested with
MODELS_REVISION = "v0.3.0"
MODELS_URL = "https://huggingface.co/aymaralima/deeplima"
REGISTRY = "lima_models.json"

_MANIFEST = None


#############################################
# Private functions

def _yesnoconfirm(msg: str) -> bool:
    """Asks the user to answer y or n. True only for y (or Y)."""
    try:
        answer = input(f"{msg} [y|N]: ").lower()
    except (KeyboardInterrupt, EOFError):  # pragma: no cover
        print()
        return False
    return answer == "y"


def _get_target_dir(dest: Optional[str] = None) -> str:
    """The LIMA resources directory to install into: dest, else
    $XDG_DATA_HOME/lima/resources, else ~/.local/share/lima/resources."""
    if dest:
        return dest
    prefix = os.environ.get("XDG_DATA_HOME") or os.path.join(
        os.path.expanduser("~"), ".local", "share")
    return os.path.join(prefix, "lima", "resources")


def _base_url() -> str:
    base = (os.environ.get("LIMA_MODELS_URL") or MODELS_URL).rstrip("/")
    if os.path.isdir(base):
        return base
    revision = os.environ.get("LIMA_MODELS_REVISION") or MODELS_REVISION
    return f"{base}/resolve/{revision}"


def _fetch(path: str, target: str, size: int = 0) -> None:
    """Copies the release file <path> to target, from the Hub or a local
    directory, with a progress bar when tqdm is available."""
    base = _base_url()
    if os.path.isdir(base):
        shutil.copyfile(os.path.join(base, path), target)
        return
    request = urllib.request.Request(f"{base}/{path}", headers={"User-Agent": "lima_models"})
    try:
        from tqdm import tqdm
    except ImportError:  # pragma: no cover
        tqdm = None
    with urllib.request.urlopen(request) as response, open(target, "wb") as out:
        total = int(response.headers.get("Content-Length") or size or 0)
        bar = (tqdm(total=total, unit="iB", unit_scale=True, desc=os.path.basename(path))
               if tqdm and total > 1 << 20 else None)
        for block in iter(lambda: response.read(1 << 20), b""):
            out.write(block)
            if bar:
                bar.update(len(block))
        if bar:
            bar.close()


def _sha256(path: str) -> str:
    h = hashlib.sha256()
    with open(path, "rb") as f:
        for block in iter(lambda: f.read(1 << 24), b""):
            h.update(block)
    return h.hexdigest()


def _download(entry: dict, target: str) -> None:
    """Downloads a manifest file entry ({path, size, sha256}) to target,
    checking its checksum; raises RuntimeError on a mismatch."""
    Path(target).parent.mkdir(parents=True, exist_ok=True)
    tmp = target + ".part"
    _fetch(entry["path"], tmp, entry.get("size", 0))
    if entry.get("sha256") and _sha256(tmp) != entry["sha256"]:
        os.unlink(tmp)
        raise RuntimeError(f"{entry['path']}: checksum mismatch, download corrupted")
    os.replace(tmp, target)


def _manifest() -> dict:
    """The release's models.json (fetched once)."""
    global _MANIFEST
    if _MANIFEST is None:
        with tempfile.TemporaryDirectory() as tmp:
            target = os.path.join(tmp, "models.json")
            _fetch("models.json", target)
            with open(target, encoding="utf-8") as f:
                _MANIFEST = json.load(f)
    return _MANIFEST


def _unlink(path: str) -> None:
    """Removes a file if it exists (Path.unlink(missing_ok) needs Python 3.8)."""
    try:
        os.unlink(path)
    except FileNotFoundError:
        pass


def _registry(target_dir: str) -> dict:
    path = os.path.join(target_dir, REGISTRY)
    if os.path.exists(path):
        with open(path, encoding="utf-8") as f:
            return json.load(f)
    return {"models": {}, "defaults": {}}


def _save_registry(target_dir: str, registry: dict) -> None:
    path = os.path.join(target_dir, REGISTRY)
    with open(path + ".tmp", "w", encoding="utf-8") as f:
        json.dump(registry, f, indent=1, sort_keys=True)
    os.replace(path + ".tmp", path)


def _find_lang_code(lang_or_code: str) -> Tuple[Optional[str], Optional[str]]:
    """(language code, model stem) for a language code (fra or fr), a
    language name (french, case-insensitive) or a model stem
    (fra-UD_French-MERGE): the language's default model unless a model is
    named. (None, None) when nothing matches."""
    manifest = _manifest()
    if lang_or_code in manifest["models"]:
        return manifest["models"][lang_or_code]["tri"], lang_or_code
    wanted = lang_or_code.lower()
    for code, language in manifest["languages"].items():
        if wanted in (code, language["lang"].lower(), language["name"].lower()):
            return code, language["default"]
    return None, None


def _installed_stems(target_dir: str, language: str) -> List[str]:
    """The installed models of a language code, name or model stem."""
    models = _registry(target_dir)["models"]
    if language in models:
        return [language]
    wanted = language.lower()
    return [stem for stem, m in models.items()
            if wanted in (m["tri"], m.get("lang", "").lower(), m.get("name", "").lower())]


#############################################
# Public functions

def default_udlang(code: str, dest: Optional[str] = None) -> Optional[str]:
    """The installed default model of a language (e.g. "eng" ->
    "eng-UD_English-MERGE"), to give LIMA as udlang; None if none is
    installed. Reads only the local registry (no network)."""
    return _registry(_get_target_dir(dest))["defaults"].get(code)


def info() -> None:
    """Prints the languages and models available in the release."""
    manifest = _manifest()
    print(f"LIMA models {manifest['version']} (Universal Dependencies "
          f"{manifest['ud_version']}), {len(manifest['languages'])} languages:")
    for code, language in sorted(manifest["languages"].items(),
                                 key=lambda kv: kv[1]["name"]):
        others = [s for s in language["models"] if s != language["default"]]
        print(f"  {language['name']} ({code}): {language['default']}"
              + (f"; also {', '.join(others)}" if others else ""))


def list_installed_models(dest: Optional[str] = None) -> None:
    """Prints the installed models."""
    target_dir = _get_target_dir(dest)
    registry = _registry(target_dir)
    if not registry["models"]:
        print(f"No model installed in {target_dir}")
        return
    print(f"Models installed in {target_dir}:")
    for stem, m in sorted(registry["models"].items()):
        default = " [default]" if registry["defaults"].get(m["tri"]) == stem else ""
        print(f"  {m.get('name', m['tri'])} ({m['tri']}): {stem}, {m['variant']}, "
              f"{', '.join(m.get('components', []))}, release {m['version']}{default}")


def install_language(language: str, dest: Optional[str] = None,
                     select: Optional[List[str]] = None, force: bool = False,
                     accurate: bool = False) -> bool:
    """Installs a language's default model, or the named model, with its
    embeddings. accurate: its most accurate variant, when the release has one.
    select is ignored (all of a model's components are installed together).
    False when the language is unknown or the model already installed (and
    not force)."""
    del select   # kept for the former API
    code, stem = _find_lang_code(language)
    if stem is None:
        print(f"Language or model {language} is not available. You can check "
              f"the available ones with lima_models -a", file=sys.stderr)
        return False
    manifest = _manifest()
    model = manifest["models"][stem]
    variant = "accurate" if accurate and "accurate" in model["variants"] else "speed"
    if accurate and variant != "accurate":
        print(f"{stem} has no more accurate variant: installing its default one")
    files = model["variants"][variant]
    target_dir = _get_target_dir(dest)
    Path(target_dir).mkdir(parents=True, exist_ok=True)
    registry = _registry(target_dir)
    installed = registry["models"].get(stem)
    if (installed and not force and installed["version"] == manifest["version"]
            and installed["variant"] == variant):
        print(f"{stem} ({variant}) is already installed in {target_dir} "
              f"(force to reinstall it)")
        return False
    print(f"Installing {stem} ({variant}, {manifest['version']}) into {target_dir}")
    with tempfile.TemporaryDirectory() as tmp:
        archive = os.path.join(tmp, os.path.basename(files["path"]))
        _download(files, archive)
        with zipfile.ZipFile(archive) as z:
            members = [m for m in z.namelist() if not m.endswith("/")]
            z.extractall(target_dir)
    embeddings = []
    for name in files.get("embeddings", []):
        entry = manifest["embeddings"][name]
        target = os.path.join(target_dir, "embd", name)
        if (force or not os.path.exists(target)
                or os.path.getsize(target) != entry.get("size", -1)):
            _download(entry, target)
        embeddings.append(os.path.join("embd", name))
    registry["models"][stem] = {
        "tri": code, "lang": model["lang"],
        "name": manifest["languages"][code]["name"],
        "version": manifest["version"], "variant": variant,
        "components": files.get("components", []),
        "files": sorted(m.lstrip("./") for m in members), "embeddings": embeddings}
    # The language's default: its default model when installed, else the
    # first model installed for it
    if stem == manifest["languages"][code]["default"] or code not in registry["defaults"]:
        registry["defaults"][code] = stem
    _save_registry(target_dir, registry)
    print(f"Installed {stem}: use it with udlang {stem}"
          + (f" (the default of {code})" if registry["defaults"][code] == stem else ""))
    return True


def remove_language(language: str, dest: Optional[str] = None,
                    force: bool = False) -> bool:
    """Removes the installed models of a language (code or name), or the
    named model, and the embeddings no other installed model uses. Asks for
    confirmation unless force. False when nothing was removed."""
    target_dir = _get_target_dir(dest)
    stems = _installed_stems(target_dir, language)
    if not stems:
        print(f"No installed model for {language} in {target_dir}")
        return False
    if not force and not _yesnoconfirm(f"Remove {', '.join(stems)} from {target_dir}?"):
        return False
    registry = _registry(target_dir)
    freed = set()
    for stem in stems:
        model = registry["models"].pop(stem)
        for f in model["files"]:
            _unlink(os.path.join(target_dir, f))
        freed.update(model["embeddings"])
        if registry["defaults"].get(model["tri"]) == stem:
            others = [s for s, m in registry["models"].items() if m["tri"] == model["tri"]]
            if others:
                registry["defaults"][model["tri"]] = others[0]
            else:
                del registry["defaults"][model["tri"]]
        print(f"Removed {stem}")
    still_used = {e for m in registry["models"].values() for e in m["embeddings"]}
    for f in freed - still_used:
        _unlink(os.path.join(target_dir, f))
    _save_registry(target_dir, registry)
    return True


def main():  # pragma: no cover
    parser = argparse.ArgumentParser(
        description="Install, list and remove the LIMA deep learning models.")
    parser.add_argument("-i", "--install", nargs="?", const="", metavar="LANGUAGE",
                        help="install the model of a language (code or name, e.g. fra or "
                             "french) or a given model (e.g. fra-UD_French-MERGE); "
                             "without a value: list the available ones")
    parser.add_argument("-l", "--list", nargs="?", const="", metavar="LANGUAGE",
                        help="without a value: list the installed models; with one: "
                             "install it, as -i")
    parser.add_argument("-L", "--installed", action="store_true",
                        help="list the installed models (as -l without a value)")
    parser.add_argument("-a", "--avail", action="store_true",
                        help="list the available languages and models")
    parser.add_argument("-r", "--remove", metavar="LANGUAGE",
                        help="remove the models of a language, or a given model")
    parser.add_argument("--accurate", action="store_true",
                        help="install the most accurate variant of the model")
    parser.add_argument("-d", "--dest", help="LIMA resources directory "
                        "(default: $XDG_DATA_HOME/lima/resources)")
    parser.add_argument("-s", "--select", help=argparse.SUPPRESS)
    parser.add_argument("-f", "--force", action="store_true",
                        help="reinstall an installed model; remove without asking")
    args = parser.parse_args()

    language = args.install or args.list
    try:
        if args.remove:
            ok = remove_language(args.remove, args.dest, args.force)
        elif language:
            # Already installed is not a failure of the command
            ok = (install_language(language, args.dest, None, args.force, args.accurate)
                  or _find_lang_code(language)[1] in _registry(_get_target_dir(args.dest))["models"])
        elif args.list == "" or args.installed:
            list_installed_models(args.dest)
            ok = True
        elif args.avail or args.install == "":
            info()
            ok = True
        else:
            parser.print_help()
            ok = False
    except (OSError, RuntimeError, ValueError) as e:
        print(f"lima_models: {e}", file=sys.stderr)
        ok = False
    sys.exit(0 if ok else 1)


if __name__ == "__main__":  # pragma: no cover
    main()
