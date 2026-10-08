# Loggers and debugging process units

Loggers are process units that write intermediate results, mainly for debugging and resource development.

## StatusLogger

**Class:** StatusLogger

**Role:**
The role of this process unit is to .

**Inputs:**

Parameters |  |
--- | --- |
 |  |
 |  |

**Outputs:** 

**Preconditions:** 

**Effects:** 

```xml
    <group name="beginStatusLogger" class="StatusLogger">
      <param key="outputFile" value="beginStatus-fre.log"/>
      <list name="toLog">
        <item value="VmSize"/>
        <item value="VmData"/>
      </list>
    </group>
```

## SpecificEntitiesXmlLogger

**Class:** SpecificEntitiesXmlLogger

**Role:**
The role of this process unit is to .

**Inputs:**

Parameters |  |
--- | --- |
 |  |
 |  |

**Outputs:** 

**Preconditions:** 

**Effects:** 

```xml
    <group name="specificEntitiesXmlLogger" class="SpecificEntitiesXmlLogger">
      <param key="outputSuffix" value=".se.xml"/>
      <param key="graph" value="AnalysisGraph"/>
    </group>
    <group name="specificEntitiesXmlLoggerForLimaserver" class="SpecificEntitiesXmlLogger">
      <param key="outputSuffix" value=".se.xml"/>
      <param key="graph" value="AnalysisGraph"/>
      <param key="compactFormat" value="yes"/>
      <param key="handler" value="se"/>
      <param key="followGraph" value="true"/>
    </group>
```

## FullTokenXmlLogger

**Class:** FullTokenXmlLogger

**Role:**
The role of this process unit is to .

**Inputs:**

Parameters |  |
--- | --- |
 |  |
 |  |

**Outputs:** 

**Preconditions:** 

**Effects:** 

```xml
    <group name="fullTokenXmlLoggerTokenizer" class="FullTokenXmlLogger">
      <param key="outputSuffix" value=".tokenizer.xml"/>
    </group>
    <group name="fullTokenXmlLoggerSimpleWord" class="FullTokenXmlLogger">
      <param key="outputSuffix" value=".simpleword.xml"/>
    </group>
    <group name="fullTokenXmlLoggerHyphen" class="FullTokenXmlLogger">
      <param key="outputSuffix" value=".hyphen.xml"/>
    </group>
    <group name="fullTokenXmlLoggerIdiomatic" class="FullTokenXmlLogger">
      <param key="outputSuffix" value=".idiom.xml"/>
    </group>
```

## SentenceBoundariesXmlLogger

**Class:** SentenceBoundariesXmlLogger

**Role:**
The role of this process unit is to .

**Inputs:**

Parameters |  |
--- | --- |
 |  |
 |  |

**Outputs:** 

**Preconditions:** 

**Effects:** 

```xml
    <group name="sentenceBoundariesXmlLogger" class="SentenceBoundariesXmlLogger">
      <param key="outputSuffix" value=".sentences.xml"/>
    </group>
    <group name="fullTokenXmlLoggerDefaultProperties" class="FullTokenXmlLogger">
      <param key="outputSuffix" value=".default.xml"/>
    </group>    
```

## WordSenseXmlLogger0

**Class:** WordSenseXmlLogger

**Role:**
The role of this process unit is to .

**Inputs:**

Parameters |  |
--- | --- |
 |  |
 |  |

**Outputs:** 

**Preconditions:** 

**Effects:** 

```xml
    <group name="wordSenseXmlLogger" class="WordSenseXmlLogger">
      <param key="outputSuffix" value=".senses.xml"/>
    </group>
```

## DisambiguatedGraphXmlLogger

**Class:** DisambiguatedGraphXmlLogger

**Role:**
The role of this process unit is to .

**Inputs:**

Parameters |  |
--- | --- |
 |  |
 |  |

**Outputs:** 

**Preconditions:** 

**Effects:** 

```xml
    <group name="disambiguatedGraphXmlLogger" class="DisambiguatedGraphXmlLogger">
      <param key="outputSuffix" value=".disambiguated.xml"/>
      <param key="dictionaryCode" value="dictionaryCode"/>
    </group>
```

## DebugSyntacticAnalysisLogger

**Class:** DebugSyntacticAnalysisLogger

**Role:**
The role of this process unit is to .

**Inputs:**

Parameters |  |
--- | --- |
 |  |
 |  |

**Outputs:** 

**Preconditions:** 

**Effects:** 

```xml
    <group name="debugSyntacticAnalysisLogger-chains" class="DebugSyntacticAnalysisLogger">
      <param key="outputSuffix" value=".syntanal.chains.txt"/>
    </group>
    <group name="debugSyntacticAnalysisLogger-disamb" class="DebugSyntacticAnalysisLogger">
      <param key="outputSuffix" value=".syntanal.disamb.txt"/>
    </group>
    <group name="debugSyntacticAnalysisLogger-deps" class="DebugSyntacticAnalysisLogger">
      <param key="outputSuffix" value=".syntanal.deps.txt"/>
    </group>
```

## DotGraphWriter

**Class:** DotGraphWriter

**Role:**
The role of this process unit is to .

**Inputs:**

Parameters |  |
--- | --- |
 |  |
 |  |

**Outputs:** 

**Preconditions:** 

**Effects:** 

```xml
    <group name="dotGraphWriter-beforepos" class="DotGraphWriter">
      <param key="graph" value="AnalysisGraph"/>
      <param key="outputSuffix" value=".bp.dot"/>
      <param key="trigramMatrix" value="trigramMatrix"/>
      <param key="bigramMatrix" value="bigramMatrix"/>
      <list name="vertexDisplay">
        <item value="text"/>
        <item value="inflectedform"/>
        <item value="symbolicmicrocategory"/>
        <item value="numericmicrocategory"/>
        <!--item value="genders"/>
      <item value="numbers"/-->
      </list>
    </group>
    <group name="dotGraphWriter" class="DotGraphWriter">
      <param key="graph" value="PosGraph"/>
      <param key="outputSuffix" value=".dot"/>
      <param key="trigramMatrix" value="trigramMatrix"/>
      <param key="bigramMatrix" value="bigramMatrix"/>
      <list name="vertexDisplay">
        <item value="text"/>
        <item value="inflectedform"/>
        <item value="symbolicmicrocategory"/>
        <item value="numericmicrocategory"/>
        <!--item value="genders"/>
      <item value="numbers"/-->
      </list>
    </group>
```

## CorefSolvingLogger

**Class:** CorefSolvingLogger

**Role:**
The role of this process unit is to .

**Inputs:**

Parameters |  |
--- | --- |
 |  |
 |  |

**Outputs:** 

**Preconditions:** 

**Effects:** 

```xml
    <group name="corefLogger" class="CorefSolvingLogger">
      <param key="outputSuffix" value=".wh"/>
    </group>
    <group name="dotGraphWriterAfterSA" class="DotGraphWriter">
      <param key="outputSuffix" value=".afterSA.dot"/>
      <param key="trigramMatrix" value="trigramMatrix"/>
      <param key="bigramMatrix" value="bigramMatrix"/>
      <list name="vertexDisplay">
        <item value="lemme"/>
        <item value="symbolicmicrocategory"/>
        <item value="numericmicrocategory"/>
        <!--item value="genders"/>
        <item value="numbers"/-->
      </list>
    </group>
```

## DotDependencyGraphWriter

**Class:** DotDependencyGraphWriter

**Role:**
The role of this process unit is to .

**Inputs:**

Parameters |  |
--- | --- |
 |  |
 |  |

**Outputs:** 

**Preconditions:** 

**Effects:** 

```xml
    <group name="dotDepGraphWriter" class="DotDependencyGraphWriter">
      <param key="outputMode" value="SentenceBySentence"/> <!-- Valid values: FullGraph,SentenceBySentence -->
      <param key="writeOnlyDepEdges" value="false"/>
      <param key="outputSuffix" value=".sa.dot"/>
      <param key="trigramMatrix" value="trigramMatrix"/>
      <param key="bigramMatrix" value="bigramMatrix"/>
      <list name="vertexDisplay">
        <item value="inflectedform"/>
        <item value="symbolicmicrocategory"/>
        <item value="numericmicrocategory"/>
        <!--item value="genders"/>
        <item value="numbers"/-->
      </list>
      <map name="graphDotOptions">
        <entry key="rankdir" value="LR"/>
      </map>
      <map name="nodeDotOptions">
        <entry key="shape" value="box"/>
      </map>
    </group>
```

## AnnotDotGraphWriter

**Class:** AnnotDotGraphWriter

**Role:**
The role of this process unit is to .

**Inputs:**

Parameters |  |
--- | --- |
 |  |
 |  |

**Outputs:** 

**Preconditions:** 

**Effects:** 

```xml
    <group name="annotDotGraphWriter" class="AnnotDotGraphWriter">
      <param key="graph" value="PosGraph"/>
      <param key="outputSuffix" value=".ag.dot"/>
    </group>
```

## LinearTextRepresentationLogger

**Class:** LinearTextRepresentationLogger

**Role:**
The role of this process unit is to .

**Inputs:**

Parameters |  |
--- | --- |
 |  |
 |  |

**Outputs:** 

**Preconditions:** 

**Effects:** 

```xml
    <group name="linearTextRepresentationLogger" class="LinearTextRepresentationLogger">
      <param key="outputSuffix" value=".ltr"/>
    </group>
```

## SyntacticAnalysisXmlLogger

**Class:** SyntacticAnalysisXmlLogger

**Role:**
The role of this process unit is to .

**Inputs:**

Parameters |  |
--- | --- |
 |  |
 |  |

**Outputs:** 

**Preconditions:** 

**Effects:** 

```xml
    <group name="syntacticAnalysisXmlLogger" class="SyntacticAnalysisXmlLogger">
      <param key="outputSuffix" value=".sa.xml"/>
    </group>
    <group name="depTripletLogger" class="DepTripletLogger">
      <param key="outputSuffix" value=".deptrip.txt"/>
      <param key="stopList" value="stopList"/>
      <param key="useStopList" value="no"/>
      <param key="useEmptyMacro" value="no"/>
      <param key="useEmptyMicro" value="no"/>
      <map name="NEmacroCategories">
        <entry key="DateTime.DATE" value="NC"/>
        <entry key="Numex.NUMBER" value="NC"/>
        <entry key="Numex.UNIT" value="NC"/>
        <entry key="Numex.NUMEX" value="NC"/>
        <entry key="Organization.ORGANIZATION" value="NP"/>
        <entry key="Location.LOCATION" value="NP"/>
        <entry key="Person.PERSON" value="NP"/>
        <entry key="Product.PRODUCT" value="NP"/>
        <entry key="Event.EVENT" value="NP"/>
      </map>
      <param key="properNounCategory" value="NP"/>
      <param key="commonNounCategory" value="NC"/>
      <param key="NEnormalization" value="useNENormalizedForm"/>
      <list name="selectedDependency">
        <item value="ADJPRENSUB"/>
        <item value="APPOS"/>
        <item value="ATB_O"/>
        <item value="ATB_S"/>
        <item value="COD_V"/>
        <item value="COMPDUNOM"/>
        <item value="COMPL"/>
        <item value="CPL_V"/>
        <item value="SUBADJPOST"/>
        <item value="SUBSUBJUX"/>
        <item value="SUJ_V"/>
      </list>
    </group>
```
