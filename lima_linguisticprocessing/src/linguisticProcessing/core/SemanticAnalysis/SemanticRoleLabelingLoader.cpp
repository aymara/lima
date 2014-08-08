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
#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "QStringList"
#include <QString>
#include "QTextCodec"
#include <boost/regex.hpp>
#include <queue>
#include <map>
#include <utility>
#include <iostream>
#include<fstream>

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

  SEMANTICANALYSISLOGINIT;
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
  SEMANTICANALYSISLOGINIT;
  std::cerr << "je suis le fichier SRLLoader" << endl;
  AnalysisGraph* tokenList=static_cast<AnalysisGraph*>(analysis.getData(m_graph));
  if (tokenList==0) {
    LERROR << "graph " << m_graph << " has not been produced: check pipeline" << LENDL;
    return MISSING_DATA;
  }
  AnnotationData* annotationData = static_cast<AnnotationData*>(analysis.getData("AnnotationData"));
  int sentencesNb;
  LimaConllTokenIdMapping* limaConllMapping= static_cast<LimaConllTokenIdMapping*>(analysis.getData("LimaConllTokenIdMapping"));

  QFile file("/home/clemence/textes_test/jamaica_out.conll");
  int sentenceNb=1;
  std::map <int, QString> sentence;
  if (!file.open(QIODevice::ReadOnly))
    qDebug() << "cannot open file" << endl;
  while (!file.atEnd()) {
    QByteArray text=file.readLine();
    QString textString = QString::fromUtf8(text.constData());
//     LDEBUG << " ligne : " << textString << " " << LENDL;
    if (textString.size()<3){
      sentenceNb++;
    }else {
      QString becomingSentence=sentence[sentenceNb]+textString;
      sentence[sentenceNb]= becomingSentence;
    }
  }
  qDebug() << " There is " << sentenceNb << "sentences ";

//     SemanticRoleLabelingLoader::ConllHandler cHandler(m_language, analysis, tokenList);
//     if(cHandler.extractSemanticInformations(sentenceNb, limaConllMapping,sentence)){
// // 
// // //         AnnotationGraphEdge semAnnotation=annotationData->createAnnotationEdge(it->first, );
// // //         annotationData->annotate(it->first, semRole, semRole);
// // //         annotationData->annotate(semAnnotation, "SemAnnot", roleName);
//     }
  
  return SUCCESS_ID;
}



SemanticRoleLabelingLoader::ConllHandler::ConllHandler(MediaId language, AnalysisContent& analysis, LinguisticAnalysisStructure::AnalysisGraph* graph):
m_language(language),
m_analysis(analysis),
m_graph(graph),
m_descriptorSeparator("\\t+"),
m_tokenSeparator("\\n+")
{
  SEMANTICANALYSISLOGINIT;
  LDEBUG << "SemanticRoleLabelingLoader::ConllHandler constructor";
}
SemanticRoleLabelingLoader::ConllHandler::~ConllHandler(){}

// repeated on each sentence
bool SemanticRoleLabelingLoader::ConllHandler::extractSemanticInformations(int sentenceNb, LimaConllTokenIdMapping* limaConllMapping, const QString & sent)
{
  SEMANTICANALYSISLOGINIT;
  SemanticRoleLabelingLoader::ConllHandler cHandler(m_language, m_analysis, m_graph);
  QStringList sentenceTokens=cHandler.splitSegment(sent, m_tokenSeparator);
  QStringList::const_iterator tokensIterator;
  QString firstSentenceToken=(*sent.constBegin());
  int classIndex=0;
  int classNumbers=cHandler.splitSegment(firstSentenceToken, m_descriptorSeparator).size()-11;

//   LDEBUG << "There is " << classNumbers << "verbal classes in the sentence " <<LENDL;
  
//   verbClasses=new std::pair<int, QString>[classNumbers];
  //repeated on each token of the sentence, that is on each line
//   for (tokensIterator = sentenceTokens.constBegin(); tokensIterator != sentenceTokens.constEnd();
//             ++tokensIterator){
//     int  roleNumbers=0;
//     QStringList descriptors=cHandler.splitSegment((*tokensIterator),"\\t+");
//     if (descriptors.size()>=10){
//       int conllTokenId=descriptors[0].toInt();
//       int conllToken=descriptors[1].toInt();
//       std::string semanticRoleLabel=descriptors[11].toStdString();
//       if(descriptors[10]!="-"){
//         QString vClass=descriptors[10];
//         LinguisticGraphVertex limaTokenId=cHandler.getLimaTokenId(conllTokenId, sentenceNb, limaConllMapping);
//           LDEBUG << limaTokenId <<endl;
//           verbClasses[classIndex]=make_pair(limaTokenId, vClass);
//           classIndex++;
//           break;
//       }else{
//         LDEBUG <<  "no relation found in mapping for the token " << conllToken << " in the sentence " << sentenceNb <<endl;
//       }
//       for (int i = descriptors.size()-classNumbers; i < descriptors.size(); ++i){
//         if (descriptors.at(i)!="-"){
//           LinguisticGraphVertex limaTokenId=cHandler.getLimaTokenId(conllTokenId, sentenceNb, limaConllMapping);
//           semanticRoles=new std::vector<std::pair<int,QString>>[classNumbers];
//           semanticRoles[classNumbers][roleNumbers]=make_pair(limaTokenId,descriptors.at(i));
//           roleNumbers++;
//           delete [] semanticRoles;
//         }
//       }
//       return true;
//     }else{
//       LERROR << "no token on line" <<endl;
//       return false;
//     }
//   }
//   delete [] verbClasses;
}


bool SemanticRoleLabelingLoader::ConllHandler::newSentence(const QString & line){
  SemanticRoleLabelingLoader::ConllHandler cHandler(m_language, m_analysis, m_graph);
  QStringList columns;
  columns=cHandler.splitSegment(line, m_descriptorSeparator);
  if (columns[0].toInt()==1){
    return true;
  }else{
    return false;
  }
}

QStringList SemanticRoleLabelingLoader::ConllHandler::splitSegment(const QString & segment, QRegExp separator)
{
QStringList segmentsSplited;
segmentsSplited =segment.split(QRegExp(separator));
return segmentsSplited;
}

LinguisticGraphVertex SemanticRoleLabelingLoader::ConllHandler::getLimaTokenId(LinguisticGraphVertex conllTokenId, int sentenceNb, LimaConllTokenIdMapping* limaConllMapping){
  std::map< int,std::map< int,LinguisticGraphVertex>>::iterator limaConllMappingIt = limaConllMapping->find(sentenceNb);
  if (limaConllMappingIt!= limaConllMapping->end() and limaConllMapping[sentenceNb].find(conllTokenId)!= limaConllMapping->end()){
  LinguisticGraphVertex limaTokenId=limaConllMappingIt->second.at(conllTokenId);
  return limaTokenId; 

  }
}

}
}
} // end namespace


