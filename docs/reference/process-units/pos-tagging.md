# Part-of-speech tagging process units

Legacy (non-neural) part-of-speech taggers. The neural tagger is described with the [neural units](neural.md).

## ViterbiPosTagger

**Class:** ViterbiPosTagger

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
    <group name="viterbiPostagger-freq" class="ViterbiPosTagger">
      <param key="trigramFile" value="Disambiguation/trigramMatrix-fre.dat"/>
      <param key="bigramFile" value="Disambiguation/bigramMatrix-fre.dat"/>
      <param key="costFunction" value="FrequencyCost"/>
      <param key="defaultCategory" value="PONCTU_FORTE"/>
      <list name="stopCategories">
        <item value="PONCTU_FORTE" />
      </list>
    </group>
    <group name="viterbiPostagger-int" class="ViterbiPosTagger">
      <param key="trigramFile" value="Disambiguation/trigramMatrix-fre.dat"/>
      <param key="bigramFile" value="Disambiguation/bigramMatrix-fre.dat"/>
      <param key="costFunction" value="IntegerCost"/>
      <param key="defaultCategory" value="PONCTU_FORTE"/>
      <list name="stopCategories">
        <item value="PONCTU_FORTE" />
      </list>
    </group>
    <group name="viterbiPostagger-int-none" class="ViterbiPosTagger">
      <param key="trigramFile" value="Disambiguation/trigramMatrix-fre.dat"/>
      <param key="bigramFile" value="Disambiguation/bigramMatrix-fre.dat"/>
      <param key="costFunction" value="IntegerCost"/>
      <param key="defaultCategory" value="NONE_1"/>
      <list name="stopCategories">
        <item value="PONCTU_FORTE" />
      </list>
    </group>
```

## SvmToolPosTagger

**Class:** SvmToolPosTagger

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
    <group name="SvmToolPosTagger" class="SvmToolPosTagger">
      <param key="model" value="Disambiguation/SVMToolModel-fre/lima"/>
      <param key="defaultCategory" value="PONCTU_FORTE"/>
      <list name="stopCategories">
        <item value="PONCTU_FORTE" />
      </list>
    </group>
```

## DynamicSvmToolPosTagger

**Class:** DynamicSvmToolPosTagger

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
    <group name="DynamicSvmToolPosTagger" class="DynamicSvmToolPosTagger">
      <param key="model" value="Disambiguation/SVMToolModel-fre/lima"/>
      <param key="defaultCategory" value="PONCTU_FORTE"/>
      <list name="stopCategories">
        <item value="PONCTU_FORTE" />
      </list>
    </group>
```
