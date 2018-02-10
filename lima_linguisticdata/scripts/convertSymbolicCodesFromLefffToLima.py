#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# kate: encoding utf-8;

import re

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


def convertSymbolicCodesFromLefffToLima(codes):
    pclose = re.compile(r'</code>')
    p = re.compile(r'<code key="([^"]+)">')
    stored1 = []
    stored2 = []
    store = False
    with open('symbolicCode-fre.xml', 'r') as f:
        with open('symbolicCode-fre-new.xml', 'w',
                  encoding='utf-8', newline='\n') as of:
            for code_line in f:
                code_line = code_line.rstrip()
                if not code_line:
                    continue
                mclose = pclose.match(code_line)
                m = p.match(code_line)
                if mclose:
                    store = False
                    for s1 in stored1:
                        print(s1, file=of)
                    stored1 = []
                    print(code_line, file=of)
                    for s2 in stored2:
                        print(s2, file=of)
                    stored2 = []
                    print(code_line, file=of)
                elif m:
                    store = True
                    lefff_code = m.group(1)
                    i = 0
                    while i <= len(lefff_code):
                        if lefff_code == 'Ff':
                            print('{}, {}, {}'.format(lefff_code, i, lefff_code[:i]))
                        if lefff_code[:i] in codes:
                            code_line = '<code key="{}:{}">'.format(
                                codes[lefff_code[:i]],
                                lefff_code[i:])
                            stored1.append(code_line)
                            code_line = '<code key="{}">'.format(
                                codes[lefff_code[:i]])
                            stored2.append(code_line)
                            break
                        i = i + 1
                    if i > len(lefff_code):
                        stored1.append(code_line)
                        stored2.append(code_line)
                elif store:
                    stored1.append(code_line)
                    stored2.append(code_line)
                else:
                    print(code_line, file=of)


codes = loadLefff2LimaCodesDict()
print(codes)
convertSymbolicCodesFromLefffToLima(codes)

