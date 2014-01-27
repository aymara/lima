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
#!/usr/bin/perl -s
#
# Perl Program created by besancon on Mon Nov  8 2004
# Version : $Id$ 

# Help mode
if ($main::h || $main::help) {
    print <<EOF;
usage test-positions.pl [-noAnalysis] [files]
  if files are indicated, test only on these, otherwise test on *.xml
  in data dir

  -noAnalysis: do not launch analysis, use existing result
  -datadir=.. : indicate the directory containing data \(default is 'data'\)
EOF
    exit;
}

use strict;

# default value for data dir
$main::datadir="data" unless $main::datadir;

my (@files,$file);

if (@ARGV==0) {
    opendir(DIR,$main::datadir);
    @files=map {$_=$main::datadir."/".$_;} grep { /.xml$/; } readdir(DIR);
    closedir(DIR);
}
else {
    @files=@ARGV;
}

print join(" ",@files)."\n";

my (%RefPositions);
my $nbErrorsFile=0;
foreach $file (@files) {
    if (! -e "$file.pos.ref") {
        print STDERR "no reference position file for $file: ignored\n";
        next;
    }
    undef %RefPositions;
    next unless &readRefPositions("$file.pos.ref",\%RefPositions);
    
    if ($main::noAnalysis) {
        if (! -e "$file.bin") {
            print STDERR "no existing result for $file: ignored\n";
            next;
        }
    }
    else {
        system("analyzeXmlDocuments --language=fre $file");
    }

    my $nbErrors=0;
    open(FBOW,"readBowFile --output-format=xml $file.bin |");
    while (<FBOW>) {
        if (m%<bowToken id="[^\"]*" lemma="([^\"]*)" category="[^\"]*" position="([^\"]*)" length="([^\"]*)"/>%) {
            my $lemma=$1;
            my $pos=$2;
            if (! exists ${$RefPositions{$lemma}}{$pos}) {
                print "$file:position of \"$lemma\" is $pos, should be ".
                    join(",",sort { $a <=> $b } 
                         keys %{$RefPositions{$lemma}})."\n";
                $nbErrors++;
            }
        }
    }
    close(FBOW);
    if ($nbErrors) {
        print "$file: $nbErrors errors found\n";
        $nbErrorsFile++;
    }
    else {
        print "$file: OK\n";
    }
}
print "Errors in $nbErrorsFile file out of ".(scalar @files)."
";

sub readRefPositions {
    my ($file,$RefPositions)=@_;
    local ($_);
    if (! open(FREF,$file)) {
        print STDERR "cannot open file $file\n";
        return 0;
    }
    while (<FREF>) {
        chop;
        split;
        ${$$RefPositions{$_[0]}}{$_[1]}=1;
    }
    close(FREF);
    return 1;
}
