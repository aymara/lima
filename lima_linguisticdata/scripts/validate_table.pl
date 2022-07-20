#!/usr/bin/perl -s

# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

open (MICROCODELIST,"$ARGV[0]");
open (MACROCODELIST,"$ARGV[1]");
open (TABLECONVERT,"$ARGV[2]");

####
# Chargement de la table de conversion
####

$line = <MICROCODELIST>;
while ($line ne "") {
    chomp($line);
    @data = split(/;/,$line);
    $codelist{$data[0]} = "ok";
    $line=<MICROCODELIST>;
}

$line = <MACROCODELIST>;
while ($line ne "") {
    chomp($line);
    @data = split(/;/,$line);
    $macrocodelist{$data[0]} = "ok";
    $line=<MACROCODELIST>;
}


$info = <TABLECONVERT>;

while ($info ne "") {
	chomp($info);
	@data = split(/	/,$info);
    if ($codelist{$data[1]} eq "") {
      print ("$data[1] is an unknwon micro category !\n");
    }
    if ($macrocodelist{$data[1]} eq "ok") {
      print ("$data[1] is a macro category ! \n");
    }
	$info = <TABLECONVERT>;
}
