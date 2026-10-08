# Linguistic resources

Linguistic resources are declared in the `Resources` module of the
`lima-lp-<lang>.xml` [configuration files](configuration-files.md). Each one is a
`<group>` whose `name` is how [process units](process-units/index.md) refer to it and
whose `class` is the identifier of the C++ class that loads it. Paths are relative to
the directories listed in `LIMA_RESOURCES`.

The examples below come from the French legacy configuration (`lima-lp-fre.xml`).
The sources of these resources live in
[`lima_linguisticdata/`](https://github.com/aymara/lima/tree/master/lima_linguisticdata)
and are compiled at build time.


## FsaStringsPool
```xml
    <group name="FsaStringsPool">
      <param key="mainKeys" value="globalFsaAccess"/>
    </group>
```

## flatcharchart — `FlatTokenizerCharChart`
```xml
    <group name="flatcharchart" class="FlatTokenizerCharChart">
      <param key="charFile" value="LinguisticProcessings/fre/tokenizerAutomaton-fre.chars.tok"/>
    </group>
```

## mainDictionary — `EnhancedAnalysisDictionary`
```xml
    <group name="mainDictionary" class="EnhancedAnalysisDictionary">
      <param key="accessKeys" value="globalFsaAccess"/>
      <param key="dictionaryValuesFile" value="LinguisticProcessings/fre/dicoDat-fre.dat"/>
    </group>
```

## globalFsaAccess — `FsaAccess`
```xml
    <group name="globalFsaAccess" class="FsaAccess">
      <param key="keyFile" value="LinguisticProcessings/fre/dicoKey-fre.dat"/>
    </group>
```

## dictionaryCode — `DictionaryCode`
```xml
    <group name="dictionaryCode" class="DictionaryCode">
      <param key="codeFile" value="LinguisticProcessings/fre/code-fre.dat"/>
      <param key="codeListFile" value="LinguisticProcessings/fre/codesList-fre.dat"/>
    </group>
```

## idiomaticExpressionsRecognizer — `AutomatonRecognizer`
```xml
    <group name="idiomaticExpressionsRecognizer" class="AutomatonRecognizer">
      <param key="rules" value="LinguisticProcessings/fre/idiomaticExpressions-fre.bin"/>
    </group>
```

## trigramMatrix — `TrigramMatrix`
```xml
    <group name="trigramMatrix" class="TrigramMatrix">
      <param key="trigramFile" value="Disambiguation/trigramMatrix-fre.dat"/>
    </group>
```

## bigramMatrix — `BigramMatrix`
```xml
    <group name="bigramMatrix" class="BigramMatrix">
      <param key="bigramFile" value="Disambiguation/bigramMatrix-fre.dat"/>
    </group>
```

## stopList — `StopList`
```xml
    <group name="stopList" class="StopList">
      <param key="file" value="LinguisticProcessings/StopLists/stopList-fre.dat"/>
    </group>
```

## frequencyDictionary — `CompactDict16`
```xml
    <group name="frequencyDictionary" class="CompactDict16">
      <param key="dictionaryKey" value="Reformulation/frequency-dico-fre-keys.dat"/>
      <param key="dictionaryValues" value="Reformulation/frequency-dico-fre-val.dat"/>
    </group>
```

## chainMatrix — `SyntagmDefinitionStructure`
```xml
    <group name="chainMatrix" class="SyntagmDefinitionStructure">
      <param key="file" value="SyntacticAnalysis/chainsMatrix-fre.bin"/>
    </group>
```

## pass1HomoSyntagmaticRelationRules — `AutomatonRecognizer`
```xml
    <group name="pass1HomoSyntagmaticRelationRules" class="AutomatonRecognizer">
      <param key="rules" value="SyntacticAnalysis/rules-fre-homodeps-pass1.txt.bin"/>
      <param key="applySameRuleWhileSuccess" value="true"/>
    </group>
    <group name="pass2HomoSyntagmaticRelationRules" class="AutomatonRecognizer">
      <param key="rules" value="SyntacticAnalysis/rules-fre-homodeps-pass2.txt.bin"/>
      <param key="applySameRuleWhileSuccess" value="true"/>
    </group>
    <group name="pass0HomoSyntagmaticRelationRules" class="AutomatonRecognizer">
      <param key="rules" value="SyntacticAnalysis/rules-fre-homodeps-pass0.txt.bin"/>
      <param key="applySameRuleWhileSuccess" value="true"/>
    </group>
    <group name="pleonasticPronouns" class="AutomatonRecognizer">
      <param key="rules" value="SyntacticAnalysis/rules-fre-pleonasticPronouns.txt.bin"/>
      <param key="applySameRuleWhileSuccess" value="true"/>
    </group>
    <group name="compoundTensesRules" class="AutomatonRecognizer">
      <param key="rules" value="SyntacticAnalysis/rules-compoundTense.txt.bin"/>
      <param key="applySameRuleWhileSuccess" value="true"/>
    </group>
    <group name="simplifyAutomatonFirst" class="AutomatonRecognizer">
      <param key="rules" value="SyntacticAnalysis/simplification-first-rules-fre.txt.bin"/>
    </group>
    <group name="simplifyAutomaton" class="AutomatonRecognizer">
      <param key="rules" value="SyntacticAnalysis/simplification-rules-fre.txt.bin"/>
    </group>
    <group name="simplifyAutomatonCoord" class="AutomatonRecognizer">
      <param key="rules" value="SyntacticAnalysis/rules-fre-coord.bin"/>
    </group>
    <group name="simplifyAutomatonLast" class="AutomatonRecognizer">
      <param key="rules" value="SyntacticAnalysis/simplification-last-rules-fre.txt.bin"/>
    </group>
    <group name="heteroSyntagmaticRelationRules" class="AutomatonRecognizer">
      <param key="rules" value="SyntacticAnalysis/rules-fre-heterodeps.txt.bin"/>
    </group>
    <group name="l2rDummyRules" class="AutomatonRecognizer">
      <param key="rules" value="SyntacticAnalysis/l2rDummy-fre.bin"/>
    </group>
```

## selectionalPreferences — `SelectionalPreferences`
```xml
    <group name="selectionalPreferences" class="SelectionalPreferences">
      <param key="file" value="SyntacticAnalysis/selectionalPreferences-fre.bin"/>
    </group>
    <group name="automatonCompiler" class="AutomatonRecognizer">
      <param key="rules" value=""/>
    </group>
```

## bowTextWriter — `BowTextWriter`
```xml
    <group name="bowTextWriter" class="BowTextWriter"/>
```

## bowTextXmlWriter — `BowTextXmlWriter`
```xml
    <group name="bowTextXmlWriter" class="BowTextXmlWriter"/>
```

## bowTextHandler — `BowTextHandler`
```xml
    <group name="bowTextHandler" class="BowTextHandler"/>
```

## bowDocumentHandler — `BowDocumentHandler`
```xml
    <group name="bowDocumentHandler" class="BowDocumentHandler"/>
```

## simpleStreamHandler — `SimpleStreamHandler`
```xml
    <group name="simpleStreamHandler" class="SimpleStreamHandler"/>
    <group name="xmlSimpleStreamHandler" class="SimpleStreamHandler"/>    
    <group name="fullXmlSimpleStreamHandler" class="SimpleStreamHandler"/>    
```

## xmlDocumentHandler — `xmlDocumentHandler`
```xml
    <group name="xmlDocumentHandler" class="xmlDocumentHandler"/>
```
