import argparse
import os
import re
import sys

from tqdm import tqdm


def reformat_pos(text, partOfSpeech, Pos):
    return re.sub(r'\[([^\]]*)\]([^ ]*)',
                  lambda m: f"{m.group(2)}{Pos.get(m.group(1), '')}", text)


def reformat_parenth(text, Pos):
    text = re.sub(r'\(\<([0-9]*)\)', r'[^\t_sentence_brk{0-\1}]', text)
    text = re.sub(r'\(([^\)]*)\)',
                  lambda m: f"[{Pos.get(m.group(1), '')}?]", text)
    text = re.sub(r'(\^t_sentence_brk)', r'(\1)', text)
    return text


def read_LingProp(LingPropFile, LingProp, args):
    print(f"read_LingProp {LingPropFile}", file=sys.stderr)
    try:
        with open(LingPropFile, 'r') as file:
            for line in file:
                line = line.strip()
                if ';' in line:
                    key, value = line.split(';')
                    LingProp[key] = value
                elif '\t' in line:
                    parts = line.split()
                    if len(parts) == 2:
                        LingProp[parts[0]] = parts[1]
                else:
                    sys.stderr.write(f"Error line: {line}\n")
    except IOError:
        sys.stderr.write(f"cannot open file {LingPropFile}\n")
        if not args.noCatConvert:
            sys.exit(1)


def main():
    parser = argparse.ArgumentParser(
        description='Reformat idioms from a file')
    parser.add_argument('file', type=argparse.FileType('r'), help='Input file')
    parser.add_argument(
        '-categoriesConvert', default='categories_convert',
        help='Specify the category conversion file (default is '
             'categories_convert)')
    parser.add_argument(
        '-noCatConvert', action='store_true',
        help='No conversion of categories (already in Grace format)')
    parser.add_argument('-help', action='help',
                        help='Display this help message')

    args = parser.parse_args()

    charsep = ':'
    # headSymbol = '&'
    Pos = {
        'pp': '$PRON',
        'D': '$ADV',
        'V': '$V',
        'N': '$NC',
        'S': '$NC',
        'A': '$ADJ',
        'J': '$ADJ',
        'C': '$CONJ',
        'P': '$PREP'
    }
    LingProp = {}

    file_path = os.path.dirname(os.path.abspath(args.file.name))
    LingPropFile = (os.path.join(file_path, args.categoriesConvert)
                    if not args.noCatConvert else None)

    if LingPropFile:
        read_LingProp(LingPropFile, LingProp, args)

    print("set encoding=utf8")
    print("using modex lima-analysis.xml")
    print("using groups LinguisticProcessing")
    print("set defaultAction=>CreateIdiomaticAlternative()\n")

    idiom_lines = args.file.readlines()

    with tqdm(total=len(idiom_lines), desc="Processing idioms") as pbar:
        for line in idiom_lines:
            pbar.update(1)
            # Remove end of line comments and whitespace
            line = line.split('#')[0].strip()
            if not line:
                continue

            fields = line.split(';')
            if len(fields) < 7:
                continue
            (contextual, trigger, automaton,
             partOfSpeech, lemma) = (fields[2], fields[3], fields[4],
                                     fields[5], fields[6])

            quotemetaTrigger = re.escape(trigger)
            match = re.match(f"(.*?)(^| ){quotemetaTrigger}( |$)(.*)",
                             automaton)
            if not match:
                sys.stderr.write(
                    f"Warning : cannot find trigger [{trigger}] "
                    f"in expression [{automaton}]\n")
                continue

            left, right = match.groups()[0], match.groups()[-1]
            left, right = left.rstrip(), right.lstrip()

            trigger = reformat_pos(trigger, partOfSpeech, Pos)
            left = reformat_pos(left, partOfSpeech, Pos)
            left = reformat_parenth(left, Pos)
            right = reformat_pos(right, partOfSpeech, Pos)
            right = reformat_parenth(right, Pos)

            newCat = partOfSpeech
            if not args.noCatConvert:
                if partOfSpeech not in LingProp:
                    sys.stderr.write(
                        f"Error on line {line}: "
                        f"cannot find category \"{partOfSpeech}\"\n")
                    continue
                newCat = LingProp[partOfSpeech]

            partOfSpeech = f"IDIOM${newCat}"
            if contextual == "A":
                partOfSpeech = f"ABS_{partOfSpeech}"

            if partOfSpeech == "preposition":
                right = re.sub(r"(de|des|du|d')$", r"(de|des|du|d')", right)
                right = re.sub(r"(à|au|aux)$", r"(à|au|aux)", right)
            if partOfSpeech == "preposition_verbale":
                right = re.sub(r"(de|d')$", r"(de|d')", right)

            if not lemma:
                lemma = re.sub(r'\[[^\]*]\]', '', automaton)
                lemma = re.sub(r'\&', '', lemma)
                lemma = re.sub(r'\([^\)*]\)', '', lemma)
                lemma = re.sub(r'  +', ' ', lemma).strip()

            print(f"{trigger}{charsep}{left}{charsep}{right}{charsep}"
                  f"{partOfSpeech}{charsep}{lemma}")


if __name__ == "__main__":
    main()
