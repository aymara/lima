# Dumpers

Dumpers are the process units that write the final analysis results, usually at the end of a pipeline.

## AbstractTextualAnalysisDumper

**Class:** AbstractTextualAnalysisDumper

**Role:**
This is an abstract class. You cannot use it directly. Instead, use the other
dumper classes. It is documented here because all dumpers can use its
parameters.

**Inputs:**

Parameters |  |
--- | --- |
handler | the name of the handler process unit in the configuration file that will receive and handle the data written by the dumper |
temporaryFileMetadata | the name of the analysis metadata entry that contains the name of the file where to write. It supercedes the outputFile option and the suffix handling options |
outputFile | the name of the file where the dumper will write. It supercedes the suffix handling options |
stripInputSuffix | whether to remove the suffix of the input file before using it (when outputFile is not set) |
outputSuffix | the suffix to add to the name of the input file (when outputFile is not set) |
append | whether we will append content to the output file or erase its content if it exists |
 

**Outputs:** 
No outputs. This is an abstract class. See the other dumpers documentation for
details.

**Preconditions:** 
None

**Effects:** 
None

## AnnotationGraphXmlDumper

**Class:** AnnotationGraphXmlDumper

**Role:**
The role of this process unit is to .

**Inputs:**

See [AbstractTextualAnalysisDumper](#abstracttextualanalysisdumper) for 
parameters common to all dumpers.

Parameters |  |
--- | --- |
 |  |
 |  |

**Outputs:** 

**Preconditions:** 

**Effects:** 

```xml
    <group name="agXmlDumper" class="AnnotationGraphXmlDumper">
      <param key="handler" value="xmlSimpleStreamHandler"/>
    </group>
    <group name="normalizationBowDumper" class="BowDumper">
      <param key="handler" value="bowTextWriter"/>
      <param key="stopList" value="stopList"/>
      <param key="useStopList" value="false"/>
      <param key="useEmptyMacro" value="false"/>
      <param key="useEmptyMicro" value="false"/>
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
    </group>
```

## BowDumper

**Class:** BowDumper

**Role:**
The role of this process unit is to .

**Inputs:**

See [AbstractTextualAnalysisDumper](#abstracttextualanalysisdumper) for 
parameters common to all dumpers.

Parameters |  |
--- | --- |
 |  |
 |  |

**Outputs:** 

**Preconditions:** 

**Effects:** 

```xml
    <group name="bowDumper" class="BowDumper">
      <param key="handler" value="bowTextWriter"/>
      <param key="stopList" value="stopList"/>
      <param key="useStopList" value="true"/>
      <param key="useEmptyMacro" value="true"/>
      <param key="useEmptyMicro" value="true"/>
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
    </group>
    <group name="bowTextHandler" class="BowDumper">
      <param key="handler" value="bowTextHandler"/>
      <param key="stopList" value="stopList"/>
      <param key="useStopList" value="true"/>
      <param key="useEmptyMacro" value="true"/>
      <param key="useEmptyMicro" value="true"/>
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
    </group>
    <group name="textQueryHandler" class="BowDumper">
      <param key="handler" value="bowTextHandler"/>
<!--       <param key="handler" value="bowTextWriter"/> -->
      <param key="stopList" value="stopList"/>
      <param key="useStopList" value="true"/>
      <param key="useEmptyMacro" value="true"/>
      <param key="useEmptyMicro" value="true"/>
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
    </group>
    <group name="bowDocumentDumper" class="BowDumper">
      <param key="handler" value="bowDocumentHandler"/>
      <param key="stopList" value="stopList"/>
      <param key="useStopList" value="false"/>
      <param key="useEmptyMacro" value="false"/>
      <param key="useEmptyMicro" value="false"/>
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
    </group>
    <group name="bowTextDumper" class="BowDumper">
      <param key="handler" value="bowTextHandler"/>
      <param key="stopList" value="stopList"/>
      <param key="useStopList" value="false"/>
      <param key="useEmptyMacro" value="false"/>
      <param key="useEmptyMicro" value="false"/>
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
    </group>
```

## ConllDumper

**Class:** ConllDumper

**Role:**
The role of this process unit is to dump in a stream the result of the syntaxic analysis of sentences following the CoNLL-U format.

See [Universal Dependencies](https://universaldependencies.org/format.html) for details on the CoNLL-U format.

The `UPOS` and `XPOS` values of the LIMA ConllDumper are not exactly compliant with the vanilla format.

For each language, see in `lima_linguisticdata/analysisDictionary/<lang>/code/code-<lang>.xml` the possible values of `MACRO` and `MICRO` tags that LIMA will dump respectively as the `UPOS` and `XPOS` tags.

The ConllDumper can be configured via the boolean `withColsHeader` option to write header lines giving the column names of the CoNLL-U format as a reminder.

**Inputs:**

See [AbstractTextualAnalysisDumper](#abstracttextualanalysisdumper) for 
parameters common to all dumpers.

Parameters |  |
--- | --- |
outputSuffix | default value is '.conll' |
 |  |

**Outputs:** 

**Preconditions:** 

**Effects:** 

## DepTripleDumper

**Class:** DepTripleDumper

**Role:**
The role of this process unit is to .

**Inputs:**

See [AbstractTextualAnalysisDumper](#abstracttextualanalysisdumper) for 
parameters common to all dumpers.

Parameters |  |
--- | --- |
 |  |
 |  |

**Outputs:** 

**Preconditions:** 

**Effects:** 

```xml
    <group name="depTripleDumper" class="DepTripleDumper">
      <param key="handler" value="simpleStreamHandler"/>
      <list name="selectedDependency">
        <item value="ADJPRENSUB"/>
        <!--item value="ADVADV"/-->
        <!--item value="AdvSub"/-->
        <item value="APPOS"/>
        <item value="ATB_O"/>
        <item value="ATB_S"/>
        <item value="COD_V"/>
        <!--item value="COMPADJ"/-->
        <!--item value="COMPADV"/-->
        <!--item value="CompDet"/-->
        <item value="COMPDUNOM"/>
        <item value="COMPL"/>
        <!--item value="COORD1"/-->
        <!--item value="COORD2"/-->
        <item value="CPL_V"/>
        <!--item value="DETSUB"/-->
        <!--item value="MOD_A"/-->
        <!--item value="MOD_N"/-->
        <!--item value="MOD_V"/-->
        <!--item value="Neg"/-->
        <!--item value="PrepDet"/-->
        <!--item value="PrepPron"/-->
        <!--item value="PREPSUB"/-->
        <item value="SUBADJPOST"/>
        <item value="SUBSUBJUX"/>
        <item value="SUJ_V"/>
      </list>
    </group>
```

## EasyXmlDumper

**Class:** EasyXmlDumper

**Role:**
The role of this process unit is to .

**Inputs:**

See [AbstractTextualAnalysisDumper](#abstracttextualanalysisdumper) for 
parameters common to all dumpers.

Parameters |  |
--- | --- |
 |  |
 |  |

**Outputs:** 

**Preconditions:** 

**Effects:** 

```xml
    <group name="easyXmlDumper" class="EasyXmlDumper">
      <param key="handler" value="simpleStreamHandler"/>
      <map name="typeMapping">
        <entry key="COMPDUNOM" value="MOD-N"/>
        <entry key="ADJPRENSUB" value="MOD-N"/>
        <entry key="SUBADJPOST" value="MOD-N"/>
        <entry key="SUBSUBJUX" value="MOD-N"/>
        <entry key="TEMPCOMP" value="AUX-V"/>
        <entry key="SujInv" value="SUJ-V"/>
        <entry key="CodPrev" value="COD-V"/>
        <entry key="CoiPrev" value="CPL-V"/>
        <entry key="PronSujVerbe" value="SUJ-V"/>
        <entry key="ADVADV" value="MOD-R"/>
        <entry key="ADVADJ" value="MOD-A"/>
        <entry key="NePas2" value="MOD-V"/>
        <entry key="AdvVerbe" value="MOD-V"/>
        <entry key="COMPADJ" value="MOD-A"/>
        <!--entry key="Neg" value="MOD-V"/-->
        <!--change '_' to '-' -->
        <entry key="SUJ_V" value="SUJ-V"/>
        <entry key="SUJ_V_REL" value="SUJ-V"/>
        <entry key="COD_V" value="COD-V"/>
        <entry key="CPL_V" value="CPL-V"/>
        <entry key="CPLV_V" value="CPL-V"/>
        <entry key="MOD_V" value="MOD-V"/>
        <entry key="MOD_N" value="MOD-N"/>
        <entry key="MOD_A" value="MOD-A"/>
        <entry key="ATB_S" value="ATB-SO,s-o valeur=sujet"/>
        <entry key="ATB_O" value="ATB-SO,s-o valeur=objet"/>
        <entry key="COORD1" value="COORD"/>
        <entry key="COORD2" value="COORD"/>
        <entry key="COMPL" value="COMP"/>
        <entry key="JUXT" value="JUXT"/>
      </map>
      <map name="srcTag">
        <entry key="MOD-N" value="modifieur"/>
        <entry key="MOD-V" value="modifieur"/>
        <entry key="SUJ-V" value="sujet"/>
        <entry key="AUX-V" value="auxiliaire"/>
        <entry key="COD-V" value="cod"/>
        <entry key="CPL-V" value="complement"/>
        <entry key="MOD-R" value="modifieur"/>
        <entry key="APPOS" value="premier"/>
        <entry key="JUXT" value="suivant"/>
        <entry key="ATB-SO" value="attribut"/>
        <entry key="MOD-A" value="modifieur"/>
        <entry key="COMP" value="complementeur"/>
        <entry key="COORD" value="coordonnant"/>
      </map>
      <map name="tgtTag">
        <entry key="MOD-N" value="nom"/>
        <entry key="MOD-V" value="verbe"/>
        <entry key="SUJ-V" value="verbe"/>
        <entry key="AUX-V" value="verbe"/>
        <entry key="COD-V" value="verbe"/>
        <entry key="CPL-V" value="verbe"/>
        <entry key="MOD-R" value="adverbe"/>
        <entry key="APPOS" value="appose"/>
        <entry key="JUXT" value="premier"/>
        <entry key="ATB-SO" value="verbe"/>
        <entry key="MOD-A" value="adjectif"/>
        <entry key="COMP" value="verbe"/>
        <entry key="COORD" value="coord-g"/>
      </map>
    </group>
```

## FullXmlDumper

**Class:** FullXmlDumper

**Role:**
The role of this process unit is to .

**Inputs:**

See [AbstractTextualAnalysisDumper](#abstracttextualanalysisdumper) for 
parameters common to all dumpers.

Parameters |  |
--- | --- |
 |  |
 |  |

**Outputs:** 

**Preconditions:** 

**Effects:** 

## GeoDumper

**Class:** GeoDumper

**Role:**
The role of this process unit is to .

**Inputs:**

See [AbstractTextualAnalysisDumper](#abstracttextualanalysisdumper) for 
parameters common to all dumpers.

Parameters |  |
--- | --- |
graph | PosGraph |
 |  |

**Outputs:** 

**Preconditions:** 

**Effects:** 

## LTRDumper

**Class:** LTRDumper

**Role:**
The role of this process unit is to .

**Inputs:**

See [AbstractTextualAnalysisDumper](#abstracttextualanalysisdumper) for 
parameters common to all dumpers.

Parameters |  |
--- | --- |
handler | default is 'simpleStreamHandler' |
 |  |

**Outputs:** 

**Preconditions:** 

**Effects:** 

## NullDumper

**Class:** NullDumper

**Role:**
The role of this process unit is to dump nothing.

**Inputs:**
See [AbstractTextualAnalysisDumper](#abstracttextualanalysisdumper) for 
parameters common to all dumpers.

**Outputs:** 
None.
**Preconditions:** 
None.

**Effects:** 
None.

## posGraphXmlDumper

**Class:** posGraphXmlDumper

**Role:**
The role of this process unit is to .

**Inputs:**

See [AbstractTextualAnalysisDumper](#abstracttextualanalysisdumper) for 
parameters common to all dumpers.

Parameters |  |
--- | --- |
 |  |
 |  |

**Outputs:** 

**Preconditions:** 

**Effects:** 

## SimpleXmlDumper

**Class:** SimpleXmlDumper

**Role:**
The role of this process unit is to .

**Inputs:**

See [AbstractTextualAnalysisDumper](#abstracttextualanalysisdumper) for 
parameters common to all dumpers.

Parameters |  |
--- | --- |
 |  |
 |  |

**Outputs:** 

**Preconditions:** 

**Effects:** 

## TextDumper

**Class:** TextDumper

**Role:**
The role of this process unit is to .

**Inputs:**

See [AbstractTextualAnalysisDumper](#abstracttextualanalysisdumper) for 
parameters common to all dumpers.

Parameters |  |
--- | --- |
outputSuffix | defaultValue is '.out' |
 |  |

**Outputs:** 

**Preconditions:** 

**Effects:** 
