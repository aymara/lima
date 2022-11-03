#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# Copyright 2021 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

import json
import re

import requests

url = "https://query.wikidata.org/sparql"


def download_lang_table():
    query = """
    SELECT ?item ?c ?i ?j ?k ?itemLabel
    {
      ?item wdt:P424 ?c .  # Wikimedia language code
      ?item wdt:P218 ?i .  # ISO 639-1
      ?item wdt:P219 ?j .  # ISO 639-2 (two different codes for one language are
                           # possible: for terminology and for bibliography)
      ?item wdt:P220 ?k .  # ISO 639-3
      MINUS{?item wdt:P31/wdt:P279* wd:Q14827288} #exclude Wikimedia projects
      MINUS{?item wdt:P31/wdt:P279* wd:Q17442446} #exclude Wikimedia internal stuff
      SERVICE wikibase:label { bd:serviceParam wikibase:language "[AUTO_LANGUAGE],en". }
    }
    """
    r = requests.get(url, params={"format": "json", "query": query})
    data = r.json()
    rv = {}
    for record in data["results"]["bindings"]:
        item = {
            "wdobj": record["item"]["value"],
            "wmcode": record["c"]["value"],
            "iso-639-1": record["i"]["value"],
            "iso-639-2": [record["j"]["value"]],
            "iso-639-3": record["k"]["value"],
            "label": record["itemLabel"]["value"],
        }

        mo = re.search("(Q[0-9]+)$", item["wdobj"])
        if mo:
            item["wdobj"] = mo.group(1)
        else:
            raise

        if item["iso-639-1"] not in rv:
            rv[item["iso-639-1"]] = {}
        if item["iso-639-3"] in rv[item["iso-639-1"]]:
            if item["wdobj"] in rv[item["iso-639-1"]][item["iso-639-3"]]:
                existing_item = rv[item["iso-639-1"]][item["iso-639-3"]][item["wdobj"]]
                if item["iso-639-2"][0] not in existing_item["iso-639-2"]:
                    existing_item["iso-639-2"].append(item["iso-639-2"][0])
                    continue
        else:
            rv[item["iso-639-1"]][item["iso-639-3"]] = {}
        rv[item["iso-639-1"]][item["iso-639-3"]][item["wdobj"]] = item

    return rv


if __name__ == "__main__":
    table = download_lang_table()
    print(json.dumps(table, indent=2, ensure_ascii=False, sort_keys=True))
