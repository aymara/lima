#!/usr/bin/env python3

# Copyright 2002-2024 CEA LIST
# SPDX-FileCopyrightText: 2024 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

import argparse
import codecs

from tqdm import tqdm


def print_usage():
    print("USAGE : xmlforms [OPTIONS] inputfile outputfile")
    print("where [OPTIONS] are :")
    print("  -h or -help : print usage")
    print(
        "  -desacc=[yes|no] : specify desacc attribute for entries. "
        "default is none, that equals 'yes'"
    )
    print(
        "  -entryop=[add|replace|delete] : specify op attribute for entries. "
        "default is none, that equals 'add'"
    )
    print(
        "  -lingop=[add|replace|delete] : specify op attribute for linginfos. "
        "default is none, that equals 'add'"
    )


def main():
    parser = argparse.ArgumentParser(
        description="Process input and output files with options."
    )
    parser.add_argument("inputfile", type=str, help="Input file")
    parser.add_argument("outputfile", type=str, help="Output file")
    parser.add_argument(
        "-desacc",
        type=str,
        choices=["yes", "no"],
        help="Specify desacc attribute for entries. "
        'Default is none, that equals "yes"',
    )
    parser.add_argument(
        "-entryop",
        type=str,
        choices=["add", "replace", "delete"],
        help="Specify op attribute for entries. " 'Default is none, that equals "add"',
    )
    parser.add_argument(
        "-lingop",
        type=str,
        choices=["add", "replace", "delete"],
        help="Specify op attribute for linginfos. "
        'Default is none, that equals "add"',
    )

    args = parser.parse_args()

    with codecs.open(args.inputfile, "r", "utf-8") as source:
        with codecs.open(args.outputfile, "a", "utf-8") as out:
            form = ""
            lemma = ""
            norm = ""

            count = 0
            icount = 0

            lines = source.readlines()
            for line in tqdm(lines, desc="Processing lines", unit="line"):
                line = line.strip()
                line = (
                    line.replace("&", "&amp;")
                    .replace('"', "&quot;")
                    .replace("<", "&lt;")
                    .replace(">", "&gt;")
                )
                if line == "":
                    continue

                line = line.split("#")[0].strip()
                data = line.split("\t")
                if line == "" or len(data) != 4:
                    print(f"xmlform: Invalid line '{line}'")
                    continue

                if data[0] != form:
                    form = data[0]
                    if count > 0:
                        out.write("  </i>\n</entry>\n")
                    out.write(f'<entry k="{form}"')
                    if args.desacc:
                        out.write(f' desacc="{args.desacc}"')
                    if args.entryop:
                        out.write(f' op="{args.entryop}"')
                    out.write(">\n")
                    icount = 0
                    count += 1

                if icount == 0 or data[1] != lemma or data[2] != norm:
                    lemma = data[1]
                    norm = data[2]
                    if icount > 0:
                        out.write("  </i>\n")
                    out.write("  <i")
                    if lemma:
                        out.write(f' l="{lemma}"')
                    if norm:
                        out.write(f' n="{norm}"')
                    if args.lingop:
                        out.write(f' op="{args.lingop}"')
                    out.write(">\n")
                    icount += 1

                out.write(f'    <p v="{data[3]}"/>\n')

        if count > 0:
            out.write("  </i>\n</entry>\n")


if __name__ == "__main__":
    main()
