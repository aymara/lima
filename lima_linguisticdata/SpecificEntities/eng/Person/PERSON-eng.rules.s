set encoding=utf8
using modex Person-modex.xml
using modex Location-modex.xml
using modex Organization-modex.xml
using groups Person
using groups Location
using groups Organization
set defaultAction=>CreateSpecificEntity()

#----------------------------------------------------------------------
# recognition of person names
#----------------------------------------------------------------------

# known identified persons
#Hafez::Al-Assad:PERSON:Hafez Al-Assad
#Saddam::Hussein?:PERSON:Saddam Hussein
#Laden:(Oussama|Ousama|Osama|Usama)? (ben|bin|Ben|Bin)::PERSON:Ousama Ben Laden
#Arafat:Yasser?::PERSON:Yasser Arafat
#Sharon:Ariel?::PERSON:Ariel Sharon
#Mobutu::(Sese Seko)?:PERSON:Mobutu
#Yitzhak::Rabin:PERSON:Yitzhak Rabin
#Isaac::Rabin:PERSON:Yitzhak Rabin
#Chirac:::PERSON:Jacques Chirac

Hafez::Al-Assad:PERSON:=>NormalizePersonName("Hafez,Al-Assad")

Saddam::Hussein:PERSON:=>NormalizePersonName("Saddam,Hussein")

Laden:(Oussama|Ousama|Osama|Usama)? (ben|bin|Ben|Bin)::PERSON:=>NormalizePersonName("Oussama,Ben Laden")

Arafat:Yasser::PERSON:=>NormalizePersonName("Yasser,Arafat")

Sharon:Ariel::PERSON:=>NormalizePersonName("Ariel,Sharon")

Mobutu::(Sese Seko)?:PERSON:=>NormalizePersonName(",Mobutu")

Yitzhak::Rabin:PERSON:=>NormalizePersonName("Yitzhak,Rabin")

Isaac::Rabin:PERSON:=>NormalizePersonName("Yitzhak,Rabin")

Chirac:::PERSON:=>NormalizePersonName("Jacques,Chirac")


#----------------------------------------------------------------------
# recognition of person names
#----------------------------------------------------------------------

use firstname.lst,title.lst,function_title.lst,person_ending.lst

#@Firstname:[(@Title|@FunctionTitle)?]:($NP|T_Amh){0-3} @PersonEnding?:PERSON:=>NormalizePersonName()

#$NP:[(@Title|@FunctionTitle)]:$NP{0-2} @PersonEnding?:PERSON:=>NormalizePersonName()
#$NP:$NP{0-2}:@PersonEnding? , @FunctionTitle:PERSON:=>NormalizePersonName()

# hack to take initials into account -> should be in tokenizer

@Initial=(A.,B.,C.,D.,E.,F.,G.,H.,I.,J.,K.,L.,M.,N.,O.,P.,Q.,R.,S.,T.,U.,N.,W.,X.,Y.,Z.)

@Firstname::t_capital_1st{1-2} [<Location.LOCATION>]:PERSON:N_PERSON

@Initial:[(@Title|@FunctionTitle)]:[($DET|$PRON|$ADJ|$V)]:PERSON:=>NormalizePersonName()

@Initial:[(@Title|@FunctionTitle)?]:((de|da|le|la|de la)? (T_A1|T_Amh|$NP)) [($DET|$PRON|$ADJ|$V)]:PERSON:=>NormalizePersonName()

@Initial:[(@Title|@FunctionTitle)?]:((de|da|le|la|de la)? (T_A1|T_Amh|$NP)){1-2}:PERSON:=>NormalizePersonName()

@Firstname:[(@Title|@FunctionTitle)]:[($DET|$PRON|$ADJ|$V)]:PERSON:=>NormalizePersonName()

@Firstname:[(@Title|@FunctionTitle)?]:@Initial? ((de|da|le|la|de la)? (T_A1|T_Amh|$NP)) [($DET|$PRON|$ADJ|$V)]:PERSON:=>NormalizePersonName()

@Firstname:[(@Title|@FunctionTitle)?]:@Initial? ((de|da|le|la|de la)? (T_A1|T_Amh|$NP)){1-2}:PERSON:=>NormalizePersonName()

# @Firstname:[(@Title|@FunctionTitle)?]:@Initial? ((de|da|le|la|de la)? (T_A1|T_Amh)){1-2} @PersonEnding?:PERSON:=>NormalizePersonName()

# @Firstname:[(@Title|@FunctionTitle)?]:@Initial? ((de|da|le|la|de la)? (T_A1|T_Amh)){1-2} @PersonEnding?:PERSON:=>NormalizePersonName()

# [@FunctionTitle]:t_capital_1st::PERSON:N_PERSON

# t_capital_1st{1-3}:[@FunctionTitle]::PERSON:

T_A1:[(@Title|@FunctionTitle)]:(T_A1|T_Amh|@Initial){0-2} @PersonEnding?:PERSON:=>NormalizePersonName()

T_A1:(T_A1|T_Amh|$NP|@Initial){0-2}:@PersonEnding? [(, @FunctionTitle)]:PERSON:=>NormalizePersonName()

$NP:(T_A1|T_Amh|$NP|@Initial){0-2}:@PersonEnding? [(, @FunctionTitle)]:PERSON:=>NormalizePersonName()

# "bla bla", Machin said.
T_A1::(T_A1|T_Amh|$NP|@Initial){0-2} [(said .)]:PERSON:
T_A1:[said]:(T_A1|T_Amh|$NP|@Initial){0-2}:PERSON:

@Pronouns=(
I,
You,
It,
He,
She,
They,
We
)

@Conjunctions=(
But,
And,
For,
All,
)


[who]:[<Location.LOCATION>] [,]::NOT_PERSON:
[said]:[<Location.LOCATION>]::NOT_PERSON:
[who]:[<Organization.ORGANIZATION>] [,]::NOT_PERSON:
[said]:[<Organization.ORGANIZATION>]::NOT_PERSON:
[who]:@Pronouns [,]::NOT_PERSON:
[said]:[(@Pronouns|@Conjunctions)] t_capital_1st::PERSON:
[said]:[@Conjunctions] t_capital_1st::PERSON:
[who]:t_capital_1st{1-3} [,]::PERSON:
#problem with these rules for over detection (org, or nouns such as "Analysts" solutions??)
#------------------------------
# a list of known persons
use person.lst

@person:::PERSON:=>NormalizePersonName()

# firstnames alone ?
#@Firstname:::PERSON:=>NormalizePersonName()


set encoding=utf8
using modex Location-modex.xml
using groups Location
set defaultAction=>CreateSpecificEntity()


#-----------#---------------------------------------------
#Using found LOCATIONS for bootstrapping
#---------------------------------------------

@Months=(
January,
February,
March,
April,
May,
June,
July,
August,
September,
November,
October,
December
)


[<Location.LOCATION>]:(t_capital|t_capital_1st) [-] (t_capital|t_capital_1st) [,]::LOCATION:N_LOCATION
[<Location.LOCATION>]:(t_capital_1st|t_capital){1-3} [,]::LOCATION:N_LOCATION
[<Location.LOCATION>]:Community of::LOCATION:N_LOCATION
[<Location.LOCATION>]:(t_capital|t_capital_1st) [,]::LOCATION:N_LOCATION
[<Location.LOCATION>]::(CITY|City):LOCATION:N_LOCATION

@locationPre=(
East,
Eastern,
Western,
West,
Southwestern,
Southwest,
Southern,
Southeastern,
Southeast,
South-Western,
South-Eastern,
South,
Puerto,
Porto,
Northwestern,
Northwest,
Northern,
Northeastern,
Northeast,
North-Western,
North-Eastern)

[<Location.LOCATION>]:@locationPre::LOCATION:N_LOCATION

[in]::@Months:NOT_LOCATION:
[in]::[<Person.PERSON>]:NOT_LOCATION:
[in]::t_capital_1st{1-3}:LOCATION:
