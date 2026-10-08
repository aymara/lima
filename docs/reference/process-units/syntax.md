# Syntactic analysis process units

Rule-based syntactic analysis (legacy pipelines). The neural dependency parser is described with the [neural units](neural.md).

## SyntacticAnalyzerChains

**Class:** SyntacticAnalyzerChains

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
    <group name="syntacticAnalyzerChains" class="SyntacticAnalyzerChains">
      <param key="chainMatrix" value="chainMatrix"/>
      <param key="maxChainsNbByVertex" value="30"/>
      <param key="maxChainLength" value="12"/>
    </group>
```

## SyntacticAnalyzerNoChains

**Class:** SyntacticAnalyzerNoChains

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
    <!-- syntacticAnalyzerNoChains replaces syntacticAnalyzerChains. It is an
    experimental module used to test if LIMA analysis works without nominal and
    verbal. It allows also to build compounds using verbs and heterosyntagmatic
    dependencies. For that, one have to add adequate relations in
    CompoundRelations in mm-common. -->
    <group name="syntacticAnalyzerNoChains" class="SyntacticAnalyzerNoChains">
      <param key="chainMatrix" value="chainMatrix"/>
      <param key="disambiguated" value="true"/>
      <param key="maxChainsNbByVertex" value="30"/>
      <param key="maxChainLength" value="12"/>
    </group>
```

## SyntacticAnalyzerDisamb

**Class:** SyntacticAnalyzerDisamb

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
    <group name="syntacticAnalyzerDisamb" class="SyntacticAnalyzerDisamb">
      <param key="depGraphMaxBranchingFactor" value="100"/>
    </group>
```

## SyntacticAnalyzerDeps

**Class:** SyntacticAnalyzerDeps

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
    <group name="syntacticAnalyzerDeps" class="SyntacticAnalyzerDeps">
      <list name="actions">
          <item value="pass0HomoSyntagmaticRelationRules"/>
          <item value="pass1HomoSyntagmaticRelationRules"/>
          <item value="pass2HomoSyntagmaticRelationRules"/>
          <item value="pleonasticPronouns"/>
          <item value="compoundTensesRules"/>
      </list>
      <param key="applySameRuleWhileSuccess" value="true"/>
    </group>
```

## SyntacticAnalyzerSimplify

**Class:** SyntacticAnalyzerSimplify

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
    <group name="syntacticAnalyzerSimplifyFirst" class="SyntacticAnalyzerSimplify">
      <param key="simplifyAutomaton" value="simplifyAutomatonFirst"/>
    </group>
    <group name="syntacticAnalyzerSimplify" class="SyntacticAnalyzerSimplify">
      <param key="simplifyAutomaton" value="simplifyAutomaton"/>
    </group>
    <group name="syntacticAnalyzerSimplifyCoord" class="SyntacticAnalyzerSimplify">
      <param key="simplifyAutomaton" value="simplifyAutomatonCoord"/>
    </group>
    <group name="syntacticAnalyzerSimplifyLast" class="SyntacticAnalyzerSimplify">
      <param key="simplifyAutomaton" value="simplifyAutomatonLast"/>
    </group>
```

## SyntacticAnalyzerDepsHetero

**Class:** SyntacticAnalyzerDepsHetero

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
    <group name="syntacticAnalyzerDepsHetero" class="SyntacticAnalyzerDepsHetero">
      <param key="rules" value="heteroSyntagmaticRelationRules"/>
      <param key="selectionalPreferences" value="selectionalPreferences"/>
      <param key="unfold" value="true"/>
      <param key="linkSubSentences" value="true"/>
      <param key="applySameRuleWhileSuccess" value="true"/>
      <map name="subSentencesRules">
        <entry key="SubSent" value="heteroSyntagmaticRelationRules"/>
        <entry key="SubordRel" value="heteroSyntagmaticRelationRules"/>
        <entry key="Parent" value="heteroSyntagmaticRelationRules"/>
        <entry key="Quotes" value="heteroSyntagmaticRelationRules"/>
        <entry key="VirguleSeule" value="heteroSyntagmaticRelationRules"/>
        <entry key="Appos" value="heteroSyntagmaticRelationRules"/>
        <entry key="AdvSeul" value="heteroSyntagmaticRelationRules"/>
        <entry key="AdvInit" value="heteroSyntagmaticRelationRules"/>
        <entry key="CompAdv" value="heteroSyntagmaticRelationRules"/>
        <entry key="Adverbe" value="heteroSyntagmaticRelationRules"/>
        <entry key="ConjInfSecond" value="heteroSyntagmaticRelationRules"/>
        <entry key="CCInit" value="heteroSyntagmaticRelationRules"/>
        <entry key="Infinitive" value="heteroSyntagmaticRelationRules"/>
        <entry key="SUBSUBJUX" value="heteroSyntagmaticRelationRules"/>
        <entry key="CompDuNom1" value="heteroSyntagmaticRelationRules"/>
        <entry key="CompDuNom2" value="heteroSyntagmaticRelationRules"/>
        <entry key="CompAdj1" value="heteroSyntagmaticRelationRules"/>
        <entry key="CompAdj2" value="heteroSyntagmaticRelationRules"/>
        <entry key="SubordParticipiale" value="heteroSyntagmaticRelationRules"/>
        <entry key="ElemListe" value="heteroSyntagmaticRelationRules"/>
        <entry key="ConjSecond" value="heteroSyntagmaticRelationRules"/>
        <entry key="InciseNom" value="heteroSyntagmaticRelationRules"/>
        <entry key="CompCirc" value="heteroSyntagmaticRelationRules"/>
        <entry key="SubordInit" value="heteroSyntagmaticRelationRules"/>
        <entry key="ConjNominale" value="heteroSyntagmaticRelationRules"/>
      </map>
    </group>
    <group name="syntacticAnalyzerDummy" class="SyntacticAnalyzerDeps">
      <list name="actions">
        <item value="l2rDummyRules"/>
      </list>
    </group>
```
