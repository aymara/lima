Design of the rule based recognizer within Lima
===========================================

Object of the document
----------------------
This document aims at presenting the design of the rule based recognizer of expression of Lima.
This technical component is of main importance because it is used in the implementation of 3 different ProcessUnit:

 - the named entity recognizer (SpecificEntityRecognizer),
 - the syntactic dependency analyzer (SyntacticAnalyzerDeps) and
 - the identifier of idiomatic expression (idiomaticAlternatives).
 
All of them rely on the ApplyRecognizer.

Existing documents are:
---------------------
"Specifications du format des regles pour la construction d'automates de reconnaissance d'expressions" in specif_regles.pdf.
This document explains syntax and format of rules used within the different recognizer in Lima 
It is for use by people who wants to customize an existing Recognizer (change a set of existing rules) or create their own recognizer with a new set of rules.

"Specifications de la reconnaissance des entites nommees et de leur utilisation"
which presents the specification and performance of the named entity recognizer in Lima

These both documents must be read first.

Organisation of the code
------------------------
All the code is in the _lima\_linguisticprocessing_ project.
Data (rules) is in the _lima\_linguisticdata_ project.

The main components are:

 - Recognizer, Rule, Automaton, ConstraintFunction...(located in src/linguisticProcessing/core/Automaton): the data structure and the engine to run an automaton
 - ApplyRecognizer (located in src/linguisticProcessing/core/ApplyRecognizer): the wrapper to run an automaton as a ProcessUnit
 - SpecificEntitiesConstraint, SpecificEntitiesLogger, (located in src/linguisticProcessing/core/SpecificEntities): some specific classes to add additional features to generic classes from Automaton, and the wrapper ot run a Recognizer for NER
 - compile-rules, AutomatonCompiler,... (located in tools/automatonCompiler): compiler to interpret rules and build automaton


Overview of compilation process
------------------------------

The input of compilation process of a Recognizer is a file of rules specification, written in human readable way, and optionally one or more gazetteers. A gazetteer is a list of predefined elements of same type, each defined with a litteral.

The source file of a Recognizer includes optionally the definition of  gazetteers, and is mainly  composed of  a list of  rules.  A rule is decomposed in parts, each part specifies some regular expression.
Additionaly, some functions may be associated to elements of the rule.

The output of the compilation process is a Recognizer object, which is composed of a set of Rules objects,  a Rule is made of one or two Automaton, an Automaton is composed of TSates and TransitionUnit. Additionaly, different types of function (Constraint objects and Actions) are attached to Transition and Rule objects.

The compilation process follows in a top down manner the hierarchical decomposition from Recognizer down to Transition.

- compile-rule.cpp: compile-rule = utility, read a source file containing a set of rule, call RecognizerCompiler::buildRecognizer
- -> RecognizerCompiler: build a recognizer = set of rules with ressources (gazeteer)
     Read Modex definition file, load additional library (plugins), initialize Gazeteer: read gazeteer file
     Initialize Rule: call RuleCompiler::initRule
- -> RuleCompiler: build a RuleString from a string, create Constraints, and then create a Rule, call buildAutomaton from AutomatonCompiler
- -> AutomatonCompiler: build an Automaton (call recursively buildAutomaton), call createTransition in TransitionCompiler and Automaton::addTransition
- -> TransitionCompiler: build transitionUnit with Constraints attached to transition
- then, call writer.writeRecognizer to write output of compilation in a binary file.

Some objects like RuleString and AutomatonString are only intermediate by-products of the compilation process and are not saved in the binary file.

Constraints are only the specification of a ConstraintFunction. At run time, ConstaintFunction will be built and associated to objects which contains code to be executed during triggering of a rule.

High level initialisation process
-----------------------------
The input of initialisation process of a Recognizer is a binary file of rules.

The initialisation process follows the same way in a top down manner from Recognizer down to TransitionUnit and ConstraintFunction.
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
- ApplyRecognizer: process, select a search algorithm (whole graph ar sentence by sentence) and call Recognizer::apply on a set of vertex in the the graph
- Recognizer: apply, call testOnVertex and get results in RecognizerData
- Recognizer: testOnVertex, select rules that match trigger with current vertex, call testSetOfRules
- Recognizer: testSetOfRules, check constraint on trigger, call test of rule
- Rule: test, find best match in right part and left part
- Automaton: getBestMatch, getAllMatches, build a stackOfVertex and call testFromState
- Automaton::testFromState, call RtransitionUnit::checkConstraints,  call RecognizerMatch::addBackVertex
- TransitionUnit: checkConstraints, call checkConstraint and return false at first failure
- Constraint: checkConstraint, according to numlber of arguments, update the ConstraintCheckList

- RecognizerMatch::addBackVertex
  
ConstraintCheckList: datastructure containing a vertex (started) and a stack of vertices

  
Structure of Rule
-----------------
A rule is composed of 4 parts: the trigger, a left part, a right part and an action.

The trigger, left part and right part are regular expressions. Some additional elements are attached to a rule as decorator to define constraints or actions. Constraints may have one or more arguments. For exemple, in the rule:

    @Firstname: : T_Amh{1-2}:PERSON:
    +AddEntityFeature(trigger.1,"firstname")
    +AddEntityFeature(right.1,"lastname")
    =>NormalizeEntity()
    =<ClearEntityFeatures()

The different parts are:

 - trigger = @Firstname : match a token which is an element of predefined list of firstname)
 - left part = NIL : left part (nothing to be match a the left side of the trigger)
 - right part = T_Amh{1-2} : (1 or 2 token with an uppercase as first letter)
 - action = PERSON
 
There are two aditional constraints (AddEntityFeature) and one additional action (NormalizeEntity).

At compile time, a rule is read and is interpreted as the specification of an automaton to be built.
The automaton is a set of transitions connected to states.

                                 
----------------------

let's start with a more simple rule

@Nickname=(Abe,Nick)

@Nickname::$NP:PERSON:
+AddEntityFeature(right.1)
=>NormalizeEntity()
=<ClearEntityFeatures()


In this case, the first line defines a gazeteer, with a list of firstName.
The trigger element of the rule reference a gazeteer, so we duplicate the rule at compile time (one for each element of the gazetteer as trigger) and then we initialize each rule:

  RuleCompiler:initRule (init rule from string  "Abe::$NP:PERSON:+AddEntityFeature(right.1)=>NormalizeEntity()=<ClearEntityFeatures()=>CreateSpecificEntity()")

![](images/Autom1.gif?raw=true)

  
more details about compilation process
------------------------------
In a first step, initRule create a RuleString object (with 3 input arguments: a string, a set of gazeteer, a set of subAutomaton)
RuleString is a data structure which organize elements of the rule after parsing.
RuleString is an intermediate artefact of the compilation process.

RuleString constructor call RuleString::treatConstraints which call RuleString::addConstraint 
void RuleString::addConstraint(LimaString& constraint...) call Constraint constructor and add typed Constraint object (addAction, addUnaryConstraint, addBinaryConstraint..) and call Automaton insertConstraint.

In a second step, initRule create and initialize a Rule Object from the RuleString.
Rule is a final product of the compilation process, which will be saved in a binary file, an then loaded at run time.

  TransitionUnit* m_trigger; // the trigger
  Automaton m_left;  // left context (i.e. preceding context)
  Automaton m_right; // right context (i.e. following context)
  Common::MediaticData::EntityType m_type; // type of the expression
  LinguisticCode m_lingProp; // linguistic properties associated 
                             // to the recognized expression
  LimaString m_normalizedForm;      // normalized form of the expression
  uint64_t m_numberOfConstraints; // possible agreement constraints
  bool m_contextual; // is the expression ambiguous or not (ambiguity must 
                     // be solved by context if it is the case)

  bool m_negative; // the rule is negative
  bool m_hasLeftRightConstraint; // the rule has at least one 
                                 // binary constraint that deals with left AND right parts
  std::vector<Constraint> m_actions;

  double m_weight; // weight of the rule
  std::string m_ruleId; // id of the rule

Rule is made of a TransitionUnit object

During this second step, function AutomatonCompiler::buildAutomaton is called twice, one for the left part of the rule in BACKWARDSEARCH mode , and once for the right part of the rule in FORWARDSEARCH mode .

building automaton from :  "Abe"
building automaton from :  "$NP"
str=' "Abe::$NP:PERSON:=>NormalizeEntity()=<ClearEntityFeatures()=>CreateSpecificEntity()" ' position=' 15 ' next=' -1 ' 
left= none 
right= "$NP"
creating transition from string [ Abe ]
build automaton from  "$NP"
build non-optional automaton from  "$NP"
creating transition from string [ $NP ]
rule id is ' PERSON-fre.sample.rules:10 ' / filename= PERSON-fre.sample.rules ,lineNumber= 10
Adding rule no  0 ( PERSON-fre.sample.rules:10 ) : multiple trigger (first is  Abe )
creating transition from string [ Abe ]
creating transition from string [ Nick ]

After call to initRule, a transition is added to the recognizer with the trigger

Now, let's have a look to the Automaton buildAutomaton operation.

The first step is to creat an initial state of type Tstate.
The second step is to create other states transition and by calling in a kind of recursive way buildAutomaton operation
The thirs step is to set the final state
The fourth step to make the automaton determinist
If in BACKWARDSEARCH, the automaton is reversed
The last step is to minimize the automaton (Brzozowski algorithm)

inside the buildAutomaton operation, new state and transition are created.

TransitionUnit *transition = createTransition(automatonString,language,activeEntityGroups);
finalState = a.addState()
a.addTransition( initState, finalState, transition)

At last, createTransition is an operation of TransitionCompiler

Some details about Constraints and their arguments
---------------------------------------------------
We have seen where are created Constraint objects. During compilation process, RuleString constructor call RuleString::treatConstraints which call RuleString::addConstraint, which call Constraint constructor and add typed Constraint object (addAction, addUnaryConstraint, addBinaryConstraint..) and at least, call Automaton::insertConstraint.

We want to look at some details about management of arguments for these Constraints.

Let's have the folowing rule:

    @Firstname: : (@particule? T_Amh){1-2}:PERSON:
    +AddEntityFeature(right.1.1,"lastname")
    +AppendEntityFeature(right.1.2,"lastname")
    
The AddEntityFeature constraint will use values of vertices right.1.1 to set value of lastname property of the created named entity.<p>
The AppendEntityFeature constraint will use values of vertices right.1.2 to be concatenated to value of lastname property of the created named entity.<p>
During first phase of building of the ruleString object, we got:

  Augustin::(((de|le){0-1} T_Amh){1-2}:PERSON:+AddEntityFeature(right.1.1,"lastname")+AddEntityFeature(right.1.2,"lastname")

all Constraints are rejected at the end of the rule definition, as in the source file.<p>
Rule::treatConstraints consists in placing the constraints in the right place (near the elements which concerns the constraint)<p>
for this purpose, it reads each constraints one at a time and decode the arguments<p>
Suppose that constraint has only one argument as in the example, for exemple "right.1.2"<p>
The first part reference which part of the rule is concerns, the right automaton , so Automaton::addConstraint is called on the right part which is ((de|le){0-1} T_Amh){1-2}<p>

Then, from left to right, the index is decoded.<p>
Next numbering element is 1, which means first element in automaton, so Automaton::addConstraint is called on the first part which is (de|le){0-1} T\_Amh<p>
Next numbering element is 2, which means second element in sub automaton, so Automaton::addConstraint is called on the scond part which is T\_Amh<p>
Now we have found the right place, the constraint is append to the rule element. T\_Amh+AddEntityFeature(right.1.2,"lastname")<p>
After call the rule has been transformed in<p>
  "Augustin::(((de|le)+4294967295/5/AddEntityFeature/lastname{0-1} T_Amh+4294967295/5/AddEntityFeature/lastname){1-2}):PERSON:"
Which means:<p>
  "Augustin::(((de|le)+noIndex/TEST/AddEntityFeature/lastname{0-1} T_Amh+noIndex/TEST/AddEntityFeature/lastname){1-2}):PERSON:"

Let's have the folowing rule:

    @Firstname: : (@particule? T\_Amh){1-2}:PERSON:
    +AddEntityFeature(right.1.1, right.1.2,"lastname")
The AddEntityFeature constraint will use values of set of vertices from right.1.1 to right.1.2 to set value of lastname property of the created named entity.<p>
    
addConstraint(constraintIndex: 0 constraintName: AddEntityFeature storeAction: 1 )
addConstraint(constraintIndex: 0 constraintName: AddEntityFeature compareAction: 2 )

After call the rule has been transformed in<p>
"Augustin::(((de|le)+0/1/AddEntityFeature/lastname{0-1} T_Amh+0/2/AddEntityFeature/lastname){1-2}):PERSON:"
Which means:
"Augustin::(((de|le)+0/STORE/AddEntityFeature/lastname{0-1} T_Amh+0/COMPARE/AddEntityFeature/lastname){1-2}):PERSON:"

...

The analysis of text "Augustin Cebron de Lisle" will lead to call twice the function AddEntityFeature

Currently, arguments of constraints are operated this way: 
During compilation process, constraint are attached to transition.
When a constraint has two arguments, it is attached to more than one transition.

![](images/Autom2WithConstraints.gif?raw=true)


TransitionUnit.addConstraint

Low level execution process
----------------------------

At run time, we go through both the analysis graph and the automaton.

 - Constraints functions are called during this walkthrough.
 - During this walkthrough, vertexes are added to the RecognizerMatch
 - Action functions are called if final state is reached.

The RecognizerMatch is created with the current node as initial state (the node where the trigger consition is tested)

...
for example, inside SpecificEntitiesRecognizer::process
...


- the automaton of left part (in backward direction) is matched with graph from current node
- the automaton of right part is matches with graph from curent node.

We have seen that an automaton can be 'decorated' with function of two different types: 'actions' and 'constraints'.
constraints are applied upon initial state of transition
action are applied upon recognizerMatch (which node??)

Data structures
---------------

 -  |run time 			| compilation time		|
 -  |:---------------------------------------------|:------------------------------------|
 -  | Recognizer			|			|
 -  | Rule				| RuleString			|
 -  | Automaton			| AutomatonString		|
 -  | TRansitionUnit			| Transition and State		|
 -  | Constraint and ConstraintFunction	|Constraint			|


An automaton is a vector of states, each state is represented as a vector of Transition.
Automaton 

Additionaly there is a second data structure to hold action: the RecognizerMatch
bool addTransition(Tstate initialState, Tstate finalState, 
             TransitionUnit* transition);


RuleString is a data structure which decompose elements of the rule after parsing .

 -  AutomatonString m_trigger;    //< trigger : isUnit() must be true
 -  AutomatonString m_left;       /**< left context */
 -  AutomatonString m_right;      /**< right context */
 -  LimaString m_type;               /**< type of the rule  */
 -  LimaString m_norm;               /**< normalized form */
 -  int m_nbConstraints; /**< number of constraints in the rule */
 -  bool m_hasLeftRightConstraint; /**< indicates if at least one constraint is on left and right parts */
 -  std::string m_ruleId; /**< identifier of the rule (file + line number, for debug) */
 -  // possible actions attached to the rule (not to transitions)
 -  std::vector<Constraint> m_actions;

AutomatonString is a data structure which decompose an automaton inside the Rule

 -  LimaString m_unit;               /**< expression (when element is unit) */
 -  ElementType m_type;           /**< type of the element */
 -  std::vector<AutomatonString> m_parts; /**< parts of the expression (when 
 -                                           element is sequence or choice)*/
 -  // possible modifiers
 -  int m_minOccurrences;
 -  int m_maxOccurrences;
 -  bool m_keep; /**< the expression is kept in the recognized expression */
 -  bool m_negative; /**< the expression is negative */
 -  // possible constraints
 -  std::vector<Constraint> m_constraints;
 -  // if the sequence has been added to ease construction
 -  bool m_artificialSequence;
 -  // information kept on artifical split to handle
 -  // constraints on repetitive structures
 -  bool m_isSplittedFirst;
 -  bool m_isSplittedLast;



Todo
----
There are 2 kinds of function: 

- constraints, which can hold zero, one or two arguments,
- actions, without arguments.

We need to create actions with arguments.

 - Constraints are called during matching process. Constraint have been designed as conditions which must be fulfilled to pass a transition or to set the matching of the rule as a success.
 - Action are side-effect operation and are called when applying the rule (after the matching process, if it is a success).
 - During the matching process, vertices are added to (or removed from?) the RecognizerMatch.

Arguments in functions (constraint or actions) reference elements of the rule.
So we need identifier of rule element. Let's call them RuleElmtId.
These elements of rules are abstraction which specifies the transitions of the atomaton.
These transitions are mapped to vertices in the graph during the matching process.

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
We have to solve a combinatorial problem.

So we need to keep the information related to the ruleElement that was the source of the TransitionUnit  when building the automaton. This way, when we match a vertex in the graph during triggring process, we can associate a ruleElementId to it.

The idea is to add an argument of type RuleElmtId to the two functions addBackVertex and addFrontVertex.
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
                                              