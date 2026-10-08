# Configuration files

Configuration files are searched in the directories listed in
[Configuring LIMA](../usage/configuration.md#search-paths), which also explains
how to override them and how to debug the search with `LIMA_SHOW_CONFIG_PATH`.

## Overview

The main configuration files are [`lima-common.xml`](https://github.com/aymara/lima/blob/master/lima_common/conf/lima-common.xml) and [`lima-analysis.xml`](https://github.com/aymara/lima/blob/master/lima_common/conf/lima-analysis.xml). This can be overridden on the command line using the `--common-config-file` and the `--lp-config-file` parameters respectively. `lima-common.xml` defines some general information common to all languages and the names of the files defining language specific data (by default [`lima-common-<lang>.xml`](https://github.com/aymara/lima/blob/master/lima_common/conf/lima-common-ud.xml)  for the *lang* language). `lima-analysis.xml` defines the names of the files describing language specific pipelines and [process units](process-units/index.md) (by default `lima-lp-<lang>.xml`  for the lang language). It also defines a mapping between global and language specific pipeline names.

The file that you will mainly have to look at to change the behavior of LIMA  on a given *&lt;lang&gt;* language is [`lima-lp-<lang>.xml`](https://github.com/aymara/lima/blob/master/lima_linguisticprocessing/conf/lima-lp-ud.xml).

The last configuration files, that can be very helpful to help debugging or to understand the internals of LIMA are the `log4cpp.properties` filed. They allow to activate several levels of debugging information.

All LIMA XML configuration files have the following structure:

```xml
<?xml version='1.0' encoding='UTF-8'?>
<modulesConfig>
<module name="moduleName">
    <group name="groupName">
        <param key="paramName" value="param value"/>
      <list name="listName">
        <item value="1st item value"/>
        <item value="2nd item value"/>
        <item value="..."/>
      </list>
      <map name="mapName">
        <entry key="FirstKey" value="1st key value"/>
        <entry key="SecondKey" value="2nd key value"/>
        <entry key="..." value="..."/>
      </map>
    </group>
    <group name="...">
      ...
    </group>
</module>
<module name="...">
...
</module>
</modulesConfig>
```

One can include configuration data from external files with the following syntax:

```xml
    <group name="include">
      <list name="includeList">
        <item value="<filename to include>/<module name to include>"/>
      </list>
    </group>
```

This must be  placed as a child of a module tag. This will include the content of the target module in the target file into the current module where the include statement is.

### `lima-common-<lang>.xml`

Language-specific data shared by all the analysis steps: the `MediaData`
module (media class) and the `LinguisticData` module, which declares the
part-of-speech categories, the syntactic relations and the parameters of the
syntactic analysis of the language. The UD languages share
[`lima-common-ud.xml`](https://github.com/aymara/lima/blob/master/lima_common/conf/lima-common-ud.xml).

### `lima-analysis.xml`

The configuration of the analysis client (module `lima-coreclient`):

- the `mediaProcessingDefinitionFiles` group lists the available languages
  ("media") and, for each of them, the `lima-lp-<lang>.xml` file defining its
  processing;
- the `pipelines` group maps each global pipeline name (the one given with
  `-p`) to the pipeline used for each language. For example, the `main`
  pipeline is the `main` pipeline of the legacy `eng` and `fre` languages,
  and `deepud` for the UD languages.

### `lima-lp-<lang>.xml`

This is the file defining all processing done during linguistic analysis and  the resources they use. It contains  two modules: `Processors` for pipelines and [process units](process-units/index.md) and `Resources` for the linguistic resources.

#### The `Processors` module

It contains several groups in four categories:

1. Definition of pipelines
2. Definition of [process units](process-units/index.md)
3. Definition of loggers
4. Definition of dumpers

In fact loggers and dumpers are kind of [process units](process-units/index.md) but with a special role, respectively to write log messages tracing the results of some [process units](process-units/index.md) and to write or print final results.

Each group has a name and a class, which is the identifier of the C++ class to instantiate using the dedicated factory.
 
The pipeline groups are  all of the class `ProcessUnitPipeline`:
```xml
    <group name="main" class="ProcessUnitPipeline" >
```
They contain one list named `processUnitSequence` whose items are the names  of [process units](process-units/index.md), loggers and  dumpers. When a pipeline is selected (`main` by default or the one selected with the `--pipeline=` or `-p`option), its elements are executed in sequence. There is no check of the possible dependencies between units. This is the role of the user to define coherent sequences. Please refer to [process units reference documentation](process-units/index.md) for details about the role, dependencies, and configuration of each process unit.

The beginning of   the `main` pipeline at the time of this writing is: 
```xml
    <group name="main" class="ProcessUnitPipeline" >
      <list name="processUnitSequence">
        <!--item value="beginStatusLogger"/-->
        <item value="flattokenizer"/>
        <item value="regexmatcher"/>
        <!--item value="fullTokenXmlLoggerTokenizer"/-->
        <item value="simpleWord"/>
        <item value="hyphenWordAlternatives"/>
        <item value="idiomaticAlternatives"/>
```
As you can see, several elements are commented out. Some of them are pipeline units that can be activated to do more  things, like semantic analysis. The others are loggers that one can activate to see the results of previous modules and dumpers alternative to the default one, the `conllDumper`. Note that there can  be several dumpers activated. Note also that some dumpers need to use a handler different than the default one. The correct handler is activated on the command line by using the `--dumper=` or `-d` parameter. 

The other preexisting pipelines (but one can define others) are:
* limaserver:  the pipeline for the LIMA HTTP/JSON server;
* easy: produce output for the Easy parsers evaluation campaign;
* none: an empty pipeline that does nothing, for tests.

All other `Processors` groups have parameters specific to each class. Some of the parameters are references to linguistic resources defined in the next module.

#### The `Resources` module

This module describes linguistic resources that are loaded at   initialization time and that can be referenced from the [process units](process-units/index.md) using their name. Each one is described in a group that has a name and and a class which is the id of the C++ class used to instantiate it.

Resources are described on a [dedicated page](linguistic-resources.md)

### `log4cpp.properties`
These files are  in the log4j format. They allow to setup several categories defined in the C++ code. Each debug message is  emitted  in one category and at one level. If the level set for  this category in the log4cpp.properties files is lower or equal to this category, then the message is printed on the standard output.

The levels are: 
`NOTSET < TRACE < DEBUG < INFO < NOTICE < WARN < ERROR < CRIT < ALERT < FATAL = EMERG`

Note that the destination of  each category (file, standard output, system logs, etc.) should be configurable, but  it is not the case currently.

The log4cpp.properties files are searched in the same places as the XML configuration files. They are all loaded in reverse order of their discovery, so that definitions in places before the others overwrite those that come later. In each searched folder (given by `LIMA_CONF`, etc.), all `.properties` files in the `log4cpp` subfolder are loaded first and then the `log4cpp.properties` file itself.
 
