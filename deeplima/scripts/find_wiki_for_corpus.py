#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# Copyright 2021 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

import argparse
import json
import os
import re
import sys


def get_iso_639_1_code(corpus_path):
    for root, dirs, files in os.walk(corpus_path):
        for file_name in files:
            mo = re.match(r"^([^_]+)_([^-]+)-([a-z]+)-([a-z]+)\.conllu", file_name)
            if mo:
                code = mo.group(1)
                if len(code) > 0:
                    return code
    raise


if __name__ == "__main__":
    lang_list = json.load(open("langlist.json"))

    parser = argparse.ArgumentParser(description="Find Wikipedia URL")
    parser.add_argument(
        "path", metavar="P", type=str, nargs="+", help="Path to UD corpus"
    )
    args = parser.parse_args()
    for path in args.path:
        lang_code = get_iso_639_1_code(path)
        if lang_code not in lang_list:
            sys.stderr.write('Unknown language: "%s"\n' % (lang_code))
        else:
            wdobjects = []
            if len(lang_list[lang_code].keys()) > 1:
                sys.stderr(str(lang_list[lang_code]))
                raise
            for iso_639_3 in lang_list[lang_code]:
                if len(lang_list[lang_code][iso_639_3].keys()) > 1:
                    raise
                for wdobj in lang_list[lang_code][iso_639_3]:
                    item = lang_list[lang_code][iso_639_3][wdobj]
                    wmcode = item["wmcode"]
                    print(
                        f"https://dumps.wikimedia.org/{wmcode}wiki/latest/"
                        "{wmcode}wiki-latest-pages-articles.xml.bz2"
                    )
