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

###############################################
# Programme de conversion de categories :
# convertit les dictionnaires de Chritian Fluhr
# en format Hub
###############################################

print STDERR ("INFO : Debut du traitement convertstd\n");

open(SOURCE,"$ARGV[0]") || die("Impossible d'ouvrir le fichier $ARGV[0]\n");
print STDERR ("INFO : Fichier dico.txt ouvert\n");

my $convertdatafile = "convstd.txt";
open (LISTETAGS,"$convertdatafile") || die ("Impossible d'ouvrir le fichier $convertdatafile\n");
print STDERR ("INFO : Fichier $convertdatafile ouvert\n");

open(CIBLE,">$ARGV[1]") || die("Impossible d'ouvrir le fichier $ARGV[1]\n");
print STDERR ("INFO : Fichier dicostd.txt ouvert\n");


#chargement dans une table des cat�ories et de leurs correspondances
my %tags;
while (<LISTETAGS>) 
{
  chomp;
  if ($_ =~ /^\s*$/) {next;}
  s/#.*$//;
  if ($_ !~ /([^;]+);([^;]+)/)
  {
    print STDERR "$_\n";
    next;
  }
  my ($strcateg, $gracecateg) = ($1,$2);
  $strcateg =~ s/\s+/ /g;
  print STDOUT "$strcateg -> $gracecateg";
  $tags{$strcateg} = $gracecateg;
}

#fin du chargement des categories et de leurs correspondances

print STDERR ("----------------------------------\n");
print STDERR ("Lignes non traitees par convertstd\n\n");

$motstraites = 0;

$motsnontraites = 0;

$ligne = <SOURCE>;
while ($ligne ne "") {
	
	$motstraites++;
	
	print STDERR ("INFO : Lignes traitees : $motstraites\n");
	
	@donnees = split(/	/,$ligne);
	
	$entree = $donnees[0];
	$info1 = $donnees[1];
	$info2 = $donnees[2];
	$info3 = $donnees[3];
	$lemme = $donnees[4];
	$normalisation = $donnees[5];
	$info = $info1." ".$info2." ".$info3;
	$info =~ s/\s+/ /g;
	$info =~ s/\s+$//;




	# Codage des categories
	
	#Fin du codage des categories
	
	if ($tags{$info} ne "") {
		$codestags = $tags{$info};
		@codetag = split(/\+/,$codestags);
		$tailleCodetag = @codetag;
		for ($k=0 ; $k<$tailleCodetag ; $k++) { 
			print CIBLE ("$entree	$lemme	$normalisation	$codetag[$k]\n");
		}
	}
	else {
		$motsnontraites++;
		print STDERR ("$entree	$lemme	$normalisation	$info not found in $convertdatafile\n");
	}

	$ligne = <SOURCE>;
}

print STDERR ("\nINFO : Entrees non prises en compte : $motsnontraites \n");
print STDERR ("INFO : Traitement convertstd termine\n");
