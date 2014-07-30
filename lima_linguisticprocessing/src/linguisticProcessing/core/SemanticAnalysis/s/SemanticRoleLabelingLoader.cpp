/*
Copyright 2002-2014 CEA LIST

This file is part of LIMA.

LIMA is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

LIMA is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with LIMA. If not, see <http://www.gnu.org/licenses/>
*/
/************************************************************************
*
* @file SemanticRoleLabelingLoader.cpp
* @author Clémence Filmont <clemence.filmont@cea.fr>
* @date 2014--
* copyright Copyright (C) 2014 by CEA LIST
* Project mm_linguisticprocessing
*
*
***********************************************************************/

#include "SemanticRoleLabelingLoader.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "common/Data/strwstrtools.h"
#include "linguisticProcessing/core/Automaton/recognizerMatch.h"
#include "linguisticProcessing/core/Automaton/recognizerData.h"
#include "common/MediaticData/mediaticData.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationGraph.h"
#include <queue>
#include "QStringList"

using namespace std;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::LinguisticProcessing::ApplyRecognizer;
using namespace Lima::Common::XMLConfigurationFiles;



namespace Lima {
namespace LinguisticProcessing {
namespace SemanticAnalysis {

SimpleFactory<MediaProcessUnit,SemanticRoleLabelingLoader> SemanticRoleLabelingFactory(SEMANTICROLELABELINGLOADER_CLASSID);


//***********************************************************************
SemanticRoleLabelingLoader::SemanticRoleLabelingLoader():
m_language(0),
m_graph("AnalysisGraph"),
m_suffix(".conll")
{}

SemanticRoleLabelingLoader::~SemanticRoleLabelingLoader()
{
}

//***********************************************************************

void SemanticRoleLabelingLoader::init(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
            Manager* manager){

  PROCESSORSLOGINIT;
  m_language=manager->getInitializationParameters().media;
   AnalysisLoader::init(unitConfiguration,manager);
  try
  {
    m_suffix=unitConfiguration.getParamsValueAtKey("outputSuffix");
  }
   catch (NoSuchParam& ) {} // keep default value
    AnalysisLoader::init(unitConfiguration,manager);
  }

  
  LimaStatusCode SemanticRoleLabelingLoader::process(AnalysisContent& analysis) const{
    QFile file("/home/clemence/textes_test/jamaica_out.conll");
  }


SemanticRoleLabelingLoader::ConllHandler::ConllHandler(MediaId language, AnalysisContent& analysis, LinguisticAnalysisStructure::AnalysisGraph* graph):
m_tagIndex(),
m_language(language),
m_analysis(analysis),
m_graph(graph),
m_position(0),
m_length(0),
m_type(),
m_string(),
m_currentElement()
{
  PROCESSORSLOGINIT;
  LDEBUG << "SemanticRoleLabelingLoader::ConllHandler constructor";
}
SemanticRoleLabelingLoader::ConllHandler::~ConllHandler(){}

// repeated on each line beginning
bool extractSemanticRole(const QString & tokenDescription)
{
  QStringList descriptors;
  descriptors=tokenDescription.split(QRegExp("\\t+"));
// cout << descriptors[11]<<endl;
}


}
}
} // end namespace


