#!/usr/bin/env python3
#
# kate: encoding utf

dict = {
    "CC": "CCONJ",
    "CD": "NUM",
    "COLON": "PUNCT",
    "COMMA": "PUNCT",
    "CPAR": "PUNCT",
    "CUR": "PUNCT",
    "DT": "DET",
    "EX": "PRON",
    "FW": "X",
    "IN": "ADP",
    "JJ": "ADJ",
    "JJR": "ADJ",
    "JJS": "ADJ",
    "LS": "X",
    "MD": "AUX",
    "NN": "NOUN",
    "NNS": "NOUN",
    "NNP": "PROPN",
    "NNPS": "PROPN",
    "OPAR": "PUNCT",
    "OQU": "PUNCT",
    "PDT": "DET",
    "POS": "PART",
    "PRP": "PRON",
    "PRP$": "PRON",
    "QUOT": "PUNCT",
    "RB": "ADV",
    "RBR": "ADV",
    "RBS": "ADV",
    "RP": "ADP",
    "SENT": "PUNCT",
    "SYM": "SYM",
    "TO": "PART",
    "UH": "INTJ",
    "VB": "VERB",
    "VBD": "VERB",
    "VBG": "VERB",
    "VBN": "VERB",
    "VBP": "VERB",
    "VBZ": "VERB",
    "WDT": "PRON",
    "WP": "PRON",
    "WP$": "PRON",
    "WRB": "ADV",
}

with open("freeling.dic", "r") as freeling:
    for line in freeling:
        line = line.strip()
        # print(line)
        try:
            token, lemma, null, ptbtag = line.split("\t")
            print("{}, {}, {}".format(token, lemma, ptbtag))
            freeling
        except ValueError:
            pass
