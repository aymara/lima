#   Copyright 2002-2013 CEA LIST
#    
#   This file is part of LIMA.
#
#   LIMA is free software: you can redistribute it and/or modify
#   it under the terms of the GNU Affero General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   LIMA is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU Affero General Public License for more details.
#
#   You should have received a copy of the GNU Affero General Public License
#   along with LIMA.  If not, see <http://www.gnu.org/licenses/>
#!/usr/bin/env python2
# -*- coding: utf-8 -*-

# Converts wolf-0.1.4 from its xml format with WordNet 2.0 IDs
# to the DebVisDic format with WordNet 3.0 IDs

from lxml import etree
from lxml.etree import ElementTree, Element
from collections import defaultdict
import sys

result = {}

codes = etree.fromstring(sys.stdin.read())
extract = Element("codes")

for code in codes.findall("code"):
    k = code.attrib["key"]
    f = {}
    for field in code.findall("field"):
        f[field.attrib["key"]] = field.attrib["value"]
    #MICRO,GENDER,NUMBER,PERSON,TIME,SYNTAX;code
    if "ICRO" in f:
        resultkey = "{},{},{},{},{},{}".format(f["MICRO"], f["SYNTAX"],
                                               f["TIME"], f["PERSON"],
                                               f["GENDER"], f["NUMBER"])

        if resultkey in result:
            result[resultkey].append(k)
        else:
            result[resultkey] = [k]
        extract.append(code)

for resultkey in result:
    print("{};{}".format(resultkey, str.join(",", result[resultkey])))

#print(etree.tostring(extract,encoding="UTF-8",
  #xml_declaration=True,
  #pretty_print=True))
