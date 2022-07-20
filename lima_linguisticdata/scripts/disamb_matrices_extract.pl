#!/usr/bin/perl -s

# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

##########
# Création des bigrammes et des trigrammes
##########

print STDERR ("Création des bigrammes et des trigrammes\n");

open(SOURCE,"$ARGV[0]") || die ("Impossible d'ouvrir le fichier $ARGV[0]");
open(BI,">bi") || die ("Impossible d'ouvrir le fichier bi");
open(TRI,">tri") || die ("Impossible d'ouvrir le fichier tri");



$cat1 = "";
$cat2 = <SOURCE>;
chomp($cat2);
$cat3 = <SOURCE>;
chomp($cat3);

$ligne = <SOURCE>;

print BI ("$cat2	$cat3\n");

while ($ligne ne "") {
    chomp($ligne);

    $cat1 = $cat2;
    $cat2 = $cat3;
    $cat3 = $ligne;

    print BI ("$cat2	$cat3\n");
    print TRI ("$cat1	$cat2	$cat3\n");

    $ligne = <SOURCE>;

}

close(SOURCE);
close(BI);
close(TRI);

############################
# 2e partie du programme
# Tri des bigrammes et des trigrammes
############################


system("sort bi >bigramssort.txt");
system("sort tri >trigramssort.txt");


############################
# 3e partie du programme
# Calcul des fréquences des bigrammes et des trigrammes
############################

open(BI,"bigramssort.txt") || die("Impossible d'ouvrir le fichier bigramssort.txt");
open(TRI,"trigramssort.txt") || die("Impossible d'ouvrir le fichier trigramssort.txt");

open(CIBLEBI,">bigramsdef.txt") || die ("Impossible d'ouvrir le fichier bigramsdef.txt");
open(CIBLETRI,">trigramsdef.txt") || die ("Impossible d'ouvrir le fichier trigramsdef.txt");

### On commence par les bigrammes

$ligneant = <BI>;
$nbrebi = 1;
chop($ligneant);
$nbre = 1;
$lignecour = <BI>;

while ($lignecour ne "") {
    $nbrebi++;
    chop($lignecour);

    if ($lignecour eq $ligneant) {
	$nbre++;
    }
    else {
	print CIBLEBI ("$ligneant	$nbre\n");
	$nbre = 1;
    }
    $ligneant = $lignecour;
    $lignecour = <BI>;
}

### On poursuit ensuite avec les trigrammes

$ligneant = <TRI>;
$nbretri = 1;
chop($ligneant);
$nbre = 1;
$lignecour = <TRI>;

while ($lignecour ne "") {
    $nbretri++;
    chop($lignecour);

    if ($lignecour eq $ligneant) {
	$nbre++;
    }
    else {
	print CIBLETRI ("$ligneant	$nbre
");
	$nbre = 1;
    }
    $ligneant = $lignecour;
    $lignecour = <TRI>;
}

close(BI);
close(TRI);
close(CIBLEBI);
close(CIBLETRI);


#########
# 4e partie du programme
# calcul des réels
# NB: cette partie est remplacée par une sortie des fréquences (et non des probas)
#########


open (BI,"bigramsdef.txt") || die ("Impossible d'ouvrir le fichier bigramsdef.txt");
open (TRI,"trigramsdef.txt") || die ("Impossible d'ouvrir le fichier trigramsdef.txt");

open (CIBLEBI,">bigramsend.txt") || die ("Impossible d'ouvrir le fichier bigramsend.txt");
open (CIBLETRI,">trigramsend.txt") || die ("Impossible d'ouvrir le fichier trigramsend.txt");

# On commence par les bigrammes

$ligne = <BI>;

while ($ligne ne "") {

    @infos = split(/	/,$ligne);
    $freq = $infos[2];
    chop($freq);
    # $freqreel = ($freq + 0.0) / $nbrebi;

    # printf CIBLEBI ("$infos[0]	$infos[1]	%11.10f\n",$freqreel);
	printf CIBLEBI ("$infos[0]	$infos[1]	%d\n",$freq);
    $ligne = <BI>;
}

# On poursuit avec les trigrammes

$ligne = <TRI>;

while ($ligne ne "") {

    @infos = split(/	/,$ligne);
    $freq = $infos[3];
    chop($freq);
    # $freqreel = ($freq + 0.0) / $nbretri;

    # printf CIBLETRI ("$infos[0]	$infos[1]	$infos[2]	%11.10f\n",$freqreel);
	printf CIBLETRI ("$infos[0]	$infos[1]	$infos[2]	%d\n",$freq);
    $ligne = <TRI>;
}
