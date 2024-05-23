# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

import argparse


def replace_semicolons_with_tabs(fin, fout):
    with open(fin, 'r', encoding='utf-8') as infile:
        content = infile.read()

    # Replace semicolons with tabs
    modified_content = content.replace(';', '\t')

    with open(fout, 'w', encoding='utf-8') as outfile:
        outfile.write(modified_content)


def main():
    parser = argparse.ArgumentParser(
        description="Replace all semicolons with tabulations in a file.")
    parser.add_argument('fin', help="The input file")
    parser.add_argument('fout', help="The output file")

    args = parser.parse_args()

    replace_semicolons_with_tabs(args.fin, args.fout)


if __name__ == "__main__":
    main()
