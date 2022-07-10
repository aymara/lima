#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

# -*- coding: UTF-8 -*-

import argparse
import bz2
import json
import os.path
import re
import sys

RE_CHARS_TO_ESCAPE = re.compile(r"([:\.&@\,\(\)\[\]])")
RE_SPACES = re.compile(r"\s+")
RE_LATIN = re.compile(r"[A-Za-z]")
RE_FINAL_PARANTHESES = re.compile(r"^\s*(.+?)\s+(\([^\)]+)\)\s*$")
RE_AFTER_COMMA = re.compile(r"^\s*(.+?)\s*,\s+([^,]+)$")


def has_latin(text):
    return RE_LATIN.search(text) is not None


def is_instance(js, classes):
    if "claims" not in js:
        return False
    if "P31" not in js["claims"]:
        return False
    for claim in js["claims"]["P31"]:
        if "mainsnak" not in claim:
            continue
        mainsnak = claim["mainsnak"]
        if "datavalue" not in mainsnak:
            continue
        if "value" not in mainsnak["datavalue"]:
            continue
        if "id" not in mainsnak["datavalue"]["value"]:
            continue
        val_id = mainsnak["datavalue"]["value"]["id"]
        if val_id in classes:
            return True
    return False


def escape_for_rules(text):
    return RE_CHARS_TO_ESCAPE.sub(r"\\\1", text)


def make_rule(text, obj_type):
    parts = re.split(RE_SPACES, escape_for_rules(text))  # .lower()))
    res = "%s::%s:%s:" % (parts[0], " ".join(parts[1:]), obj_type)
    return res


def preprocess_name(text):
    res = [text]

    wo_parantheses = RE_FINAL_PARANTHESES.sub(r"\1", text)
    if wo_parantheses not in res:
        res.append(wo_parantheses)

    text_after_comma = RE_AFTER_COMMA.sub(r"\1", text)
    if text_after_comma not in res:
        res.append(wo_parantheses)

    return res


def save_label(aliases, labels, files, obj_type):
    if aliases is not None:
        for a in aliases:
            if "value" in a:
                val = a["value"]
                if not has_latin(val):
                    continue
                for v in preprocess_name(val):
                    if " " not in v:
                        files["words"].write(v + "\n")
                    else:
                        files["rules"].write(make_rule(v, obj_type) + "\n")


def parse_line(args, line, classes, files):
    if len(line) < 4:
        return True
    line = line.rstrip().rstrip(",")

    try:
        js = json.loads(line)
    except Exception as e:
        print(e, file=sys.stderr)
        return False
    sys.stderr.write(js["id"] + "\r")
    aliases = None
    labels = None
    if "aliases" in js and args.lang in js["aliases"]:
        aliases = js["aliases"][args.lang]
    if "labels" in js and args.lang in js["labels"]:
        labels = [js["labels"][args.lang]]
    if aliases is not None or labels is not None:
        for p in classes:
            if is_instance(js, classes[p]):
                save_label(aliases, labels, files[p], args.type)
    return True


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-i", "--input", help="Input file name")
    parser.add_argument(
        "-c", "--classes", nargs="+", help="Comma-separated list of classes"
    )
    parser.add_argument(
        "-f", "--file", nargs="+", help="File with new line separated list of classes"
    )
    parser.add_argument("-l", "--lang", required=True, help="WikiData language")
    parser.add_argument("-t", "--type", help="Object type for rules")
    args = parser.parse_args()

    classes = {}
    all_wd_classes = {}

    if args.classes is not None:
        for c in args.classes:
            cls_str, out_prefix = c.split(":")
            if out_prefix in classes:
                sys.stderr.write(
                    'ERROR: prefix "%s" mentioned twice in command line\n'
                    % (out_prefix)
                )
                sys.exit(-1)
            classes[out_prefix] = {x: 1 for x in cls_str.split(",")}

    if args.file is not None:
        for f in args.file:
            classes_fn, out_prefix = f.split(":")
            if out_prefix in classes:
                sys.stderr.write(
                    'ERROR: prefix "%s" mentioned twice in command line\n'
                    % (out_prefix)
                )
                sys.exit(-1)
            classes[out_prefix] = {
                x: 1 for x in re.split(r"\s+", open(classes_fn, mode="r").read())
            }

    for p in classes:
        for x in classes[p]:
            if x in all_wd_classes:
                sys.stderr.write(
                    'WARNING: class "%s" mentioned at least twice in input\n' % (x)
                )
                # sys.exit(-1)
            else:
                all_wd_classes[x] = 0
            all_wd_classes[x] += 1

    files = {}
    for p in classes:
        fn = p + "." + args.lang + ".words"
        if os.path.isfile(fn):
            sys.stderr.write('ERROR: file "%s" already exists\n' % (fn))
            sys.exit(-1)
        if p not in files:
            files[p] = {}
        files[p]["words"] = open(fn, mode="w")

        fn = p + "." + args.lang + ".rules"
        if os.path.isfile(fn):
            sys.stderr.write('ERROR: file "%s" already exists\n' % (fn))
            sys.exit(-1)
        if p not in files:
            files[p] = {}
        files[p]["rules"] = open(fn, mode="w")

    if args.input is None or args.input.lower() in ["-", "stdin"]:
        for line in sys.stdin:
            parse_line(args, line, classes, files)
    elif args.input.endswith(".bz2"):
        with open(args.input, "rb") as f:
            decomp = bz2.BZ2Decompressor()
            remains_to_parse = ""
            for chunk in iter(lambda: f.read(1024 * 64), b""):
                dc = decomp.decompress(chunk)
                if len(dc) == 0:
                    continue
                lines = (remains_to_parse + dc.decode("utf-8")).splitlines(
                    keepends=False
                )
                for line in lines:
                    if not parse_line(args, line, classes, files):
                        remains_to_parse = line
    else:
        with open(args.input, "rb") as f:
            for line in f:
                if not parse_line(args, line, classes, files):
                    sys.stderr.write("ERROR: can't parse line '%s'\n" % (line))
                    sys.exit(1)


if __name__ == "__main__":
    main()
