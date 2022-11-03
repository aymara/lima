#!/usr/bin/env python3

# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

# Converts wolf-0.1.4 from its xml format with WordNet 2.0 IDs
# to the DebVisDic format with WordNet 3.0 IDs

import sys

from lxml import etree
from lxml.etree import Element

result = {}

codes = etree.fromstring(sys.stdin.read())
extract = Element("codes")

for code in codes.findall("code"):
    k = code.attrib["key"]
    f = {}
    for field in code.findall("field"):
        f[field.attrib["key"]] = field.attrib["value"]
    # MICRO,GENDER,NUMBER,PERSON,TIME,SYNTAX;code
    if "ICRO" in f:
        resultkey = "{},{},{},{},{},{}".format(
            f["MICRO"], f["SYNTAX"], f["TIME"], f["PERSON"], f["GENDER"], f["NUMBER"]
        )

        if resultkey in result:
            result[resultkey].append(k)
        else:
            result[resultkey] = [k]
        extract.append(code)

for resultkey in result:
    print("{};{}".format(resultkey, str.join(",", result[resultkey])))

# print(etree.tostring(extract,encoding="UTF-8",
# xml_declaration=True,
# pretty_print=True))
