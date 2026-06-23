import argparse
import re
from collections import defaultdict

from tqdm import tqdm


def load_conversion_table(tableconvert_file):
    conversion_table = {}
    with open(tableconvert_file, 'r') as file:
        for line in tqdm(file, desc="Loading conversion table"):
            line = line.strip()
            if line:
                data = line.split('\t')
                data[0] = data[0].replace(' ', '')
#                 TODO check that, comes from the perl version but seems wrong
                conversion_table[data[1]] = data[1]
    return conversion_table


def process_corpus(source_file, conversion_table, excludes_pattern):
    tableauprior = defaultdict(lambda: defaultdict(int))
    excludes_re = re.compile(excludes_pattern) if excludes_pattern else None

    with open(source_file, 'r') as source:
        for line in tqdm(source, desc="Processing corpus"):
            line = line.strip()
            if line:
                data = line.split('\t')
                lemma = data[0].lower()
                category = conversion_table.get(data[1], '')

                if (category and (not excludes_re
                                  or not excludes_re.match(category))):
                    tableauprior[lemma][category] += 1

    return tableauprior


def write_output(cible_file, tableauprior):
    with open(cible_file, 'w') as cible:
        for lemma in sorted(tableauprior.keys()):
            for category in sorted(tableauprior[lemma].keys()):
                count = tableauprior[lemma][category]
                cible.write(f"{lemma}\t{category}\t{count}\n")


def main():
    parser = argparse.ArgumentParser(
      description="Process corpus file and conversion table.")
    parser.add_argument("source_file", help="The input corpus file")
    parser.add_argument("cible_file", help="The output file to create")
    parser.add_argument("tableconvert_file", help="The conversion table file")
    parser.add_argument("excludes",
                        help="Comma-separated list of patterns to exclude")

    args = parser.parse_args()

    # print(f"Arguments: {args.source_file}, {args.cible_file},
    # {args.tableconvert_file}, {args.excludes}")

    print(f"Loading conversion table from {args.tableconvert_file}")
    conversion_table = load_conversion_table(args.tableconvert_file)

    excludes = args.excludes.replace(',', '|').replace('*', '.*')
    print(f"Loading and sorting corpus prior data (excluding {args.excludes}) "
          f"from {args.source_file}")
    tableauprior = process_corpus(args.source_file, conversion_table, excludes)

    print(f"Writing output to {args.cible_file}")
    write_output(args.cible_file, tableauprior)

    print("Done")


if __name__ == "__main__":
    main()
