#!/usr/bin/env python3
# -*- coding: UTF-8 -*-

# Copyright 2002-2023 CEA LIST
# SPDX-FileCopyrightText: 2023 CEA LIST <benjamin.labbe@cea.fr>
#
# SPDX-License-Identifier: MIT

import xml.etree.ElementTree
import os
from typing import Tuple , List, Dict
from dataclasses import dataclass, field, fields
from collections import Counter
import subprocess
import logging
import sys
import io
import argparse


logging.basicConfig(stream=sys.stderr,level=logging.INFO)
logger = logging.getLogger("readMultFile_stats")


def readMultFile(filename: str, execDir: str = ".", environ: Dict = {}) -> Tuple[bool, str]:
    command_name = 'readMultFile'
    command_args = [command_name, filename]
    stdout_msg = ""
    stderr_msg = ""
    try:
        res = subprocess.run(command_args,
                             stdout=subprocess.PIPE,
                             stderr=subprocess.PIPE,
                             cwd=execDir,
                             env=environ)

        if res.stdout:
            stdout_msg = res.stdout.decode("utf-8").rstrip()
        if res.stderr:
            stderr_msg = res.stderr.decode("utf-8").rstrip()
        if res.returncode != 0:
            logger.error("Error: testing file %s. Returncode:%d" %
                         (filename, res.returncode))
            logger.error("Error: %s" % (stderr_msg))
            return False, ""
        else:
            if stdout_msg:
                logger.debug("Stdout: %s" % (stdout_msg))
            if stderr_msg:
                logger.debug("Stderr: %s" % (stderr_msg))
    except (subprocess.CalledProcessError, subprocess.SubprocessError) as e:
        logger.error("Error: testing file %s. Returncode:%d. Error: %s . %s" % (
            filename, res.returncode, str(e), stderr_msg))
        return False, ""
    return True, stdout_msg


def uniq_list(seq: list) -> list:
    """Select uniq elements in a list while keeping order
    """
    seen: list = []
    seen_add = seen.append
    return [x for x in seq if not (x in seen or seen_add(x))]


@dataclass
class DocumentStatitics:

	# nb tag of type <node indexingNode="yes">
	nb_indexingNode : int = 0
	# nb elements inside <tokens> tag
	nb_tokens : int = 0
	# nb elements of type <bowNamedEntity>
	nb_bowNamedEntity : int = 0
	# nb elements of type <bowToken>
	nb_bowToken : int = 0
	# nb elements of type <bowTerm>
	nb_bowTerm : int = 0

	# List of NamedEntity groups, with counts
	bowNamedEntity_type_groups : Counter = field(default_factory=Counter)

	# List of NamedEntity groups, with counts
	bowNamedEntity_type_names : Counter = field(default_factory=Counter)

	# nb distinct tags of type <property name="identPrpty"  value="??" />
	nb_identPrpty : int = 0

	# nb distinct tags of type <property name="srcePrpty"  value="??" />
	nb_srcePrpty : int = 0


	def to_string(self) -> str:
		out : str = ""
		for field in fields(self):
			out += f" {field.name} : {getattr(self, field.name)}\n"
		return out


def get_group(entity_name : str) -> str:
    gr = ""
    if "." in entity_name:
        gr, ent = entity_name.split(".")
    return gr


def get_name(entity_name : str) -> str:
    ent = entity_name
    if "." in entity_name:
        gr, ent = entity_name.split(".")
    return ent


def split_entity_name(entity_name : str) -> Tuple[str,str]:
    gr = ""
    ent = entity_name
    if "." in entity_name:
        gr, ent = entity_name.split(".")
    return gr, ent


def processOneInputFile(inputFile : str) -> DocumentStatitics :

	statitics : DocumentStatitics = DocumentStatitics()

	st, stdout_msg = readMultFile(inputFile, environ=os.environ)
	if not st:
		return statitics

	sstream = io.StringIO(stdout_msg)
	tree = xml.etree.ElementTree.parse(sstream)

	indexingNodes = tree.findall('.//node[@indexingNode=\'yes\']')
	statitics.nb_indexingNode = len( indexingNodes )

	tokens = tree.findall('.//tokens')
	statitics.nb_tokens = len( tokens )

	bowNamedEntitys = tree.findall('.//bowNamedEntity')
	statitics.nb_bowNamedEntity = len( bowNamedEntitys )

	bowTokens = tree.findall('.//bowToken')
	statitics.nb_bowToken = len( bowTokens )

	bowTerms = tree.findall('.//bowTerm')
	statitics.nb_bowTerm = len( bowTerms )

	identPrptys = tree.findall('.//property[@name=\'identPrpty\']')
	uniq_identPrptys = uniq_list( [ prpty.attrib["value"] for prpty in identPrptys ] )
	statitics.nb_identPrpty = len( uniq_identPrptys )

	srcePrptys = tree.findall('.//property[@name=\'srcePrpty\']')
	uniq_srcePrptys = uniq_list( [ prpty.attrib["value"] for prpty in srcePrptys ] )
	statitics.nb_srcePrpty = len( uniq_srcePrptys )

	# ----
	bowNamedEntity_types = [ bowNamedEntity.attrib["type"] for bowNamedEntity in bowNamedEntitys ]

	for bowNamedEntity_type in bowNamedEntity_types:
		gr, ent = split_entity_name( bowNamedEntity_type )
		statitics.bowNamedEntity_type_groups[ gr ] += 1
		statitics.bowNamedEntity_type_names[ bowNamedEntity_type ] += 1
	# ----

	return statitics


def processInputFiles(inputFiles : List[str], verbose : int = 0) -> List[Tuple[str,DocumentStatitics]] :

	global_statitics : List[Tuple[str,DocumentStatitics]] = []
	nbFiles : int = len(inputFiles)
	for i, inputFile in enumerate(inputFiles):
		logger.debug(f" {i}/{nbFiles} {inputFile}")
		statitics = processOneInputFile(inputFile)
		logger.debug("\n"+statitics.to_string())
		global_statitics.append( (inputFile, statitics) )
	return global_statitics


def mergeStatistics(many_statistics : List[Tuple[str,DocumentStatitics]] ) -> DocumentStatitics :

	global_statitics : DocumentStatitics = DocumentStatitics()
	for field in fields(global_statitics):
		for filename, statitics in many_statistics:
			cur_value = getattr(global_statitics, field.name)
			val = getattr(statitics, field.name)
			setattr(global_statitics, field.name , val+cur_value)
		final_value = getattr(global_statitics, field.name)

	return global_statitics


def main():

	parser = argparse.ArgumentParser(
	    description='Compute statitics on a collection of mult files')
	parser.add_argument('mult_files', type=str, nargs="+",
	                    help='List of mult files from the semantic analysis')
	parser.add_argument('--logging', dest="logging_level", type=str,
	                default="INFO", help="Set the logging level")
	# Parse command line options
	args = parser.parse_args()

	args.logging_level = getattr(logging, args.logging_level.upper(), logging.INFO)
	logger.setLevel( args.logging_level )

	logger.info("Compute statistics on input files. Input list : %d documents" %
                    len(args.mult_files))
	all_statitics : List[Tuple[str,DocumentStatitics]] = processInputFiles(args.mult_files)

	logger.info("Merge statistics ...")
	global_statitics : DocumentStatitics = mergeStatistics(all_statitics)

	logger.info("Global statistics:\n"+global_statitics.to_string())

if __name__=="__main__":
	main()
