import argparse
import codecs
import os
import sys

from tqdm import tqdm

output_file, filetrace = None, None
excludes = {}


def load_data(file, categs_types, struct):
    with codecs.open(file, 'r', 'utf-8') as deffile:
        for line_num, line in enumerate(deffile, 1):
            line = line.strip()
            if not line or line.startswith('#'):
                continue
            if not line.count(';') == 3:
                sys.exit(f"Malformed definition line {line_num} "
                         f"in {file}: {line}")

            type_, categs, ficmodel, fictable = line.split(';')
            categ_list = categs.split(',')

            for categ in categ_list:
                if categ not in categs_types:
                    categs_types[categ] = []
                categs_types[categ].append(type_)

            models, modelstab = {}, []
            nbmodel = load_models(ficmodel, models, modelstab)
            print(f"Got {nbmodel} {type_} modelword")

            modelsdata = {}
            nbtable = load_table(type_, fictable, modelsdata)
            print(f"Got {nbtable} {type_} table elems")

            struct[type_] = {"models": models,
                             "modelsdata": modelsdata,
                             "modelstab": modelstab}


def load_models(dicfile, models, modelstab):
    print(f"Loading {dicfile}")
    with codecs.open(dicfile, 'r', 'utf-8') as filedic:
        for line_num, line in enumerate(filedic, 1):
            line = line.strip()
            if not line or line.startswith('#'):
                continue
            if not line.count(';') == 2:
                sys.exit(f"Malformed model line {line_num} "
                         f"in {dicfile}: {line}")

            model, val, _ = line.split(';')
            modelstab.append(model)
            models[model] = val

    return line_num


def load_table(type_, tablefile, modelsdata):
    print(f"Loading {tablefile}")
    oldmodel, maps = "", []
    ok = True
    with codecs.open(tablefile, 'r', 'utf-8') as filetable:
        for line_num, line in enumerate(filetable, 1):
            line = line.strip()
            if not line or line.startswith('#'):
                continue
            parts = line.split(';')
            if len(parts) < 3 or len(parts) > 4:
                print(f"Malformed table line {line_num} "
                      f"in {tablefile}: {line}",
                      file=sys.stderr)
                ok = False
                continue
            if type_ != "verbe" and len(parts) == 4:
                print(f"Malformed non-verb table line {line_num} "
                      f"in {tablefile} (should have 3 columns):\n{line}",
                      file=sys.stderr)
                ok = False
                continue

            modelword, formes, thirds = parts[0], parts[1], parts[2]
            fourths = parts[3] if len(parts) == 4 else ""

            if oldmodel and oldmodel != modelword:
                modelsdata[oldmodel] = maps[:]
                maps.clear()

            oldmodel = modelword
            maps.append({"forme": formes, "third": thirds, "fourth": fourths})

        if oldmodel:
            modelsdata[oldmodel] = maps[:]
    if not ok:
        sys.exit(f"Had problems reading {tablefile}")
    return line_num


def load_excludes(excludesfile, excludes):
    print(f"Loading exclusions file {excludesfile}")
    if os.path.isfile(excludesfile):
        with codecs.open(excludesfile, 'r', 'utf-8') as excludefile:
            for line in excludefile:
                line = line.strip()
                excludes[line] = True
    else:
        print(f"File {excludesfile} not found", file=sys.stderr)


def racine(type_, model, lem, commun):
    if type_ == "verbe":
        llem, lmodel = len(lem) - 1, len(model) - 1
        while llem >= 0 and lmodel >= 0 and lem[:llem] == model[:lmodel]:
            llem -= 1
            lmodel -= 1
        return lem[:llem + 1], lmodel + 1

    lfin = len(commun) - (1 if commun.endswith('$') else 0)
    racine = lem[:len(lem) - lfin]
    longueur = len(model) - lfin
    if '^' in model:
        longueur -= 1

    return racine, longueur


def traitement(struct, line, types, lem, leminv, categ, norm, filetrace):
    if types is None:
        print_output(f"{lem};{categ};;;{lem};{norm};")
        return

    for type_ in types:
        models, modelsdata = (struct[type_]['models'],
                              struct[type_]['modelsdata'])
        subleminv = leminv + '$'
        model, commun = None, None

        while subleminv:
            if subleminv in models:
                model = models[subleminv]
                commun = subleminv
                break
            subleminv = subleminv[:-1]

        if model is None:
            filetrace.write(f"Unable to handle line (no model found) {line}\n")
            return

        if model not in modelsdata:
            filetrace.write(f"Unable to handle line "
                            f"(no data for model {model}) {line}\n")
            return

        racine_val, longueur = racine(type_, model, lem, commun)
        for map_ in modelsdata[model]:
            forme1 = map_['forme']
            third = map_['third'] if type_ == 'verbe' else map_['fourth']
            fourth = map_['fourth'] if type_ == 'verbe' else map_['third']

            forme_sortie = racine_val + forme1[longueur:]
            if '/' in map_['forme']:
                table = map_['forme'].split('/')
                forme_sortie = racine_val[table[0]:table[0]+longueur]
                outline = (f"{forme_sortie};{categ};{third}"
                           f";{fourth};{lem};{norm};")
                outline = outline.replace('?', '')
                print_output(outline)
                forme1 = table[1]

            forme_sortie = racine_val[forme1:forme1+longueur]
            outline = f"{forme_sortie};{categ};{third};{fourth};{lem};{norm};"
            outline = outline.replace('?', '')
            print_output(outline)


def print_output(line):
    global output_file, filetrace, excludes
    if line not in excludes:
        output_file.write(f"{line}\n")
    else:
        filetrace.write(f"Excluding line: {line}\n")


def main():
    parser = argparse.ArgumentParser(description="Process some files.")
    parser.add_argument('deffile', help="The definition file")
    parser.add_argument('infile', help="The input file")
    parser.add_argument('workdir', help="The working directory")
    parser.add_argument('outfile', help="The output file")
    parser.add_argument('--excludesfile', help="The excludes file")

    args = parser.parse_args()

    global output_file, filetrace, excludes
    output_file = codecs.open(
      os.path.join(args.workdir, args.outfile), 'w', 'utf-8')
    filetrace = codecs.open(
      os.path.join(args.workdir, f"{args.infile}.log"), 'w', 'utf-8')

    categs_types, struct = {}, {}

    load_data(args.deffile, categs_types, struct)

    if args.excludesfile:
        load_excludes(args.excludesfile, excludes)
    else:
        print("no excludes file", file=sys.stderr)

    print("Processing simple words file")
    with codecs.open(args.infile, 'r', 'utf-8') as infile:
        for line_num, line in enumerate(
          tqdm(infile, desc="Flexing lemmas"), 1):
            line = line.strip()
            if not line or line.startswith('#'):
                continue
            if len(line.split(';')) < 5:
                filetrace.write(f"Malformed line {line_num}: {line}\n")
                continue

            parts = line.split(';')
            lem, categ, norm = parts[2], parts[3], parts[4]
            leminv = lem[::-1]

            if sum(1 for char in lem if char.isupper()) >= 2:
                print_output(f"{lem};{categ};;;{lem};{norm};")
            else:
                traitement(struct, line, categs_types.get(categ), lem,
                           leminv, categ, norm, filetrace)

    output_file.close()
    filetrace.close()


if __name__ == "__main__":
    main()
