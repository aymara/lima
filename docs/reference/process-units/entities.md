# Entities and rules process units

Process units applying ModEx rules and recognizing specific entities.

## GeoEntitiesTagger

**Class:** GeoEntitiesTagger

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
    <group name="geoEntities" class="GeoEntitiesTagger">
      <param key="charChart" value="flatcharchart"/>
      <param key="dbms" value="mysql"/>
      <param key="dbConnection" value="dbname=GAZETIKI_DB user=gazetiki password=gazpwd"/>
      <param key="maxEntityLength" value="10" />
      <param key="graph" value="PosGraph"/>
      <param key="fieldClass" value="CLASS_3"/>
      <map name="Trigger">
        <!--entry key="t_capital_1st" value="Status" unlessSatusBefore="t_sentence_brk" unlessMicroBefore="PONCTU_PARAGRAPHE" unlessFirstToken="YES"/-->
        <entry key="t_capital_1st" value="Status" unlessSatusBefore="t_sentence_brk" unlessMicroBefore="" unlessFirstToken="YES"/>
        <entry key="NP" value="Micro" unlessSatusBefore="" unlessMicroBefore="" unlessFirstToken="NO"/>
      </map>
      <map name="EndWord">
        <!--entry key="PONCTU_PARAGRAPHE" value="Micro" unlessSatusBefore="" unlessMicroBefore="" unlessFirstToken="NO"/-->
        <entry key="T_COMMA_NUMBER" value="Status" unlessSatusBefore="" unlessMicroBefore="" unlessFirstToken="NO"/>
      </map>
    </group>
```

## ApplyRecognizer

**Class:** ApplyRecognizer

**Role:**
The role of this process unit is to apply compiled recognition rules. The specification of the rules source format is described elsewhere.

This kind of process unit and rules is used extensively in LIMA, for things like idiomatic expressions or named entities. But also for parsing and other things.

**Inputs:**

Parameters |  |
--- | --- |
automaton |  |
automatonList |  |
useSentenceBounds |  |
applyOnGraph |  |
updateGraph |  |
resolveOverlappingEntities |  |
overlappingEntitiesStrategy |  |
storeInData |  |
testAllVertices | if true, test all vertices, otherwise, skip recognized expressions (default is false) |
stopAtFirstSuccess | if true, stop testing rules on the current node after one rule succeeded (default is true) |
onlyOneSuccessPerType | if true, stop testing rules with same type as a previously successful rule (only used if stopAtFirstSuccess is false) (default is false) |
returnAtFirstSuccess | if true, abort the search as soona rule is successful (if true, stopAtFirstSuccess will be set to true) (default is false) |
applySameRuleWhileSuccess | if true, when a rule succeeds, retry to apply it on same vertex until the rule does not apply (use with care: setting this argument to true may cause loops if rules are not well written). Will not apply if stopAtFirstSuccess. (default is false) |

**Outputs:** 

**Preconditions:** 

**Effects:** 
