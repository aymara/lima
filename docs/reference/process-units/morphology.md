# Morphology process units

Process units that look tokens up in dictionaries, split compounds and assign default properties to unknown words.

## EnchantSpellingAlternatives

**Class:** EnchantSpellingAlternatives

**Role:**
Use the enchant spell checker to find corrections for tokens not found in the dictionary.

**Inputs:** the AnalysisGraph.

Parameters |  |
--- | --- |
dictionary | the LIMA dictionary resource (usually mainDictionary) where to search for suggestions by Enchant |

**Outputs:** the same AnalysisGraph enriched with spelling corrections

**Preconditions:** the AnalysisGraph must already exist

**Effects:** the AnalysisGraph will tokens that had no linguistic information are enriched with spelling alternatives.

**Notes:**

  * This process unit is available only if the Enchant spell checker has been found at compile time.

## RegexMatcher

**Class:** RegexMatcher

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
    <group name="regexmatcher" class="RegexMatcher">
      <map name="regexes">
        <entry key="[\w\-_]+(\.[\w\-_]+)*\@[\w\-_](\.[\w\-_]+)+" value="t_url"/>
        <entry key="((mailto|http|ftp|https):\/\/)?[\w\-_]+(\.[\w\-_]+)+([\w\-\.,@?^=%&amp;:/~\+#]*[\w\-\@?^=%&amp;/~\+#])?" value="t_url"/>
      </map>
    </group>
```

## SimpleWord

**Class:** SimpleWord

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
    <group name="simpleWord" class="SimpleWord">
            <param key="dictionary" value="mainDictionary"/>
        <param key="confidentMode" value="true"/>
        <param key="charChart" value="flatcharchart"/>
        <param key="parseConcatenated" value="false"/>
    </group>
```

## HyphenWordAlternatives

**Class:** HyphenWordAlternatives

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
    <group name="hyphenWordAlternatives" class="HyphenWordAlternatives">
      <param key="dictionary" value="mainDictionary"/>
      <param key="charChart" value="flatcharchart"/>
      <param key="tokenizer" value="flattokenizer"/>
    </group>
```

## DefaultProperties

**Class:** DefaultProperties

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
    <group name="defaultProperties" class="DefaultProperties">
      <param key="dictionary" value="mainDictionary"/>
      <param key="charChart" value="flatcharchart"/>
      <param key="defaultPropertyFile" value="LinguisticProcessings/fre/default-fre.dat"/>
      <list name="skipUnmarkStatus">
        <item value="t_dot_number"/>
        <item value="t_capital_1st"/>
      </list>
    </group>
```

## SimpleDefaultProperties

**Class:** SimpleDefaultProperties

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
    <group name="simpleDefaultProperties" class="SimpleDefaultProperties">
      <list name="defaultCategories">
        <item value="NP NP"/>
      </list>
    </group>
```
