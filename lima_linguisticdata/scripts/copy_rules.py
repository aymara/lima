#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# author     Romaric Besançon (romaric.besancon@cea.fr)
# date       Tue Mar 22 2022
# copyright  Copyright (C) 2022 by CEA - LIST
#

import argparse
import glob
import logging
import os
import re
import sys

logger = logging.getLogger(__name__)
# logging.basicConfig(level=logging.DEBUG)
logging.basicConfig(level=logging.WARN)


# ----------------------------------------------------------------------
def fatal_error(msg):
    command = " ".join(sys.argv[:])
    logger.error(f"Error in command: {command}")
    logger.error(msg)
    exit(1)


class Mapping:
    def __init__(self):
        self.mapping = {}

    def read(self, f):
        for line in f:
            pos1, pos2 = line.strip().split()
            if not pos1.startswith("$"):
                pos1 = f"${pos1}"
            if not pos2.startswith("$"):
                pos2 = f"${pos2}"
            self.mapping[pos1] = pos2

    def convert(self, line, linenum, f):
        converted = line
        # brute force
        # for pos1, pos2 in self.mapping.items():
        #     converted = converted.replace(pos1, pos2)
        # use regex (checks that all POS are converted)
        # need to process the matches in reverse (so that the positions are correct)
        matches = list(re.finditer("(\\$[A-Za-z_]*)", line))
        matches.reverse()
        for m in matches:
            pos = m.group(1)
            begin = m.start(1)
            end = m.end(1)
            if begin > 0 and line[begin - 1] == "\\":
                continue
            try:
                # logger.debug(f"convert {pos} to {self.mapping[pos]}")
                converted = f"{converted[:begin]}{self.mapping[pos]}{converted[end:]}"
            except KeyError:
                fatal_error(
                    f"Part-of-speech not found in mapping : {m.group(1)} "
                    f"(file {f}, line {linenum})"
                )

        return converted


class Copier:
    def __init__(self, mapping, target_dir, src_lang=None, tgt_lang=None):
        self.mapping = mapping
        self.target_dir = target_dir
        self.src = src_lang
        self.tgt = tgt_lang

    def copy_rules(self, filein, fileout, src_dir="."):
        logger.debug(f"copy rules {filein} -> {fileout}")
        try:
            with open(filein) as fin, open(fileout, "w") as fout:
                linenum = 0
                for line in fin:
                    linenum += 1
                    if line.startswith("use "):
                        lst_files = line.rstrip()[4:].split(",")
                        tgt_files = []
                        logger.debug(lst_files)
                        for f in lst_files:
                            f = os.path.join(src_dir, f.strip())
                            target = os.path.join(
                                self.target_dir,
                                os.path.basename(f),
                            )
                            # logger.debug(f"lst {f=},{target=}")
                            self.deps_lst[f] = target
                            tgt_files.append(os.path.basename(f))
                        fout.write(f"use {','.join(tgt_files)}\n")

                    elif line.startswith("include "):
                        f = os.path.join(src_dir, line.rstrip()[8:])
                        # add only if not already treated
                        target = os.path.join(
                            self.target_dir,
                            os.path.basename(f),
                        )
                        if f not in self.processed:
                            self.deps_rules[f] = target
                        fout.write(f"include {os.path.basename(f)}\n")
                    else:
                        fout.write(self.mapping.convert(line, linenum, filein))
        except IOError as e:
            logger.error(f"Failed to open file: {str(e)}")
        self.processed.add(filein)

    def copy_convert(self, filein, fileout):
        logger.debug(f"copy gazetteer {filein} -> {fileout}")
        try:
            with open(filein) as fin, open(fileout, "w") as fout:
                linenum = 0
                for line in fin:
                    linenum += 1
                    fout.write(self.mapping.convert(line, linenum, filein))
        except IOError as e:
            logger.error(f"Failed to open file: {str(e)}")

    def copy_from(self, d):
        self.deps_lst = {}
        self.deps_rules = {}
        # use a memory of what was already processed
        # (in case of cross-dependencies)
        self.processed = set()
        for f in glob.glob(os.path.join(d, "*.rules")):
            fileout = os.path.join(
                self.target_dir,
                os.path.basename(f).replace(f"-{self.src}", f"-{self.tgt}"),
            )
            self.copy_rules(f, fileout, src_dir=d)
        # deal with dependencies
        # for rules, copy one at a time (may include more rules)
        while len(self.deps_rules):
            fin = next(iter(self.deps_rules))
            fout = self.deps_rules.pop(fin).replace(f"-{self.src}", f"-{self.tgt}")
            self.copy_rules(fin, fout, src_dir=d)
        # for lst, just copy files (ensuring POS conversion)
        for fin, fout in self.deps_lst.items():
            self.copy_convert(fin, fout)


# ---------------------------------------------------------------------
# main function
def main(argv):
    # parse command-line arguments
    parser = argparse.ArgumentParser(
        description="copy rules and all dependencies, "
        "changing POS tagset according to given mapping"
    )
    # optional arguments
    parser.add_argument(
        "--mapping",
        type=argparse.FileType("r", encoding="UTF-8"),
        help="a file containing the mapping between part-of-speech tags",
    )
    parser.add_argument("--from", dest="src", help="source language")
    parser.add_argument("--to", help="target language")
    parser.add_argument(
        "--target_dir",
        default=".",
        help="target directory",
    )
    # positional arguments
    parser.add_argument("input_dir", help="input directory")

    param = parser.parse_args()

    # do main
    mapping = Mapping()
    if param.mapping:
        mapping.read(param.mapping)
    c = Copier(mapping, param.target_dir, src_lang=param.src, tgt_lang=param.to)
    c.copy_from(param.input_dir)


if __name__ == "__main__":
    main(sys.argv[1:])
