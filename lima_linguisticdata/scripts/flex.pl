#!/usr/bin/perl -W
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
use strict;
use utf8;

my ($deffile, $infile, $workdir, $outfile, $excludesfile, $debug) = @ARGV;
open (OUTFILE,">:utf8","$workdir/$outfile") || die "Cannot create $outfile\n";
open (FILETRACE,">:utf8","$workdir/$infile.log") || die "Cannot create $infile.log\n";

(defined $debug) || ($debug=0);

my %categstypes;
my %struct;
my %excludes;

loadData($deffile, \%categstypes, \%struct);

if (defined $excludesfile) {
    loadExcludes($excludesfile, \%excludes);
}
else {
    print STDERR "no excludes file\n";
}

# traitement des formes
print ("Traitement du fichier des mots simples\n");
open (INFILE,"<:utf8",$infile) || die "Cannot open mots-simples.txt\n";

my $oldfh = select(STDOUT);
$| = 1;
select($oldfh);

my $infileLineNum = 0;
while (<INFILE>)
{

  $infileLineNum++;
  if ($infileLineNum%1000 == 0)
  {
	  print "\n$infileLineNum";
  }
  chomp;
  next if ($_ eq "" || (/^#/) );
  my $line = $_;
  if ($_ !~ /^([^;]*);([^;]*);([^;]*);([^;]*);([^;]*)/)
  {
    print FILETRACE "Malformed line $infileLineNum: $_\n";
    next;
  } 
  my $lem = $3;
  my $categ = $4;
  my $norm = $5;
  # faire un miroir du lemme
  my $leminv = "";
  for (my $i=length($lem)-1;$i>=0;$i=$i-1) 
  {
      $leminv=$leminv . substr($lem,$i,1);
  }
  # transcoder la vg pour macrocategorie et prendre les autres informations
  # traitement des noms a ne pas flechir
#  if ($lem=~/-/)    # Greg: on va traiter les mots avec tirets 4/1/05
#  {
#    print OUTFILE ($lem,";",$categ,";;;",$lem,";",$norm,";\n");
#  }
#  elsif ($lem=~/[A-Z].*[A-Z]/) 
  if ($lem=~/[A-Z].*[A-Z]/)
  {
    &printOutput($lem.";".$categ.";;;".$lem.";".$norm.";");
  }
  else
  {    
    traitement(\%struct, $line, $categstypes{$categ}, $lem, $leminv, $categ, $norm);
  }
}
print "\n";

sub traitement
{
  my ($struct, $line, $types, $lem, $leminv, $categ, $norm) = @_;
  if (!defined $types)
  {
    &printOutput($lem.";".$categ.";;;".$lem.";".$norm.";");
    return;
  }

  my @types = @$types;
  foreach my $type (@types)
  {
    my ($models, $modelsdata, $modelstab);
    $models = \%{${$struct{$type}}{"models"}};
    $modelstab = \@{${$struct{$type}}{"modelstab"}};
    $modelsdata = \%{${$struct{$type}}{"modelsdata"}};
    
    my $model;
    my $commun;
    my $allDone;

    my $subleminv;


    $subleminv=$leminv."\$"; 
    $allDone=0; 

    while($allDone==0) {
      $debug && print "subleminv='$subleminv'\n";
    if (defined ($$models{$subleminv})) 
      {
        $model = $$models{$subleminv};
        $commun = $subleminv;
        last;
      }
      if(length($subleminv)>0) {
            chop ($subleminv);
	}
      else { $allDone=1; }
  }

#    foreach my $masque (@$modelstab)
#    {
#      $debug && print "masque='$masque'\n";
#      if ($leminv=~/^$masque/)
#      {
#        $model = $$models{$masque};
#        $commun = $masque;
#        last;
#      }
#    }

    if (!defined($model)) 
    {
      print FILETRACE "Unable to handle line (no model found) $infileLineNum: $line\n";
      return;
    }
    
    $debug && print "$type: lemme='$lem' leminv='$leminv' model='$model' commun='$commun'\n";
    if (!defined($$modelsdata{$model})) 
    {
      print FILETRACE "Unable to handle line (no data for model $model) $infileLineNum: $line\n";
      return;
    }
    my ($racine, $longueur);
    racine($type, \$racine,\$longueur,$model,$lem,$commun);
    foreach my $map ( @{$$modelsdata{$model}} ) 
    {
      my %map = %{$map};
      if (length($$map{"forme"}) > 0)
      {
        #on cherche la partie a conserver du lemme en comparant la longueur mot modifie a celui de fin
        #la difference est la longueur a garder.
        #pour chaque ligne ou le mot modifie est present on construit la forme en prenant la partie fixe et en ajouta$adjectifnt la forme modifiee amputee de sa 
        #forme fixe
        #ecrire dans le fichier de sortie la forme, la vg , les proprietes, le lemme  
        my $forme1=$$map{"forme"};
        my $third;
        my $fourth;
        if ($type eq "verbe")
        {
          $third = $$map{"third"};
          $fourth = $$map{"fourth"};
        }
        else
        {
          $fourth = $$map{"third"};
          $third = $$map{"fourth"};
        }
        # traitement des variantes separees par "/" ( attention une seule variante possible)
        my $forme_sortie;
        if ($$map{"forme"}=~/\//) 
        {
          my @table=split("\/",$$map{"forme"});
          
          $forme_sortie=$racine . substr($table[0],$longueur);
          my $outline = "$forme_sortie;$categ;$third;$fourth;$lem;$norm;";
          $outline =~ s/\?//g;
          &printOutput($outline);
          $forme1=$table[1];
        }
        $forme_sortie = $racine . substr($forme1,$longueur);
#        $debug && print "forme1 = '$forme1' ; forme_sortie = '$forme_sortie'\n";
        my $outline = "$forme_sortie;$categ;$third;$fourth;$lem;$norm;";
        $outline =~ s/\?//g;
        &printOutput($outline);
      }
    }
  }
}


sub racine
{
  my ($type, $racine,$longueur,$model,$lem,$commun) = @_;
  
  if ($type eq "verbe")
  {
    my $llem=length($lem)-1;
    my $lmodel=length($model)-1;
    while (substr($lem,$llem) eq substr($model,$lmodel) && $llem >= 0 && $lmodel >=0) 
    { 
      $llem--; $lmodel--;
    }
    $$racine=substr($lem,0,$llem+1);
    $$longueur = $lmodel+1;
  }
  else
  {
    my $lfin=length($commun); 
    if (chop($commun) eq "\$") 
    {
      $lfin=$lfin-1;
    }
    $$racine=substr($lem,0,length($lem)-$lfin);
    $$longueur=length($model)-$lfin;
    if ($model=~/\^/) 
    {
      $$longueur--;
    }
  }
  $debug && print "Apres racine(): racine='$$racine' longueur='$$longueur'\n"
}

sub loadData
{
  my ($file, $categsTypes, $struct) = @_;

  open (DEFFILE,"<:utf8", $file) || die "Cannot open $file\n";
  
  my $lineNum = 0;
  while (<DEFFILE>)
  {
    chomp;
    $lineNum++;
    if ($_ eq "" || (/^#/) ) {next ;}
    die "Malformed definition line $lineNum in $file: $_\n" unless (/^[^;]*;[^;]*;[^;]*;[^;]*\s*$/);
    my ($type, $categs, $ficmodel, $fictable) = split(";", $_);
    my @categs = split(",", $categs);
    foreach my $categ (@categs)
    {
      if (! defined $$categsTypes{$categ} )
      {
        my @types;
        $$categsTypes{$categ} = \@types;
      }
      push @{$$categsTypes{$categ}}, $type;
    }
    my (@modelentries, %models, @modelstab);

    my $nbmodel = loadModels($ficmodel, \%models, \@modelstab);
    print "Got $nbmodel $type modelword\n";

    my (@models,@third,@fourth,%direct,%modelsdata);

    my $nbtable = loadTable($type, $fictable, \%modelsdata);
    print "Got $nbtable $type table elems\n";
    
    my %nomtabTab;
    $nomtabTab{"models"} = \%models;
    $nomtabTab{"modelsdata"} = \%modelsdata;
    $nomtabTab{"modelstab"} = \@modelstab;
  
    $$struct{$type} = \%nomtabTab;
  }
  close DEFFILE;
}

sub loadModels
{
  my ($dicfile, $models, $modelstab) = @_;
  print ("Chargement de $dicfile\n");
  open (FILEDIC,"<:utf8", $dicfile) || die "Cannot open $dicfile\n";
  
  my $lineNum = 0;
  while (<FILEDIC>)
  {
    chomp;
    if ($_ eq "" || (/^#/) ) {next;}
    $lineNum++;
    die "Malformed model line $lineNum in $dicfile: $_\n" unless (/^([^;]*);([^;]*);\s*$/);
    
    push @$modelstab, $1;
    $$models{$1} = $2;
  }
  close (FILEDIC);
  return $lineNum;
}
    
sub loadTable
{
  # third = temps pour verbe, genre pour autres
  # fourth = personne pour verbe, nombre pour autres
  my ($type, $tablefile, $modelsdata) = @_;
  print ("Chargement de $tablefile\n");
  open (FILETABLE,"<:utf8", $tablefile) || die "Cannot open $tablefile\n";

  my $oldmodel = "";
  my $lineNum = 0;
  my $ok = 1;
  my @maps; 
  while (<FILETABLE>) 
  {
    chomp;
    if ($_ eq "" || (/^#/) ) {next;}
    $lineNum++;
    if ($_ !~ /^([^;]*);([^;]*);([^;]*);([^;]*)$/)
    {
      print STDERR "Malformed table line $lineNum in $tablefile: $_\n";
      $ok = 0;
      next;
    }
    my $modelword = $1 ;
    my $formes = $2;
    my $thirds = $3;
    my $fourths = "";
    if (/^([^;]*);([^;]*);([^;]*);([^;]+;?)\s*$/)
    {
      if ($type ne "verbe")
      {
        print STDERR "Malformed non-verb table line $lineNum in $tablefile (should have 3 columns):\n$_\n\n";
        $ok = 0;
        next;
      }
      $fourths = $4;
    }
#    push @$models, $modelword;
    if ($oldmodel ne $modelword && $oldmodel ne "")
    {
      my @modelwordmaps = @maps;
      $debug && print "fixing modelsdata{$oldmodel}\n";
      $$modelsdata{$oldmodel} = \@modelwordmaps;
      @maps = ();
    }
    $oldmodel = $modelword;
    my %map;
    $map{"forme"} = $formes;
    $map{"third"} = $thirds;
    $map{"fourth"} = $fourths;
    $debug && print "forme='$formes' third='$thirds' fourth='$fourths'\n\n";
    push @maps, \%map;
  }
  my @modelwordmaps = @maps;
  $debug && print "fixing modelsdata{$oldmodel}\n";
  $$modelsdata{$oldmodel} = \@modelwordmaps;
  close (FILETABLE);
  die "Had problems reading $tablefile\n"  unless ($ok);
  return $lineNum;
}

sub loadExcludes
{
  my ($excludesfile, $excludes) = @_;
  print ("Chargement du fichier d'exclusions $excludesfile\n");
  if ( -f $excludesfile )
  {
    open (EXCLUDES,"<:utf8", $excludesfile) 
        || die "Cannot open $excludesfile\n";
    while (<EXCLUDES>)
    {
      chomp;
      $$excludes{$_}=1;
    }
    close EXCLUDES;
  }
  else {
      print STDERR "file $excludesfile not found\n";
  }
}

sub printOutput {
    my ($line)= @_;
    if ( ! exists $excludes{$line} )
    {
        print OUTFILE ("$line\n");
    }
    else 
    {
        print FILETRACE "Excluding line: $line\n";
    }
}
