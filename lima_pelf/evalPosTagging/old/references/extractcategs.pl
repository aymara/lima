#! /usr/bin/perl -s

# vérification de toutes les categories
open(REF,"$ARGV[0]") || die ("impossible d ouvrir le fichier $ARGV[1]");

$i=0;
while ($ligne=<REF>)
{
    $i++;
    @infos=split(/\|/,$ligne);
	chomp($infos[2]);
	$infos[2]=~ s/^\s*//;
        print "$infos[2]\n";
}
