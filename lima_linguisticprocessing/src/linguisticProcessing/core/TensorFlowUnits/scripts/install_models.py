#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os
import re
import argparse
import tempfile
import unix_ar
import tarfile
import requests
import urllib.request
from tqdm import tqdm


URL_DEB = 'https://github.com/aymara/lima-models/releases/download/v0.1.5-beta/lima-deep-models-%s-%s_0.1.5_all.deb'
URL_C2LC = 'https://raw.githubusercontent.com/aymara/lima-models/master/c2lc.txt'
C2LC = { 'lang2code': {}, 'code2lang': {} }

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-i', '--info',
                        help='print list of available languages and exit', action='store_true')
    parser.add_argument('-l', '--lang', type=str,
                        help='language name or language code (example: \'english\' or \'eng\')')
    parser.add_argument('-d', '--dest', type=str,
                        help='destination directory')
    parser.add_argument('-s', '--select', type=str,
                        help='select particular models to install: tokenizer, morphosyntax, lemmatizer (comma-separated list)')
    args = parser.parse_args()

    if args.info is not None and args.info:
        find_lang_code('eng')
        for code in C2LC['code2lang']:
            print('%-10s\t%s' % (code, C2LC['code2lang'][code]))
        return

    if args.lang is None:
        sys.stderr.write('ERROR: the following argument is required: -l/--lang\n\n')
        parser.print_help()
        sys.exit(-1)

    code, lang = find_lang_code(args.lang.lower())
    deb_url = URL_DEB % (code, lang)

    if args.dest is None or len(args.dest) == 0:
        home_dir = os.path.expanduser('~')
        target_dir = os.path.join(home_dir, '.lima', 'resources')
    else:
        target_dir = args.dest

    print('Language: %s, code: %s' % (lang, code))
    print('Installation dir: %s' % target_dir)
    print('Downloading %s' % deb_url)

    prefix_list = []
    if args.select is not None:
        prefix_list = [ x.lower().strip() for x in args.select.split(',') ]
        if 'morphosyntax' in prefix_list:
            prefix_list.append('fasttext')

        print('Installing only: %s' % ( ', '.join(prefix_list)))

    with tempfile.TemporaryDirectory() as tmpdirname:
        download_binary_file(deb_url, tmpdirname)
        install_model(target_dir, os.path.join(tmpdirname, deb_url.split('/')[-1]), code, prefix_list)


def install_model(dir, fn, code, prefix_list):
    ar_file = unix_ar.open(fn)
    tarball = ar_file.open('data.tar.gz')
    tar_file = tarfile.open(fileobj=tarball)
    members = tar_file.getmembers()
    for m in members:
        if m.size > 0:
            file = tar_file.extractfile(m)
            if file is not None:
                full_dir, name = os.path.split(m.name)
                if len(prefix_list) > 0:
                    name_prefix, _ = name.split('-')
                    if name_prefix not in prefix_list:
                        continue
                mo = re.match(r'./usr/share/apps/lima/resources/(TensorFlow[A-Za-z\/\-\.0-9]+)', full_dir)
                if mo:
                    subdir = mo.group(1)
                    if subdir is None or len(subdir) == 0:
                        sys.stderr.write('Error: can\'t parse \'%s\'\n' % full_dir)
                        sys.exit(1)
                    target_dir = os.path.join(dir, subdir)
                    os.makedirs(target_dir, exist_ok=True)
                    with open(os.path.join(target_dir, name), 'wb') as f:
                        while True:
                            chunk = file.read(4096)
                            if len(chunk) == 0:
                                break
                            f.write(chunk)
                    # LIMA historically uses 'fre' for French.
                    # This workaround adds symlinks 'fre' -> 'fra' to support this.
                    if code in [ 'fra' ]:
                        src_name = os.path.join(target_dir, name)
                        symlink_name = re.sub(r'-fra.(conf|model|bin)$', r'-fre.\1', name, 1)
                        symlink_name = os.path.join(target_dir, symlink_name)
                        if not os.path.isfile(symlink_name):
                            os.symlink(src_name, symlink_name)


def download_binary_file(url, dir):
    chunk_size = 4096
    local_filename = os.path.join(dir, url.split('/')[-1])
    totalbytes = 0
    response = requests.get(url, stream=True)
    if response.status_code == 200:
        total_size_in_bytes= int(response.headers.get('content-length', 0))
        progress_bar = tqdm(total=total_size_in_bytes, unit='iB', unit_scale=True)
        with open(local_filename, 'wb') as f:
            for chunk in response.iter_content(chunk_size=chunk_size):
                progress_bar.update(len(chunk))
                if chunk:
                    totalbytes += chunk_size
                    f.write(chunk)
    progress_bar.close()


def find_lang_code(lang_str):
    if len(list(C2LC['lang2code'].keys())) == 0:
        with urllib.request.urlopen(URL_C2LC) as f:
            for line in f.read().decode('utf-8').lower().split('\n'):
                line.strip()
                if len(line) > 0:
                    corpus, code = line.split(' ')
                    lang, corp_id = corpus.split('-')
                    C2LC['lang2code'][lang] = code
                    C2LC['code2lang'][code] = lang

    if lang_str in C2LC['lang2code']:
        return C2LC['lang2code'][lang_str], lang_str
    elif lang_str in C2LC['code2lang']:
        return lang_str, C2LC['code2lang'][lang_str]
    return None


if __name__ == '__main__':
    main()

