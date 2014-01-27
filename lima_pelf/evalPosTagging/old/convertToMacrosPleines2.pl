#! /usr/bin/perl -s

open(TABLE,$ARGV[0]) || die ("impossible d ouvrir le fichier $ARGV[0]");

#lit la table

while ($ligne = <TABLE>)
{
    chomp($ligne);
    @data = split(/\|/,$ligne);

	$micro=$data[0];
	$macro=$data[1];

    $mapping{ $micro } = $macro;
	if ($data[2] eq "X")
	{
	    $flag{ $micro } = 1;
	}

}

#pour chaque fichier fait les remplacements
for ($i=1;$i<scalar(@ARGV);$i++)
{
    push(@allFiles,$ARGV[$i]);
}

while (scalar(@allFiles)>0)
{

    $inputFile = pop(@allFiles);
	$outputFile = "$inputFile.macropleine2";

    open(INPUT,$inputFile) || die ("impossible d ouvrir le fichier $inputFile");
    open(OUTPUT,">$outputFile") || die ("impossible d ouvrir le fichier $outputFile");

	print "process $inputFile into $outputFile\n";

	while ($ligne = <INPUT>) {

        @data = split(/\|/,$ligne);

    	for ($i=2;$i<scalar(@data);$i++)
    	{
	        chomp($data[$i]);
    	    $data[$i]=~ s/^\s*//;
    	    $data[$i]=~ s/\s*$//;

			# if empty categ then log non
			if ($flag{$data[$i]}!=1)
			{
			    while (scalar(@macros))
				{
				    pop(@macros);
				}
			    last;
			}

			$macro = $mapping{$data[$i]};
			if ($macro eq "")
			{
			    print "WARNING : micro $data[$i] has no corresponding macro !\n";
			}
			push(@macros, $macro);
	    }

		print OUTPUT "$data[0] | $data[1]";
		$lastMacro="truc";
		if (scalar(@macros)>0) {
		    sort(@macros);
		    while (scalar(@macros)>0)
		    {
    		    $current=pop(@macros);
			    if ($current eq $lastMacro)
			    {
    			    next;
			    } else {
    			    $lastMacro=$current;
				    print OUTPUT " | $current";
			    }
		    }
		} else {
		    print OUTPUT " | none";
		}
		print OUTPUT "\n";
	}
}
