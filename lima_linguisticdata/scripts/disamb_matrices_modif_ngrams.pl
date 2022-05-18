#!/usr/bin/perl -s -W

# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

use strict;

open (SOURCE,"$ARGV[0]")  || die ("Impossible d ouvrir le fichier source");
open (CIBLE,">$ARGV[1]")  || die ("Impossible d ouvrir le fichier cible");
open (MODIFS,"$ARGV[2]")  || die ("Impossible d ouvrir le fichier de modifications");


my %matrice;
my $largeur=0;
my $linenum=0;
# Chargement du fichier source
while (<SOURCE>) 
{
  chomp;
  $linenum++;
  my @ligne = split(/\s+/, $_);
  if ($#ligne == 2)
  {
    $largeur=2 if $largeur==0;
    if ($largeur!=2)
    {
      print STDERR "Invalid line $linenum: $_ in $ARGV[0]
  got a width of $#ligne while $largeur was expected\n";
      next;
    }
    $matrice{$ligne[0]}{$ligne[1]}=$ligne[2];
  }
  elsif ($#ligne == 3)
  {
    $largeur=3 if $largeur==0;
    if ($largeur!=3)
    {
      print STDERR "Invalid line $linenum: $_ in $ARGV[0]
  got a width of $#ligne while $largeur was expected\n";
      next;
    }
    $matrice{$ligne[0]}{$ligne[1]}{$ligne[2]}=$ligne[3];
  }
  else
  {
      print STDERR "Invalid line $linenum: $_ in $ARGV[0]
  got a width of $#ligne\n";
  }
}
close(SOURCE);

# Ajout ou suppression d elements

my $nbreModifs = 0;

while (<MODIFS>) 
{
  chomp;
  $nbreModifs++;
  if (s/^\+\s+//) 
  {
    my @dataLigne = split(/\s+/,$_);
    if ($largeur==3 && $#dataLigne == 3)
    {
      $matrice{$dataLigne[0]}{$dataLigne[1]}{$dataLigne[2]}=$dataLigne[3];
    }
    elsif ($largeur==2 && $#dataLigne == 2)
    {
      $matrice{$dataLigne[0]}{$dataLigne[1]}=$dataLigne[2];
    }
    else 
    {
	    print STDERR "Invalid addition for ${largeur}gram: $_"; 
	    next;
    }
  }
  elsif (s/^-\s+//) 
  {
    my @dataLigne = split(/\s+/,$_);
    if ($#dataLigne == 0)
    {
      $matrice{$dataLigne[0]}=undef;
      foreach my $first (keys %matrice)
      {
        $matrice{$first}{$dataLigne[0]}=undef;
        my %tmp = %{$matrice{$first}};
        foreach my $second (keys %tmp)
        {
          $matrice{$first}{$second}{$dataLigne[0]}=undef;
        }
      }
    }
    elsif ($#dataLigne == 1) 
    {
      $matrice{$dataLigne[0]}{$dataLigne[1]}=undef;
      foreach my $first (keys %matrice)
      {
        $matrice{$first}{$dataLigne[0]}{$dataLigne[1]}=undef;
      }
    }
    elsif ($#dataLigne == 2) 
    {
      $matrice{$dataLigne[0]}{$dataLigne[1]}{$dataLigne[2]}=undef;
    }
  }
}
close MODIFS;

# Impression de la matrice

if ($largeur==3)
{
  foreach my $first (keys %matrice)
  {
    if (defined $matrice{$first})
    {
      my %tmp = %{$matrice{$first}};
      foreach my $second (keys %tmp)
      {
        if (defined $tmp{$second})
        {
          my %tmp2 = %{$tmp{$second}};
          foreach my $third (keys %tmp2)
          {
            if (defined $tmp2{$third})
            {
              print CIBLE "$first	$second	$third	$tmp2{$third}\n";
            }
          }
        }
      }
    }
  }
}
else
{
  foreach my $first (keys %matrice)
  {
    foreach my $second (keys %{$matrice{$first}})
    {
        print CIBLE "$first	$second	$matrice{$first}{$second}\n";
    }
  }
}
close CIBLE;

print "Nombre de modifications effectuees a la matrice : $nbreModifs\n";
