import argparse

from tqdm import tqdm


def create_ngrams(input_file):
    print("Creating bigrams and trigrams")

    with open(input_file, 'r') as source, \
         open('bi', 'w') as bi, \
         open('tri', 'w') as tri:

        cat1 = ""
        cat2 = source.readline().strip()
        cat3 = source.readline().strip()

        bi.write(f"{cat2}\t{cat3}\n")

        for ligne in tqdm(source, desc="Creating ngrams"):
            ligne = ligne.strip()

            cat1 = cat2
            cat2 = cat3
            cat3 = ligne

            bi.write(f"{cat2}\t{cat3}\n")
            tri.write(f"{cat1}\t{cat2}\t{cat3}\n")


def sort_file(input_file, output_file):
    with open(input_file, 'r') as infile:
        lines = infile.readlines()
    lines.sort()
    with open(output_file, 'w') as outfile:
        for line in lines:
            outfile.write(line)


def calculate_frequencies(input_file, output_file):
    with open(input_file, 'r') as infile, \
         open(output_file, 'w') as outfile:

        ligneant = infile.readline().strip()
        nbre = 1

        for lignecour in tqdm(
                infile, desc=f"Calculating frequencies for {input_file}"):
            lignecour = lignecour.strip()

            if lignecour == ligneant:
                nbre += 1
            else:
                outfile.write(f"{ligneant}\t{nbre}\n")
                nbre = 1
            ligneant = lignecour


# def output_frequencies(input_file, output_file, nbre_total):
#     with open(input_file, 'r') as infile, \
#          open(output_file, 'w') as outfile:
#
#         for ligne in tqdm(infile, desc=f"Outputting frequencies for {input_file}"):
#             infos = ligne.strip().split('\t')
#             freq = int(infos[-1])
#             outfile.write(f"{'\t'.join(infos[:-1])}\t{freq}\n")
#

def main():
    parser = argparse.ArgumentParser(
        description="Process bigrams and trigrams from a text file.")
    parser.add_argument("input_file", help="The input file to process")
    args = parser.parse_args()

    create_ngrams(args.input_file)

    sort_file('bi', 'bigramssort.txt')
    sort_file('tri', 'trigramssort.txt')

    # calculate_frequencies('bigramssort.txt', 'bigramsdef.txt')
    # calculate_frequencies('trigramssort.txt', 'trigramsdef.txt')
    #
    # output_frequencies('bigramsdef.txt', 'bigramsend.txt', None)
    # output_frequencies('trigramsdef.txt', 'trigramsend.txt', None)
    calculate_frequencies('bigramssort.txt', 'bigramsend.txt')
    calculate_frequencies('trigramssort.txt', 'trigramsend.txt')


if __name__ == "__main__":
    main()
