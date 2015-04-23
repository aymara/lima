/*
    Copyright 2002-2014 CEA LIST

    This file is part of LIMA.

    LIMA is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    LIMA is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with LIMA.  If not, see <http://www.gnu.org/licenses/>
 
************************************************************************/
#include "SpecificEntitiesCRFTag.h"


#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "linguisticProcessing/core/LinguisticResources/AbstractResource.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"

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


class SpecificEntitiesCRFTagPrivate {

  friend class SpecificEntitiesCRFTag;

  

public:
  SpecificEntitiesCRFTagPrivate();
  ~SpecificEntitiesCRFTagPrivate();
  SpecificEntitiesCRFTagPrivate(const SpecificEntitiesCRFTagPrivate& sp);
  SpecificEntitiesCRFTagPrivate& operator= ( const SpecificEntitiesCRFTagPrivate& sp );

  std::string m_crflib;
  std::deque<std::string> m_listmodel;
  MediaId m_lg;
  std::vector<mdl_t*> m_vecmod;

 
};
 
SpecificEntitiesCRFTagPrivate::SpecificEntitiesCRFTagPrivate() {

}

SpecificEntitiesCRFTagPrivate::~SpecificEntitiesCRFTagPrivate() {

}

SpecificEntitiesCRFTagPrivate::SpecificEntitiesCRFTagPrivate(const SpecificEntitiesCRFTagPrivate& sp) {

  m_crflib=sp.m_crflib;
  m_vecmod=sp.m_vecmod;
  m_listmodel=sp.m_listmodel;
  m_lg=sp.m_lg;

}
 
SpecificEntitiesCRFTagPrivate& SpecificEntitiesCRFTagPrivate::operator= ( const SpecificEntitiesCRFTagPrivate& sp ) {
  if (this!= &sp) {
    m_crflib=sp.m_crflib;
    m_vecmod=sp.m_vecmod;
    m_listmodel=sp.m_listmodel;
    m_lg=sp.m_lg;
  }
  return *this;

}



SpecificEntitiesCRFTag::SpecificEntitiesCRFTag() {
  m_spt=new SpecificEntitiesCRFTagPrivate();
}

SpecificEntitiesCRFTag::~SpecificEntitiesCRFTag() {
  for (int i=0; i<m_spt->m_vecmod.size(); ++i) {
    free(m_spt->m_vecmod[i]->opt);
    mdl_free(m_spt->m_vecmod[i]);
  }
  delete m_spt;
}

void SpecificEntitiesCRFTag::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{  
  SELOGINIT;
  try
    {
      m_spt->m_crflib=unitConfiguration.getParamsValueAtKey("crflibrary");
    }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
    {
      // optional parameter: keep default value
    }

  m_spt->m_lg=manager->getInitializationParameters().media;
  std::string resourcePath;
  try {
    resourcePath=Common::MediaticData::MediaticData::single().getResourcesPath();
    LDEBUG << "resourcePath " << resourcePath << LENDL;
    m_spt->m_listmodel=unitConfiguration.getListsValueAtKey("modelList");
    
  } catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LERROR << "no parameter 'modelList' in group for language " << (int) m_spt->m_lg << " !" << LENDL;
    throw InvalidConfiguration();
  }


  //load model
  mdl_t *mod;
  for (int i=0; i<m_spt->m_listmodel.size(); i++) {
    mod = mdl_new(rdr_new(false));
    opt_t *opt=(opt_t*) malloc(sizeof(opt_t));
    opt->mode=-1;
    opt->input = NULL;
    opt->output=NULL;
    opt->type="crf";
    opt->maxent=false;
    opt->algo="l-bfgs";
    opt->pattern=NULL;
    opt->model=strdup((resourcePath+"/SpecificEntities/"+m_spt->m_listmodel[i]).c_str());
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
      }
    mdl_load(mod, file);
    fclose(file);
    m_spt->m_vecmod.push_back(mod);
  }
   
  
  
}


LimaStatusCode SpecificEntitiesCRFTag::process(AnalysisContent& analysis) const
{
  LimaStatusCode lm=SUCCESS_ID;
  LinguisticMetaData *lMD=static_cast<LinguisticMetaData*>(analysis.getData("LinguisticMetaData"));

  AnalysisGraph* anagraph=static_cast<AnalysisGraph*>(analysis.getData("AnalysisGraph"));
  
  
  MediaId l;
  LimaString lmS;
  AbstractTagger *abTag;
  std::map<std::string, std::string> options;
  std::string resourcePath=Common::MediaticData::MediaticData::single().getResourcesPath();
  for (int i=0; i<m_spt->m_listmodel.size(); ++i) {
    
    options["model"]=resourcePath+"/SpecificEntitiesCRF/"+m_spt->m_listmodel[i];
    options["inputFile"]=lMD->getMetaData("FileName");
   
    abTag=TaggerWapitiFactory.getFactory(m_spt->m_crflib)->create();
    if (abTag!=NULL) {
      abTag->setMod(m_spt->m_vecmod[i]);
      abTag->initOptions(options);
      abTag->tag(analysis, m_spt->m_lg);

    }
    free(abTag);
    
  }

  


  return lm;
  
}
  
  
 
}
}
}
