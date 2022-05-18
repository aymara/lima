#!/usr/bin/perl -s

# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

# global variables
# not optimal : keep all possible conversions
my (@Categories,
    %NumericDecoding,
    %NumericCodeName,
    %NumericCodeText,
    %NumericCodes);
my (%SymbolicCodes,
    %SymbolicCodesProperties);

#---------------------------------------------------------------------------
# functions to read the content of the files describing the codes
#---------------------------------------------------------------------------
sub readNumericCodes {
    my ($filenameCodes) = @_;
    local ($_);
    my ($categoryName,$categorySize,$cumulSize,$inValue,$valueCodeName,
	$valueNumValue,$valueStringValue,$allset);

    # xxxCode.xml is generally in UTF16 
    # 2004/08/08 - gael - not anymore in UTF16
    open(FILECODES,"$filenameCodes") 
	|| die "cannot open code file $filenameCodes";
    $inValue=0;
    $cumulSize=0;
    $categorySize=0;
    $inCategory=0;
    while(<FILECODES>) {
	if (m%<cat_name>([^<]*)</cat_name>%) {
	    $categoryName=$1;
            # keep macro for incomplete codes (macro specified
            # but not macro_micro
#             if ($categoryName eq 'MACRO') {
#                 $inCategory=0;
#                 next;
#             }
            $inCategory=1;
	    push @Categories, $categoryName;
	}
	elsif ($inCategory) {
            if (m%<cat_size>([^<]*)</cat_size>%) {
                # size of previous code
                # macro_micro is at same place as macro
                if ($categoryName ne 'MICRO') {
                    $cumulSize+=$categorySize;
                }
                $categorySize=$1;
                $NumericDecoding{$categoryName}[0]=$categorySize;
            }
            elsif (m%<cat_value>%) {
                $inValue=1;
            }
            elsif (m%</cat_value>%) {
                $inValue=0;
            }
            elsif ($inValue) {
                if (m%<n>([^<]*)</n>%) {
                    $valueCodeName=$1;
                    $allset=0;
                }
                if (m%<v>([^<]*)</v>%) {
                    $valueNumValue=$1;
                }
                if (m%<m>([^<]*)</m>%) {
                    $valueStringValue=$1;
                    $allset=1;
                }
                
                if ($allset) {
                    $NumericDecoding{$categoryName}[1]{hex($valueNumValue)}=$valueCodeName;
                    $NumericCodeName{$valueStringValue}=$valueCodeName;
                    $NumericCodeText{$valueCodeName}=$valueStringValue;
                    $NumericCodes{$valueCodeName}=[$valueNumValue,$cumulSize];
                }
            }
        }
    }
    close(FILECODES);
}

sub readSymbolicCodes {
    my ($filenameCodes) = @_;
    local ($_);
    my ($code,$lingPropName,$lingPropValue);

    open(FILECODES,$filenameCodes)
	|| die "cannot open code file $filenameCodes";
    $macroSize=0;
    while(<FILECODES>) {
        if (m%<code key=\"(.*)\" *>%) {
            $code=quotemeta($1);
            $cumulProp="";
        }
        elsif (m%<field key=\"(.*)\" value=\"(.*)\" */>%) {
            $lingPropName=$1;
            $lingPropValue=$2;
            if ($lingPropValue) {
                ${$SymbolicCodes{$code}}{$lingPropName}=$lingPropValue;
            }
        }
        elsif (m%</code>%) {
            # inverse access
            my ($cat,$cumulProp);
            foreach $cat (@Categories) {
                next if ($cat eq 'MACRO' && 
                         exists ${$SymbolicCodes{$code}}{'MICRO'});
                if (exists ${$SymbolicCodes{$code}}{$cat}) {
                    $cumulProp.=${$SymbolicCodes{$code}}{$cat};
                }
            }
            $SymbolicCodesProperties{$cumulProp}=$code;
        }
    }

    close(FILECODES);
}

#---------------------------------------------------------------------------
# functions that operate the transcoding
#---------------------------------------------------------------------------
sub SymbolicToNumeric {
    my ($code) = @_;
    my $properties = $SymbolicCodes{quotemeta($code)};
    return &computeNumericCodeFromProperties($properties);
}

sub NumericToSymbolic {
    my ($code) = @_;
    my $properties = &parseNumericCode($code);
    return &computeSymbolicCodeFromProperties($properties);
}

sub SymbolicToText {
    my ($code) = @_;
    my $properties = $SymbolicCodes{$code};
    return &computeTextFromProperties($properties);
}

sub NumericToText {
    my ($code) = @_;
    my $properties = &parseNumericCode($code);
    return &computeTextFromProperties($properties);
}

#---------------------------------------------------------------------------
# function to parse the numeric Code
#---------------------------------------------------------------------------
sub parseNumericCode {
    my ($code) = @_;
    my ($category);
    my (%properties);
    local ($_);

    foreach $category (@Categories) {
	my $size = $NumericDecoding{$category}[0];
	# get the numeric value for this category
	my $value = $code%(2**$size);
	#print STDERR "--[$code:$category-$size-$value]--\n";
	
	my $textCode=$NumericDecoding{$category}[1]{$value};
	
        if ($textCode) {
            $properties{$category}=$textCode;
        }
        $code = int($code/(2**$size)) unless $category eq 'MACRO';
    }
    
    return (\%properties);
}

#---------------------------------------------------------------------------
# get the symbolic code from a set of properties
sub computeSymbolicCodeFromProperties {
    my ($properties) = @_;
    my $cumulProp;
    foreach $prop (@Categories) {
        next if ($prop eq 'MACRO' && 
                 exists $$properties{'MICRO'});
        if (exists $$properties{$prop}) {
            $cumulProp.=$$properties{$prop};
        }
    }
    if (exists $SymbolicCodesProperties{$cumulProp}) {
        return $SymbolicCodesProperties{$cumulProp};
    }
    else {
        print STDERR "cannot find symbolic code for ".$cumulProp."\n";
    }
    return "";
}

#---------------------------------------------------------------------------
# compute the numeric code from a set of properties
sub computeNumericCodeFromProperties {
    my ($properties) = @_;

    my $numcode;
    my $prop;
    foreach $prop (@Categories) {
        # do not use macro if macro_micro is set
        next if ($prop eq 'MACRO' && 
                 exists $$properties{'MICRO'});
        #print STDERR "getNumericCode of $prop\n";
        if (exists $$properties{$prop}) {
            $numcode += &getNumericCode($$properties{$prop});
        }
    }
    
    return $numcode;
}

#---------------------------------------------------------------------------
# build the textual representation from a set of properties
sub computeTextFromProperties {
    my ($properties) = @_;
    my ($text,$cat,$first);
    #$text=join(" ",values %{$properties});

    $first=1;
    # take categories in reverse
    # foreach $cat (@Categories) {
    foreach $cat (reverse @Categories) {
        next if ($prop eq 'MACRO' && 
                 exists $$properties{'MICRO'});
        if (exists $$properties{$cat}) {
            if ($first) { $first=0; }            
            else { $text .= " "; }
            $text .= $NumericCodeText{$$properties{$cat}};
        }
    }
    
    $text =~ s/ +$//;
    return $text;
}

#---------------------------------------------------------------------------
# useful functions for the numeric codes
#---------------------------------------------------------------------------
# convert text name (masc) to internal code name (MASC)
sub getCodeName {
    my ($textValue)=@_;
    return $NumericCodeName{$textValue}; 
}

# compute the numeric code of a property
sub getNumericCode {
    my ($propertyName) = @_;
    my $numvalue=$NumericCodes{$propertyName};

    #print STDERR "$propertyName,".$$SymbolicCodes{$propertyName}."->(";
    if ($numvalue == undef) { 
	#print STDERR "-)\n";
	#print STDERR "Warning: $propertyName not defined\n";
	return 0; 
    }

    #print STDERR join(",",@{$numvalue}).")\n";
    
    return ( (hex($$numvalue[0])) * (2**$$numvalue[1]) );
}

1;
