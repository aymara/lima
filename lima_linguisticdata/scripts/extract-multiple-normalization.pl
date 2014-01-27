#!/usr/bin/perl
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

#  Ce script prend en entrée un dictionnaire au format XML et affiche la liste des entrée pour lesquelles il
#  existe 2 lemmes différents ou 2 formes normalisées différentes.

my $current,$lemma,$lemmaFlag,$norm,$normalFlag;

while (<>) {
  chop();
  if (/<k>(.*)<\/k>/)
  {
    $current=$1;
    $lemmaFlag=0;
    $normalFlag=0;
  }
  if (/<l>(.*)<\/l>/)
  {
    if (($lemmaFlag == 1) && ($1 ne $lemma))  {
      print "$current has at least 2 lemma : $lemma ; $1\n";
    }
    $lemmaFlag=1;
    $lemma=$1;
  }
  if (/<l\/>/)
  {
    if (($lemmaFlag == 1) && ($current ne $lemma)) {
      print "$current has at least 2 lemma : $lemma ; $current\n";
    }
    $lemmaFlag=1;
    $lemma=$current;
  }
  if (/<n>(.*)<\/n>/)
  {
    if (($normalFlag == 1) && ($1 ne $norm)) {
      print "$current has at least 2 norms : $norm ; $1\n";
    }
    $normalFlag=1;
    $norm=$1;
  }
  if (/<n\/>/)
  {
    if (($normalFlag == 1) && ($current ne $norm)) {
      print "$current has at least 2 norms : $norm ; $current\n";
    }
    $normalFlag=1;
    $norm=$current;
  }
}
