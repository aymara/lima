#!/usr/bin/python3

import sys
import re
import os

if len(sys.argv) < 4 or len(sys.argv) > 5:
    print('Three or four arguments are expected: language, dictionary file, '
          'corpus file and optionally project source dir')
    exit(1)

language = sys.argv[1]  # 'por'
dicofile = sys.argv[2]
corpus = sys.argv[3]
project_source_dir = ''
if len(sys.argv) > 4:
    project_source_dir = sys.argv[4]


def loadLefff2LimaCodesDict():
    codes = {}
    # associate Lefff code prefix to pos tag
    with open('{}/disambiguisationMatrices/fre/code_symbolic2lima.txt'
              .format(project_source_dir),
              'r') as f:
        for code_line in f:
            code_line = code_line.rstrip()
            if not code_line:
                continue
            symb, lima = code_line.split('\t')
            codes[symb] = lima
    return codes



def loadDicoEntries(codes):
    # dic form_POS => set(dico line)
    dico_entries = {}

    with open(dicofile, 'r') as f:
        for dico_line in f:
            # beaucoup<tab>beaucoup<tab><tab>Rg
            dico_line = dico_line.rstrip()
            if not dico_line:
                continue
            dico_line_array = dico_line.split('\t')
            pos_traits = dico_line_array.pop()
            dico_line_array.extend(pos_traits.split(':'))
            # print(dico_line_array)
            # dico_line_array[3] for por
            tagpos = 3
            if language == 'eng':
                tagpos = 2
            #elif language == 'fre':
                #lefff_code = dico_line_array[3]
                #i = 0
                ## search beginnings of the lefff code in the codes dictionary from
                ## disambiguation matrices code_symbolic2lima.txt
                #while i < len(lefff_code):
                    #if lefff_code[:i] in codes:
                        ## the lefff code prefix is found. replace it by the pos tag
                        ## keep the flexions part after a colon
                        #dico_line_array[3] = '{}:{}'.format(codes[lefff_code[:i]],
                                                            #lefff_code[i:])
                        #break
                    #i = i + 1

            if len(dico_line_array) >= (tagpos+1):
                entry = '{}_{}'.format(dico_line_array[0],
                                        dico_line_array[tagpos])
                if entry not in dico_entries:
                    dico_entries[entry] = set()
                dico_entries[entry].add(dico_line)
            else:
                print('ERROR on line {}'.format(dico_line_array))
    return dico_entries


codes = {}

#if language == 'fre':
    ## Convert Lefff PoS format to Delaf format
    ## Vpifi2-p to V:fi2-p
    
    ## codes contain a dico lefff code -> lima code
    #codes = loadLefff2LimaCodesDict()
    #print(codes)

dico_entries = loadDicoEntries(codes)

# dic form => set(MacMorpho line)
corpus_forms = {}

# dic form_POS => set(MacMorpho line)
corpus_entries = {}

with open(corpus, 'r') as f:
    for corpus_line in f:
        corpus_line = corpus_line.rstrip()
        if not corpus_line:
            continue
        corpus_line_array = corpus_line.split('\t')
        # print(corpus_line_array)
        if not corpus_line_array[0] in corpus_forms:
            corpus_forms[corpus_line_array[0]] = set()
        corpus_forms[corpus_line_array[0]].add(corpus_line)

        entry = '{}_{}'.format(corpus_line_array[0],
                                corpus_line_array[1])
        if entry not in corpus_entries:
            corpus_entries[entry] = set()
        corpus_entries[entry].add(corpus_line)

# dico forms with form+pos in corpus
for entry in dico_entries.keys():
    if entry in corpus_entries.keys():
        for line in dico_entries[entry]:
            print(line)

# dico forms not in corpus minus those with same form but with other pos
# in corpus
for entry in dico_entries.keys():
    if entry not in corpus_entries.keys():
        # print(entry)
        form, pos = entry.rsplit('_', 1)
        if form not in corpus_forms:
            for line in dico_entries[entry]:
                print(line)

# corpus forms not in dico
for entry in corpus_entries.keys():
    if entry not in dico_entries.keys():
        for line in corpus_entries[entry]:
            try:
                form, pos = line.split('\t')
                print('{}\t{}\t\t{}'.format(form, form, pos))
            except ValueError as err:
                print('Error "{}" while handling line "{}"'.format(
                    err, line))
                raise
