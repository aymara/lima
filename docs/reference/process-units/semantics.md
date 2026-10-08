# Semantic process units

Coreference resolution and word sense disambiguation.

## CoreferencesSolving

**Class:** CoreferencesSolving

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
    <group name="coreferencesSolving" class="CoreferencesSolving">
      <param key="scope" value="3" />
      <param key="threshold" value="60" />
      <param key="Resolve Definites" value="0" />
      <param key="Resolve non third person pronouns" value="0" />
      <map name="MacroCategories">
        <entry key="PronMacroCategory" value="PRON"/>
        <entry key="VerbMacroCategory" value="V" />
        <entry key="PrepMacroCategory" value="PREP" />
        <entry key="NomCommunMacroCategory" value="NC" />
        <entry key="NomPropreMacroCategory" value="NP" />
      </map>
      <list name="LexicalAnaphora">
        <item value="CLR"/>
      </list>
      <list name="UndefinitePronouns">
        <!--item value="PRON_INDEFINI"/>
        <item value="PRON_INDEFINI_VAL_NEG"/-->
      </list>
      <list name="PossessivePronouns">
        <!--item value="PRON_POSSESSIF_SUJET" />
        <item value="PRON_POSSESSIF_COD" />
        <item value="PRON_POSSESSIF_COI" /-->
      </list>
      <list name="PrepRelation">
        <item value="PREPSUB"/>
        <item value="PrepDetInt"/>
        <item value="PrepInf"/>
        <item value="PrepPronRelCa"/>
        <item value="PrepPron"/>
        <item value="PrepPronRel"/>
        <item value="PrepPronCliv"/>
        <item value="PrepAdv"/>
      </list>
    <list name="PleonasticRelation">
      <item value="Pleon"/>
    </list>
    <list name="DefiniteRelation">
      <item value="DETSUB"/>
    </list>
    <list name="SubjectRelation">
      <item value="SUJ_V" />
      <item value="SUJ_V_REL" />
      <item value="PronSujVerbe" />
      <item value="SujInv" />
    </list>
    <list name="AttributeRelation">
      <item value="ATB_S"/>
    </list>
    <list name="CODRelation">
      <item value="COD_V" />
      <item value="CodPrev" />
      <item value="PronReflVerbe" />
    </list>
    <list name="COIRelation">
      <item value="CPL_V" />
      <item value="CoiPrev" />
    </list>
    <list name="AdjunctRelation">
          <item value="CPLV_V" />
          <item value="CC_TEMPS" />
          <item value="CC_LIEU" />
           <item value="CC_BUT" />
          <item value="CC_MOYEN" />
          <item value="CC_MANIERE" />
          <item value="COMPADJ" />
          <item value="COMPADV" />
      </list>
      <list name="AgentRelation">
          <item value="COMPADJ" />
      </list>
      <list name="NPDeterminerRelation">
          <item value="COMPDUNOM" />
           <item value="COMPDUNOM2" />
          <item value="SUBSUBJUX" />
          <item value="COMP_N-N" />
          <item value="COMPDUNOM_INC" />
      </list>
      <!-- Lappin & Leass salience factors -->
      <map name="SalienceFactors">
        <entry key="SentenceRecency" value="90"/>
        <entry key="SubjEmph" value="90"/>
        <entry key="ExistEmph" value="70"/>
        <entry key="CodEmph" value="50"/>
        <entry key="CoiCoblEmph" value="40"/>
        <entry key="HeadEmph" value="80"/>
        <entry key="NonAdvEmph" value="50"/>
        <entry key="IsInSubordinate" value="-70"/>
        <!-- local factors -->
        <entry key="Cataphora" value="-120"/>
        <entry key="SameSlot" value="90"/>
        <entry key="Itself" value="-140"/>
      </map>
      <map name="SlotValues">
        <entry key="SubjectRelation" value="4"/>
        <entry key="AgentRelation" value="3"/>
        <entry key="CODRelation" value="2"/>
        <entry key="COIRelation" value="1"/>
        <entry key="AdjunctRelation" value="1"/>
     </map>
    </group>
```

## WordSenseDisambiguation

**Class:** WordSenseDisambiguation

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
    <group name="wordSenseDisambiguation" class="WordSenseDisambiguation" >
      <!--param key="mode" value="b_Romanseval_most_frequent"/>     
      <param key="sensesPath" value="/home/cm218888/opendata/romanseval_data/SenseInventory" /-->      
      <param key="mode" value="b_Jaws_most_frequent"/>
      <param key="sensesPath" value="/home/cm218888/otherdata/jaws-1.0/SenseInventory" />      
      <!--param key="mode" value="s_Wsi_mrd"/>
      <param key="sensesPath" value="/home/cm218888/otherdata/wsi/clustersBin" />      
      <param key="mapping" value="m_Jaws_senses" />      
      <param key="mappingFile" value="mapping.txt" /-->
      <param key="dictionaryFile" value="/home/cm218888/otherdata/words.ids" />
      <param key="bestNNDir" value="knnall" />  
      <list name="NounContextList">
        <item value="COD_V"/>
        <!--item value="SUJ_V"/>
        <item value="COMPDUNOM"/>
        <item value="COMPDUNOM.reverse"/>
        <item value="SUBADJPOST.reverse"/>
        <item value="ADJPRENSUB.reverse"/>
        <item value="window5"/>
        <item value="window20"/-->
      </list>
      <map name="knnsearchConfig">
        <entry key="hashedDir" value="/home/cm218888/otherdata/hasheddb"/>
        <entry key="totalPermutations" value="10" />
        <entry key="beam" value="20" />
        <entry key="k" value="50" />
      </map>
    </group>
```
