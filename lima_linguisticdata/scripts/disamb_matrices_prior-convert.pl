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

# usage example : ../../scripts/disamb_matrices_prior.pl corpus/corpus_fr_merge.txt matrices/priorUnigramMatrix-fre.dat DIVERS_*,NP*,PONCTU_*,CONJ*,NC_MESURE,NC_NUM_CARD

print "@ARGV\n";

open(SOURCE,"$ARGV[0]") || die "Cannot open $ARGV[0]\n";
open(CIBLE,">$ARGV[1]") || die "Cannot create $ARGV[1]\n";
$excludes = $ARGV[3];

###
# Counting lemmas and their tags
###

print "Loading and sorting corpus prior data (excluding $excludes)from $ARGV[0]\n";
if($excludes ne "") {
  $excludes =~ s/,/|/g;
  $excludes =~ s/\*/.*/g;
}
$lineNum = 1;

while (<SOURCE>) {
  $ligne = $_;
  while (chomp($ligne)>0) {}
  @data = split(/\t/,$ligne);
  $lemma = $data[0];
  $lemma=lc $lemma;
#   print "Handling $lemma / '$data[1]' / $data[1]\n";
  if ($data[1] ne "" and $data[1] !~ m/$excludes/) {
#     print "Counting $lemma / $data[1]\n";
    $tableauprior{$lemma}{$data[1]}++;
  }
  $ligne = <SOURCE>;
}
close (SOURCE);

for $lemma (sort keys %tableauprior) {
    for $categ (sort keys %{$tableauprior{$lemma}}) {
#     print "$lemma / $categ : $tableauprior{$lemma}{$categ}\n";
    print CIBLE ("$lemma\t$categ\t$tableauprior{$lemma}{$categ}\n");
  }
}
close (CIBLE);

print "Done\n";
