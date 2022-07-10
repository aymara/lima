#!/usr/bin/perl -s

# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

open(SOURCE,"$ARGV[0]") || die ("Impossible d'ouvrir le fichier $ARGV[0]");
open(CIBLE,">$ARGV[1]") || die ("Impossible d'ouvrir le fichier $ARGV[1]");

$ligne = <SOURCE>;

while ($ligne ne "") {

    $ligne=~ s/;/	/g;
    print CIBLE ($ligne);

    $ligne=<SOURCE>;

} 
