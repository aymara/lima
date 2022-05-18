#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# kate: encoding utf-8;

# Copyright 2002-2020 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

import argparse
import re
import sys


def main(argv):
    tokens_re = re.compile(r"^[0-9]+")
    # parse command-line arguments
    parser = argparse.ArgumentParser(
        description="Convert UD corpus to success cate retag used to learn pos "
        "tagging models"
    )
    # optional arguments
    parser.add_argument(
        "--features",
        type=str,
        nargs="?",
        default="Mood,PronType,Tense,VerbForm",
        help="comma separated list of feature names",
    )
    parser.add_argument(
        "--output", type=str, default="-", help="the name of output file. - for stdout"
    )
    parser.add_argument(
        "corpus",
        type=argparse.FileType("r", encoding="utf-8"),
        nargs="+",
        help="input file: UD corpus file",
    )
    param = parser.parse_args()
    print("param.features: {}".format(param.features), file=sys.stderr)
    output_file = None
    if param.output is not None and param.output != "-":
        output_file = open(param.output, "w", encoding="utf-8")
    for corpus in param.corpus:
        # tokens accumulates the adjacent tokens that must be grouped in the
        # output. This is only named entities parts (PROPN linked by a compound
        # relation) currently
        tokens = ""
        for line in corpus:
            # print('line: {}'.format(line), file=sys.stderr)
            line = line.strip()
            m = tokens_re.match(line)
            if m:
                # 22	i	i	PRON	PRP	Case=Nom|Number=Sing|Person=1|PronType=Prs
                # 24	nsubj	24:nsubj	SpaceAfter=No
                (
                    tokid,
                    token,
                    _,
                    udtag,
                    _,
                    token_features,
                    targetid,
                    reltype,
                    _,
                    _,
                ) = line.split("\t")
                if (
                    reltype == "compound"
                    and udtag == "PROPN"
                    and int(targetid) > int(tokid)
                ):
                    # Named entities
                    # Current token is a named entity part, not final
                    tokens = tokens + token
                    if "SpaceAfter=No" not in token_features:
                        tokens = tokens + "_"
                elif (
                    reltype == "obl:npmod"
                    and "SpaceAfter=No" in token_features
                    and int(targetid) > int(tokid)
                ):
                    # Handled as idioms by LIMA
                    # 15	$	$	SYM	$	_	17	obl:npmod	17:obl:npmod
                    # SpaceAfter=No
                    tokens = tokens + token
                elif len(tokens) > 0:
                    # Current token is the last part of a named entity or an idiom
                    token = tokens + token
                    tokens = ""

                if len(tokens) == 0:
                    features = token_features.split("|")
                    features_kept = []
                    for feature in features:
                        # print(feature, file=sys.stderr)
                        if feature != "_":
                            feature_name, feature_value = feature.split("=")
                            if (
                                param.features == "all"
                                or feature_name in param.features
                            ):
                                features_kept.append(feature)
                    features_kept_string = ""
                    if features_kept:
                        features_kept_string = "-{}".format("|".join(features_kept))
                    output_line = "{}\t{}{}".format(token, udtag, features_kept_string)
                    if output_file is None:
                        print(output_line)
                    else:
                        output_file.write(output_line + "\n")


main(sys.argv[1:])
