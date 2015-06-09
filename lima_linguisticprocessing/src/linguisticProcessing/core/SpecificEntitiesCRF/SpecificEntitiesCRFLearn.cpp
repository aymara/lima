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


 ***************************************************************************
 *   Copyright (C) 2004-2014 by CEA LIST                                   *
 *                                                                         *
 ***************************************************************************/

#include "SpecificEntitiesCRFLearn.h"


#include "common/AbstractFactoryPattern/SimpleFactory.h"

#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"


#include "AbstractTrainerFactory.h"
#include "TrainerWap.h"


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

SimpleFactory<MediaProcessUnit,SpecificEntitiesCRFLearn> specificEntitiesCRFLearnFactory(SPECIFICENTITIESCRFLEARN_CLASSID);

  TrainerFactory<TrainerWap> TrainerWapitiFactory(TrainerWap_ID);
  //  TrainerFactory<TrainerSE> TrainerCRFSuiteFactory(TrainerSE_ID);


class SpecificEntitiesCRFLearnPrivate {

  friend class SpecificEntitiesCRFLearn;

  

public:
  SpecificEntitiesCRFLearnPrivate();
  ~SpecificEntitiesCRFLearnPrivate();
  SpecificEntitiesCRFLearnPrivate(const SpecificEntitiesCRFLearnPrivate& sp);
  SpecificEntitiesCRFLearnPrivate& operator= ( const SpecificEntitiesCRFLearnPrivate& sp );

  std::string m_crflib;

  std::string m_pattern;
  std::string m_model;

  MediaId m_lg;

};
 
SpecificEntitiesCRFLearnPrivate::SpecificEntitiesCRFLearnPrivate() {

}

SpecificEntitiesCRFLearnPrivate::~SpecificEntitiesCRFLearnPrivate() {

}

SpecificEntitiesCRFLearnPrivate::SpecificEntitiesCRFLearnPrivate(const SpecificEntitiesCRFLearnPrivate& sp) {

  m_crflib=sp.m_crflib;
  m_pattern=sp.m_pattern;
  m_model=sp.m_model;
  m_lg=sp.m_lg;

}
 
SpecificEntitiesCRFLearnPrivate& SpecificEntitiesCRFLearnPrivate::operator= ( const SpecificEntitiesCRFLearnPrivate& sp ) {
  if (this!=&sp) {
    m_crflib=sp.m_crflib;
    m_pattern=sp.m_pattern;
    m_model=sp.m_model;
    m_lg=sp.m_lg;
  }
  return *this;

}

SpecificEntitiesCRFLearn::SpecificEntitiesCRFLearn() {
  m_sp=new SpecificEntitiesCRFLearnPrivate();
}

SpecificEntitiesCRFLearn::~SpecificEntitiesCRFLearn() {
  delete m_sp;
}

void SpecificEntitiesCRFLearn::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  
  SELOGINIT;
   
  try
    {
      m_sp->m_crflib=unitConfiguration.getParamsValueAtKey("crflibrary");
    }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
    {
      // optional parameter: keep default value
    }

   

  m_sp->m_lg=manager->getInitializationParameters().media;
  

  try {
    std::string resourcePath=Common::MediaticData::MediaticData::single().getResourcesPath();
    m_sp->m_pattern=unitConfiguration.getParamsValueAtKey("patternFile");
  } catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LERROR << "no parameter 'patternList' in group for language " << (int) m_sp->m_lg << " !" << LENDL;
    throw InvalidConfiguration();
  }


  try {
     std::string resourcePath=Common::MediaticData::MediaticData::single().getResourcesPath();
     m_sp->m_model=unitConfiguration.getParamsValueAtKey("model");
   } catch (Common::XMLConfigurationFiles::NoSuchParam& )
     {
       
       m_sp->m_model="";
     }
   

}

LimaStatusCode SpecificEntitiesCRFLearn::process(
  AnalysisContent& analysis) const
{

  LimaStatusCode lm;
  lm = SUCCESS_ID;
 
  //LINFO << "start process" << LENDL;
  
  LinguisticMetaData *lMetaData=static_cast<LinguisticMetaData*>(analysis.getData("LinguisticMetaData"));  

  AbstractTrainer *abTr;

  std::string resourcePath=Common::MediaticData::MediaticData::single().getResourcesPath();
  std::map<std::string, std::string> options;
  //options["model"]=lMetaData->getMetaData("FileModel");
  options["inputFile"]=lMetaData->getMetaData("FileName");
  //options["patternFile"]=lMetaData->getMetaData("patternFile");

  options["patternFile"]=resourcePath+"/SpecificEntities/"+m_sp->m_pattern;

  if (m_sp->m_model!="") {
    options["model"]=resourcePath+"/SpecificEntitiesCRF/"+m_sp->m_model;
  }

  abTr=TrainerWapitiFactory.getFactory(m_sp->m_crflib)->create();

  if (abTr!=NULL) {
    abTr->initOptions(options);
      abTr->training();
  }

  
  free(abTr);
  
  
	
  return lm;
}

 
 
}
}
}
