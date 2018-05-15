#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# kate: encoding utf-8;

import sys,argparse
import re
import os


#----------------------------------------------------------------------
# main function
def main(argv):
    p = re.compile(r'<value name="([^"]+)"')
    # parse command-line arguments
    parser=argparse.ArgumentParser(
        description="Convert codes file to symbolic code file")
    # optional arguments
    parser.add_argument("codefile",
                        type=argparse.FileType('r',
                                               encoding='utf-8',
                                               errors='ignore'),
                        help="input file: codes file")
    param=parser.parse_args()
    
    #with open(param.codefile, 'r') as f:
    in_micro = False
    print('<?xml version=\'1.0\' encoding=\'UTF-8\'?>')
    print('<codes>')

    for code_line in param.codefile:
        if '<subproperty name="MICRO" parent="MACRO">' in code_line:
            in_micro = True
        if in_micro and '</subproperty>' in code_line:
            in_micro = False
        m = p.search(code_line)
        if in_micro and m:
            print('  <code key="{}">'.format(m.group(1)))
            print('    <field key="MICRO" value="{}"/>'.format(m.group(1)))
            print('  </code>')
    print('</codes>')


main(sys.argv[1:])
