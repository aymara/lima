#!/usr/bin/python3

# dic form => set(Delaf line)
dico_forms = {}

# dic form_POS => set(Delaf line)
dico_entries = {}

with open('analysisDictionary/por/delaf/DelafPB.txt','r') as f:
    for dico_line in f:
        dico_line = dico_line.rstrip()
        if not dico_line: continue
        dico_line_array = dico_line.split('\t')
        pos_traits = dico_line_array.pop()
        dico_line_array.extend(pos_traits.split(':'))
        #print(dico_line_array)
        if not dico_line_array[0] in dico_forms:
            dico_forms[dico_line_array[0]] = set()
        dico_forms[dico_line_array[0]].add(dico_line)
        
        entry = '{}_{}'.format(dico_line_array[0],dico_line_array[3])
        if not entry in dico_entries:
            dico_entries[entry] = set()
        dico_entries[entry].add(dico_line)

# dic form => set(MacMorpho line)
corpus_forms = {}

# dic form_POS => set(MacMorpho line)
corpus_entries = {}

with open('disambiguisationMatrices/por/corpus/macmorpho.conll.txt','r') as f:
    for corpus_line in f:
        corpus_line = corpus_line.rstrip()
        if not corpus_line: continue
        corpus_line_array = corpus_line.split('\t')
        #print(corpus_line_array)
        if not corpus_line_array[0] in corpus_forms:
            corpus_forms[corpus_line_array[0]] = set()
        corpus_forms[corpus_line_array[0]].add(corpus_line)
        
        entry = '{}_{}'.format(corpus_line_array[0],corpus_line_array[1])
        if not entry in corpus_entries:
            corpus_entries[entry] = set()
        corpus_entries[entry].add(corpus_line)

# dico forms with form+pos in corpus
for entry in dico_entries.keys():
    if entry in corpus_entries.keys():
        for line in dico_entries[entry]:
            print(line)

# dico forms not in corpus minus those with same form but with other pos in corpus
for entry in dico_entries.keys():
    if entry not in corpus_entries.keys():
        form, pos = entry.split('_')
        if form not in corpus_forms:
            for line in dico_entries[entry]:
                print(line)

# corpus forms not in dico
for entry in corpus_entries.keys():
    if entry not in dico_entries.keys():
        for line in corpus_entries[entry]:
            form, pos = line.split('\t')
            print('{}\t{}\t\t{}'.format(form,form,pos))

