#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys,argparse
import re
import os

#----------------------------------------------------------------------
def loadLefff2LimaCodesDict(project_source_dir):
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



def loadDicoEntries(dicofile,language,codes):
    # dic form_POS => set(dico line)
    dico_entries = {}

    for dico_line in dicofile.readlines():
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
        #if language == 'eng':
        #    tagpos = 2
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
            sys.stderr.write('ERROR on line {}\n'.format(dico_line_array))
    return dico_entries


def mergeEntries(language,dicofile,corpus,project_source_dir):
    codes = {}
    
    #if language == 'fre':
        ## Convert Lefff PoS format to Delaf format
        ## Vpifi2-p to V:fi2-p
        
        ## codes contain a dico lefff code -> lima code
        #codes = loadLefff2LimaCodesDict(project_source_dir)
        #print(codes)
    
    dico_entries = loadDicoEntries(dicofile,language,codes)
    
    # dic form => set(MacMorpho line)
    corpus_forms = {}
    
    # dic form_POS => set(MacMorpho line)
    corpus_entries = {}
    
    for corpus_line in corpus:
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
#----------------------------------------------------------------------
# main function
def main(argv):
    # parse command-line arguments
    parser=argparse.ArgumentParser(description="Merge the dictionary entries with entries created from an POS-annotated corpus")
    # optional arguments
    #parser.add_argument("--arg",type=int,default=42,help="description")
    # positional arguments
    parser.add_argument("language",help="language")
    parser.add_argument("dicofile",type=argparse.FileType('r',encoding='UTF-8'),help="input file")
    parser.add_argument("corpus",type=argparse.FileType('r',encoding='UTF-8'),help="corpus file")
    parser.add_argument("project_source_dir",default="",nargs="?",help="project source dir")
    
    param=parser.parse_args()
    
    # do main
    mergeEntries(param.language,param.dicofile,param.corpus,param.project_source_dir)
    
main(sys.argv[1:])
