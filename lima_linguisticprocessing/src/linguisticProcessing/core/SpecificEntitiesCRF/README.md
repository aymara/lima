This document presents how to use to crf unit in LIMA.
The crf is provided by the library Wapiti. http://wapiti.limsi.fr/

Generate a model:
To generate a model, you have to use the process unit SpecificEntitiesCRFLearn, in pipeline learn. This pipeline need a file in format .dat in entry. You can use the crfsedumper to generate a file in this format.
Pattern
You may also specify a pattern to use, the file is searched in lima_linguisticdata/SpecificEntities/Patterns repertory
You can also specify the name of a model already present in lima_linguisticdata/SpecificEntities/Modeles to train again this model with a new set of data. In this case, the new data must have the same entities than the previous model (no possibility to had new labels)

To use the generated model, you must place to file .mod created in the repertory lima_linguisticdata/SpecificEntities/Modeles

Label datas:
To label a text, you have to use the process unit SpecificEntitiesCRFTag in pipeline tag
You must specify the model to use in the parameter modellist in the file mm-lp-eng (if you specify more than one model, the models will be applied in following). The model files are found in the repertory lima_linguisticdata/SpecificEntities/Modeles

CrfSEDumper: this dumper is used to dump a text in a format .dat compatible with wapiti. You can precise which entities you wanted to dump in the configuration file.