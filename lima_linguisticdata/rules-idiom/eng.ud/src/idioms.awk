BEGIN { FS="\t" }
{ $1=tolower($1) }
{ nb=split($1,table,/ /);
 max=0; 
 for(i=1;i<=nb;i++) { if(length(table[i])>max) { max=length(table[i]) ; trigger=table[i] }
 }
 full=$1
 }
  
/L_ADJ_GEN/                               { tag="adjective"}
/L_ADVERBE_DE_DEGRE/   {                 tag="adverb"}
/L_ADVERBE_INTRODUISANT_UNE_CONSTRUCTION_APPOSITIONNELLE/   { tag="adverb"}
/L_ADVERBE_LOCATIF/                    { tag="adverb"}
/L_ADVERBE_SUIVANT_LA_TETE_NOMINALE/   { tag="adverb"}
/L_ADVERBE_TEMPS/                     { tag="adverb"}
/L_ADV_GENERAL/                     { tag="adverb"}
/L_CONJ_BEFORE_CLAUSE_MARKER/      { tag="conjunction"}
/L_CONJ_SUBORD/                   { tag="conjunction"}
/L_CONJ_WHETHER/                  { tag="conjunction"}
/L_NOMBRE_CARDINAL/                { tag="noun of number"}
/L_NOM_COMMUN/                      { tag="noun"}
/L_NOM_COMMUN_UNITE_DE_MESURE/   { tag="noun"}
/L_PREP_GENERAL/                   { tag="preposition"}
/L_PRONOM_PERSONNEL_REFLECHI/   { tag="pronoun" }
{ print "B;;;"trigger";"full";"tag";"$1";" }
{ $1=toupper(substr($1,1,1))""substr($1,2); 
  nb=split($1,table,/ /);
 max=0; 
 for(i=1;i<=nb;i++) { if(length(table[i])>max) { max=length(table[i]) ; trigger=table[i] }
 }
 full=$1
 }
{ print "B;;;"trigger";"full";"tag";"tolower($1)";" }
