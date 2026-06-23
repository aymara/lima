import argparse

from tqdm import tqdm


def count_columns_and_sum_frequencies(source_file):
    with open(source_file, 'r') as source:
        infos = source.readline().strip().split('\t')
        count = len(infos)
        sum = int(infos[-1])
        for line in tqdm(source, desc="Calculating sum of frequencies"):
            if line.strip():
                infos = line.strip().split('\t')
                sum += int(infos[-1])

    return count, sum


def normalize_frequencies(source_file, cible_file, count, somme=1):
    with open(source_file, 'r') as source, open(cible_file, 'w') as cible:
        for line in tqdm(source, desc="Normalizing frequencies"):
            if line.strip():
                infos = line.strip().split('\t')
                freq = int(infos[-1])
                # freq = freq / somme  # Uncomment if normalization is needed

                if count == 4:
                    cible.write(
                        f"{infos[0]}\t{infos[1]}\t{infos[2]}\t{freq}\n")
                elif count == 3:
                    cible.write(
                        f"{infos[0]}\t{infos[1]}\t{freq}\n")
                else:
                    raise ValueError(
                        f"Erreur --- nombre de colonnes non valable : "
                        f"{count}")


def main():
    parser = argparse.ArgumentParser(
        description="Normalize frequencies in a file.")
    parser.add_argument("source_file", help="The source file to read from")
    parser.add_argument("cible_file", help="The target file to write to")
    args = parser.parse_args()

    print("Reading source file and counting columns and frequencies")
    count, somme = count_columns_and_sum_frequencies(args.source_file)

    print("Normalizing frequencies and writing to target file")
    normalize_frequencies(args.source_file, args.cible_file, count,
                          somme)

    print("Done")


if __name__ == "__main__":
    main()
