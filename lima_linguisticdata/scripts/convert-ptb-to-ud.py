#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# kate: encoding utf-8;

import sys,argparse
import re
import os

dict = {
  'CC' : 'CCONJ',
  'CD' : 'NUM-NumType=Card',
  'COLON' : 'PUNCT',
  'COMMA' : 'PUNCT',
  'CPAR' : 'PUNCT',
  'CUR' : 'PUNCT',
  'DT' : 'DET',
  'EX' : 'PRON',
  'FW' : 'X-Foreign=Yes',
  'IN' : 'ADP',
  'JJ' : 'ADJ',
  'JJR' : 'ADJ-Degree=Cmp',
  'JJS' : 'ADJ-Degree=Sup',
  'LS' : 'X',
  'MD' : 'AUX-VerbForm=Fin',
  'NN' : 'NOUN-Number=Sing',
  'NNS' : 'NOUN-Number=Plur',
  'NNP' : 'PROPN-Number=Sing',
  'NNPS' : 'PROPN-Number=Plur',
  'OPAR' : 'PUNCT',
  'OQU' : 'PUNCT',
  'PDT' : 'DET',
  'POS' : 'PART',
  'PRP' : 'PRON-PronType=Prs',
  'PRP$' : 'PRON-Poss=Yes|PronType=Prs',
  'QUOT' : 'PUNCT',
  'RB' : 'ADV',
  'RBR' : 'ADV-Degree=Cmp',
  'RBS' : 'ADV-Degree=Sup',
  'RP' : 'ADP',
  'SENT' : 'PUNCT',
  'SYM' : 'SYM',
  'TO' : 'PART',
  'UH' : 'INTJ',
  'VB' : 'VERB-VerbForm=Inf',
  'VBD' : 'VERB-Mood=Ind|Tense=Past|VerbForm=Fin',
  'VBG' : 'VERB-VerbForm=Ger',
  'VBN' : 'VERB-Tense=Past|VerbForm=Part',
  'VBP' : 'VERB-Mood=Ind|Tense=Pres|VerbForm=Fin',
  'VBZ' : 'VERB-Mood=Ind|Number=Sing|Person=3|Tense=Pres|VerbForm=Fin',
  'WDT' : 'PRON-PronType=Rel',
  'WP' : 'PRON-PronType=Rel',
  'WP$' : 'PRON-PronType=Int',
  'WRB' : 'ADV-PronType=Int',
}

#----------------------------------------------------------------------
# main function
def main(argv):
    p = re.compile(r'(.*\t.*\t\t)(.*)')
    # parse command-line arguments
    parser=argparse.ArgumentParser(
        description="Convert freedict (PTB) codes to UD")
    # optional arguments
    parser.add_argument("dictfile",
                        type=argparse.FileType('r',
                                               encoding='utf-8',
                                               errors='ignore'),
                        nargs='+',
                        help="input file: freedict file")
    param=parser.parse_args()
    for dictfile in param.dictfile:
        for line in dictfile:
            line = line.strip()
            m = p.match(line)
            if m and m.group(2) in dict:
                value = dict[m.group(2)]
                print('{}{}'.format(m.group(1),value))

main(sys.argv[1:])
