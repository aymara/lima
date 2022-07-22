#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

# -*- coding: UTF-8 -*-

import argparse

import requests

url = "https://query.wikidata.org/bigdata/namespace/wdq/sparql"


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-q", "--query", help="Query text")
    parser.add_argument("-f", "--file", help="File with query")
    args = parser.parse_args()

    if args.query is not None:
        query = args.query
    elif args.file is not None:
        query = open(args.file, mode="r").read()
    else:
        raise

    r = requests.get(url, {"query": query, "format": "json"})
    if r.status_code in [200]:
        # extracting data in json format
        data = r.text
        print(data)
    else:
        print(r)


if __name__ == "__main__":
    main()
