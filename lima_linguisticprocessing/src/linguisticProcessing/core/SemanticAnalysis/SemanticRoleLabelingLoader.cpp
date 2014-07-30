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
*/
/************************************************************************
 *
 * @file       SemanticRoleLabelingLoader.cpp
 * @author     Clémence Filmont <clemence.filmont@cea.fr>
 * @date       2014--
 * copyright   Copyright (C) 2014 by CEA LIST
 * Project     mm_linguisticprocessing
 *
 *
 ***********************************************************************/

#include "SemanticRoleLabelingLoader.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "common/Data/strwstrtools.h"
#include "linguisticProcessing/core/Automaton/recognizerMatch.h"
#include "linguisticProcessing/core/Automaton/recognizerData.h"
#include "common/MediaticData/mediaticData.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "QStringList"
#include <queue>
#include <map>

using namespace std;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::LinguisticProcessing::ApplyRecognizer;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::LinguisticProcessing::SemanticAnalysis;
using namespace Lima::Common::AnnotationGraphs;




namespace Lima {
namespace LinguisticProcessing {
namespace SemanticAnalysis {

SimpleFactory<MediaProcessUnit,SemanticRoleLabelingLoader> SemanticRoleLabelingFactory(SEMANTICROLELABELINGLOADER_CLASSID);


//***********************************************************************
SemanticRoleLabelingLoader::SemanticRoleLabelingLoader():
m_language(0),
m_graph("PosGraph"),
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
    m_graph=unitConfiguration.getParamsValueAtKey("graph");

  }
  catch (NoSuchParam& ) {} // keep default value
  try
  {
    m_suffix=unitConfiguration.getParamsValueAtKey("outputSuffix");
  }
   catch (NoSuchParam& ) {} // keep default value
    AnalysisLoader::init(unitConfiguration,manager);
  }


  LimaStatusCode SemanticRoleLabelingLoader::process(AnalysisContent& analysis) const{
    PROCESSORSLOGINIT;
    AnalysisGraph* tokenList=static_cast<AnalysisGraph*>(analysis.getData(m_graph));
    if (tokenList==0) {
      LERROR << "graph " << m_graph << " has not been produced: check pipeline" << LENDL;
      return MISSING_DATA;
    }
    AnnotationData* annotationData = static_cast<AnnotationData*>(analysis.getData("AnnotationData"));
    int sentenceNb;
    QFile file("/home/clemence/textes_test/jamaica_out.conll");
    LimaConllTokenIdMapping* limaConllMapping= static_cast<LimaConllTokenIdMapping*>(analysis.getData("LimaConllTokenIdMapping"));

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
      std::cerr << "cannot open file" << endl;

    while (!file.atEnd()) {
      QByteArray line = file.readLine();
      SemanticRoleLabelingLoader::ConllHandler cHandler(m_language, analysis, tokenList);
      if (!cHandler.newSentence(line)){
        sentenceNb++;
      }
      if(cHandler.extractSemanticRole(sentenceNb, limaConllMapping,line)){
        std::map <LinguisticGraphVertex, std::map <LinguisticGraphVertex, string> >::iterator iter = cHandler.m_semanticRoleIndex.begin();
//         iter->first; //this is the "string" part
        std::map <LinguisticGraphVertex, string>::iterator iter2;  //this is the map<string, int> part 
        for(iter2 =iter->second.begin(); iter2!=iter->second.end(); iter2++){
        LDEBUG << iter2->second << " was found for the line " << line <<endl;
//         AnnotationGraphEdge semAnnotation=annotationData->createAnnotationEdge(it->first, );
//         annotationData->annotate(it->first, semRole, semRole);
//         annotationData->annotate(semAnnotation, "SemAnnot", roleName);
        }
      }
    }
    return SUCCESS_ID;
  }



SemanticRoleLabelingLoader::ConllHandler::ConllHandler(MediaId language, AnalysisContent& analysis, LinguisticAnalysisStructure::AnalysisGraph* graph):
m_semanticRoleIndex(),
m_language(language),
m_analysis(analysis),
m_graph(graph)
{
  PROCESSORSLOGINIT;
  LDEBUG << "SemanticRoleLabelingLoader::ConllHandler constructor";
}
SemanticRoleLabelingLoader::ConllHandler::~ConllHandler(){}

// repeated on each line beginning
bool SemanticRoleLabelingLoader::ConllHandler::extractSemanticRole(int sentenceNb, LimaConllTokenIdMapping* limaConllMapping, const QString & line)
{
  PROCESSORSLOGINIT;
  SemanticRoleLabelingLoader::ConllHandler cHandler(m_language, m_analysis, m_graph);
  QStringList descriptors;  
  descriptors=cHandler.splitConllColumn(line);
  if (!descriptors[0].isEmpty() && !descriptors[1].isEmpty() && !descriptors[6].isEmpty() && !descriptors[11].isEmpty()){
    int conllTargetTokenId=descriptors[0].toInt();
    int conllToken=descriptors[1].toInt();
    int conllSourceTokenId=descriptors[6].toInt();
    std::string semanticRoleLabel=descriptors[11].toStdString();
    std::map< int,std::map< int,LinguisticGraphVertex>>::iterator limaConllMappingIt = limaConllMapping->find(sentenceNb);
    if (limaConllMappingIt!= limaConllMapping->end() and limaConllMapping[sentenceNb].find(conllTargetTokenId)!= limaConllMapping->end() and limaConllMapping[sentenceNb].find(conllSourceTokenId)!= limaConllMapping->end()){
      LinguisticGraphVertex LimaTargetTokenId=limaConllMappingIt->second.at(conllTargetTokenId);
      LDEBUG << LimaTargetTokenId <<endl;
      LinguisticGraphVertex LimaSourceTokenId=limaConllMappingIt->second.at(conllSourceTokenId);
      LDEBUG << LimaSourceTokenId <<endl;
      std::map< LinguisticGraphVertex,std::string> RoleSourceTokenMapping;
      RoleSourceTokenMapping.insert(std::make_pair(LimaSourceTokenId,semanticRoleLabel));
      m_semanticRoleIndex.insert(std::make_pair(LimaTargetTokenId,RoleSourceTokenMapping));
    }else{
      LDEBUG <<  "not relation found in mapping for the token " << conllToken << " in the sentence " << sentenceNb <<endl;
    }
    return true;
  }else{
    LERROR << "no token on line" <<endl;
    return false;
  }
}


bool SemanticRoleLabelingLoader::ConllHandler::newSentence(const QString & line){
  SemanticRoleLabelingLoader::ConllHandler cHandler(m_language, m_analysis, m_graph);
  QStringList columns;
  columns=cHandler.splitConllColumn(line);
  if (columns[0].toInt()==1){
    return true;
  }else{
    return false;
  }
}

QStringList SemanticRoleLabelingLoader::ConllHandler::splitConllColumn(const QString & line){
  QStringList descriptors;
  descriptors=line.split(QRegExp("\\t+"));
  return descriptors;  
}


}
}
} // end namespace


