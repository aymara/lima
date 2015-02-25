/***************************************************************************
 *   Copyright (C) 2004-2014 by CEA LIST                              *
 *                                                                         *
 ***************************************************************************/

#include "SpecificEntitiesCRFLearn.h"


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
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"
#include "linguisticProcessing/core/AnalysisDumpers/CrfSEDumper.h"

//#include <crfsuite_api.hpp>

#include "AbstractTrainerFactory.h"
#include "TrainerWap.h"

//#include "common/LimaCommon.h"
#include <common/QsLog/QsLog.h>

#include <QtXml/QXmlStreamReader>
#include <QFile>

#include <iostream>
#include <fstream>



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


SpecificEntitiesCRFLearn::SpecificEntitiesCRFLearn() {
}

SpecificEntitiesCRFLearn::~SpecificEntitiesCRFLearn() {

}

void SpecificEntitiesCRFLearn::init(
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

   try {
     string useAnnot=unitConfiguration.getParamsValueAtKey("useAnnot");
    if (useAnnot=="yes" ||
        useAnnot=="true" ||
        useAnnot=="1") {
      m_useAnnot=true;
    }
    else {
      m_useAnnot=false;
    }
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {
    // optional parameter: keep default value
  }
  

  m_lg=manager->getInitializationParameters().media;
  

  try {
    std::string resourcePath=Common::MediaticData::MediaticData::single().getResourcesPath();
    m_listpattern=unitConfiguration.getListsValueAtKey("patternList");
  } catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LERROR << "no parameter 'patternList' in group for language " << (int) m_lg << " !" << LENDL;
    throw InvalidConfiguration();
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

  options["patternFile"]=resourcePath+"/SpecificEntitiesCRF/"+m_listpattern[0];
  abTr=TrainerWapitiFactory.getFactory(m_crflib)->create();

  if (abTr!=NULL) {
    abTr->initOptions(options);
    if (m_useAnnot) {
      abTr->training();
    } else {
      abTr->learning(analysis, m_lg);
    }
  }

  // transformer rs to LimaStatusCode
  free(abTr);
  
  
	
  return lm;
}

 
 
}
}
}
