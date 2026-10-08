# ModEx rules format

*Initial author: Romaric Besançon.* For a step-by-step introduction, start with
the [Writing a ModEx](../guides/writing-a-modex.md) guide.

## Goal

LIMA Modules of Extraction (or Modex) are used to recognize sequences of tokens in the text being analyzed and to take appropriate actions. They are composed of three kind of elements:
* A XML configuration file describing the types of the elements to find and several other parameters. See the dedicated page for details;
* Optionaly dynamicaly loadable libraries implementing rules constraints and actions;
* Binary representations of the rules allowing to recognize sequences of tokens.

The purpose of this document is the definition of the rules declarative format that is then compiled into the binary representation. This recognition is based on a format of regular expressions that allows the construction of automatons for the recognition of particular expressions. These automatons are used in linguistic processing for:
* Recognition of idioms;
* Recognition of specific entities such as numbers, dates and named entities;
* The dependency relationships extraction for parsing.

## Rules specification

### Needed rules expressiveness

The elements that the rules must be able to describe for the recognition of the targeted phenomenons are:
* A _trigger_: it is the element (token) that will initiate the recognition process when it appears in a text; the author should chose it as the least common element or the most characteristic expression considered, to avoid too many triggers that do not succeed. It is not necessarily at the beginning of the expression.
* Preceding and following _contexts_ defining the expressions around the trigger. We often use the terms "left" and "right" in reference to languages being read left to right (LTR). These contexts are defined using a formalism similar to regular expressions. The elements needed to define these contexts are:
* Recognition units: they can be virtually any property (or a combination of properties) associated with a token after morphological analysis or after disambiguation. In practice, the properties that we think are _a priori_ interesting for defining the rules are:
  * Simple words: recognition is then done on the surface form (direct form of the word in the text or one of its variant spellings obtained during the morphological analysis);
  * Grammatical categories alone, e.g. to take into account the inclusion of one or more adjectives;
  * Standardized forms of words: the correspondence will then be with all inflected forms of the worf (the grammatical category of the word must necessarily be specified, e.g. "door" will not accept the same inflections if noun or verb); 
  * Classes, which include a number of words or items (in a list). For example, proper names announcers like "Dr" or "Mr"; 
  * Other properties can also be considered, such as semantic or morphological traits (initial capital letter, for example), or constraints on numerical values;
  * Named entity types.
* Operations on these units (usual regular expressions operations):
  * Words grouping such as "de la" (in French) can be considered as a group to handle as a unit;
  * Alternatives for units or groups: "M." or "Sir";
  * Cardinalities on the occurrence of a unit or group: we can introduce one or two adverbs in a composed verbal form, but not more than three. We may also need to specify an unlimited cardinality (the limit will then be the limit of the sentence or text, depending on usage);
  * Optional units or groups: This property is a special case of cardinality (at least 0, at most 1 case), but it is kept for easier writing;
  * Negation of a unit or group: we may want to express a unit is matching if it misses some property (eg, any word except for a dot).
* The _type_ of the recognized expression;
* The _normalized form_ of the expression (for some entities whose standardized form needs to be computed, such as numbers or dates, a code to indicate the type of standardization may also be considered);
* The _boundaries of the recognized expression_ or the indication of what words among those recognized are not part of the recognized expression: triggers and contexts can help to recognize or describe an entity, without being part of this entity;
* Additional _constraints_ on some elements or between elements can have to be specified (_e.g._, gender, number or person agreement constraints);
* Optional indication of the expression head word. This indication is particularly useful for idioms. An idiom can indeed be flexed and linguistic properties associated with the expression as a whole will be those of the head of the expression (for example, the reflexive form _se trompait_ (French) should be recognized as a verbal form at the _imparfait de l'indicatif_ tense, the head word being _trompait_;
* An indication of the _application relativity_ of the rule is also useful idioms. This indication is intended to indicate whether the recognition of the rule is absolute (_e.g._ _au fur et  à mesure_ is _always_ an idiomatic expression) or if it is a possibility that will have to be disambiguated later (_rendez-vous_ may be a compound word or a verb followed by a pronoun);
* The possibility to indicate the negation of a specific entity type: if the rule applies then no other rules of the specified type will be applied (allows to better structure the rules and to bypass easily some problems).

## Header

Before rules, a header specifies some metadata
* Encoding of the file. For example: `set encoding=utf8`. Supported values are `utf8` and `latin1` (the default);
* Modex used by the rules. For example: `using modex DateTime-modex.xml,Numex-modex.xml`;
* Modex groups to use, avoiding to prefix their entities with the group name. For example: `using groups DateTime,Numex`;
* Default action setting. By specifying a default action, this one will automatically be added to all the rules. For example: `set defaultAction=>CreateSpecificEntity()`.

## Rules Format

The main separator of the rule is the colon: "_:_". A rule is defined by the following kind of expression:

    <trigger> ':' <left context> ':' <right context> ':' <expression type> ':' <standardized form>
    '+' < constraint >
    ...
    '=>' < action on success >
    ...
    '=<' < action on failure >

Both the _`<left context>`_ and _`<right context>`_ use the formalism defined in the previous section. Constraints express properties holding between tokens matched by the rule (_e.g._ gender or number agreement) and actions are things to be done in case  of success or failure (_e.g._ creating a names entity or a dependency relation).

### Automaton formalism for describing contexts

#### Single units

| **Units**                | **Examples**     |
| ------------------------ | ---------------- |
| Simple word              | `cats`           |
| Tag                      | `$NC`            |
| Lemma normalized form    | `cat$NC`         |
| Morphological Properties | `t_alphanumeric` |
| Classes                  | `@Surnames`      |
| Subautomatons            | `%NounGroup`     |
| Named entity types       | `<TypeName>`     |
| Any word                 | `*`              |

##### Grammatical categories (part of speech tags)

Grammatical categories (or part-of-speech tags or tags) are defined using the symbolic codes internal to the system. The category can specify a macro-category only or a macro-category/micro-category pair separated by a hyphen. Examples: `$NC` or `$NC-NNS` are correct tag specifications.

##### Classes

Classes are explicitly defined in the rules file or an external file with the list of elements of the class. The syntax for defining a class of words is simply `@class=(unit1,unit2,unit3,...,unitn)` The elements can be any unit defined in the previous section. The comma is the separator between the elements. There is no space after the comma but line breaks are allowed. For parsing, the definition of tags classes is possible. Note that very large classes (like thousands of place names for example) can be very efficiently parsed, but only when containing only simple strings. Very large classes containing categories or morphological properties will take a very long time to compile (due to a combinatorial explosion).

To load classes defined in an external file, use the `use` keyword with a relative path   to the file:

    use filename.classes
    
#### Subautomatons

One can define a pattern and use it in several rules. For example:

    define subautomaton NounGroup {
      pattern=$DET? ($ADV{0-2} $ADJ|$NC|$NP|$CONJ-CC){0-n} @Substantif
    }
    
    $PREP:%NounGroup:%NounGroup:SYNTACTIC_RELATION:
    ...

#### Named entity types

Entities can contain other entities. For example, a measure can be defined as a number followed by a unit. If we have the named entity types NUMBER, UNIT and MEASURE, we can write the rule:

    <NUMBER>::<UNIT>:MEASURE:

#### Morphological properties

Morphological properties are described by the types given to linguistic units by the tokenizer.

Type names are prefixed (by convention in the tokenizer) with `t_`. This prefix is used as to directly recognize the type names in the rules. These types correspond to the values in parentheses in the language tokenizer automaton.

For example, the types defined by the French tokenizer are:

|    |     |     |     |
|--- | --- | --- | --- |
| `t_acronym`       | `t_alphanumeric`   | `t_capital`         | `t_capital_1st`   |
| `t_capital_small` | `t_cardinal_roman` | `t_comma_number`    | `t_dot_number`    |
| `t_fraction`      | `t_integer`        | `t_ordinal_integer` | `t_ordinal_roman` |
| `t_pattern`       | `t_sentence_brk`   | `t_small`           | `t_word_brk`      |

#### Indication of the head of an expression

The head of an expression is indicated by the character _`&`_ before the unit identified as the head.

_Example: _

    abondance:&corne$NC d'::IDIOM:corne d'abondance

#### Constraints on numerical values

such constraints are particularly useful for the recognition of dates. They focus on digital forms of numbers and can indicate the desired specific value or range of desired values (between _m_ and _n_), with the following notation: `t_integer=n` or `t_integer>m<n`.

#### Operations on single units

The following table gives the possible operations. Elements noted _elt_ are either words or groups (sequences or alternatives).

| _Operations_ | |
| --- | --- |
| sequence (one after another) | `(elt1 elt2 ...)` | 
| alternative (one or the other) | `(elt1 \| elt2 \| ...)` |
| optional element | `elt?` |
| cardinality of the occurrence of an element between _i_ and _j_ times | `elt {i-j}` |
| cardinality of the occurrence of an element between _i_ and an infinite number of times | `elt{i-n} or (...){i-n}` |
| negation of a unit (the negation of a group is not handled, it should be avoided) | `^unit` |

#### Boundaries of the expression

Sometimes, one wants to use some tokens in the rules because they are useful to indicate a context but they must not be part of the recognized expression. This is indicated by surrounding these portions of expressions by square brackets `[...]`. These brackets can be placed around single units or complex expressions: in case of a complex expression, the whole expression must be enclosed in parentheses, as in the following example:

```
[(word1 word2 (@A|$NOUN))]
```
Failing to put the expression in parentheses will lead to a message error "got confused while reading expression".

The trigger itself can be excluded from the recognized expression. This is also indicating by placing brackets around it.

### Type of the expression

The list of possible types is defined in the Modex defintion XML file.

The type of the expression field may also contain additional information:
* Linguistic properties associated with the recognized term: these properties are useful when the recognition of the expression allows the creation of a new token (this is the case with idiomatic expressions). The combination of linguistic properties is done by adding the "$" sign after the type of the expression, followed by the code of linguistic properties (this must be a numeric code, but compilation scripts allows to use a symbolic code _à la_ Grace, like `IDIOM$Ncms`);
* Relativity of the rule application (also useful for idioms). Adding `ABS_` in front of the expression makes it absolute. It is always true regardless of context, then the new token will directly replace the recognized ones without letting the disambiguer make the choice;
* Negation of the type: Adding `NOT_` before the type of the expression is used to indicate that if the rule applies, then no other rule of the indicated type will be applied with this trigger.

Some of the types of expressions defined for the standard named entities are:

| | |
| --- | --- |
| `NUMBER` | for numbers |
| `TIME` | for hours |
| `PERSON` | for person names |
| `LOCATION` | for place names |
| `ORGANIZATION` | for names of organizations |
| `PRODUCT` | for product names  |
| `EVENT` | for events |

For idioms, only one type is defined: `IDIOM`.

For parsing, there is also only one type: `SYNTACTIC_RELATION`. The actual type of the relation is a parameter of the `CreateRelationBetween` action constraint. Here is a partial list of these types:

| | |
| --- | --- |
| `DETSUB` | Relationship between a determiner and asubstantive (the -> chat). Replaced now by the Universal dependency `det` |
| `ADJPRENSUB` | Relationship between prenominal adjective and noun (beau -> chat) |
| `COMPADJ` | Adjective complementer |
| `COMPADV` | Adverb complementer |
| `ADVADJ` | Relation between an adverb and the adjective it modifies |
| `ADVADV` | Relation between two adverbs |
| `SUBADJPOST` | Relationship between a noun and a postnominal adjective (chat <- noir) |
| `COMPDUNOM` | complementary relationship name (cat <\ - Pierre in "Cat Stone") |
| `SUBSUBJUX` | Two juxtaposed nouns in French |
| `TEMPCOMP` | Compound time (now replaced by the Universal Dependencies aux and auxpass) |

### Standardized form of the entity

The normalized form of the expression is simply given as a string.

### Constraints and actions

Constraints or actions may be attached to the expression recognition rules. Constraints can focus on one or two elements of the rule. They correspond to a function whose arguments are these one or two focus nodes. They return a boolean. Actions are functions called at the end of the application of the rule, depending on the success of the rule (if the term was recognized or not). They do not use the elements of the rule. They can use the result of the rule.

Note that constraints are applied during the rule testing. Thus, if they are supposed to have side effects, this side should be cancelable by the failure action and validated only by the success action. For example, in parsing, the `CreateRelationBetween` constraint prepares a new relation, which is really added by the `AddRelationInGraph` action canceled by the `ClearStoredRelations` action.

Following the rule expressions (or in the following lines), constraints begin with `+` and are written:

    +constraintName (elt1, elt2 "complement")

or

    +constraintName (elt, "complement")

In the first case, _elt1_ and _elt2_ on which the constraint should hold. The _complement_ (in double quotes) is optional and can be used to pass additional information to the function.

In the second case, _elt_  is the only element on which the contraint holds.

The elements are identified by their position in the rule, in two steps: First, the context, which can be _right_ (right context) _left_ (left context) or _trigger_ (the trigger) and then the position of the token in the context using a dot notation to recursively indicate the embedded element.

Example accessing the third element of the second element of the left context (the head noun of  the noun group):
    @Copule:@OpenQuot %NounGroup (@Adjectif){0-n} @ClosQuot:(@Adverb){0-2} @PastParticiple:SYNTACTIC_RELATION:
    +!GovernorOf(left.1,"ANY")
    +SecondUngovernedBy(left.2.3,right.2,"ANY")
    +CreateRelationBetween(left.2.3,right.2,"SUJ_V")
    =>AddRelationInGraph()
    =<ClearStoredRelations()

A constraint return true if and only if:
  * its element(s) is/are found;
  * its function returns true.

This means that a constraint will return false if refering to an absent optional element. For example, the following rule will **not** match the text `a c`:

    a:b? c::TYPE:
    +Constraint(left.1,"value")

The actions are defined following the constraints (or in the following lines) by a `=` followed by a `>` or `<` sign and the function name.
* `=>DoSomething()` indicates that the _DoSomething_ action will be executed in case of successful matching of the rule;
* `=<Otherwise() indicates that the _Otherwise_ action will be taken in case of failure.

As with constraints, a complement can be passed to the function.

## Other syntax elements

The use of an escape character (\\) allows to introduce in the definition of units the reserved characters of the format `() [] {} ^ | @ $ &`, without them being interpreted.

To allow to make more structured and readable rules files, the following syntax elements are defined:
* Lines starting with `#` are comments (a `#` not at the beginning of the line is not interpreted as a comment);
* `include <file name>` (or more file names, separated by commas) allows the inclusion of external rules files. These files are interpreted completely independently: for example, the classes defined in included files are not accessible in the including file;
* `use <file name>` (or more file names, separated by commas) allows to include definitions of external word classes;

## Rules formal specification

This section presents the EBNF grammar describing formaly the rules format.

     <definition> :: = {<rule>}
     <rule> :: = <trigger> ":" <leftContext> ""
                           <rightContext> ":" <type> ":" <normalizedForm>
                           [<constraint> *] [<action> *]
     <trigger> :: = ["["] <simpleUnit> ["]"]
     <leftContext> :: = {<item>}
     <rightContext> :: = {<item>}
     <type> :: = <string> [<class>]
     <normalizedForm> :: = <string>
     <constraint> :: = "+" <functionName> "(" <eltIndex> [, <eltIndex>]
                           [, "\" "<String>" \ ""] ")"
     <action> :: = "=" <actionAppl> <functionName>
                           "(" ["\" "<String>" \ ""] ")"
     <functionName> :: = <string>
     <actionAppl> :: = ">" | "<"
     <eltIndex> :: = <part> "." <index>
     <part> :: = "trigger" | "left" | "right"
     <index> :: = <integer>
     <item> :: = ["["] [<preModifier>] <complexUnit>
                           [<postModifier>] ["]"]
     <complexUnit> :: = <simpleUnit> | <Group> | <alternative>
     <group> :: = "(" <complexUnit> * ")"
     <alternative> :: = "(" <complexUnit> ("|" <complexUnit>) + ")"
     <simpleUnit> :: = ["&"] <generalizedWord>
     <generalizedWord> :: = <simpleWord> [<class>] | <category> |
                           <class> | <Tstatus>
     <preModifier> :: = "^"
     <postModifier> :: = "?" | "{" <cardinality> "-" <cardinality> "}"
     <cardinality> :: = <integer> | n '|' N '
     <simpleWord> :: = <NoSpaceString> | "*"
     <category> :: = "$" <string>
     <class> :: = "@" <string>
     <Tstatus> :: = "" t_ "<string> 

## Limitations

### Limits on rules expressiveness

The formalism defined here do not express:
* Grouping several properties for a single unit: the current formalism does not allow to  specify that a unit must be of several types at once (one can not express that a unit must be a word beginning with a uppercase letter and also a proper noun). One can only add that into constraints.
* The negation of a group of words: the negation of a complex group of words (succession or alternative) introduces additional problems because it cannot be (for a logic theory reason) handled just as the  application of the property negation on each element, but this is how it is currently implemented. Thus the result is undefined;
  * For alternatives: the expression _not(a | b)_ is interpreted as _(not(a) | not(b))_, which is false;
  * For groups: the expression _not(a b)_ is interpreted as _(not(a) not(b))_ when it should be interpreted as _(not(ab)|a not(b))_;

## Examples of rules

Here is an example of a few simple rules for the recognition of the names of French newspapers:
    
     Libération:::ORGANIZATION:
     Monde:Le:Diplomatique:ORGANIZATION:
     Monde:Le:de l'Éducation:ORGANIZATION:
     Monde:Le::ORGANIZATION:
     Courrier::International:ORGANIZATION:
     Canard::Enchaîné:ORGANIZATION:  

Another example of more complex rules for the recognition of names of people: 

     @Firstname:[(@Title|@FunctionTitle)?]:((de|da|le)? t_capital_1st){1-2}:PERSON:
    t_capital_1st:[(@Title|@FunctionTitle)]:t_capital_1st{0-2}:PERSON:

The first rule is triggered on a first name (the list of first names is explicitly defined in the rules file). The left context optionaly contains a title (Mr, Mrs, Dr, ...) or a function name (Chairman, MP, ...), which is not kept in the rule result. The right context must be composed of one or two capitalized words, possibly preceded by "de", "da" or "le".

The second rule recognizes the names of persons introduced by titles or function names, but without a first name.

You will find numerous other examples in the [lima_linguisticdata](https://github.com/aymara/lima/tree/master/lima_linguisticdata) subproject sources.
