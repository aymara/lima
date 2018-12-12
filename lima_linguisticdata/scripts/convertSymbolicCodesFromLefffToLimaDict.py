#!/usr/bin/python3
# -*- coding: utf-8 -*-
# kate: encoding utf-8;

import sys
import re
import os


def loadLefff2LimaCodesDict():
    codes = {}
    # associate Lefff code prefix to pos tag
    with open('../../../disambiguisationMatrices/fre/code_symbolic2lima.txt',
              'r') as f:
        for code_line in f:
            code_line = code_line.rstrip()
            if not code_line:
                continue
            symb, lima = code_line.split('\t')
            codes[symb] = lima
    return codes


def convertDictFromLefffToLima(codes):
    p = re.compile(r'^.*\t.*\t\t.*$')
    with open('lefff-ext-lima.dic', 'r') as f:
        with open('lefff-ext-lima-new.dic', 'w',
                  encoding='utf-8', newline='\n') as of:
            for line in f:
                line = line.rstrip()
                if not line:
                    continue
                #m = p.match(line)
                #if m:
                print(line, file=sys.stderr)
                form, lemma, none, lefff_code = line.split('\t')
                lima_code = lefff_code
                i = 0
                while i < len(lefff_code):
                    if lefff_code[:i] in codes:
                        lima_code = '{}:{}'.format(codes[lefff_code[:i]],
                                                lefff_code[i:])
                        break
                    i = i + 1
                if i == len(lefff_code):
                    if lefff_code in codes:
                        lima_code = '{}'.format(codes[lefff_code])
                print('\t'.join([form, lemma, none, lima_code]), file=of)


codes = loadLefff2LimaCodesDict()
print(codes)
convertDictFromLefffToLima(codes)

