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
#
# my OUT files: AH
#use utf8;

my $pwd = shift @ARGV;

#open (FILEOUT,">:utf8","formes-de.txt");   
# lecture des tables de conjugaison
open (FILEDICVB,"<:utf8","$pwd/dicvb.txt");
open (FILETABLEVDE,"<:utf8","$pwd/tablevde.txt");
open (FILETRACE,">:utf8","trace.txt");

my $i=0;
my (@v, @fin);

while (<FILEDICVB>) 
{
    @table = split (/;/,$_);
    #$vmiroir[$i]=$table[0];
    $v[$i]=$table[1];
    
    $fin[$i]=$table[2];
    $i++;
}

my $nbmodel=@fin;
$i=0; 
my $oldmodel="";
my (@vmodel, %direct, @forme, @temps, @personne);
while (<FILETABLEVDE>) 
{
    
    @table = split (/;/,$_);
    $vmodel[$i]=$table[0];
    if ($oldmodel ne $vmodel[$i]){$direct{$vmodel[$i]}=$i;}
    
    $oldmodel=$vmodel[$i];
    $forme[$i]=$table[1];
    $temps[$i]=$table[2];
    $personne[$i]=$table[3];
    #chop($personne[$i]);
    $i++;
}
my $nbtable=@forme;

close (FILETABLEVDE);
close (FILEDICVB);
print ("nbtable=",$nbtable," verbe\n");

# lecture des tables des adjectifs

open (FILEDICADJ,"<:utf8","$pwd/adjectif-model2.txt");
open (FILETABLEAFR,"<:utf8","$pwd/adjectif-tablede2.txt");

$i=0;
my (@va, @fina);
while (<FILEDICADJ>) 
{
    @table = split (/;/,$_);
    $va[$i]=$table[1]; chop($va[$i]);
    $fina[$i]=$table[0];
    #print FILETRACE ("-A-",$fina[$i],";",$va[$i],";\n");
    $i++;
}
my $nbmodeladj=@fina;

$i=0; 
$oldmodel="";
my (@vmodela, @formea, @gn);
while (<FILETABLEAFR>) 
{
    @table = split (/;/,$_);
    $vmodela[$i]=$table[0];
    if ($oldmodel ne $vmodela[$i]){$directa{$vmodela[$i]}=$i; }
    #print FILETRACE ($vmodela[$i],";",$directa{$vmodela[$i]},";\n");
    $oldmodel=$vmodela[$i];
    $formea[$i]=$table[1];
    $gn[$i]=$table[2];
    $i++;
}
#$nbtableadj=@formea;
close (FILEDICADJ);
close (FILETABLEAFR);

# lecture des tables des noms

open (FILEDICNOM,"<:utf8","$pwd/nom-modelde.txt");
open (FILETABLENDE,"<:utf8","$pwd/nom-tablede.txt");
print ("adjectif\n");

$i=0;
my (@vn, @finn);
while (<FILEDICNOM>) 
{
    @table = split (/;/,$_);
    $vn[$i]=$table[1];
    $finn[$i]=$table[0];
    #print FILEOUT ("-N-",$finn[$i],";",$vn[$i],";\n");
    $i++;
}
my $nbmodelnom=@finn;

$i=0; $oldmodel="";
my (@vmodeln, @formen,@gnn);
while (<FILETABLENDE>) 
{
    chomp;
    
    @table = split (/;/,$_);
    $vmodeln[$i]=$table[0];
    if ($oldmodel ne $vmodeln[$i]){$directn{$vmodeln[$i]}=$i; }
    #print FILEOUT ($i,";",$vmodeln[$i],";",$directn{$vmodeln[$i]},";\n");
    $oldmodel=$vmodeln[$i];
    $formen[$i]=$table[1];
    $gnn[$i]=$table[2]; 
    $i++;
}
#$nbtablen=@formen;
close (FILEDICNOM);
close (FILETABLENDE);
print ("noms\n");

if (! -e "formes")
{
  mkdir "formes";
}

traitementFichier("$pwd/noms.01.txt", "formes/formes-de-nom.txt");
traitementFichier("$pwd/adjectifs.01.txt", "formes/formes-de-adjectifs.txt");
traitementFichier("$pwd/nc_mesures.01.txt", "formes/formes-de-mesures.txt");
traitementFichier("$pwd/verbes.01.txt", "formes/formes-de-verbes.txt");

open FILEEND, ">:utf8", "formes-ger.txt" || die "cannot create formes-ger.txt";
close FILEEND;

sub traitementFichier
{
  my ($filein,$fileout) = @_;
  open FILEIN, "<:utf8",$filein or die "cannot open $filein";  
  open FILEOUT, ">:utf8","$fileout" or die "cannot create $fileout";
  my $fileInLineNum = 0;
  while (<FILEIN>)
  {
    my $fileInLine = $_;
    $fileInLineNum++;
    my @table = split (/;/,$fileInLine);
    my $lem=$table[2];
    my $vg=$table[3];
    #print FILEOUT ($vg,"\n");
    my $norm=$table[4]; 
    my $code=$table[5];
    my $domaine=$table[6];
    
    $pospart=index($lem,"\+");
    $lemverbe=$lem;
    if ($pospart == -1) {$pospart=0;$particule="";}
    else {$particule = substr($lem,0,$pospart);$lem=substr($lem,$pospart+1); } 
    if ($lem=~/\%/) {$lem=~s/\%$//; $partnonsep=1;} 
    else {$partnonsep=0;}
    
    $lemverbe=~s/\%//; $lemverbe=~s/\+//;
    # faire un miroir du lemme
    $leminv="";
    for ($j=length($lem)-1;$j>=0;$j=$j-1) 
    {
      # print FILEOUT (length($vdir[$i]),";",$vdir[$i],";",$j,";",$vinv[$i],";",substr($vdir[$i],$j,1),";\n");
      $leminv=$leminv . substr($lem,$j,1);
    }
    # transcoder la vg pour macrocatégorie et prendre les autres informations
    # traitement des noms à ne pas fléchir
    #if ($lem=~/-/) {print FILEOUT ($lem,";",$vg,";;;",$lem,";",$norm,";\n");}
    #elsif ($lem=~/[A-Z].*[A-Z]/) {print FILEOUT ($lem,";",$vg,";;;",$lem,";",$norm,";\n");}
    #if ($lem=~/[A-Z]\.*[A-Z]/) {print FILEOUT ($lem,";",$vg,";;;",$lem,";",$norm,";\n");}
    
    
    
    # traitement des verbes 
    # =====================
    # prévoir les verbes pronom : $vg eq "verbe pronominal" || $vg eq "verbe-pronom" || $vg eq "verbe pronominal-pronom"
    
    # !!! vérifier bonne catégorie en entrée
    if ($vg eq "verbe transitif" || 
        $vg eq "verbe intransitif" || 
        $vg =~/^verbe auxiliaire|^verbe de copule|^verbe modal|^verbe de prédication/ || 
        $vg eq 'verbe d\'état' || 
        $vg eq "verbe impersonnel" || 
        $vg eq "verbe transitif indirect" || 
        $vg eq "verbe") 
    {
      # recherche séquentielle dans fin; on compare fin au début du mot miroir du malongueur de fin penser au $
      #print FILEOUT ("*",$lem,";",$leminv,";",$nbmodel,"\n");
      
      # +AH: garder les formes avec '+' pour former un infinitif avec "zu" interne: eg. weiter+kommen, inf. weiterzukommen
      my $infzu; 
      if ($table[2] =~ /\+/) 
      {
          $infzu = $table[2]; 
          $infzu =~ s/\%//;
          $infzu =~ s/\+/zu/;
          print FILEOUT ("$infzu;$vg;infinitif avec zu;;$lemverbe;$norm;$domaine;\n");
      }
  
      # + AH: ajouter les formes "verbe dans composition": à partir de l'infinitif des verbes PLEINS
      
      if ($vg eq "verbe") 
      {
          my $vcomp = $lemverbe;
          $vcomp =~ s/e?n$//;
          print FILEOUT ("$vcomp;$vg;dans composition;;$lemverbe;$norm;$domaine;\n");
      }
      
      for ($j=0; $j<$nbmodel; $j++) 
      {
          #print FILEOUT ("**",$leminv,";",$masque,";",$fin[$j],"\n");
          $masque="\^" . $fin[$j];
          if ($leminv=~/$masque/) {goto trouve;}
      }
      trouve:
      #print FILEOUT ("***",$v[$j],";",$direct{$v[$j]},"\n");
      $verbe=$v[$j];
      if ( ! defined $verbe )
      {
        print FILETRACE "Unable to handle $filein line $fileInLineNum : $fileInLine\n";
        next; 
      }
      # détermination de la partie du lem à conserver
      $llem=length($lem)-1;
      $lverbe=length($verbe)-1;
      while (substr($lem,$llem) eq substr($verbe,$lverbe) && $llem >= 0 && $lverbe >=0) 
      {
        $llem--; $lverbe--;
      }
      $racine=substr($lem,0,$llem+1);
      #le premier = donne le mot modele dans $v
      #$commun=$fin[$j];
      
      $j=$direct{$v[$j]};
  
      if ( ! defined $j )
      {
        print FILETRACE "Unable to handle $filein line $fileInLineNum : $fileInLine (direct not found)\n";
        next; 
      }
      if ( ! defined $vmodel[$j] )
      {
        print FILETRACE "Unable to handle $filein line $fileInLineNum : $fileInLine (vmodel not found)\n";
        next; 
      }
      while ($verbe eq $vmodel[$j]) 
      {
          # print FILEOUT ($lfin,";",$racine,";",$lracine_modele,"\n");
          # 
          if (length($forme[$j]) > 0)
          {
            #on cherche la partie à conserver du lemme en comparant la longueur mot modèle à celui de fin
            #la différence est la longueur à garder.
            #pour chaque ligne ou le mot modèle est présent on construit la forme en prenant la partie fixe et en ajoutant la forme modèle amputée de sa 
            #forme fixe
            #écrire dans le fichier de sortie la forme, la vg , les propriétés, le lemme  
            $forme1=$forme[$j];
            # traitement des variantes séparées par "/" ( attention une seule variante possible)
  
            if ($forme[$j]=~/\//) 
            {
                @table=split("\/",$forme[$j]); 
    
                if ($temps[$j] eq "participe passé" && $forme1=~/^ge/ ) 
                { 
                  if ( $partnonsep == 1) {$forme_sortie=$particule . $racine . substr($forme1,$lverbe+3);}
                  else {$forme_sortie=$particule . "ge" . $racine . substr($table[0],$lverbe+3);}
                }
                else {$forme_sortie=$particule . $racine . substr($table[0],$lverbe+1);}
                
                print FILEOUT ($forme_sortie,";",$vg,";",$temps[$j],";",$personne[$j],";",$lemverbe,";",$norm,";",$domaine,";\n");
                $forme1=$table[1];
            }
          
            if ($temps[$j] eq "participe passé" && $forme1=~/^ge/) 
            {
                if ( $partnonsep == 1) {$forme_sortie=$particule . $racine . substr($forme1,$lverbe+3);}
                else { $forme_sortie=$particule . "ge" . $racine . substr($forme1,$lverbe+3);}
            }
            else 
            {
                #print FILEOUT ("****",$racine,";",$forme1,";", substr($forme1,$lverbe+1),";",$lverbe,"\n");
                $forme_sortie=$particule . $racine . substr($forme1,$lverbe+1);
            }
            #print FILEOUT ("**",$lem,";",substr($lem,0,3),"\n");
            print FILEOUT ($forme_sortie,";",$vg,";",$temps[$j],";",$personne[$j],";",$lemverbe,";",$norm,";",$domaine,";\n");
          }
          $j++
        }
    }
    
    # traitement des adjectifs
    # =====================
    elsif ($vg eq "adjectif qualificatif" || 
            $vg eq "adjectif numéral ordinal" || 
            $vg eq "adjectif participe présent" || 
            $vg eq "adjectif participe passé") 
    {
      # recherche séquentielle dans fin; on compare fin au début du mot miroir du malongueur de fin penser au $
      #print FILEOUT ("*",$lem,";",$vg,";",$leminv,";",$nbmodel,"\n");
      for ($j=0; $j<$nbmodeladj; $j++) 
      {
          $masque="\^" . $fina[$j];
          #print FILETRACE ($leminv,";",$masque,"\n");
          if ($leminv=~/$masque/) {goto trouve2;}
      }
    
      trouve2:
      #print FILEOUT ($va[$j],";",$fina[$j],";",$directa{$va[$j]},";\n");
      #le premier = donne le mot modele dans $v
      $commun=$fina[$j];
      $adjectif=$va[$j];
      $j=$directa{$va[$j]};
      #print FILETRACE ($commun,";",$adjectif,";",$j,";\n");
      $lfin=length($commun); if (chop($commun) eq "\$") {$lfin=$lfin-1;}
      $racine=substr($lem,0,length($lem)-$lfin);
      $lracine_modele=length($adjectif)-$lfin;
      if ($adjectif=~/\^/) {$lracine_modele=$lracine_modele - 1;}
      #if ($formea[$j]=~/\*/) {$lracine_modele=$lracine_modele + 1;}
      #print FILETRACE ($lfin,";",$racine,";",$lracine_modele,";",$formea[$j],";\n");
      while ( (defined $vmodela[$j]) && ($adjectif eq $vmodela[$j]) )
      {
        #print FILETRACE ($lfin,";",$racine,";",$lracine_modele,"\n");
        if (length($formea[$j]) > 0)
        {
          #on cherche la partie à conserver du lemme en comparant la longueur mot modèle à celui de fin
          #la différence est la longueur à garder.
          #pour chaque ligne ou le mot modèle est présent on construit la forme en prenant la partie fixe et en ajoutant la forme modèle amputée de sa 
          #forme fixe
          #écrire dans le fichier de sortie la forme, la vg , les propriétés, le lemme  
          $forme1=$formea[$j];
          # traitement des variantes séparées par "/" ( attention une seule variante possible)
          if ($formea[$j]=~/\//) 
          {
            @table=split("\/",$formea[$j]);
            
            $forme_sortie=$racine . substr($table[0],$lracine_modele);
            print FILEOUT ($forme_sortie,";",$vg,";;",$gn[$j],";",$lem,";",$norm,";",$domaine,";\n");
            $forme1=$table[1];
          }
          $forme_sortie=$racine . substr($forme1,$lracine_modele);
          print FILEOUT ($forme_sortie,";",$vg,";;",$gn[$j],";",$lem,";",$norm,";",$domaine,";\n");
        }
        $j++
      }
    }
    
    # traitement des noms 
    # Attention traitement particulier pour l'allemand
    # ======================================
    elsif ($vg =~/^nom masculin/ || 
          $vg =~/^nom neutre|^nom commun.*unite de mesure/ || 
          $vg =~/^nom f[ée]minin/ || 
          $vg eq "annp masculin" || 
          $vg eq "annp féminin" || 
          $vg eq "unité de mesure masculin" || 
          $vg eq "unité de mesure féminin" || 
          $vg eq "nom" || 
          $vg eq "nom adjectif") 
    { 
      #print FILETRACE "<$lem;$vg>\n";
      # recherche séquentielle dans fin; on compare fin au début du mot miroir du malongueur de fin penser au $
      #print FILEOUT ("*",$lem,";",$leminv,";",$nbmodel,"\n");
      # traitement des mots irreguliers
      
      if ($code eq "irr") 
      {
        $code=$lem; $j=0;
        $commun=$lem;
        
        while ($j< @vmodeln && $vmodeln[$j] ne $lem) 
        {
          $j++;
        }
        #print FILEOUT ($j,";",$vmodeln[$j],"\n");
      }
      # remplacer l'étoile par un vrai mot.
      elsif ($code eq "adj") 
      {
        $code="Abgebrannt"; $j=0;
        $commun=$lem;
        while ($j< @vmodeln && $vmodeln[$j] ne $lem) 
        {
          $j++;
        }
        #print FILEOUT ($j,";",$vmodeln[$j],"\n");
      }
      else 
      {
        for ($j=0; $j<$nbmodelnom; $j++) 
        {
          if ( $finn[$j] eq $code) {goto trouve3;}
          #$masque="\^" . $finn[$j];
          #print FILETRACE ("+",$j,";",$leminv,";",$masque,";\n");
          #print FILEOUT ($leminv,";",$masque,"\n");
          #if ($leminv=~/$masque/) {goto trouve3;}
        }
        trouve3:
        #print FILEOUT ("*+",$j,";",$code,";",$vn[$j],";",$finn[$j],";",$directn{$vn[$j]},";\n");
        #le premier = donne le mot modele dans $v
        $commun=$vn[$j];
        if ( ! defined $commun )
        {
          print FILETRACE "Unable to handle $filein line $fileInLineNum : $fileInLine\n";
          next; 
        }
        #$nom=$vn[$j];
        $j=$directn{$vn[$j]};
        #print FILETRACE ($commun,";",$adjectif,";",$j,";\n");
      }
      #détermination de la partie commune
      $llem=length($lem); $lmod=length($commun);
      while ($llem >0 && $lmod > 0 && substr($lem,$llem-1,1) eq  substr($commun,$lmod-1,1)) 
      {
        $llem--; $lmod--;
      }
      #print FILEOUT ($lem,";",$llem,";",$commun,";",$lmod,"\n");
      
      #$lfin=length($commun); if (chop($commun) eq "\$") {$lfin=$lfin-1;}
      #$racine=substr($lem,0,length($lem)-$lfin);
      #$lracine_modele=length($nom)-$lfin;
      #if ($nom=~/\^/) {$lracine_modele=$lracine_modele - 1;}
      #if ($formen[$j]=~/\*/) {$lracine_modele=$lracine_modele + 1;}
      #print FILEOUT ($lfin,";",$racine,";",$lracine_modele,";",$formea[$j],";\n");
      while ((defined $j) && (defined $vmodeln[$j]) && ($commun eq $vmodeln[$j])) 
      { 
        #print FILETRACE "  <$lem;$gnn[$j]>\n";
        
        $forme=substr($lem,0,$llem) . substr($formen[$j], $lmod, length($formen[$j])-$lmod);
        #traitement du umlaut
        if (substr($forme, length($forme)-1, 1) eq "*") 
        {
          $forme=substr($forme, 0,length($forme)-1);
          $kk=length($lem)-1;
          # première voyelle
          while (substr($forme,$kk,1) ne "a" && substr($forme,$kk,1) ne "e" && substr($forme,$kk,1) ne "i" && 
                substr($forme,$kk,1) ne "o" && substr($forme,$kk,1) ne "u" && $kk >= 0) { $kk--;}
          #est-ce une diphtongue
          
          if (substr($forme,$kk-1,1)=~/[aouAOU]/) { substr($forme,$kk-1,1)=~tr/aouAOU/äöüÄÖÜ/;}
          # cas standard
          elsif ( substr($forme,$kk,1) ne "e" ) { substr($forme,$kk,1)=~tr/aouAOU/äöüÄÖÜ/;}
          else 
          {
            $kk--;
            
            while (substr($forme,$kk,1) ne "a" && substr($forme,$kk,1) ne "e" && substr($forme,$kk,1) ne "i" && 
              substr($forme,$kk,1) ne "o" && substr($forme,$kk,1) ne "u" && $kk >= 0) { $kk--;}
            
            if ( $kk >= 0) 
            { 
              #diphtonque
              if ($kk>0 && substr($forme,$kk-1,1)=~/[aouAOU]/) { substr($forme,$kk-1,1)=~tr/aouAOU/äöüÄÖÜ/;}
              else {substr($forme,$kk,1)=~tr/aouAOU/äöüÄÖÜ/;}
            }
          }
        }
        #print FILEOUT ($lem,";",substr($lem,0,$llem),";",$formen[$j],";",substr($formen[$j], $lmod, length($formen[$j])-$lmod),";\n");
        
        #print FILETRACE ($lfin,";",$racine,";",$lracine_modele,"\n");
        #if (length($formen[$j]) > 0){
        #on cherche la partie à conserver du lemme en comparant la longueur mot modèle à celui de fin
        #la différence est la longueur à garder.
        #pour chaque ligne ou le mot modèle est présent on construit la forme en prenant la partie fixe et en ajoutant la forme modèle amputée de sa 
        #forme fixe
        #écrire dans le fichier de sortie la forme, la vg , les propriétés, le lemme  
        #$forme1=$formen[$j];
        # traitement des variantes séparées par "/" ( attention une seule variante possible)
        #if ($formen[$j]=~/\//) {
        #@table=split("\/",$formen[$j]);
        
        #$forme_sortie=$racine . substr($table[0],$lracine_modele);
        print FILEOUT ($forme,";",$vg,";;",$gnn[$j],";",$lem,";",$norm,";",$domaine,";\n");
        #$forme1=$table[1];
        #                        }
        #$forme_sortie=$racine . substr($forme1,$lracine_modele);
        #print FILEOUT ($forme_sortie,";",$vg,";;",$gnn[$j],";",$lem,";",$norm,";\n");
        #                }
        $j++;
        #print FILEOUT ("***",$commun,";",$vmodeln[$j],"\n");
      }
    }
    # traitement des FUG
    
    #    $fug="";
    #   if ($lem eq "Maus") {$forme = "Mause"; $fug="O";}
    #    elsif ($lem eq "Laus") {$forme = "Lause"; $fug="O";}
    #    elsif ($lem eq "Herz") {$forme = "Herzens"; $fug="O";}
    #    elsif ($lem eq "Schmerz") {$forme = "Schmerzens"; $fug="O";}
    #    elsif ($lem eq "Mensch") {$forme = "Menschens"; $fug="O";}
    #    elsif ($lem eq "Frau") {$forme = "Frauens"; $fug="O";}
    #    elsif ($lem eq "Narr") {$forme = "Narrens"; $fug="O";}
    #    elsif ($lem eq "Seele") {$forme = "Seelens"; $fug="O";}
    #    elsif ($lem eq "Arbeit") {$forme = "Arbeits"; $fug="O";}
    #    elsif ($lem eq "Hilfe") {$forme = "Hilfs"; $fug="O";}
    #    elsif ($lem eq "Miete") {$forme = "Miets"; $fug="O";}
    #    elsif ($lem eq "Geschichte") {$forme = "Geschichts"; $fug="O";}
    
    # MODIF AH: imprimer genre avec fug
    #if ($fug eq "O") {print FILEOUT ($forme,";","fug $vg",";;","",";",$lem,";",$norm,";",$domaine,";\n");}
    # old:
    #  if ($fug eq "O") {print FILEOUT ($forme,";","fug",";;","",";",$lem,";",$norm,";",$domaine,";\n");}
  
    elsif (($vg=~/féminin/ || $vg=~/nom$/) && 
                ($lem=~/heit$/ ||
                  $lem=~/keit$/ ||
                  $lem=~/sal$/ ||
                  $lem=~/schaft$/ ||
                  $lem=~/ung$/ ||
                  $lem=~/ion$/ ||
                  $lem=~/ität$/ ||
                  $lem=~/at$/ ||
                  $lem=~/ut$/ ||
                  $lem=~/fahrt$/ ||
                  $lem=~/flucht$/ ||
                  $lem=~/furcht$/ ||
                  $lem=~/macht$/ ||
                  $lem=~/sicht$/ ||
                  $lem=~/sucht$/ ||
                  $lem=~/nacht$/ ||
                  $lem=~/zucht$/ ||
                  $lem=~/thek$/ ||
                  $lem=~/falt$/ ||
                  $lem=~/ld$/ )) 
    {
      # old:
      #  $forme=$lem . "s"; $vg="fug";$gnn=""; print FILEOUT ($forme,";",$vg,";;",$gnn[$j],";",$lem,";",$norm,";",$domaine,";\n");}
    }
    #mots non fléchissibles
    else 
    {
        print FILEOUT ($lem,";",$vg,";;;",$lem,";",$norm,";\n");
        print FILETRACE "Mot non fléchi dans $filein, ligne $fileInLineNum : $fileInLine\n";
        #print FILETRACE "  mot non fléchi: <$lem;$vg>\n";
    }
  }
  
  close FILEIN;
  close FILEOUT;
}  
