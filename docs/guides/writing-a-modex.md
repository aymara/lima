# Writing a ModEx

## Introduction

A Modex ("Module d'Extraction") is a set of compiled regular
expression-like rules with their accompanying configuration file. It is
the base tool in Lima for various things, including idiomatic expression
recognizing and named entities extraction but also parsing in legacy languages. 
In UD-based pipelines, parsing uses deep learning models trained on Universal 
Dependencies corpora.
You can create you own Modexes to extract entities specific for your
application. For example, Twitter ids and Twitter hash tags are not
natively supported by Lima. So, if your application is targeted at
analyzing Tweets, then you will have to write your own Modex to extract
them.

## TwitterModex

### The configuration file

There is only one configuration file for all languages supported by the
Modex (e.g.: `Twitter-modex.xml`). It must be installed in your configuration directory listed in the 
`LIMA_CONF` environment variable (see [Configuring LIMA](../usage/configuration.md)). 
It contains three modules defining:

-   groups and entity types;
-   processing units (processUnit);
-   resources to use for each language;

#### Groups and Types

This first module, named "entities" contains a group for each entities
group and, in this group, the list (named entityList) of the entity
types. For example:

    <module name="entities">
        <group name="Twitter">
          <list name="entityList">
            <item value="TWITTERID"/>
            <item value="TWITTERHASH"/>
          </list>
        </group>
      </module>

#### Process units

This module, named Processors, defines the processing units groups
available for this Modex. These processing units can be pipelines (class
ProcessUnitPipeline), which allows to define a global process unit for
the Modex chaining up several rules application.

      <module name="Processors">
        <group name="TwitterModex" class="ProcessUnitPipeline" >
          <list name="processUnitSequence">
            <item value="TwitterRecognition"/>
          </list>
        </group>
        <group name="TwitterRecognition" class="ApplyRecognizer">
          <param key="automaton" value="TwitterRules"/>
          <param key="applyOnGraph" value="AnalysisGraph"/>
          <param key="useSentenceBounds" value="no"/>
        </group>
      </module>

As in the analysis configuration file, each process unit is defined in
its own group with its parameters. For an ApplyRecognizer process unit,
these parameters are:

-   automaton: the name of a resource defined later in the resources
    specific to each language (Cf. next section). Can be absent if "automatonList" is defined;
-   automatonList: a list of resource names defined later in the resources
    specific to each language. Ignored if the "automaton" parameter is defined;
-   applyOnGraph: declare the name of the analysis graph on which to
    apply the rules (AnalysisGraph if before part-of-speech tagging or
    PosGraph after). default to "PosGraph";
-   useSentenceBounds: (yes or no) defines if the automaton will be
    applied bbetween each sentence limits or on the whole graph. Use
    "no" if this Modex will be used before the
    "sentenceBoundariesFinder" process unit. Defaults to no;
-   updateGraph: (yes or no, optional) Defaults to no;
-   resolveOverlappingEntities: (yes or no, optional) Defaults to no;
-   overlappingEntitiesStrategy: (IgnoreSmallest (default), IgnoreFirst or IgnoreSecond, optional)
-   testAllVertices: (yes or no, optional) Defaults to no;
-   stopAtFirstSuccess: (yes or no, optional) Defaults to yes;
-   onlyOneSuccessPerType: (yes or no, optional) Defaults to no;
-   storeInData: (default to empty string)

The details of each process unit inputs, outputs, dependencies and configuration are described in their [reference documentation page](../reference/process-units/index.md).

#### Resources

The resources definition modules for each language are called
resources-xyz, with xyz the language trigram:
`<module name=“resources-xyz”>`. They contain a group for each automaton
defined above. This group, of the class AutomatonRecognizer defines the
extractor parameters and particularly the path to the compiled rules
file (relative to the global Lima resources directories listed in `LIMA_RESOURCES`):

    <group name="TwitterRules" class="AutomatonRecognizer">
          <param key="rules" value="Twitter/Twitter-eng.bin"/>
    </group>

Next, the module contains groups to define the microcategories (the fine-grained part-of-speech tags of the language, e.g.
`PROPN` for UD pipelines, `NNP` for legacy English or `NPP` for legacy French) that
will be affected to the token that will replace each recognized entity.
The name of each of these groups must be the one of the corresponding
group in the entities module concatenated to the string "Micros". It
contains a list of microcategories for each entity. This list is named
by the fully qualified name of the entity (\<Group name\>.\<Entity
name\>):

    <group name="TwitterMicros" class="SpecificEntitiesMicros">
          <list name="Twitter.TWITTERID">
            <item value="PROPN"/>
          </list>
    </group>

#### A complete configuration file

    <?xml version='1.0' encoding='UTF-8'?>
    <modulesConfig>
      <module name="entities">
        <group name="Twitter">
          <list name="entityList">
            <item value="TWITTERID"/>
            <item value="TWITTERHASH"/>
          </list>
        </group>
      </module>
      <module name="Processors">
        <group name="TwitterModex" class="ProcessUnitPipeline" >
          <list name="processUnitSequence">
            <item value="TwitterRecognition"/>
          </list>
        </group>
        <group name="TwitterRecognition" class="ApplyRecognizer">
          <param key="automaton" value="TwitterRules"/>
          <param key="applyOnGraph" value="AnalysisGraph"/>
          <param key="useSentenceBounds" value="no"/>
        </group>
      </module>
      <module name="resources-eng">
        <group name="TwitterRules" class="AutomatonRecognizer">
          <param key="rules" value="Twitter/Twitter-eng.bin"/>
        </group>
        <group name="TwitterMicros" class="SpecificEntitiesMicros">
          <list name="Twitter.TWITTERID">
            <item value="PROPN"/>
          </list>
          <list name="Twitter.TWITTERHASH">
            <item value="PROPN"/>
          </list>
        </group>
      </module>
    </modulesConfig>

### The rules files

The full syntax of rules files is described on the
[Modex Rules Format](../reference/modex-rules.md) page. Here, we just describe the following
example:

    set encoding=utf8
    using modex Twitter-modex.xml
    using groups Twitter
    set defaultAction=>CreateSpecificEntity()

    #----------------------------------------------------------------------
    # recognition of Twitter ids
    #----------------------------------------------------------------------

    @arobase=(\@)

    @arobase::*:TWITTERID:

    \#::*:TWITTERHASH:

The first four lines are metadata stating that the file is encoded in
UTF-8, that it is a rules file for the Twitter Modex, that the entities
created by rules will belong to the Twitter group and finally that by
default, the action associated to the rules will be to create a specific
entity.

Next comes a line describing a class of tokens, here just the tokens
composed of the arobase character. After this line, there is two rules,
one triggered by the encountering of an arobase and matching any token
after it. If matching, a TWITTERID entity is created. The second one has
the same format but triggered by a hash character token and creating a
TWITTERHASH entity.

Please refer to the full syntax description for details, but let's say
here that rules are defined by a triggering token, followed by a regular
expression describing the left context of the triggering token and a
second one describing its right context, followed by the type of the
expression and possibly constraint functions.

When the rules file is ready, you have to compile it with the following
command (don't forget to install the configuration file beforehand):

`compile-rules --language=eng --modex=Twitter-modex.xml -oTwitter-eng.bin Twitter-eng.rules `

Then copy the binary file to a `Twitter` folder in your resources directory listed in 
the `LIMA_RESOURCES` environment variable.

### Using your new Modex

In the analysis configuration file (`lima-lp-xyz.xml`, copy it from the 
system folder to your configuration folder as described above), a Modex is
included by including explicitly its processings and its resources :

    <module name="entities">
      <group name="include">
        <list name="includeList">
          <item value="Twitter-modex.xml/entities"/>
        </list>
      </group>
    </module>
    <module name="Processors">
        <group name="include">
          <list name="includeList">
            <item value="Twitter-modex.xml/Processors"/>
          </list>
        </group>
      ...
      </module>
       <module name="Resources">
        <group name="include">
          <list name="includeList">
            <item value="Twitter-modex.xml/resources-xyz"/>
          </list>
        </group>
        ...
     </module>

The Modex process unit(s) can then be called in the various pipelines.
