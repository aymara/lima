#!/usr/bin/env python3

# Copyright 2002-2024 CEA LIST
# SPDX-FileCopyrightText: 2024 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

###############################################
# Categories conversion program :
# converts dicostd into dicojys (data multiplicative coding)
###############################################

import argparse
import sys

from tqdm import tqdm


def main():
    parser = argparse.ArgumentParser(description="Convert dicostd into dicojys.")
    parser.add_argument("source", type=str, help="Source file")
    parser.add_argument("convert", type=str, help="Convert file")
    parser.add_argument("cible", type=str, help="Cible file")

    args = parser.parse_args()

    print("INFO : Start default categories conversion", file=sys.stderr)

    try:
        source = open(args.source, "r", encoding="utf-8")
    except IOError:
        print(f"Cannot open {args.source}", file=sys.stderr)
        sys.exit(1)

    try:
        convert = open(args.convert, "r", encoding="utf-8")
    except IOError:
        print(f"Cannot open {args.convert}", file=sys.stderr)
        sys.exit(1)

    try:
        cible = open(args.cible, "w", encoding="utf-8")
    except IOError:
        print(f"Cannot open {args.cible}", file=sys.stderr)
        sys.exit(1)

    try:
        error = open("error.txt", "a", encoding="utf-8")
    except IOError:
        print("Cannot open error.txt", file=sys.stderr)
        sys.exit(1)

    tags = {}
    for line in convert:
        donneestags = line.strip().split(";")
        if len(donneestags) > 1:
            tags[donneestags[0]] = donneestags[1]

    convert.close()

    line_num = 0

    source_lines = source.readlines()
    for line in tqdm(source_lines, desc="Processing lines", unit="line"):
        line_num += 1
        line = line.strip()
        if len(line) == 0 or line.startswith("#"):
            continue

        donnees = line.split("\t")
        if len(donnees) != 2:
            error_message = (
                f"in file {args.source} line {line_num}: "
                f"wrong number of columns. Ignore line: {line}\n"
            )
            print(error_message, file=sys.stderr)
            error.write(error_message)
            continue

        type_ = donnees[0]
        info = donnees[1]

        if info in tags and tags[info]:
            cible.write(f"{type_}\t{tags[info]}\n")
        else:
            error_message = (
                f"in file {args.source} line {line_num}: "
                f"Invalid properties {type_} {info}\n"
            )
            print(error_message, file=sys.stderr)
            error.write(error_message)

    source.close()
    cible.close()
    error.close()


if __name__ == "__main__":
    main()
