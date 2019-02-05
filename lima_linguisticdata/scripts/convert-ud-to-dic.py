#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# kate: encoding utf-8;

#   Copyright 2002-2013 CEA LIST
#
#   This file is part of LIMA.
#
#   LIMA is free software: you can redistribute it and/or modify
#   it under the terms of the GNU Affero General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   LIMA is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU Affero General Public License for more details.
#
#   You should have received a copy of the GNU Affero General Public License
#   along with LIMA.  If not, see <http://www.gnu.org/licenses/>

import sys
import argparse
import re
import os

 #gawk -F "\\t" "BEGIN {OFS = FS} {$2 = substr($2,0,100); print $2\"\\t\"$4\"-\"$6}" | sed -e "s@-_$@@" > corpus_eng.ud_merge.txt

def main(argv):
    tokens_re = re.compile(r'^[0-9]+')
    # parse command-line arguments
    parser=argparse.ArgumentParser(
        description='Convert UD corpus to success cate retag used to learn pos tagging models')
    # optional arguments
    parser.add_argument('corpus',
                        type=argparse.FileType('r',
                                               encoding='utf-8',
                                               errors='ignore'),
                        nargs='+',
                        help='input file: UD corpus file')
    param=parser.parse_args()
    #print('param.features: {}'.format(param.features), file=sys.stderr)
    #features_to_keep = param.features.split(',') if param.features is not None else []
    for corpus in param.corpus:
        # tokens accumulates the adjacent tokens that must be grouped in the 
        # output. This is only named entities parts (PROPN linked by a compound
        # relation) currently
        tokens = ''
        for line in corpus:
            #print('line: {}'.format(line), file=sys.stderr)
            line = line.strip()
            m = tokens_re.match(line)
            if m:
                # 22	i	i	PRON	PRP	Case=Nom|Number=Sing|Person=1|PronType=Prs	24	nsubj	24:nsubj	SpaceAfter=No
                _,token,lemma,udtag,_,token_features,_,_,_,_ = line.split('\t')
                #print('\ttoken: {}, udtag: {}, token_features: {}'.format(token, udtag, token_features), file=sys.stderr)
                features = token_features.split('|')
                features_kept = []
                for feature in features:
                    #print(feature, file=sys.stderr)
                    if feature is not '_':
                        features_kept.append(feature)
                features_kept_string = ''
                if features_kept:
                    features_kept_string = '-{}'.format('|'.join(features_kept))
                print('{}\t{}\t\t{}{}'.format(token, lemma, udtag, features_kept_string))

main(sys.argv[1:])
