#! /usr/bin/perl -s

#if (scalar($ARGV)!=2) {
#    print "Usage : showErrors.pl fichierReference fichierEvalué\n";
#	exit;
#}


open(REF,$ARGV[0]) || die ("impossible d ouvrir le fichier $ARGV[0]");
open(EVAL,$ARGV[1]) || die ("impossible d ouvrir le fichier $ARGV[1]");

&followRef;
&followEval;

while (($ligneRef ne "") && ($ligneEval ne ""))
{

    if (&RefEqEval) {

	    #print "equal\n";

	    if (scalar(@infosEval)==3) {
		    #entree non ambigue
		    if (!($infosEval[2] eq $infosRef[2]))
		    {
			    print "mismatch sur entree non ambigue\n";
				print "REF  : $ligneRef";
				print "EVAL : $ligneEval";
		    }
		} else {
		    #entree ambigue
			#$nbcat=scalar(@infosEval);
			#print "entrée ambigue ($nbcat)\n";
			$found=0;
			for ($i=2;$i<scalar(@infosEval);$i++)
			{
			    if ($infosEval[$i] eq $infosRef[2])
				{
				    # categorie trouvee
					#print "found $infosEval[$i] equals $infosRef[2]\n";
					$found=1;
					last;
				}
			}
			if (!$found) {
			    print "mismatch sur entree ambigue\n";
				print "REF  : $ligneRef";
				print "EVAL : $ligneEval";
			}
		}

        &followRef;
        &followEval;

	} else {

	    if (&RefLtEval) {
		    #print "ref < eval\n";
                    print "found only in REF : $ligneRef";
            &followRef;
		} else {
		    #print "ref > eval\n";
                    print "found only in EVAL : $ligneEval";
            &followEval;
		}
	}

}
while ($ligneRef ne "") {
	&followRef;
}
while ($ligneEval ne "") {
    &followEval;
}

sub RefEqEval {
	return (($infosRef[0]==$infosEval[0]) && (length($infosRef[1])==length($infosEval[1])));
}

sub RefLtEval {
	if ($infosRef[0]<$infosEval[0]) { return 1; }
	if ($infosEval[0]<$infosRef[0]) { return 0; }
    return (length($infosRef[1])<length($infosEval[1]));
}

sub followRef {
    $ligneRef= <REF>;
	#print "read REF : $ligneRef";
    @infosRef = split(/\|/,$ligneRef);
   	for ($i=2;$i<scalar(@infosRef);$i++)
   	{
        chomp($infosRef[$i]);
   	    $infosRef[$i]=~ s/^\s*//;
   	    $infosRef[$i]=~ s/\s*$//;
    }
}

sub followEval {
    $ligneEval= <EVAL>;
	#print "read EVAL : $ligneEval";
    @infosEval = split(/\|/,$ligneEval);
    for ($i=2;$i<scalar(@infosEval);$i++)
	{
        chomp($infosEval[$i]);
    	$infosEval[$i]=~ s/^\s*//;
    	$infosEval[$i]=~ s/\s*$//;
	}
}
