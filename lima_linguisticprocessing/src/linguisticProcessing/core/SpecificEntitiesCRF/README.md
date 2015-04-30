# The CRF-based Named Entities Recognizer of LIMA

This document presents how to use the CRF-based Named Entities recognition pipeline unit in LIMA. 
Refer to the [LIMA User Manual](https://github.com/aymara/lima/wiki/LIMA-User-Manual) for general 
information on pipeline units, named entities (Modex) and configuration files in LIMA.

The CRF (Conditional Random Fields) features are provided by the [Wapiti](http://wapiti.limsi.fr/) 
library. We include the 1.5.0 version for ease of use.


## Generate a model

To generate a model, you have to use the `SpecificEntitiesCRFLearn` process unit, in pipeline learn. 
This pipeline need a file in .dat format in input. You can use the `crfsedumper` to generate a file 
in this format.

## Pattern

You may also specify a pattern to use. The file is searched in the `$LIMA_RESOURCES/SpecificEntities/Patterns` directory.
You can also specify the name of a model already present in `$LIMA_RESOURCES/SpecificEntities/Modeles` to train again this model with a new set of data. In this case, the new data must have the same entity types than the previous model. It is not possibile to add new labels.

## Use the generated model

To use the generated model, you must place the created .mod file in the `$LIMA_RESOURCES/SpecificEntities/Modeles` directory.

To label a text, you have to use the `SpecificEntitiesCRFTag` process unit in pipeline `tag`.
You must specify the model to use in the `modellist` parameter in the file `lima-lp-eng`. If you specify more than one model, the models will be applied in sequence. The model files are found in the directory `$LIMA_RESOURCES/SpecificEntities/Modeles`

The `CrfSEDumper` dumper is used to dump a text in a .dat format compatible with Wapiti. You can precise which entities you want to dump in the configuration file.