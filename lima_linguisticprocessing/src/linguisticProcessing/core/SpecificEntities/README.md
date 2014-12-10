Functions for entities normalization during rules application 
=============================================================

Entities normalization by assigning different attributes or
*features* (eg name / surname for people) is done heuristically
in functions (NormalizePersonName in the people example) which are applied to the result of a
rule. Normalization could also be done during the application of
rules by exploiting some elements that are known (for example, 
for the recognition of people names, we use a list of known surnames).

A first implementation of this functionality is implemented
with ConstraintFunction functions that are called during
the application of the rules.

Defined functions
-----------------

The following functions were defined:

- AddEntityFeature: stores a value for a given attribute. The
   value is defined by the element of the rule given by the first argument, the 
   attribute name is defined by the second argument (a string). The name can be 
   can be followed by the type of the attribute separated by a colon. 
   Currently supported types are int, double and string (which is the default).
   Attribute names are not controlled.

- NormalizeEntity: apply all the attributes stored in the created entity. 
  Calling this function is necessary for effectively using stored attributes.

- ClearEntityFeatures: empty the structure containing the stored attributes. 
   Calling this function is necessary if the rule fails, otherwise the
   stored attributes are kept for the next rule application and will be
   associated with the next (wrong) entity.

Example
-------

An example of a rule:

    @Firstname:[(@Title|@FunctionTitle)?]:@Initial? (T_A1|T_Amh|$NP){1-2}:PERSON:
    +AddEntityFeature(trigger.1,"firstname")
    +AddEntityFeature(left.1,"title")
    +AddEntityFeature(right.2,"lastname")
    =>NormalizeEntity()
    =<ClearEntityFeatures()

In this rule, it is indicated that the trigger will be used as attribute
"firstname". The previous word, if it is in a list of titles or
known functions will be used as the "title" attribute. Following words,
after an potential initial for the *middle name* will be associated with the attribute
"lastname". The *NormalizeEntity* action is performed if the rule applies entirely.:
This action assigns the retrieved attributes to the created entity.
The *ClearEntityFeatures* action is performed if the rule
fails. It deletes the stored attributes (if we do not do this action, the
attributes will be kept for the next recognized entity).

Where is it in the code?
------------------------

The three functions are defined in the SpecificEntityConstraints.[h/cpp] file. 

These functions are called during the application of the rules: at this time,
the recognized entity (RecognizerMatch) is not available and we therefore can not directly associate
attributes to it. The attributes are stored in a side structure
and will be associated with the entity created when the rule application
is over (and successful).

For temporary storage of attributes, you must put them in an
AnalysisData: We use the RecognizerData, which already exists and is
used to store temporary information on the application of the rules
(manages the changes of the graph). It is added to an
additional structure containing the attributes. The RecognizerData is a
structure that is used temporarily while the rules:
it is created at the beginning of the execution of an ApplyRecognizer process unit 
and removed at the end (so its lifetime is the application of a rules file).

The process is thus relatively simple:

- The function *AddEntityFeature()* adds an EntityFeature in this structure
   in RecognizerData.

- The function *NormalizeEntity()* copies this structure in the created RecognizerMatch:
   called as an action after the successful application of the rule (with '=>'), the
   RecognizerMatch exists at that time. It also empty the EntityFeatures structure
   (otherwise the attributes are retained for the next entity).

- The function *ClearEntityFeatures()* empties the EntityFeatures structure in
   the RecognizerData.


TODO
----

This is a first implementation. There are still things to do:

- The *NormalizeEntity* function only copies the attributes stored in
   the created entity. It is necessary to manage the integration of this
   functionality with the previously existing heuristic normalization 
   functions. This is Currently done for the *NormalizeDate* function only.
- Implement a version of *AddEntityFeature* that takes two arguments
   delimiting a string between two elements of the rule (two
   vertices).
- In the current version, a second call to the function AddEntityFeature
   overwrites the first stored attribute: is it enough or do we
   need to merge the values (concatenation)?

