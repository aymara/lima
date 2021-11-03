#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import argparse
import os
import re
import sys
import tarfile
import tempfile
import urllib.request
from os import listdir
from os.path import isfile, join

import arpy
import requests
from tqdm import tqdm

URL_DEB = (
    "https://github.com/aymara/lima-models/releases/download/v0.1.5/"
    "lima-deep-models-%s-%s_0.1.5_all.deb"
)
URL_C2LC = "https://raw.githubusercontent.com/aymara/lima-models/master/c2lc.txt"
C2LC = {"lang2code": {}, "code2lang": {}}


def get_target_dir(dest=None):
    if not dest:
        if "XDG_DATA_HOME" in os.environ and len(os.environ["XDG_DATA_HOME"]) > 0:
            target_dir_prefix = os.environ["XDG_DATA_HOME"]
        else:
            home_dir = os.path.expanduser("~")
            target_dir_prefix = os.path.join(home_dir, ".local", "share")
        return os.path.join(target_dir_prefix, "lima", "resources")
    else:
        return dest


def install_language(language, dest=None, select=None, force=False):
    target_dir = get_target_dir(dest)

    code, lang = find_lang_code(language.lower())
    deb_url = URL_DEB % (code, lang)

    prefix_list = ["tokenizer", "morphosyntax", "lemmatizer"]
    if select is not None:
        prefix_list = [x.lower().strip() for x in args.select.split(",")]
        if "morphosyntax" in prefix_list:
            prefix_list.append("fasttext")

    if not force:
        new_prefix_list = []
        installed = list_installed_languages(target_dir)
        if "tokenizer" in prefix_list and code not in installed["tok"]:
            new_prefix_list.append("tokenizer")
        if "lemmatizer" in prefix_list and code not in installed["lemm"]:
            new_prefix_list.append("lemmatizer")
        if "morphosyntax" in prefix_list and code not in installed["ms"]:
            new_prefix_list.append("morphosyntax")
            new_prefix_list.append("fasttext")
        prefix_list = new_prefix_list

    if len(prefix_list) > 0:
        print("Language: %s, code: %s" % (lang, code))
        print("Installation dir: %s" % target_dir)
        print("Downloading %s" % deb_url)

        if len(prefix_list) < 3:
            print("Installing only: %s" % (", ".join(prefix_list)))

        with tempfile.TemporaryDirectory() as tmpdirname:
            download_binary_file(deb_url, tmpdirname)
            install_model(
                target_dir,
                os.path.join(tmpdirname, deb_url.split("/")[-1]),
                code,
                prefix_list,
            )
            return True
    else:
        print("All requested models are already installed")
    return False


def info():
    find_lang_code("eng")
    for code in C2LC["code2lang"]:
        print("%-10s\t%s" % (code, C2LC["code2lang"][code]))
    return


def install_model(dir, fn, code, prefix_list):
    with arpy.Archive(fn) as ar_file:
        with ar_file.open(b"data.tar.gz") as tarball:
            with tarfile.open(fileobj=tarball) as tar_file:
                members = tar_file.getmembers()
                for m in members:
                    if m.size > 0:
                        file = tar_file.extractfile(m)
                        if file is not None:
                            full_dir, name = os.path.split(m.name)
                            if len(prefix_list) > 0:
                                name_prefix, _ = name.split("-")
                                if name_prefix not in prefix_list:
                                    continue
                            mo = re.match(
                                (
                                    r"./usr/share/apps/lima/resources/"
                                    r"(TensorFlow[A-Za-z\/\-\.0-9]+)"
                                ),
                                full_dir,
                            )
                            if mo:
                                subdir = mo.group(1)
                                if subdir is None or len(subdir) == 0:
                                    print(
                                        "Error: can't parse '{}'\n".format(full_dir),
                                        file=sys.stderr,
                                    )
                                    sys.exit(1)
                                target_dir = os.path.join(dir, subdir)
                                os.makedirs(target_dir, exist_ok=True)
                                with open(os.path.join(target_dir, name), "wb") as f:
                                    while True:
                                        chunk = file.read(4096)
                                        if len(chunk) == 0:
                                            break
                                        f.write(chunk)
                                # LIMA historically uses 'fre' for French.
                                # This workaround adds symlinks 'fre' -> 'fra' to
                                # support this.
                                if code in ["fra"]:
                                    src_name = os.path.join(target_dir, name)
                                    symlink_name = re.sub(
                                        r"-fra.(conf|model|bin)$", r"-fre.\1", name, 1
                                    )
                                    symlink_name = os.path.join(
                                        target_dir, symlink_name
                                    )
                                    if not os.path.isfile(symlink_name):
                                        os.symlink(src_name, symlink_name)


def download_binary_file(url, dir):
    chunk_size = 4096
    local_filename = os.path.join(dir, url.split("/")[-1])
    totalbytes = 0
    response = requests.get(url, stream=True)
    if response.status_code == 200:
        total_size_in_bytes = int(response.headers.get("content-length", 0))
        progress_bar = tqdm(total=total_size_in_bytes, unit="iB", unit_scale=True)
        with open(local_filename, "wb") as f:
            for chunk in response.iter_content(chunk_size=chunk_size):
                progress_bar.update(len(chunk))
                if chunk:
                    totalbytes += chunk_size
                    f.write(chunk)
    progress_bar.close()


def find_lang_code(lang_str):
    if len(list(C2LC["lang2code"].keys())) == 0:
        with urllib.request.urlopen(URL_C2LC) as f:
            for line in f.read().decode("utf-8").lower().split("\n"):
                line.strip()
                if len(line) > 0:
                    corpus, code = line.split(" ")
                    lang, corp_id = corpus.split("-")
                    C2LC["lang2code"][lang] = code
                    C2LC["code2lang"][code] = lang

    if lang_str in C2LC["lang2code"]:
        return C2LC["lang2code"][lang_str], lang_str
    elif lang_str in C2LC["code2lang"]:
        return lang_str, C2LC["code2lang"][lang_str]
    return None


def list_installed_models(dest=None):
    target_dir = get_target_dir(dest)

    langs = list_installed_languages(target_dir)

    all_installed = []
    for k in langs:
        for lang in langs[k]:
            if lang not in all_installed:
                all_installed.append(lang)
    all_installed.sort()

    max_lang_len = 0
    for code in all_installed:
        lang = "Unknown"
        if find_lang_code(code) is not None:
            lang = find_lang_code(code)[1]
            max_lang_len = max(len(lang), max_lang_len)

    print(
        "Language {}(id ) \t Tokenizer Lemmatizer Morphosyntax".format(
            " " * (max_lang_len - len("Language"))
        )
    )
    print("---")
    for code in all_installed:
        lang = "Unknown"
        if find_lang_code(code) is not None:
            lang = find_lang_code(code)[1]
        lang = lang + " " * (max_lang_len - len(lang) + 1)
        marks = {
            "tok": langs["tok"][code] if code in langs["tok"] else "   ---   ",
            "lemm": (langs["lemm"][code] if code in langs["lemm"] else "   ---   "),
            "ms": langs["ms"][code] if code in langs["ms"] else "   ---   ",
        }
        print(
            "{} ({}) \t {} {}  {}".format(
                lang, code, marks["tok"], marks["lemm"], marks["ms"]
            )
        )


def list_installed_languages(target_dir):
    langs = {
        "tok": list_installed_languages_per_module(
            join(target_dir, "TensorFlowTokenizer", "ud"), ["tokenizer"]
        ),
        "lemm": list_installed_languages_per_module(
            join(target_dir, "TensorFlowLemmatizer", "ud"), ["lemmatizer"]
        ),
        "ms": list_installed_languages_per_module(
            join(target_dir, "TensorFlowMorphoSyntax", "ud"),
            ["morphosyntax", "fasttext"],
        ),
    }
    return langs


def list_installed_languages_per_module(target_dir, prefix_list):
    if not os.path.isdir(target_dir):
        return {}
    files = [f for f in listdir(target_dir) if isfile(join(target_dir, f))]
    d = {}
    for f in files:
        for prefix in prefix_list:
            mo = re.match(r"%s-([^\.]+)\.([a-z]+)$" % prefix, f)
            if mo and len(mo.groups()) == 2:
                lang, ext = mo.group(1), mo.group(2)
                if lang not in d:
                    d[lang] = []
                if ext in d[lang]:
                    print('Error: something wrong with "{}"'.format(f), file=sys.stderr)
                d[lang].append(ext)

    r = {}
    for lang in d:
        if lang in r:
            print(
                'Error: model for lang "{}" is installed twice?'.format(lang),
                file=sys.stderr,
            )
        if "morphosyntax" in prefix_list:
            if (
                len(d[lang]) != 3
                or "model" not in d[lang]
                or "conf" not in d[lang]
                or "bin" not in d[lang]
            ):
                print(
                    'Error: model ({}) for lang "{}" is installed incorrectly'.format(
                        ",".join(prefix_list), lang
                    ),
                    file=sys.stderr,
                )
            else:
                r[lang] = "installed"
        else:
            if len(d[lang]) != 2 or "model" not in d[lang] or "conf" not in d[lang]:
                if "lemmatizer" not in prefix_list or "conf" not in d[lang]:
                    print(
                        'Error: model ({}) for lang "{}" is installed '
                        "incorrectly".format(",".join(prefix_list), lang),
                        file=sys.stderr,
                    )
                if (
                    "lemmatizer" in prefix_list
                    and "conf" in d[lang]
                    and "model" not in d[lang]
                ):
                    r[lang] = "  empty  "
            else:
                r[lang] = "installed"

    return r


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-i",
        "--info",
        help="print list of available languages and exit",
        action="store_true",
    )
    parser.add_argument(
        "-l",
        "--lang",
        type=str,
        help="install model for the given language name or "
        "language code (example: 'english' or 'eng')",
    )
    parser.add_argument("-d", "--dest", type=str, help="destination directory")
    parser.add_argument(
        "-s",
        "--select",
        type=str,
        help="select particular models to install: tokenizer, "
        "morphosyntax, lemmatizer (comma-separated list)",
    )
    parser.add_argument(
        "-f",
        "--force",
        action="store_true",
        help="force reinstallation of existing files",
    )
    parser.add_argument(
        "-L", "--list", action="store_true", help="list installed models"
    )
    args = parser.parse_args()

    dest = get_target_dir(args.dest)
    if args.info:
        info()
        sys.exit(0)
    elif args.list is not None and args.list:
        list_installed_models(args.dest)
        sys.exit(0)
    elif args.lang is None:
        print(
            "ERROR: the following argument is required: -l/--lang",
            end="\n\n",
            file=sys.stderr,
        )
        parser.print_help()
        sys.exit(-1)
    else:
        install_language(args.lang, args.dest, args.select, args.force)
        sys.exit(0)
    sys.exit(0)
