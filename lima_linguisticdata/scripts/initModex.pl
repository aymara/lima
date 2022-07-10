#!/usr/bin/perl -s

# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

# Help mode
if ($main::h || $main::help) {
    print <<EOF;
Ce programme initie une architecture de fichiers de règles pour 
la reconnaissance d\'entités pour un modex particulier.
Ce programme n\'initie pas les relations et autres traitements propres,
ni les ressources spécifiques.

usage initModex.pl [-language=..,..] configFile

configFile contient une description de l\'organisation du 
modex en éventuellement sous-modex et entités, organisé de la 
façon suivante:
modex: entity1,entity2,...
   submodex_1: entity_3,..
   submodex_2: entity_4,..
   submodex_3: entity_5,..
     subsubmodex_1: entity6,...

EOF
    exit;
}

use strict;
no strict 'refs';

my @languages=split(',',$main::language);
die "must specify languages (use -language=..)" unless @languages;

die "must specify file name as argument" unless @ARGV;
my $file=shift @ARGV;

my ($modexName,%subModex,%entities)=@_;
&readPreConfigFile($file,\$modexName,\%subModex,\%entities);

print "modexName=$modexName\n";
foreach (sort keys %subModex) {
    print "subModex: $_=>".join(",",@{$subModex{$_}})."\n";
}
foreach (sort keys %entities) {
    print "entities $_=>".join(",",@{$entities{$_}})."\n";
}

&makeDir($modexName);
&makeDir("$modexName/conf");
&createModexConfigurationFile("$modexName/conf",$modexName,$modexName,\%subModex,\%entities,\@languages);
&createConfMakefile("$modexName/conf",$modexName)
&createGeneralMakefile($modexName,$modexName,"conf ".join(" ",@languages));
my ($lang,$subModex);
foreach $lang (@languages) {
    &makeDir("$modexName/$lang");
    &makeDir("$modexName/$lang/resources");
    &createGeneralMakefile("$modexName/$lang",$modexName,"resources ".join(" ",@{$subModex{$modexName}}));
    foreach $subModex (@{$subModex{$modexName}}) {
        &makeDir("$modexName/$lang/$subModex");
        &createModexMakefile("$modexName/$lang/$subModex",$subModex,$lang,\@{$entities{$subModex}});
        &createModexFiles("$modexName/$lang/$subModex",$subModex,$lang,\@{$entities{$subModex}});
        &createModexConfigurationFile("$modexName/conf",$modexName,$subModex,\%subModex,\%entities,\@languages);
    }
}

#----------------------------------------------------------------------
sub makeDir {
    my ($dir)=@_;
    mkdir $dir unless -d $dir; 
}

#----------------------------------------------------------------------
sub createGeneralMakefile {
    my ($dir,$modexName,$subdirs)=@_;
    if (! open(FMAKEFILE,">$dir/Makefile")) {
        print STDERR "cannot open >$dir/Makefile\n";
        return;
    }
    my $date=`date +%d/%m/%Y`;
    print FMAKEFILE <<EndOfBuffer
#
# Makefile for $modexName
# created on $date
#
SUBDIRS=$subdirs

all: install

install:
	\@for dir in \$(SUBDIRS); do \
	   echo "making \$\$dir"; \
	   cd \$\$dir; make install; cd ..; \
	done

clean:
	\@for dir in \$(SUBDIRS); do \
	   echo "cleaning \$\$dir"; \
	   cd \$\$dir; make clean; cd ..; \
	done
EndOfBuffer
;
    close(FMAKEFILE);
}

#----------------------------------------------------------------------
sub createModexMakefile {
    my ($dir,$modex,$lang,$entities)=@_;
    if (! open(FMAKEFILE,">$dir/Makefile")) {
        print STDERR "cannot open >$dir/Makefile\n";
        return;
    }
    my @entityFiles=@{$entities};
    map {$_=$_."-$lang.bin"} @entityFiles;
    my $entityFiles=join(' ',@entityFiles);
    my $date=`date +%d/%m/%Y`;
    print FMAKEFILE <<EndOfBuffer
#
# Makefile for $modex
# created on $date
#

LANGUAGE=$lang
RULES=$entityFiles

RM=/bin/rm -f

%.bin: %.rules
	compile-rules --language=\$(LANGUAGE) --output=\$@ \$<

all: install

install: \$(RULES)
	install \$(RULES) \$(MM_RESOURCES)/$modexName

clean:
	\$(RM) \$(RULES)
EndOfBuffer
;
    close(FMAKEFILE);
}

#----------------------------------------------------------------------
sub createConfMakefile {
    my ($dir,$modex)=@_;
    if (! open(FMAKEFILE,">$dir/Makefile")) {
        print STDERR "cannot open >$dir/Makefile\n";
        return;
    }
    my $date=`date +%d/%m/%Y`;
    print FMAKEFILE <<EndOfBuffer
#
# Makefile for $modex
# created on $date
#

all: install

install: 
	install *.xml \$(MM_CONF)

clean:
	\@echo Done
EndOfBuffer
;
    close(FMAKEFILE);
}

#----------------------------------------------------------------------
sub createModexFiles {
    my ($dir,$modex,$lang,$entities)=@_;
    my $entity;
    foreach $entity (@{$entities}) {
        if (!open(FRULES,">$dir/$entity-$lang.rules")) {
            print STDERR "cannot open >$dir/$entity-$lang.rules\n";
        }
        print FRULES<<EndOfBuffer
set encoding=utf8
using modex $modex-modex.xml
using groups $modex
set defaultAction=>CreateSpecificEntity()
EndOfBuffer
;
        close(FRULES);
    }
}

#----------------------------------------------------------------------
sub readPreConfigFile {
    my($file,$modexName,$subModex,$entities)=@_;
    
    open(FIN,$file) || die "cannot open file $file";
    my @modexStack;
    while(<FIN>) {
        if (m%([ 	]*)([^ ]*): *(.*)%) {
            my $spaces=length($1);
            my $modex=$2;
            my @entities=split(" ",$3);
            #print STDERR "$spaces|$modex|".join(',',@entities)."\n";
            @{$$entities{$modex}}=@entities if @entities;
            if ($spaces==0) {
                $$modexName=$modex;
                push @modexStack, [$modex,$spaces];
            }
            else {
                my ($lastModex,$lastIndent)=@{$modexStack[-1]};
                if ($spaces==$lastIndent) {
                    #print STDERR "($lastModex,$lastIndent)\n";
                    # same depth, replace last in stack
                    pop @modexStack;
                    push @{$$subModex{${$modexStack[-1]}[0]}}, $modex;
                    push @modexStack, [$modex,$spaces];
                }
                elsif ($spaces>$lastIndent) {
                    # increasing indent => submodex
                    push @{$$subModex{$lastModex}}, $modex;
                    push @modexStack, [$modex,$spaces];
                }
                else {
                    # increasing indent => pop
                    pop @modexStack;
                }
            }
        }
    }
    close(FIN);
}

#----------------------------------------------------------------------
sub createModexConfigurationFile {
    my ($dir,$resourceDir,$modexName,$subModex,$entities,$languages)=@_;
    my ($hasSubModex,$hasEntities)=0;
    $hasSubModex=1 if exists $$subModex{$modexName};
    $hasEntities=1 if exists $$entities{$modexName};
    my @subModex=@{$$subModex{$modexName}};
    my @entities=@{$$entities{$modexName}};

    my ($submodex,$language,$entity);
    
    my $file="$dir/$modexName-modex.xml";
    print STDERR "create $file\n";

    if (! open(FCONF,">$file")) {
        print STDERR "cannot open >$file\n";
        return;
    }

    print FCONF
        "<?xml version='1.0' encoding='UTF-8'?>\n",
        "<modulesConfig>\n";

    if ($hasEntities) {
        # definition of entities
        print FCONF
            "  <module name=\"entities\">\n",
            "    <group name=\"$modexName\">\n",
            "      <list name=\"entityList\">\n";
        foreach $entity (@entities) {
            print FCONF 
                "        <item value=\"$entity\"/>\n";
        }
        print FCONF
            "      </list>\n",
            "    </group>\n",
            "  </module>\n";
    }

    print FCONF 
        "  <module name=\"Processors\">\n";

    if ($hasSubModex) {
        # include submodex processors
        print FCONF
            "    <group name=\"include\">\n",
            "      <list name=\"includeList\">\n";
        
        foreach $submodex (@subModex) {
            print FCONF "        <item value=\"$submodex-modex.xml/Processors\"/>\n";
        }
        print FCONF 
            "      </list>\n",
            "    </group>\n";
    }
    print FCONF 
        "    <group name=\"".$modexName."Modex\" class=\"ProcessUnitPipeline\" >\n",
        "      <list key=\"processUnitSequence\">\n";
    
    foreach $submodex (@subModex) {
        print FCONF "        <item value=\"".$submodex."Modex\"/>\n";
    }
    foreach $entity (@entities) {
        my $entityName=ucfirst(lc($entity));
        print FCONF "        <item value=\"".$entityName."Recognition\"/>\n";
    }
    print FCONF
        "      </list>\n",
        "    </group>\n";

    foreach $entity (@entities) {
        my $entityName=ucfirst(lc($entity));
        print FCONF
            "    <group name=\"".$entityName."Recognition\" class=\"ApplyRecognizer\">\n",
            "      <param key=\"automaton\" value=\"".$entityName."Rules\"/>\n",
            "      <param key=\"applyOnGraph\" value=\"AnalysisGraph\"/>\n",
            "      <param key=\"useSentenceBounds\" value=\"no\"/>\n",
            "    </group>\n";
    }
    

    print FCONF
        "  </module>\n";
    
    foreach $language (@{$languages}) {
        print FCONF 
            "  <module name=\"resources-$language\">\n";
        if ($hasSubModex) {
            # include submodex resources
            print FCONF 
                "    <group name=\"include\">\n",
                "      <list name=\"includeList\">\n";
            foreach $submodex (@subModex) {
                print FCONF "        <item value=\"$submodex-modex.xml/resources-$language\"/>\n";
            }
            print FCONF
                "      </list>\n",
            "    </group>\n";
        }
        if ($hasEntities) {
            # include direct resources for entities
            foreach $entity (@entities) {
                my $entityName=ucfirst(lc($entity));
                print FCONF 
                    "    <group name=\"".$entityName."Rules\" class=\"AutomatonRecognizer\">\n",
                    "      <param key=\"rules\" value=\"$resourceDir/$entity-$language.bin\"/>\n",
                    "    </group>\n";
            }
            print FCONF
                "    <group name=\"".$modexName."Indexing\" class=\"EntityTypeIndexing\">\n",
                "      <map name=\"entityPartsIndexing\">\n";
            foreach $entity (@entities) {
                print FCONF 
                    "        <entry key=\"$modexName.$entity\" value=\"yes\"/>\n",
                }
            print FCONF
                "      </map>\n",
                "    </group>\n";
            print FCONF
                "    <group name=\"".$modexName."Micros\" class=\"SpecificEntitiesMicros\">\n";
            foreach $entity (@entities) {
                print FCONF 
                    "      <list name=\"$modexName.$entity\">\n",
                    "        <item value=\"NC\"/>\n",
                    "      </list>\n";
            }
            print FCONF 
                "    </group>\n";
        }
        print FCONF 
            "  </module>\n";
    }
    print FCONF "</modulesConfig>\n";
    close(FCONF);
}
