import argparse
import sys

from tqdm import tqdm


# Function to load tags from the convert data file
def load_tags(filename):
    tags = {}
    with open(filename, 'r') as file:
        for line in file:
            line = line.strip()
            if not line or line.startswith('#'):
                continue
            parts = line.split(';')
            if len(parts) != 2:
                print(f"Error: {line} in {filename}")
                continue
            strcateg, gracecateg = parts
            strcateg = ' '.join(strcateg.split())
            print(f"{strcateg} -> {gracecateg}", file=sys.stderr)
            tags[strcateg] = gracecateg
    return tags


def main():
    parser = argparse.ArgumentParser(
        description="Convert Christian Fluhr's dictionaries to Hub format")
    parser.add_argument("source", help="Source file")
    parser.add_argument("convert_data_file", help="Convert data file")
    parser.add_argument("output", help="Output file")
    args = parser.parse_args()

    print("INFO: Debut du traitement convertstd")

    try:
        source = open(args.source, 'r')
        print(f"INFO: Fichier {args.source} ouvert")
    except IOError:
        print(f"Impossible d'ouvrir le fichier {args.source}")
        return

    try:
        convert_data_file = open(args.convert_data_file, 'r')
        print(f"INFO: Fichier {args.convert_data_file} ouvert")
    except IOError:
        print(f"Impossible d'ouvrir le fichier {args.convert_data_file}")
        return

    try:
        output = open(args.output, 'w')
        print(f"INFO: Fichier {args.output} ouvert")
    except IOError:
        print(f"Impossible d'ouvrir le fichier {args.output}")
        return

    tags = load_tags(args.convert_data_file)

    print("----------------------------------")
    print("Lignes non traitees par convertstd\n")

    motstraites = 0
    motsnontraites = 0

    lines = source.readlines()
    for ligne in tqdm(lines, desc="Processing lines"):
        motstraites += 1

        donnees = ligne.strip().split('\t')

        if len(donnees) < 6:
            print(f"Skipping malformed line: {ligne.strip()} "
                  f"from {args.source}")
            continue

        entree, info1, info2, info3, lemme, normalisation = donnees[:6]
        info = " ".join([info1, info2, info3]).strip()

        codestags = tags.get(info, "")
        if codestags:
            codetag = codestags.split('+')
            for tag in codetag:
                output.write(f"{entree}\t{lemme}\t{normalisation}\t{tag}\n")
        else:
            motsnontraites += 1
            print(f"{entree}\t{lemme}\t{normalisation}\t{info} not found")

    source.close()
    convert_data_file.close()
    output.close()

    print(f"INFO: Entries not taken into account : {motsnontraites}",
          file=sys.stderr)
    print(f"INFO: convertstd done for {args.source}", file=sys.stderr)


if __name__ == "__main__":
    main()
