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

class DicEntries:
    def __init__(self):
        # dic form => POS => set(dico line)
        self.entries={}

    def add(self,form,pos,entry):
        if form not in self.entries:
            self.entries[form]={}
        if pos not in self.entries[form]:
            self.entries[form][pos]=set()
        self.entries[form][pos].add(entry)
        
    def get(self,form,pos):
        return self.entries[form][pos]
        
    def exists(self,form,pos):
        if form not in self.entries:
            return False
        if pos not in self.entries[form]:
            return False
        return True
        
    def formExists(self,form):
        if form not in self.entries:
            return False
        return True

def loadDicoEntries(dicofile,language,codes):
    # dic form => POS => set(dico line)
    dico_entries = DicEntries()

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
            dico_entries.add(dico_line_array[0],dico_line_array[tagpos],dico_line)
        else:
            sys.stderr.write('ERROR on line {}\n'.format(dico_line_array))
    return dico_entries

def loadCorpusEntries(corpus):
    # dic form => POS => set(MacMorpho line)
    corpus_entries = DicEntries()
    
    for corpus_line in corpus:
        corpus_line = corpus_line.rstrip()
        if not corpus_line:
            continue
        corpus_line_array = corpus_line.split('\t')
        # print(corpus_line_array)
        (form,pos)=(corpus_line_array[0],corpus_line_array[1])
        corpus_entries.add(form,pos,"%s\t%s\t\t%s"%(form,form,pos))
    return corpus_entries

def mergeEntries(language,dicofile,corpus,project_source_dir):
    codes = {}
    
    #if language == 'fre':
        ## Convert Lefff PoS format to Delaf format
        ## Vpifi2-p to V:fi2-p
        
        ## codes contain a dico lefff code -> lima code
        #codes = loadLefff2LimaCodesDict(project_source_dir)
        #print(codes)
    
    dico_entries = loadDicoEntries(dicofile,language,codes)
    corpus_entries = loadCorpusEntries(corpus)
    
    for form,posentries in dico_entries.entries.items():
        for (pos,entries) in posentries.items():
            # dico forms with form+pos in corpus
            if corpus_entries.exists(form,pos):
                print("\n".join(list(entries)))
            else:
                # dico forms not in corpus minus those with same form but with other pos
                # in corpus
                if not corpus_entries.formExists(form):
                    print("\n".join(list(entries)))
    
    # corpus forms not in dico
    for form,posentries in corpus_entries.entries.items():
        for (pos,entries) in posentries.items():
            if not dico_entries.exists(form,pos):
                print("\n".join(list(entries)))
                
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
