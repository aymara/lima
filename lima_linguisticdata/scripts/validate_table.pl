#!/usr/bin/perl -s
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
