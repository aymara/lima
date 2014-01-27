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
# Perl Program created by besancon on Tue Jun 28 2005
# Version : $Id$ 

# Help mode
if ($main::h || $main::help) {
    print <<EOF;
usage create-index-html.pl [options]
EOF
    exit;
}

use strict;

my $dir=".";

open(FOUT,">index.html") || die "cannot create file index.html";

$main::header="$dir/index-html-header" unless $main::header;
$main::footer="$dir/index-html-footer" unless $main::footer;

opendir(DIR,$dir);
my @files=grep {-f "$dir/$_"} readdir(DIR);
closedir(DIR);

my (@xmlfiles,%htmlfiles);

@xmlfiles=grep {/.xml$/} @files;
map { $htmlfiles{$_}=1} grep { /\.html$/ } @files;

&printHeader();

print FOUT "<h2>Spécifications et Documentations Utilisateur</h2>\n";
print FOUT 'Pour commencer, lire <a href="'.$dir.'/docTL.html">'.
        &getTitle("docTL.xml").'</a>'."<br/><br/>\n";
print FOUT 'Pour apprendre la syntaxe des règles de construction d&quot;automates utilisées par de nombreux modules, lire <a href="'.$dir.'/specif_regles.html">'.
        &getTitle("specif_regles.xml")."</a>.<br/><br/>\n";

print FOUT "Concernant les expressions idiomatiques, on lira:\n";
print FOUT "<ul>\n";
      print FOUT '  <li><a href="'.$dir.'/compile-idiomatic.html">'.
          &getTitle("compile-idiomatic.xml").'</a></li>'."\n";
      print FOUT '  <li><a href="spec-idiomatic.html">'.
          &getTitle("spec-idiomatic.xml").'</a></li>'."\n";
print FOUT "</ul>\n";

print FOUT "Les autres documents ci-dessous peuvent aussi être utiles&nbsp;:\n";
print FOUT "<ul>\n";

my ($xmlfile,$htmlfile,$linkfile);
foreach $xmlfile (@xmlfiles) {
    if ($xmlfile ne "docTL.xml" &&
        $xmlfile ne "specif_regles.xml" &&
        $xmlfile ne "spec-idiomatic.xml" &&
        $xmlfile ne "compile-idiomatic.xml" ) {
      $htmlfile=$xmlfile;
      $htmlfile=~s/.xml/.html/;
      if (exists $htmlfiles{$htmlfile}) {
          $linkfile=$htmlfile;
      }
      else {
          $linkfile=$xmlfile;
      }
      print FOUT '  <li><a href="'.$dir.'/'.$linkfile.'">'.
          &getTitle($xmlfile).'</a></li>'."\n";
  }
}

print FOUT "</ul>\n";

print FOUT "<h2>Documentations du Code</h2>\n";
print FOUT "<ul>\n";
if (-d "$dir/html") {
    print FOUT '  <li><a href="'.$dir.'/html/index.html">'.
        'Documentation des APIs (générées par Doxygen)</a></li>'."\n";
}
print FOUT "</ul>\n";

&printFooter();

sub printHeader() {
    if (-e "$main::header") {
        print FOUT `cat $main::header`;
        return;
    }
    # default header
    print FOUT <<EndOfBuffer
<?xml version="1.0" encoding="utf-8"?>
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<title>Documentation of Linguistic Processing Modules</title>
</head>
<body bgcolor="#ffffff">
<h1>Documentation of Linguistic Processing Modules</h1>
EndOfBuffer
;
}

sub printFooter() {
    if (-e "$main::footer") {
        print FOUT `cat $main::footer`;
        return;
    }
    # default header
    my $date=`date +"%d/%m/%Y %H:%M"`;
    chop $date;
    print FOUT <<EndOfBuffer
<p/>
<hr/>
<p>Last modified: $date</p>
</body>
</html>
EndOfBuffer
;
}

sub getTitle() {
    my ($file)=@_;
    if (!open(FILE,$file)) {
        print STDERR "Cannot open file $file: ignored...\n";
        return $file;
    }
    # stop at first <title> found
    my ($title,$intitle)=("",0);
    while (<FILE>) {
        if (m%<title>(.*)</title>%) {
            $title=$1;
            last;
        }
        if (m%<title>(.*)%) {
            $intitle=1;
            $title=$1;
        }
        elsif (m%(.*)</title>%) {
            $title.=$1;
            last;
        }
        elsif ($intitle) {
            $title.=$_;
        }
    }
    if (! $title) {
        print STDERR "Cannot find title in file $file";
        $title=$file;
    }
    close(FILE);
    return $title;
}
