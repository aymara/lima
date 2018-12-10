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


def convertDefaultsFromLefffToLima(codes):
    with open('default-fre.txt', 'r') as f:
        with open('default-fre-new.txt', 'w',
                  encoding='utf-8', newline='\n') as of:
            for line in f:
                line = line.rstrip()
                if not line:
                    continue
                tstatus, lefff_code = line.split('\t')
                lima_code = lefff_code
                i = 0
                while i < len(lefff_code):
                    if lefff_code == 'Ff':
                        print('{}, {}, {}'.format(lefff_code, i, lefff_code[:i]))
                    if lefff_code[:i] in codes:
                        lima_code = '{}:{}'.format(codes[lefff_code[:i]],
                                                   lefff_code[i:])
                        break
                    i = i + 1
                if i == len(lefff_code):
                    if lefff_code in codes:
                        lima_code = '{}'.format(codes[lefff_code])
                print('\t'.join([tstatus, lima_code]), file=of)


codes = loadLefff2LimaCodesDict()
print(codes)
convertDefaultsFromLefffToLima(codes)

