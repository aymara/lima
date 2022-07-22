#!/usr/bin/perl -s

# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

# Help mode
if ($main::h || $main::help) {
    print <<EOF;
extract a list of reflexive verbs from the list of idiomatic
expressions and store them in a file as a class used for
automaton rules
usage buildReflexiveVerbList.pl idiom-file
EOF
    exit;
}

use strict;

die "need output file for lists (-list=..)" unless $main::list;
die "need output file for rules (-rules=..)" unless $main::rules;

open(FLIST,">$main::list") 
    || die "cannot open output file \"$main::list\"";

open(FRULES,">$main::rules") 
    || die "cannot open output file \"$main::rules\"";

my (%ReflexiveVerbs);

while (<>) {
    if (/;verbe pronominal;/) {
        if (/^[^;]*;[^;]*;([^;]*);([^;]*);([^;]*);/) {
            my ($abs,$trigger,$exp)=($1,$2,$3); 
            $abs='C' if ($abs eq ""); # C(contextual) if not A(bsolute)
            if ($trigger =~ /\[V\]/) {
                if ($exp eq "s' (pp) $trigger" ||
                    $exp eq "se (pp) $trigger") {
                    $trigger =~ s/\[V\]\&?//;
                    push @{$ReflexiveVerbs{$abs}}, $trigger;
                }
                else { # rule contain more than juste se+verb
                    &addReflexiveVerbRule($abs,$trigger,$exp,1);
                }
            }
            else { # verb is not trigger
                if ($exp=~/\[V\]\&?([^ ]*) /) {
                    &addReflexiveVerbRule($abs,$trigger,$exp,0);
                }
                else { # keep rule as is
                    print STDERR "Warning: did not know how to deal ".
                        "with pronominal expression $exp\n";
                    print;
                }
            }
        }
        else {
            print;
        }
    }
#     elsif (/\(.*\)/ && /;verbe/) {
#         # separable verb expressions (prendre part)
#     }
    else {
        print;
    }
}

if (exists $ReflexiveVerbs{'C'}) {
    print FLIST "\@ReflexiveVerbs=(".
        join("\$V,\n",@{$ReflexiveVerbs{'C'}})."\$V)\n\n";
}
else {
    print STDERR "Warning: no contextual rule for pronominal verb\n";
}
if (exists $ReflexiveVerbs{'A'}) {
    print FLIST "\@AbsReflexiveVerbs=(".
        join("\$V,\n",@{$ReflexiveVerbs{'A'}})."\$V)\n\n";
}
else {
    print STDERR "Warning: no absolute rule for pronominal verb\n";
}

close(FLIST);
close(FRULES);

# add a verb rule for expression that is more complex than
# just reflexive pronoun + verb (s'en aller, s'emmeler les pinceaux)
# rule can be trigger on something else than the verb (pinceaux in
# s'emmeler les pinceaux)
sub addReflexiveVerbRule {
    my ($abs,$trigger,$exp,$triggerIsVerb) =@_;
    my ($pron,$pre,$verb,$post);
    if ($exp =~ /(s\'|se)(.*) (\[V\]\&?[^ ]*)(.*)/) {
        ($pron,$pre,$verb,$post)=($1,$2,$3,$4);
    }
    # remove possible space at beginning of pre and post
    $pre=~s/^ *//;
    $post=~s/^ *//;
    
    #print STDERR "--[pron=$pron|pre=$pre|verb=$verb|post=$post]--\n";
    
    $pre=~s/\(pp\)//; # remove (pp) => rewritten in rule
    $post=~s/\(D\)/[(\$ADV?)]/g; 

    $pre=~s/^ *//;
    $post=~s/^ *//;

    if ($triggerIsVerb) {
        $trigger =~ s/\[V\]//;
        $trigger.='$V';

        # cannot determine if possible pronouns,auxiliaries and adverbs
        # are before or after $pre (s'en aller => je m'en suis allé,
        # s'entre-déchirer => je me suis entre-déchiré)
        # => set both possibilities
        print FRULES
            "$trigger".
            ':$PRON-CLR';
        if ($pre ne "") {
            print FRULES "(($pre ".
            '[$PRON?] [(@AuxiliaireEtre ($ADV){0-2})?])'.
            '|([$PRON?] [(@AuxiliaireEtre ($ADV){0-2})?] '.
            "$pre))";
        }
        else {
            print FRULES 
                '([$PRON?] [(@AuxiliaireEtre ($ADV){0-2})?])';
        }
        print FRULES ':'.
            "$post".
            ':ABS_IDIOM$V---p---:'."\n".
            '+AgreementConstraint(trigger.1,left.1,"NUMBER")'."\n".
            '+AgreementConstraint(trigger.1,left.1,"PERSON")'."\n";
    }
    else {
        $verb =~ s/\[V\]//;
        $verb.='$V';

        # find trigger in rule, either in pre or post
        my $quotemetaTrigger=quotemeta($trigger);
        my ($left,$right);
        if ($pre =~ /(.*)(^| )$quotemetaTrigger( |$)(.*)/)  {
            $left=$1; 
            $right=$4;
            print FRULES
                "$trigger".
                ':$PRON-CLR';
            print FRULES " $left" if $left;
            print FRULES ':';
            print FRULES "$right " if $right;
            print FRULES '[$PRON?] [(@AuxiliaireEtre ($ADV){0-2})?]'.
                " $verb".
                ':ABS_IDIOM$V---p---:'."\n".
                '+AgreementConstraint(left.1,right.4,"NUMBER")'."\n".
                '+AgreementConstraint(left.1,right.4,"PERSON")'."\n";
        }
        elsif ($post =~ /(.*)(^| )$quotemetaTrigger( |$)(.*)/)  {
            $left=$1; 
            $right=$4;
            print FRULES
                "$trigger".
                ':$PRON-CLR ';
            if ($pre ne "") {
                print FRULES "(($pre ".
                    '[$PRON?] [(@AuxiliaireEtre ($ADV){0-2})?])'.
                    '|([$PRON?] [(@AuxiliaireEtre ($ADV){0-2})?] '.
                    "$pre))";
            }
            else {
                print FRULES 
                    '([$PRON?] [(@AuxiliaireEtre ($ADV){0-2})?])';
            }
            print FRULES " $verb";
            print FRULES " $left" if $left;
            print FRULES ':';
            print FRULES "$right " if $right;
            print FRULES ':ABS_IDIOM$V---p---:'."\n".
                '+AgreementConstraint(left.1,left.3,"NUMBER")'."\n".
                '+AgreementConstraint(left.1,left.3,"PERSON")'."\n";
        }
        else {
            print STDERR "Warning : cannot find trigger [".$trigger.
                "] in expression \"$exp\"\n";
        }
    }
}
