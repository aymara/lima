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

    @Firstname:[(@Title|@FunctionTitle)?]:(@Initial .?)? (T_A1|T_Amh|$NP){1-2}:PERSON:
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

This is a first implementation.
Suppose we wants to apply the following rule:

    @Firstname:[(@Title|@FunctionTitle)?]:(@Initial .?)? (T_A1|T_Amh|$NP){1-2}:PERSON:
    +AddEntityFeature(trigger.1,"firstname")
    +AddEntityFeature(left.1,"title")
    +AddEntityFeature(right.1.1, right.1.2,"firstname")
    +AddEntityFeature(right.2,"lastname")
    =>NormalizeEntity()
    =<ClearEntityFeatures()
 
There are still things to do:
    
- Implement a new function *AugmentEntityFeature* that takes the same argument as function
  *AddEntityFeature()* but concatenate the value with a previously stored value if it already exists. The semantic of AddEntityFeature is to replace the value of already existing feature.

- Implement a version of *AddEntityFeature* that takes two arguments
   delimiting a string between two elements of the rule (two
   vertices). Not sure it is usefull...
   The default behavior of constraints is that the rule is not applied when one node does not exist. This is the case when one node is an optional premise as in the following example:
    *Gaspard P. Monge* is OK because right.1.1 and right.2 nodes both exist.
    *Gaspard Monge* is not OK because right.1.1does not exist, so constraint failed and rules is not applied.

- In the current version, a second call to the function AddEntityFeature
   overwrites the first stored attribute: is it enough or do we
   need to merge the values (concatenation)?

- The *NormalizeEntity* function only copies the attributes stored in
   the created entity. It is necessary to manage the integration of this
   functionality with the previously existing heuristic normalization 
   functions. This is Currently done for the *NormalizeDate* function only.

- Enable AddEntityFeature to be called after triggering the rule.

As explained in the description of the process, the function AddEntityFeature is called during the application of the rules. When there is optional premise in the rule (a? or b*), the case exist where there is more than one possible match.

for example:

*Gaspard P Monge* can match *P* as initial and *Monge* as lastname or *P* as lastname and *Monge* as lastname.

The rule is applied to the longuest match (specification). But during the triggering, the function *adEntityFeature* is called more than one time.

During the triggering, function *adEntityFeature* is called more than one time.
Once with the parameter P as firstname and a second one with the parameter P as lastname.

The better is to be able to call function *adEntityFeature* as a side effect of applying the rule, only for the node actuallly matching the rule as in the following example:

    @Firstname:[(@Title|@FunctionTitle)?]:(@Initial .?)? (T_A1|T_Amh|$NP){1-2}:PERSON:
    =>AddEntityFeature(trigger.1,"firstname")
    =>AddEntityFeature(left.1,"title")
    =>AddEntityFeature(right.1.1, right.1.2,"firstname")
    =>AddEntityFeature(right.2,"lastname")
    =>NormalizeEntity()
    =<ClearEntityFeatures()
 
