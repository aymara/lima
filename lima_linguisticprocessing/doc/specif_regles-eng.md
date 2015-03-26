Traduction
# Rules format specifications for the construction of phrases recognition automata

### Romaric Besancon

CEA-list` <[romaric.besancon@cea.fr] (mailto: romaric.besancon@cea.fr)> `

Copyright 2005 Romaric Besançon - CEA-LIST

** ** Revision History
---dropoff window
Revision 0.1 | February 7, 2005 | Revised by: RB
resumption of preliminary specification document (in LaTeX)
  
* * *dropoff Window

** ** Table of Contents
1 \. objective
2 \. Specifications rules
    

2.1. Need expressiveness rules
3 \. Format Rules
    

3.1. automata formalism describing the context
    

3.1.1. single units
3.1.2. Operations on single units
3.1.3. Terminals of expression
3.1.4. Type of the expression
3.1.5. Standardized form of the entity
3.1.6. Supplements syntax rules file
3.2. Formal specification rules
3.3. limitations
    

3.3.1. Limits expressiveness rules
3.4. Examples of rules
    

3.4.1. Rules of named entity recognition
3.4.2. recognition rules idioms

## 1 \. objective

The purpose of this document is the definition of rules declarative format to recognize expressions in a text analyzed (under analysis). This recognition is based on a format of regular expressions that allows the construction of automata for the recognition of particular expressions. These controllers are used in linguistic processing for

   * Recognition of idioms

   * Recognition of specific entities such as numbers, dates and named entities

   * The dependency relationships extraction for parsing

* * *dropoff Window

## 2 \. Specifications rules

* * *dropoff Window

### 2.1. Need expressiveness rules

The elements necessary for defining the rules for the recognition of particulères specific expression are:

   * A _élément déclencheur_: it is the element that will initiate the recognition process when it appears in a text; it will be chosen as the least common element or the most characteristic expression considered, to avoid too many triggers that do not succeed. The shutter need not be at the beginning of the expression.

   * Left _contextes and droit_ defining expression around the shutter release (the terms "left" and using "right" to refer to contexts "before-seen" and "next": it is also how these terms should be understood in a language like Arabic): these contexts are defined using the near formalism of regular expressions. The elements needed to define these settings are:

     * Recognition units: they can be virtually any property (or property combination) associated with a unit element of the sentence output from the morphological analysis or after disambiguation (a _FullToken_ or _DicoWord_). In practice, the properties that we think are interesting priori for defining the rules are:

       * Simple words: recognition is then done on the simplest surface (direct form of the word in the text or one of its variant spellings obtained during the morphological analysis);

       * The only grammatical categories, eg to take into account the inclusion of one or more adjectives;

       * Standardized Forms of Words: correspondence will then with all inflected word forms (the grammatical category of the word must necessarily be specified, eg "door" will not accept the same inflections if noun or verb ) ;dropoff window

       * Classes, which include a number of words or items (in a list) that share a property that is not identified in the dictionary (this may be the case for the proper names of advertisers, for example ) ;dropoff window

       * Other properties can also be considered, such as semantic or morphological traits (initial capital letter, for example), or constraints on numerical values;

     * Operations on these devices (conventional operations of regular expressions)

       * Some groups of words such as "the" can be considered as a group to handle as a unit;

       * Alternative units or groups "M." or "Sir";

       * The cardinality on the occurrence of a unit or group: we can introduce one or two adverbs in a verbal form composed, but not more than three. We may also need to specify an unlimited cardinality (the limit will then be the limit of the sentence or text, depending on usage).

       * Units or optional groups: This property is a special case of cardinality (at least 0, at most 1 case), but you can keep it for easier writing;

       * Denial of a unit or group: we may want to express a unit is recognized if it does not have some property (eg, any word except for one point).

   * The _type_ expression recognized;

   * The _forme normalisée_ expression (for some entities whose standardized form to be calculated, such as numbers or dates, a code to indicate the type of standardization may also be considered);

   * The _bornes of the recognized expression_ or indication of what words among those recognized are not part of the expression recognized: triggers and context can indeed afford to recognize or describe an entity, not part of this entity;

   * Additional _contraintes_ on certain recognized or between elements of the rule must also be specified (eg, agree in gender constraints in number, person etc.)

   * Possible indication of principal_ _mot of the recognized entity (head of the expression). This indication is particularly useful idioms. An idiom can indeed be flexed and linguistic properties associated with the expression as a whole will be those of the head of the expression (for example, the reflexive form "was wrong" should be recognized as a verbal form the imperfect indicative, word linguistic properties "wrong", head of the expression).

   * An indication of the _relativité Application_ of the rule also useful idioms, this indication is intended to indicate whether the recognition of the rule is absolute (eg "to as" an expression is _always_ idiomatic) where is a possibility to disambiguate in a later phase of treatment ("appointment" may be a compound word or a verb followed by a pronoun).

   * Can specify the negation of a kind: if the rule applies when no other rules of the specified type will be applied (to better structure the rules and bypass or easily catch problems).

* * *dropoff Window

## 3 \. Format Rules

To meet the needs of expressiveness rules specified in the previous section, the rules of formality currently used is:

The main separator of the rule is the sign "_: _" A rule is defined by an expression of the type:

_ <trigger> _: _ <left context> _: _ <context right> _: _ <expression type> _: _ <standardized form> _

The _ <left context> _ and _ <right context> _ using the formalism defined in the following section.

Constraints are added following the rules.

* * *dropoff Window

### 3.1. automata formalism describing the context

* * *dropoff Window

#### 3.1.1. single units

Units | azer
--- | ---
Simple words | word
Only grammatical categories | $ CG
Standardized forms of words | $ lemma CG
Morphological Properties | t_xx or t_xx
Classes |classe
Any word | *
  
* * *dropoff Window

##### 3.1.1.1. grammatical categories

grammatical categories are defined using the internal symbolic codes to the system (type L_NOM_COMMUN). The category can only specify a macro-category or macro-pair category / micro category separated by a hyphen. Examples: $ L_NC $ L_NC-L_NC-GEN are correct specification categories.

* * *dropoff Window

##### 3.1.1.2. classes

Classes are explicitly defined in the rules file or an external file with a list of elements of the class. The syntax for defining a class of words is simply class _ _ @ _ = (_ _ unit1, unit2 _ _, _ _ unit3 ...) _ The elements can be any unit defined in the previous section. The comma is the separator between the elements, there is no space after the comma, against, the line breaks are allowed. For parsing, the definition of grammatical categories classes is possible.

* * *dropoff Window

##### 3.1.1.3. morphological

The morphological properties are identified by the types given to linguistic units by tokeniseur (if given types of output tokeniseur change, these formalisms morphological properties will be modified accordingly).

Type names are prefixed (by convention, the tokeniseur) with "t_" this prefix is used as to directly recognize the type names in the rules. These types correspond to the inputs "<default>" possible in the tokeniseur PLC.

For example, the types defined by the French tokeniseur are:

t_acronym | t_alphanumeric | t_capital | t_capital_1st
--- | --- | --- | ---
t_capital_small | t_cardinal_roman | t_comma_number | t_dot_number
t_fraction | t_integer | t_ordinal_integer | t_ordinal_roman
t_pattern | t_sentence_brk | t_small | t_word_brk
  
The types defined by tokeniseur other languages are also used seamlessly with this rating.

A structured rating types of tokeniseur is also possible (this notation is considered obsolete because it does not consider certain types introduced by the new languages such as Arabic and Chinese, but it is still functional and is still used in rules files, so its specification is retained here).

T_A- | Alphabetical | T_N- | Digital
--- | --- | --- | ---
T_Ac- | uppercase alphabetical | T_Ni | entire digital
T_As- | lowercase alphabetical | T_Nc | Digital with commas
T_A1- | Alphabetical first capital letter | T_Nd | Digital with points
T_Aa- | Alphabetical acronym | T_Nf | Digital fraction
> T_Am- | uppercase and lowercase alphabetical | T_NO | Digital ordinal
T_A-c | Alphabetical Roman numerals Cardinal | T_U | Alphanumeric
T_A-o | Alphabetical Roman numerals ordinal | T_P | pattern
T_A-n | No Roman numerals alphabetical | T_W | word break
| | T_S | break sentence
  
* * *dropoff Window

##### 3.1.1.4. Indication of the head of an expression

The head of an expression will be indicated by the character _> & _ before the unit identified as the head.

_Exemple: _
    
    
    
Abundance: $ L_NC & Horn of $ NCFS :: IDIOM: cornucopia
              

* * *dropoff Window

##### 3.1.1.5. Constraints on numerical values

such constraints are particularly useful for the recognition of dates. They focus on digital forms of numbers and can indicate the desired specific value or range of desired values (between _m_ and _n_), with the following notation: _T_Ni = No or _T_Ni> m <No.

Note: These constraints should be treated as additional constraints on a unit (they would include units of a type other than digital).

* * *dropoff Window

#### 3.1.2. Operations on single units

In the following table giving the possible operations, the listed elements "elt" are either words or groups (sequences or alternatives).

_Opérations_ |
--- | ---
sequence (one after another) | (elt1 elt2 ...)
alternative (one or the other) | (elt1 | elt2 | ...)
optional element | elt?
  
cardinality of the occurrence of an element between _i_ and _j_ times

| I-j elt {}
  
cardinality of the occurrence of an element between _i_ and an infinite number of times

| Unit} {i-n or (...) {} i-n
denial of a unit (the negation of a group is not treated, it should be avoided) | ^ unit
  
* * *dropoff Window

#### 3.1.3. Terminals of expression

The indication of which portions of expression are not part of the expression recognition is given by framing these portions of expressions by _ brackets [...] _. These hooks should be placed around the units or entire groups (especially, they also include groups of modifiers indicating optionality of a group).

The trigger can not be part of the expression recognized (we will place the same way brackets around the trigger).

* * *dropoff Window

#### 3.1.4. Type of the expression

The list of possible types are defined in an external file. Currently, the types of expressions recognized are all put in the same file, whether named entities, idioms, dependency relationships ...

The field type of expression may also contain additional information:

   * Linguistic properties associated with the term recognized: these properties are useful when the recognition of expression allowing the creation of a new token (this is the case with idiomatic expressions). The combination of linguistic properties is done by adding the "$" sign after the type of the expression, followed by the code of linguistic properties (must be a numeric code, but compilation of scripts to consider the code symbolic Grace as $ IDIOM NCMS).

   * Relativity application of the rule (also useful idioms) _ABS__ adding to the expression of the type indicates that this expression is absolute (it is always true regardless of context).

   * Type of negation: Adding _NOT__ before the type of the expression used to indicate that if the rule applies, no other rule of the type indicated will be applied with this trigger.

The types of expressions defined for the named entities are:

NUMEX | for numbers and measures
--- | ---
TIMEX | for dates
PERSON | for names of persons
LOCATION | for place names
ORGANIZATION | for names of organizations
PRODUCT | to product names
EVENT | for events
  
To idioms, one type is defined: the IDIOM type.

For syntactical analysis, defined types correspond to different types of dependency relationships considered. Here is a partial list of these relationships:

DETSUB | Relationship between critical and substantive (the -> chat)
--- | ---
ADJPRENSUB | Relationship between prenominal adjective and noun (much> Cat)
COMPADJ | Complementary adjective
COMPADV | Complementary adverb
ADVADJ | adjective and adverb Relationship
ADVADV | Relationship between two adverbs
SUBADJPOST | Relationship between a noun and an adjective postnominal (chat <\ - black)
COMPDUNOM | complementary relationship name (cat <\ - Pierre in "Cat Stone")
SUBSUBJUX | Two juxtaposed nouns in French
TEMPCOMP | consists Time
  
* * *dropoff Window

#### 3.1.5. Standardized form of the entity

The normalized form of the expression is simply given as a string. For types whose standardized form to be calculated, can be given a code indicating how this normalization which must be made. Currently defined codes are:

N_DATE | standardization of dates by day, month, year
--- | ---
N_NUMBER | standardization numbers (calculating the value of numbers in words)
N_PERSON | standardization of names of persons name, title
  
In a later version of the rules, they should use the normalization actions.

* * *dropoff Window

##### 3.1.5.1. Constraints and actions

Constraints or actions may be attached to the expression recognition rules. Constraints can focus on one or two elements of the rule: they correspond to a function that will be called with the argument or two nodes corresponding elements and returns a boolean. Actions are functions called at the end of the application of the rule, depending on the success of the rule (is that the term was recognized or not). They do not use the elements of the rule. They can optionally use the result by the rule (this is determined to write the function, and is not directly stated in the rule).

For readability rules, constraints between elements of a rule and actions are expressed outside the definition of rules.

Following the rules (or in the following lines), constraints begin with _ + _ and are written:

+ constraintName (elt1, elt2 "complement")

or

+ constraintName (elt, "complement")

In the first case, and _elt1_ _elt2_ are the matters dealt with the stress, the complement (in quotes) is optional and can be used to pass additional information to the function.

In the second case, the only _elt_ an element on which door duress.

The elements are identified by their position in the rule, in two steps: First, the context, which can be _right_ (right background) _left_ (background left) or _trigger_ (trigger) and the position of the word in the context (indicating the positions of words is very limited for the moment: you can only access single elements in context: word groups are counted as one item)

Example: in the case of reflexive verbs, if you want to make the distinction between "I stop", "I arrest you", "you stop me", "you stop") can then be use a rule includes requirements agreed:
    
    
    
stop L_V $ $ L_PRON-L_PRON_REFLEXIFPronPrev [$?] :: IDIOM $ V: stop
     + AgreementConstraint (trigger.1, left.1 "PERSON")
     + AgreementConstraint (trigger.1, left.1, "NUMBER")
              

where _ @ PronPrev_ is the class of the categories of preverbal pronouns.

A constraint return true iff:
  * its element(s) is/are found;
  * its function returns true.

This means that a constraint will return false if refering to an absent optional element. For example, the following rule will **not** match for `a c`:

```
a:b? c::TYPE:
    +Constraint(left.1,"value")
```


The actions are defined following the rules (or in the following lines) by a _ = _ followed by a sign> or <and the function name.

   * => DoSomething () indicates that the _faitQuelqueChose_ action will be taken in case of successful application of the rule;

   * = <FaitAutreChose () indicates that the _faitAutreChose_ action will be taken in case of application of the rule failure;

As with constraints, supplements can be passed to the function.

* * *dropoff Window

#### 3.1.6. Supplements syntax rules file

The use of an escape character (\\) can be introduced in the definition of units syntax characters "() [] {} ^ | @ $ &", without being interpreted.

On the other hand, to allow to make more structured and readable rules files, the following syntax elements were also identified:

   * Lines starting with "_ _ #" are comments (a "_ # _" is not the beginning of the line is not interpété as a comment)

   * Primitive "_include_" followed by a file name (or more file names, separated by commas) allows the inclusion of external policy files (these files are interpreted completely independently: for example, the defined classes in included files are not accessible in the including file, or otherwise);

   * Primitive "_use_" followed by a file name (or more file names, separated by commas) allows to include definitions of external word classes: classes

   * An indication of the file encoding can be made at the beginning of the file, the original "_define encoding = _". The only possible encodings are currently "latin1" and "utf8". The default encoding is "latin1" (the indication should be placed before the first line could be wrong coded).

   * In order to make it more generic application of the rules, it is possible to associate a default action to all the rules of a file. This is done with the original "_set defaultAction = _" followed by action name.

* * *dropoff Window

### 3.2. Formal specification rules

The EBNF grammar describing the definition of the rules presented in this section.
    
    
    
        
     <definition> :: = {<rule>}
     <rule> :: = <trigger> ":" <contexteGauche> ""
                           <contexteDroit> ":" <type> ":" <formeNormalisée>
                           [<constraint> *] [<action> *]
     <trigger> :: = ["["] <unitéSimple> ["]"]
     <contexteGauche> :: = {<item>}
     <contexteDroit> :: = {<item>}
     <type> :: = <string> [<class>]
     <formeNormalisée> :: = <string>
     <constraint> :: = "+" <functionName> "(" <eltIndex> [, <eltIndex>]
                           [, "\" "<String>" \ ""] ")"
     <action> :: = "=" <actionAppl> <functionName>
                           "(" ["\" "<String>" \ ""] ")"
     <functionName> :: = <string>
     <actionAppl> :: = ">" | "<"
     <eltIndex> :: = <part> "." <index>
     <part> :: = "trigger" | "left" | "right"
     <index> :: = <integer>
     <item> :: = ["["] [<modifieurPre>] <unitéComplexe>
                           [<modifieurPost>] ["]"]
     <unitéComplexe> :: = <unitéSimple> | <Group> | <alternative>
     <group> :: = "(" <unitéComplexe> * ")"
     <alternative> :: = "(" <unitéComplexe> ("|" <unitéComplexe>) + ")"
     <unitéSimple> :: = ["&"] <motGénéralisé>
     <motGénéralisé> :: = <motSimple> [<class>] | <category> |
                           <class> | <Tstatus>
     <modifieurPre> :: = "^"
     <modifieurPost> :: = "?" | "{" <Cardinality> "-" <cardinality> "}"
     <cardinality> :: = <integer> | n '|' N '
     <motSimple> :: = <ChaîneSansBlanc> | "*"
     <category> :: = "$" <string>
     <class> :: = "@" <string>
     <Tstatus> :: = "" t_ "<string> | T_" <propriétéMorphologique>
     <propriétéMorphologique> :: = 'A' <MorphoAlphaCap> <MorphoAlphaRoman> |
                                  'N' <MorphoNumeric> | 'U' | 'P' | 'W' | 'S'
     <MorphoAlphaCap> :: = 'c' | 's' | '1' | 'a' | 'm'
     <MorphoAlphaRoman> :: = 'c' | 'o' | 'n'
     <MorphoNumeric> :: = 'i' | 'c' | 'd' | 'f' | 'o'
            
          

* * *dropoff Window

### 3.3. limitations

* * *dropoff Window

#### 3.3.1. Limits expressiveness rules

The formalism defined here do not express:

   * Grouping several properties for a single unit: the current formalism does not specify that a unit must be of several types at once (one can not express such a unit must be a word beginning with a upper and a proper name). One can only add that into external constraints.

   * The negation of a group of words: the negation of a complex group of words (estate or alternative) introduces additional problems because it is not just reflected in a distribution of the denial of property on each element (that because the current implementation)

     * For alternatives: _not expression (a | b) is interpreted as _ _ (not (a) | not (b)) _, which is false. The implementation of this function therefore also request a combination of properties at a unit;

     * For groups: the expression _not (ab) _ is _ interpétée like (not (a) not (b)) _ when it should be interpreted as _ (not (ab) | has not (b)) _ .dropoff window

   * The definition of sub-controllers would be a useful tool for the development of rules, but is not implemented.

* * *dropoff Window

### 3.4. Examples of rules

* * *dropoff Window

#### 3.4.1. Rules of named entity recognition

Here is an example of a few simple rules for the recognition of the names of French newspapers:
    
    
    
::: ORGANIZATION release:
     World:: Diplomatic: ORGANIZATION:
     World:: Education: ORGANIZATION:
     World: :: ORGANIZATION:
     Mail :: International: ORGANIZATION:
     :: Chained Duck: ORGANIZATION:
            

Another example of rules more complex for the recognition of names of people:
    
    
    
Firstname [(@ Title |FunctionTitle)?] ((De | da | the) T_A1?) {1-2}: PERSON: N_PERSON
     T_A1 [(@ Title |FunctionTitle)]: T_A1 {0-2}: PERSON: N_PERSON
     T_A1 (T_A1 | T_Amh) {0-2} :,FunctionTitle: PERSON: N_PERSON
            

The first rule is triggered on a name (the first names of the list is explicitly defined in the rules file), the left context contains optional way, a title (Mr., Mrs., Dr., ...) or function (Chairman, MP, ...), which is not kept in the final rule; the context must be composed of one or two words capitalized, possibly preceded by "of", "da" or "the".

The second recognizes the names of persons introduced by titles or names of functions, but without a name.

The third recognizes the names of people whose function is set after application (for example, "Sanjiv Sidhu, chairman of i2 Technologies").

Here is another example of rules for the recognition of dates, triggered on the names of days or months:
    
    
    
# Monday, May 22, 1968
     $ L_NC-L_NC_JOUR :: T_Ni> 1 <$ 31 L_NC-L_NC_MOIS (T_Ni> 1000 <3000 | T_Ni> 1 <99) ?: TIMEX: N_DATE
     # May 22, 1968
     # 18 June 40
     # October 31
     $ L_NC-L_NC_MOIS (T_Ni> 1 <31) :( next | last | next | (T_Ni> 1000 <3000 | T_Ni> 1 <99)) ?: TIMEX: N_DATE
            

* * *dropoff Window

#### 3.4.2. recognition rules idioms

Examples of rules for the recognition of French idioms:
    
    
    
& stop L_V $ $ L_PRON-L_PRON_REFLEXIFPronPrev [$?] :: IDIOM $ V: stop
     + AgreementConstraint (trigger.1, left.1 "PERSON")
     + AgreementConstraint (trigger.1, left.1, "NUMBER")
     as: to: and when (of | of): $ IDIOM Sg: as and when
     Iron: & Curtain :: IDIOM $ Nc:
            

where _ $ Sg_ indicates a general preposition.
Please help Google Translate improve quality for your language here.
Google Traduction pour les entreprises :Google Kit du traducteurGadget TraductionOutil d'aide à l'export
Désactiver la traduction instantanéeÀ propos de Google TraductionMobileCommunautéConfidentialitéAideEnvoyer des commentaires
