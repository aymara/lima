#!/usr/bin/perl -w

# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

# Helper script to speed up the creation of the dictionary mapping Lefff 
# simplified tags to Lima std tags
# Used to help creating convlefffstd.txt
use strict;

binmode(STDOUT, ":utf8");

while (my $line = <>)
{
  chomp $line;
  print $line;
  if ($line =~ /^v/)
  {
    print "V";
    if ($line =~ /^[^,]+$/)
    {
      print "p";
    }
    elsif ($line =~ /\[\@aux/)
    {
      print "a";
    }
    else
    {
      print "p";
    }

    # Mode et temps
    if ($line =~ /\@C/)
    {
      print "ic";
    }
    elsif ($line =~ /\@P/)
    {
      print "ip";
    }
    elsif ($line =~ /\@F/)
    {
      print "if";
    }
    elsif ($line =~ /\@I/)
    {
      print "ii";
    }
    elsif ($line =~ /\@J/)
    {
      print "is";
    }
    elsif ($line =~ /\@Y/)
    {
      print "mp";
    }
    elsif ($line =~ /\@S/)
    {
      print "sp";
    }
    elsif ($line =~ /\@T/)
    {
      print "si";
    }
    elsif ($line =~ /\@K/)
    {
      print "ps";
    }
    elsif ($line =~ /\@G/)
    {
      print "pp";
    }
    elsif ($line =~ /\@W/)
    {
      print "np";
    }
    else
    {
      print "--";
    }

    # Transitivite, pronominalite et personnalite
    if ($line =~/\@aux/)
    {
      print "-";
    }
    elsif ($line =~/\@impers/)
    {
      print "m";
    }
    elsif ($line =~ /\@pron/)
    {
      print "p";
    }
    elsif ($line =~ /\@trans/)
    {
      print "t";
    }
    else
    {
      print "i";
    }
    
    
    # Personne et nombre
    if ($line =~ /1p/)
    {
      print "1-p";
    }
    elsif ($line =~ /2p/)
    {
      print "2-p";
    }
    elsif ($line =~ /3p/)
    {
      print "3-p";
    }
    elsif ($line =~ /1s/)
    {
      print "1-s";
    }
    elsif ($line =~ /2s/)
    {
      print "2-s";
    }
    elsif ($line =~ /3s/)
    {
      print "3-s";
    }
    elsif ($line =~ /ms\]/)
    {
      print "-ms";
    }
    elsif ($line =~ /mp\]/)
    {
      print "-mp";
    }
    elsif ($line =~ /fs\]/)
    {
      print "-fs";
    }
    elsif ($line =~ /fp\]/)
    {
      print "-fp";
    }
    else
    {
      print "---";
    }
  }
  print "
";
}
