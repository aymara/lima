Design of the rule based recognizer within Lima
===========================================

Object of the document
----------------------
This document aims at presenting the design of the rule based recognizer of expression of Lima. The name of the main class of this module is ApplyRecognizer (we will use sometimes recognizer).
This technical component is of main importance because it is used in the implementation of 3 different ProcessUnit:

 - the named entity recognizer (SpecificEntityRecognizer),
 - the syntactic dependency analyzer (SyntacticAnalyzerDeps) and
 - the identifier of idiomatic expression (idiomaticAlternatives).
 
All of them rely on the ApplyRecognizer.

Other existing documents
---------------------
- "Specifications du format des regles pour la construction d'automates de reconnaissance d'expressions" in specif_regles.pdf.
This document explains the syntax and the format of rules used within the different recognizer in Lima 
It is for use by people who wants to customize an existing recognizer (change a set of existing rules) or create their own recognizer with a new set of rules.

- "Specifications de la reconnaissance des entites nommees et de leur utilisation"
which presents the specification and performance of the named entity recognizer in Lima

These both documents must be read first.

Organisation of the code
------------------------
All the code is in the _lima\_linguisticprocessing_ project.
Data (rules) is in the _lima\_linguisticdata_ project.

The main (C++) classes are:

 - Recognizer, Rule, Automaton, ConstraintFunction...(located in src/linguisticProcessing/core/Automaton): the data structure and the engine to run an automaton
 - ApplyRecognizer (located in src/linguisticProcessing/core/ApplyRecognizer): the wrapper to run an automaton as a ProcessUnit
 - SpecificEntitiesConstraint, SpecificEntitiesLogger, (located in src/linguisticProcessing/core/SpecificEntities): some specific classes to add additional features to generic classes from Automaton, and the wrapper to perform named entity recognition (NER)
 - compile-rules, AutomatonCompiler,... (located in tools/automatonCompiler): compiler to interpret rules and build automaton


Overview of compilation process
------------------------------

The input of compilation process of a Recognizer is a file of rules specification, written in human readable way, and optionally, one or more gazetteers and a modex file. A modex fil contains the definition of the types to be recognized. A gazetteer is a list of strings which defines the closed (exhaustive) set of values for a specific type (for example a set of values of first name in english).

The source file of a Recognizer includes optionally the definition of  gazetteers, and is mainly composed of  a list of  rules.  A rule is composed of parts, each part is made of a regular expression.
Each regular expression is the specification of an automaton (a transducer). A regular expression is made of "elements" which describe the transition of the automaton. Additionaly, some functions may be associated to one or more elements of the rule.

The output of the compilation process is a Recognizer object, which is composed of a set of Rule objects (instances of the class Rule), a Rule is made of one or two Automaton, an Automaton is composed of TSates and TransitionUnit. Additionaly, different types of function (Constraint objects and Actions) are attached to Transition and Rule objects.

The compilation process follows in a top down manner the hierarchical decomposition from Recognizer down to Transition. (Some objects like RuleString, Gazeteer and AutomatonString are only intermediate by-products of the compilation process and are not saved in the binary file).

A Gazeteer is a set of strings, which are the possible values for a type, subtype or component. For example the set of title or firstname in a language.

- compile-rule.cpp: utility code to build compile-rule binary utility = main entry of the compilation process, read a source file containing a set of rule, call RecognizerCompiler::buildRecognizer
- -> RecognizerCompiler: build a recognizer = set of rules with ressources (gazeteer)
     - Read the modex definition file, load additional library (plugins), initialize Gazeteer: read gazeteer file
     - Initialize Rule: call RuleCompiler::initRule
- -> RuleCompiler: build a RuleString from a string, create Constraints, and then create a Rule, call buildAutomaton from AutomatonCompiler
- -> AutomatonCompiler: build an Automaton (call recursively buildAutomaton), call createTransition in TransitionCompiler and Automaton::addTransition
- -> TransitionCompiler: build transitionUnit with Constraints attached to transition
- then, call writer.writeRecognizer to write output of compilation in a binary file.

Constraints are stored as the identifier of a ConstraintFunction. At run time, ConstraintFunction will be built and associated to objects which contains code to be executed during triggering of a rule.

High level initialisation process
-----------------------------
The input of initialisation process of a Recognizer is a binary file of rules.

The initialisation process follows the same hierachy of objects as in the compilation down manner from Recognizer down to TransitionUnit and ConstraintFunction.
Factory functions (readXXX) are called as data are read from input file.

- ApplyRecognizer: processUnit, create a Recognizer
- Recognizer: call AutomatonReader::readHeader and AutomatonReader::readRecognizer at init time
 - readHeader, read set of constraint functions and build map of constraint functions indexed by name (readRegisteredConstraints)
 - readRecognizer call readRule
 - readRule: call readAutomaton
 - readAutomaton: call readTState for terminal states, and for each state, create new state, create Transition from this state, read TransitionUnit
 - readTransitionUnit, create transition of different types according to read type, ant then read properties and constraints
 - readTstate

High level execution process
-----------------------------
- ApplyRecognizer: process, select a search algorithm in the graph (whole graph or sentence by sentence) and call Recognizer::apply on the set of vertices that algorithm has reached
- Recognizer: apply, call testOnVertex (which try each rule in a set of rule) and get results in RecognizerData
- Recognizer: testOnVertex, select rules that match trigger with current vertex, call testSetOfRules
- Recognizer: testSetOfRules, check constraint on trigger, call test of rule
- Rule: test, find best match in right part and left part (froim current vertex, select vertices in graph )
- Automaton: getBestMatch, getAllMatches, build a stackOfVertex and call testFromState
- Automaton::testFromState, call RtransitionUnit::checkConstraints,  call RecognizerMatch::addBackVertex
- TransitionUnit: checkConstraints, call checkConstraint and return false at first failure
- Constraint: checkConstraint, according to numlber of arguments, update the ConstraintCheckList

- RecognizerMatch::addBackVertex
  
ConstraintCheckList: datastructure containing a vertex (started) and a stack of vertices

  
Structure of Rule
-----------------
A rule is composed of 4 parts: the trigger, a left part, a right part and an action.

The trigger, left part and right part are regular expressions. Some additional elements are attached to a rule as decorator to define constraints or actions. Constraints and actions may have one or more arguments. For exemple, in the rule:

    @Firstname: : T_Amh{1-2}:PERSON:
    =>SetEntityFeature(trigger.1,"firstname")
    =>SetEntityFeature(right.1,"lastname")
    =>NormalizeEntity()
    =<ClearEntityFeatures()

The different parts are:

 - trigger = @Firstname : match a token which is an element of predefined list of firstname)
 - left part = NIL : left part (nothing to be match a the left side of the trigger)
 - right part = T_Amh{1-2} : (1 or 2 token with an uppercase as first letter)
 - action = PERSON
 
There are three aditional action (two SetEntityFeature action and a NormalizeEntity action).

At compile time, a rule is read and is interpreted as the specification of an automaton to be built.
The automaton is a set of transitions connected to states.

                                 
more details about compilation of rules: RuleString, gazeteer, Rule
-------------------------------------------------------------------

let's start with a more simple rule

    @Nickname=(Abe,Nick)

    @Nickname::$NP{1-2}:PERSON:
    =>AddEntityFeature(right.1)
    =>NormalizeEntity()
    =<ClearEntityFeatures()

In this case, the first line defines a gazeteer, with a list of values of firstName.
Gazetteers and subautomatons are objects created at compiletime within the recognizerCompiler and are not saved by themselves.

In our simple rule, the trigger element of the rule references a gazeteer.

When the source code of the rules is read, a string is built with all elements which specifies the rule:

    "Abe::$NP{1-2}:PERSON:=>AddEntityFeature(right.1)=>NormalizeEntity()=<ClearEntityFeatures()=>CreateSpecificEntity()"

Then, RuleCompiler::initRule() is called with this string as argument.

In a first step, initRule create a RuleString object (with 3 input arguments: a string, the set of gazeteers, the set of subAutomaton)
RuleString is a data structure which organize elements of the rule after parsing.
RuleString is an intermediate artefact of the compilation process.

RuleString constructor call RuleString::treatConstraints which call RuleString::addConstraint 

void RuleString::addConstraint(LimaString& constraint...) call Constraint constructor and add typed Constraint object (addAction, addUnaryConstraint, addBinaryConstraint..) and call Automaton::insertConstraint.

In a second step, initRule create and initialize a Rule Object from the RuleString.
Rule is a final product of the compilation process, which will be saved in a binary file, an then loaded at run time.

Rule is made of a TransitionUnit object

During this second step, function AutomatonCompiler::buildAutomaton is called twice, one for the left part of the rule in BACKWARDSEARCH mode , and once for the right part of the rule in FORWARDSEARCH mode .

![](images/Autom1.png?raw=true)

At compile time, Rules are indexed with their trigger within the Recognizer.
So at run time, the trigger part of the set of rules is tested only once, and all rules indexed with the same trigger are selected.

In our case, a transition is built with the first element of the trigger ("Abe").

Then, an automaton is built with the right part "$NP" (left part is empty)
The rule with the left and the right part automaton is built.

Then the rule is indexed with the trigger "Abe".
Then other transitions are created with the other values of the gazetteer ("Nick") to build triggers.
The same rule is indexed with the other valures of the trigger.

More details about Gazeteer
---------------------------
Gazeteer are used as a synthetic way to use enumeration in the specification of a rule, and especially when we use alternatives among predefined values. For example we define what is a firstname and we use this definition in the specification of a trigger or anywhere else:

    @Firstname={Abe,Luciano,Khaled}
    @Firstname::@Firstname? $NP{1-2}:PERSON:=>NormalizeEntity()=<ClearEntityFeatures()

This specification enable to specify the rule in a more synthetic way than using directly the list of possible values of firstname: like 

    Abe|Luciano|Khaled::(Abe|Luciano|Khaled)? $NP{1-2}:PERSON:=>NormalizeEntity()=<ClearEntityFeatures()

Is is possible to define the list of values of a Gazeteer in a separate file which enable easier maintenance operations:

    use firstname.lst
    @Firstname::@Firstname? $NP{1-2}:PERSON:=>NormalizeEntity()=<ClearEntityFeatures()
    
firstname.lst is a file which contains the definition of the gazeteer and is used as an include file
    @Firstname={Abe,Luciano,Khaled}
    
Gazetteer are considered as subautomaton.

![](images/Gazeteer1.png?raw=true)
    
Gazeteer seems to be managed as atomic element of a rule but it is wrong as we will see in the next example

The gazeteer Firstname shows only 2 states and there are as many transitions as different values in the gazeteer.

Gazeteer may contain multi-term string, like in:

    @Title={Admiral,1st Lieutenant,General}

"1st Lieutenant" is made of two terms and the subautomaton built from this gazeteer is made of more than two states.

![](images/Gazeteer2.png?raw=true)

Gazeteer are built with RecognizerCompiler::readGazeteers()
Gazeteers has a member of type AutomatonString (of subtype ALTERNATIVE) which owns the list of parts read from the Gazeteer definition.

RecognizerCompiler::readGazeteers() call Gazeteer::buildAutomatonString() which call AutomatonString::parse
During parsing, AutomatonString::parse considers each part ("Admiral","1st Lieutenant","General") and recursively call AutomatonString::parse for each part. According to presence/absence of space character, AutomatonString::parse considers "Admiral" part as a UNIT AutomatonString and "1st Lieutenant" part as a SEQUENCE and builds an AutomatonString of subtype SEQUENCE from "1st Lieutenant"

Gazeteers are used in addRuleWithGazeteerTrigger() operation and AutomatonString::parseUnit

More details about compilation of a rule with gazeteeer as trigger
-------------------------------------------
Lest have the simple rule 
    @Title={Admiral,1st Lieutenant,General}
    
    @FunctionTitle::George Bush:PERSON:

When a rule is read with a gazeter as trigger, the special function RecognizerCompiler::addRuleWithGazeteerTrigger() is called.
This function use gazeteer in a special way.
First, an artificial RuleString is created with a simple word transition as trigger. The word is the first element of the gazeteer. The artificial RuleString in our example is:

    Admiral::George Bush:PERSON:

A rule r is created from this Rulestring.
Then, for each element in the gazeteer, a simple transition t is created and the pair (t,r) is indexed in the recognizer.
This way, we compile only once the rule, and we index it multiple time, once for each value of the gazeteer.

Unfortunately, this does not work when the gazeteer contains multiple terms element. (for exemple "1st Lieutenant")


More details about compilation of automaton
-------------------------------------------
Now, let's have a look to the Automaton buildAutomaton operation.

The first step is to create an initial state of type Tstate.
The second step is to create other states transition and by calling in a kind of recursive way the buildAutomaton operation
The third step is to set the final state
The fourth step to make the automaton determinist
If in BACKWARDSEARCH, the automaton is reversed
The last step is to minimize the automaton (Brzozowski algorithm)

inside the buildAutomaton operation, new states and transitions are created.

TransitionUnit *transition = createTransition(automatonString,language,activeEntityGroups);
finalState = a.addState()
a.addTransition( initState, finalState, transition)

Some details about functions (Constraints and Actions) and their arguments
---------------------------------------------------
We have seen which class creates function objects and when.
During compilation process, RuleString constructor call RuleString::treatConstraints which call RuleString::addConstraint, which call Constraint constructor and add typed Constraint object (addAction, addUnaryConstraint, addBinaryConstraint..) and at least, call Automaton::insertConstraint.

  - Constraint are operation which are called when a rule is checked against the vertices in the graph
  - Action are operation which are called when the rule is triggered (executed)

We want to look at some details about management of arguments for these Constraints.

Let's have the folowing rule:

    @Firstname: : (@particule? t_capital_1st){1-2}:PERSON:
    =>AddEntityFeature(right.1.1,"lastname")
    =>AppendEntityFeature(right.1.2,"lastname")
    
The AddEntityFeature constraint will use values of vertices right.1.1 to set value of lastname property of the created named entity.<p>
The AppendEntityFeature constraint will use values of vertices right.1.2 to be concatenated to value of lastname property of the created named entity.<p>
During first phase of building of the ruleString object, we got:

    Abe::(((de|le){0-1} t_capital_1st){1-2}:PERSON:+AddEntityFeature(right.1.1,"lastname")+AddEntityFeature(right.1.2,"lastname")

all functions are rejected at the end of the rule definition, as in the source file.<p>
Rule::treatConstraints consists in placing the functions in the right place (near the elements which concerns the functions ), in order to associate the function to the right transition in the automaton<p>
For this purpose, Rule::treatConstraints reads each functions one at a time and decode the arguments<p>
Suppose that functions has only one argument as in the example, for exemple "right.1.2"<p>
right.1.2 is a what we call a RuleElementId.
The first component of a RuleElementId references which part of the rule it deals with: in our case the right automaton.<p>
So Automaton::addConstraint is called on the right part which is ((de|le){0-1} t\_capital\_1st){1-2}<p>

Then, from left to right, the RuleElementId is decoded.<p>
Next numbering element is 1, which means first element in the automaton, so Automaton::addConstraint is called on the first part which is (de|le){0-1} t\_capital\_1st<p>
Next numbering element is 2, which means second element in sub automaton, so Automaton::addConstraint is called on the second part which is t\_capital\_1st<p>
Now we have found the right place, the constraint is append to the rule element. t\_capital\_1st+AddEntityFeature(right.1.2,"lastname")<p>
After call the rule has been transformed in<p>
  "Abe::(((de|le)+4294967295/5/AddEntityFeature/lastname{0-1} t\_capital\_1st+4294967295/5/AppendEntityFeature/lastname){1-2}):PERSON:"
Which means:<p>
  "Abe::(((de|le)+noIndex/TEST/AddEntityFeature/lastname{0-1} t\_capital\_1st+noIndex/TEST/AppendEntityFeature/lastname){1-2}):PERSON:"

Action with more than one argument are not currently managed:

    @Firstname: : (@particule? T\_Amh){1-2}:PERSON:
    +AddEntityFeature(right.1.1, right.1.2,"lastname")

The analysis of text "Abe Cebron de Lisle" will lead to call twice the function AddEntityFeature

Currently, arguments of constraints are operated this way: 
During compilation process, constraint are attached to transition.

![](images/Autom2WithConstraints.png?raw=true)


TransitionUnit.addConstraint

Low level execution process
----------------------------

The execution of a rule is performed in two steps.
The first step is to select the rule (Rule:test), the second step is to execute the rule.

During the first step (Rule::test), the rule:

  - checks if the left automaton can match nodes at the left side of the current token 
  - checks if the right automaton can match nodes at the right side of the current token
  

function Automaton::getBestMatch test longuest match among all matches 
function Automaton::getAllMatches get all matches
Automaton::testFromState find match and return complete match (final state has been reached)

An automaton is a vector of states, each state is represented as a vector of Transition.

Additionaly there is a second data structure usefull to perform side-effects: the RecognizerMatch
The RecognizerMatch is created with the current node as initial state (the node where the trigger condition is tested). 

Automaton::testFromState 
The Automaton walks through both the analysis graph (from one token to next token) and through the automaton (from one state to the next state through transition), and check if the next token match the transition. It rely on a stack of pair(state,vertex).
The stack is initialized with the firstState and the current vertex
For each transition, function TransitionUnit::checkConstraints is called.

TransitionUnit::checkConstraints: test if constraint associated to the transition are verified with the current vertex. If it is the case, the vertex is added to the RecognizerMatch

- from the current state to the next state through transition

 - Constraints functions are called during this walkthrough.
 - During this walkthrough, vertices are added to the RecognizerMatch
 - Action functions are called if final state is reached.


- the left part automaton is matched with graph from current node in backward direction.
- the right part automaton is matcheed with graph from curent node in forward direction.

We have seen that an automaton can be 'decorated' with function of two different types: 'actions' and 'constraints'.
constraints are applied upon initial state of transition
action are applied upon recognizerMatch (which node??)

Data structures
---------------

Todo
----
There are 2 kinds of function: 

- constraints, which can hold zero, one or two arguments,
- actions, can hold zero or one argument.


So we need to manage these identifier of transition when adding vertices to the recognizerMatch
A rule is structured, so one way to identify a rule elment is to follow this structure.
This is the way we identify rule elments in source (in the specification of the rule).
For example, in the folowing rule: 

    @Firstname: : (@particule? T_Amh){1-2}:PERSON:

 - trigger.1 reference the element @Firstname
 - right.1.1 references the element @particule?
 - right.1.2 references the element T_Amh

We try to keep this numbering during execution, so that it will be easier to interpret the log.
RuleElmtId is a string whose value references a rule element like "trigger.1" or "right.1.2".

We need to manage this numbering as a property for each vertex added to the recognizermatch.
This way, after completion of triggering process, the recognizermatch is completed with a list of vertices, each vertex is informed of the rule element which have made it selected.
When we execute the rule, we have to execute each action.
For each action, each argument references a ruleElementId. Each ruleElementId is associated to a set of vertices.
The action is called upon the combination of vertices associated to the different ruleElementId which compose the arguments of the action.

(In case of more tha one argument, we have to solve a combinatorial problem.)

So we need to keep the information related to the ruleElement that was the source of the TransitionUnit  when building the automaton. This way, when we match a vertex in the graph during triggering process, we can associate a ruleElementId to it.

The idea is to add an argument of type RuleElmtId to the two functions addBackVertex and addFrontVertex.
When a transition is done, the RuleElmtId and the vertex is added to the RecognizerMatch.

This numbering must be transferred at compile time, and within I/O operations.
Recognizer::testSetOfRules 
 - call Rule::test  (find best match on right part and left part
 build complete match (add one argument = elmtOfRuleId? to addBackVertex...)
 Permet de construire une liste de vertex accompagné d'un elmtOfRuleId
 
 - then actionSuccess = currentRule->executeActions(graph, analysis,
                                                  constraintCheckList,
                                                  success,
                                                  match);
La liste des action est executée, pour chaque action, on retrouve les vertex concerné à partir des elmtOfRuleId
                                              