# Tokenization process units

Process units that split the input text into tokens and sentences.

## FlatTokenizer

**Class:** FlatTokenizer

**Role:**
The role of this process unit is to split the input text in tokens. It uses for this an automaton allowing a rich behavior, far away a simple tokenization on white spaces. It is usually the first element of the pipeline.

**Inputs:** an AnalysisContent containing the initial text.

Parameters |  |
--- | --- |
automatonFile | the path to the tokenization automaton file to use, relative to the main resources folder |
charChart | The name of a group in the Resources module. This defines a resource of class FlatTokenizerCharChart with a parameter named charFile giving the path to the chars chart file to use, relative to the main resources folder |

**Outputs:** an AnalysisContent

**Preconditions:** the AnalysisContent must contain an AnalysisData of type LimaStringText named "Text"

**Effects:** the AnalysisContent will contain an AnalysisData of type AnalysisGraph named "AnalysisGraph" which is a linear graph (a string) containing one vertex for each detected token.

## SentenceBoundariesFinder

**Class:** SentenceBoundariesFinder

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
    <group name="sentenceBoundariesFinder" class="SentenceBoundariesFinder">
      <param key="graph" value="PosGraph"/>
      <list name="micros">
        <item value="PONCTU_FORTE" />  
      </list>
    </group>
```
