#!/usr/bin/env python3

# Copyright (C) 2020 by CEA - LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

import argparse
import codecs
import os
import re
import subprocess
import sys


# ----------------------------------------------------------------------
def filter_words(f, lang="eng"):
    # create a tmp file with simple words, all lowercase
    # keep link to original word in original dict
    original = {}
    header, footer = "", ""
    key_file = "tmp.%s" % (os.getpid())
    with codecs.open(key_file, "w", "utf-8") as fout:
        for line in f:
            if line.startswith("@"):
                header = line.rstrip()
                continue
            if line.startswith(")"):
                footer = line
                continue
            line = line.rstrip()
            line = line.rstrip(",")
            lc = line.lower()
            original[lc] = line
            fout.write("%s\n" % lc)

    # launch analysis of words
    command = [
        "composedDict",
        "--language=%s" % lang,
        "--dicoId=mainDictionary",
        "--keyFile=%s" % key_file,
    ]
    output = subprocess.run(command, stdout=subprocess.PIPE)
    if output.returncode != 0:
        sys.stderr.write("Error: command returned %s" % output.returncode)
        return

    # parse output
    kept_words = []
    word, pos = "", []
    kept = False
    for line in output.stdout.decode("utf-8").split("\n"):
        m = re.match("===== Entry for '(.*)' =====", line)
        if m:
            # previous word
            if not kept:
                if len(pos) == 1 and pos[0] == "NP":
                    # catch-up: can keep it if in dictionary as a proper noun
                    sys.stderr.write(
                        "--kept %s: exists in dictionary but with POS %s\n"
                        % (word, pos)
                    )
                    kept_words.append(original[word])
                else:
                    sys.stderr.write(
                        "--ignored %s: exists in dictionary with POS %s\n" % (word, pos)
                    )
            word = m.group(1)
            pos = []
            kept = False
            continue
        if re.match("DictionaryEntry : empty", line):
            kept_words.append(original[word])
            kept = True
            continue
        m = re.search("foundProperties : .*MACRO=([^,]*),", line)
        if m:
            pos.append(m.group(1))

    if key_file == f.name:
        # no temp file: simple list of words
        print("\n".join(kept_words))
    else:
        print(header)
        print(",\n".join(kept_words))
        print(footer)
        # remove temp file
        os.unlink(key_file)


# ----------------------------------------------------------------------
# main function
def main(argv):
    # parse command-line arguments
    parser = argparse.ArgumentParser(
        description="Filter a list of names that are ambiguous with words in the "
        "dictionary. The input file is either a .lst for named entity rules or a "
        "simple list of single words (one word on each line). The output has the same "
        "format."
    )
    # optional arguments
    parser.add_argument("--language", default="eng", help="language (default is eng)")
    # positional arguments
    parser.add_argument(
        "inputFile", type=argparse.FileType("r", encoding="UTF-8"), help="input file"
    )

    param = parser.parse_args()

    # do main
    filter_words(param.inputFile, lang=param.language)


main(sys.argv[1:])
