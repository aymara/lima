import argparse
import re
import subprocess
import sys

from tqdm import tqdm


def getCodeConversion(convertSymbolicCodes, codeFile, symbolicCodeFiles):
    conv = {}
    command = [convertSymbolicCodes, '--code=' + codeFile] + symbolicCodeFiles
    try:
        result = subprocess.run(command, stdout=subprocess.PIPE,
                                stderr=subprocess.PIPE, text=True, check=True)
    except subprocess.CalledProcessError as e:
        sys.stderr.write(f"cannot get code conversion: {e}\n")
        sys.exit(1)

    for line in result.stdout.splitlines():
        parts = line.split(';')
        if len(parts) >= 2:
            conv[parts[0]] = parts[1]

    return conv


def main():
    parser = argparse.ArgumentParser(
        description='Transcode symbolic codes to numeric codes.')
    parser.add_argument('convertSymbolicCodes',
                        help='Path to the convertSymbolicCodes script')
    parser.add_argument('codeFile', help='Code file')
    parser.add_argument('symbolicCodeFiles', nargs='+',
                        help='Symbolic code files')
    parser.add_argument('rulesFile', help='Rules file')

    args = parser.parse_args()

    convertSymbolicCodes = args.convertSymbolicCodes
    codeFile = args.codeFile
    rulesFile = args.rulesFile
    symbolicCodeFiles = args.symbolicCodeFiles

    conv = getCodeConversion(convertSymbolicCodes, codeFile, symbolicCodeFiles)

    if not conv:
        sys.stderr.write(
            "ERROR: transcodeIdiomatic.py: can't load conv keys\n")
        sys.exit(1)

    with open(rulesFile, 'r') as fin:
        lines = fin.readlines()

    with tqdm(total=len(lines), desc="Transcoding rules") as pbar:
        for line in lines:
            pbar.update(1)
            match = re.match(r'(.*):(ABS_)?IDIOM\$(.*)(:.*)', line)
            if match:
                prefix, abs_idiom, key, suffix = match.groups()
                if key not in conv:
                    # Uncomment the next line if you want to print missing
                    # keys warnings
                    # sys.stderr.write(
                    # f"transcodeIdiomatic.py:
                    # can't find conv for \"{key}\"\n")
                    continue
                print(f"{prefix}:{abs_idiom or ''}IDIOM${conv[key]}{suffix}")
            else:
                print(line, end='')


if __name__ == "__main__":
    main()
