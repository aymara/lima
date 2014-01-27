#!/usr/bin/perl

die "need two args (training set and tagging set)" unless @ARGV>=2;
my ($train, $tag)=@ARGV;

#######
# Tag par défaut: 

my $def_tag='L_NOM_COMMUN';

#######

#TRAINING
open (TRAIN, "sort $train | sed 's/^/###vvv###/g' | uniq -c | sort -rn|");
open (DICO, ">MFT-DICO");
my %dico = ();
while(<TRAIN>){
  chomp;
  if (/([0-9]+)\s+###vvv###(.+)\s+(.+)/){
    print DICO "$2\t$3\n";
    }
}
close(TRAIN);
close(DICO);

open (TRAIN, "<MFT-DICO");
while(<TRAIN>){
  chomp;
  if (/(.+)\s+(.+)/){
    $dico{$1}=$2 unless (exists $dico[$2]);
    }
}
close(TRAIN);
system("rm MFT-DICO");

# TAGGING
open(TAG, $tag);
my $cpt=0;
my $true=0;
while(<TAG>){
  chomp;
  if (/(.+)\t(.+)/){
    $cpt++;
    if (exists $dico{$1}) {
      #print "$1\t$dico{$1}\n";
      #print "$2  $dico{$1}\n";
      $true++ if ($2 eq $dico{$1});
    }
    else {
      #print "$1\t$def_tag\n";
      print "$2  $def_tag\n";
      $true++ if ($2 eq $def_tag); 
    }
  }
}
close(TAG);

print "Score MFT: ". sprintf ("%0.2f", $true*100/$cpt)."\% \n";
