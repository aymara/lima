LIMA - Libre Multilingual Analyzer
====================

LIMA is a multilingual linguistic analyzer developed by the CEA LIST [1], LVIC laboratory [2] (Vision and Content Engineering Laboratory). LIMA is available under a dual licensing model. 

The Free/Libre Open Source (FLOSS) version available under the Affero General Public License (AGPL) is fully functional with modules and resources to analyse English and French texts. You can thus use LIMA for all purposes as soon as the software linked to it or running it through Web services is Free software too.

The commercial version is completed on a one hand with modules useful only for some CEA LIST industrial partners and on the other hand with modules and resources necessary to analyze the other supported languages (Arabic, Chinese, German, Spanish, etc.). The commercial version is available directly from CEA LIST through R&D partnerships or through our partner ANT'inno [3] with offers including support and adaptation to one's needs.

LIMA is composed of the following modules:
- lima_common: common usage libraries;
- lima_linguisticprocessing: linguistic processing libraries;
- lima_linguisticdata: linguistic resources (dictionaries, rules,...);
- lima_pelf: evaluation tools and resources editing tool;
- lima_annoqt: a corpus annotation graphical user interface.

## FEATURES
- tokenization ;
- morphologic analysis including:
  - full-form dictionaries;
  - hyphen-words splitting;
  - concatenated words splitting (we're,...);
  - idiomatic expression recognizing;
  - part of speech tagging (two taggers are available. The LIMA legacy one, which is a little bit less performant but very useful for resources development, and a SVMTool++-based one [4]);
- rules-based named entities recognition;
- coreference resolution;
- syntactic analysis (surface rule-based dependency parsing);
- semantic analysis (disambiguation and semantic role labeling);
- manual corpus annotation GUI;
- regression testing;
- evaluation tools.


## INSTALLATION
To install LIMA under GNU/Linux, please refer to the INSTALL file. In short, some environement variables must be defined and the script gbuild.sh located at the root of the repository must be run from each module directory in turn (like lima_common $ ../gbuild.sh). LIMA depends on some linguistic data available in the NLTK project. See the INSTALL file for details.

LIMA can also be built and installed from MS Windows but it is somewhat complicated and not tested a lot currently.

## DOCUMENTATION
The available documentation is currently distributed among the various doc folders of the different modules. It is usually DocBook files. Some are still in French and should be translated soon.

We welcome external contributions in the form of comments, suggestions, bug reports, bugs corrections, resources, etc. But note that before merging your contributions, we will ask you to sign a Copyright Assignment Agreement in order to allow the proper functioning of the dual licensing model.

## CREDITS
LIMA uses several open source libraries and linguistic resources. See the COPYING file for details.

## LICENCE
The Free/Libre Open Source (FLOSS) version of LIMA is available under the Affero General Public License (AGPL). A commercial version exists too.

## REFERENCES
[1] http://www-list.cea.fr/en
[2] http://www.kalisteo.fr/en/index.htm
[3] http://www.antinno.fr/
[4] http://www.lsi.upc.edu/~nlp/SVMTool/
