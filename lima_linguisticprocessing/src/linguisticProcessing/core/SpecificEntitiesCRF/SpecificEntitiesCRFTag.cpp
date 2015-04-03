/***************************************************************************
 *   Copyright (C) 2004-2014 by CEA LIST                              *
 *                                                                         *
 ***************************************************************************/

#include "SpecificEntitiesCRFTag.h"


#include "linguisticProcessing/core/Automaton/SpecificEntityAnnotation.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "common/time/traceUtils.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/core/LinguisticResources/AbstractResource.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
//#include "linguisticProcessing/core/LinguisticAnalysisStructure/SentenceBounds.h"
#include "linguisticProcessing/core/SyntacticAnalysis/SyntacticData.h"
#include "linguisticProcessing/core/Automaton/recognizerMatch.h"
#include "common/MediaProcessors/MediaProcessUnit.h"

#include "linguisticProcessing/core/LinguisticProcessors/LimaStringText.h"
//#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"

#include "linguisticProcessing/core/SpecificEntities/SpecificEntitiesConstraints.h"
#include "linguisticProcessing/core/Automaton/recognizerData.h"
#include "common/MediaticData/mediaticData.h"


//#include <crfsuite_api.hpp>

#include <stdlib.h>
#include <stdio.h>

#include "AbstractTaggerFactory.h"
#include "TaggerWap.h"

using namespace std;
using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::LinguisticProcessing::Automaton;

namespace Lima
{

namespace LinguisticProcessing
{

namespace SpecificEntitiesCRF
{

SimpleFactory<MediaProcessUnit,SpecificEntitiesCRFTag> specificEntitiesCRFTagFactory(SPECIFICENTITIESCRFTAG_CLASSID);

 
TaggerFactory<TaggerWap> TaggerWapitiFactory(TaggerWap_ID);
  //TaggerFactory<TaggerSE> TaggerCRFSuiteFactory(TaggerSE_ID);

SpecificEntitiesCRFTag::SpecificEntitiesCRFTag() {

}

SpecificEntitiesCRFTag::~SpecificEntitiesCRFTag() {
  for (int i=0; i<m_vecmod.size(); ++i) {
    free(m_vecmod[i]->opt);
    mdl_free(m_vecmod[i]);
  }

}

void SpecificEntitiesCRFTag::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{  
  SELOGINIT;
  try
    {
      m_crflib=unitConfiguration.getParamsValueAtKey("crflibrary");
    }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
    {
      // optional parameter: keep default value
    }

  m_lg=manager->getInitializationParameters().media;
  std::string resourcePath;
  try {
    resourcePath=Common::MediaticData::MediaticData::single().getResourcesPath();
    LDEBUG << "resourcePath " << resourcePath << LENDL;
    m_listmodel=unitConfiguration.getListsValueAtKey("modelList");
    
  } catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LERROR << "no parameter 'modelList' in group for language " << (int) m_lg << " !" << LENDL;
    throw InvalidConfiguration();
  }


  //load model
  mdl_t *mod;
  for (int i=0; i<m_listmodel.size(); i++) {
    mod = mdl_new(rdr_new(false));
    opt_t *opt=(opt_t*) malloc(sizeof(opt_t));
    opt->mode=-1;
    opt->input = NULL;
    opt->output=NULL;
    opt->type="crf";
    opt->maxent=false;
    opt->algo="l-bfgs";
    opt->pattern=NULL;
    opt->model=strdup((resourcePath+"/SpecificEntities/"+m_listmodel[i]).c_str());
    opt->devel=NULL;
    opt->rstate=NULL;
    opt->sstate=NULL;
    opt->compact=false;
    opt->sparse=NULL;
    opt->nthread=1;
    opt->jobsize=64;
    opt->maxiter=0;
    opt->rho1=0.5;
    opt->rho2=0.0001;
    opt->objwin=5;
    opt->stopwin=5;
    opt->stopeps=0.02;
    opt->lbfgs.clip=false;
    opt->lbfgs.histsz=5;
    opt->lbfgs.maxls=40;
    opt->sgdl1.eta0=0.8;
    opt->sgdl1.alpha=0.85;
    opt->bcd.kappa=1.5;
    opt->rprop.stpmin= 1e-8;
    opt->rprop.stpmax=50.0;
    opt->rprop.stpdec=0.5;
    opt->rprop.cutoff=false;
    opt->label=false;
    opt->check=false;
    opt->outsc=false;
    opt->lblpost=false;
    opt->nbest=1;
    opt->force=false;
    opt->prec=5;
    opt->all=false;
    
    mod->opt=opt;
    // First, load the model provided by the user. This is mandatory to
    // label new datas ;-)
    if (mod->opt->model == NULL)
      {
	//LERROR  << "you must specify a model" << LENDL;
	std::cout << "you must specify a model" << std::endl;
      }
    FILE *file = fopen(mod->opt->model, "r");
    if (file == NULL)
      {
	std::string mess="cannot open input model file";
	throw(std::runtime_error(mess));
	//LERROR << "cannot open input model file" << LENDL;
      }//pfatal("cannot open input model file");
    mdl_load(mod, file);
    fclose(file);
    m_vecmod.push_back(mod);
  }
   
  
  
}


LimaStatusCode SpecificEntitiesCRFTag::process(AnalysisContent& analysis) const
{
  std::cout << "process" << std::endl;
  LimaStatusCode lm=SUCCESS_ID;
  //LINFO << "start process" << LENDL;
  LinguisticMetaData *lMD=static_cast<LinguisticMetaData*>(analysis.getData("LinguisticMetaData"));

  AnalysisGraph* anagraph=static_cast<AnalysisGraph*>(analysis.getData("AnalysisGraph"));
  
  
  MediaId l;
  LimaString lmS;
  AbstractTagger *abTag;
  std::map<std::string, std::string> options;
  std::string resourcePath=Common::MediaticData::MediaticData::single().getResourcesPath();
  for (int i=0; i<m_listmodel.size(); ++i) {
    
    options["model"]=resourcePath+"/SpecificEntitiesCRF/"+m_listmodel[i];
    //std::cout <<"model: " << options["model"] << std::endl;
    //options["model"]=lMD->getMetaData("modelFile");
    options["inputFile"]=lMD->getMetaData("FileName");
    //options["patternFile"]=lMD->getMetaData("patternFile");  
    
    abTag=TaggerWapitiFactory.getFactory(m_crflib)->create();
    if (abTag!=NULL) {
      abTag->setMod(m_vecmod[i]);
      abTag->initOptions(options);
      abTag->tag(analysis, m_lg);

    }
    free(abTag);
    
  }

  


  return lm;
  
}
  
  
 
}
}
}
