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
#include "linguisticProcessing/core/Automaton/recognizerData.h"
#include "common/MediaticData/mediaticData.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"
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

void SemanticRoleLabelingLoader::init(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration, Manager* manager){

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
  AnalysisGraph* tokenList=static_cast<AnalysisGraph*>(analysis.getData(m_graph));
  if (tokenList==0) {
    LERROR << "graph " << m_graph << " has not been produced: check pipeline" << LENDL;
    return MISSING_DATA;
  }
  LinguisticGraph* resultGraph=tokenList->getGraph();
  AnnotationData* annotationData = static_cast<AnnotationData*>(analysis.getData("AnnotationData"));
  LimaConllTokenIdMapping* limaConllMapping= static_cast<LimaConllTokenIdMapping*>(analysis.getData("LimaConllTokenIdMapping"));

  QFile file("/home/clemence/textes_test/jamaica.outpython.conll");
  if (!file.open(QIODevice::ReadOnly))
    qDebug() << "cannot open file" << endl;
  int sentenceNb=1;
  std::map <int, QString> sentences;
  while (!file.atEnd()) {
    QByteArray text=file.readLine();
    QString textString = QString::fromUtf8(text.constData());
    //One assume that the input file does not start with a blank line
    if (textString.size()<3){
      sentenceNb++;
    }else {
      QString becomingSentence=sentences[sentenceNb]+textString;
      sentences[sentenceNb]= becomingSentence;
    }
  }

  SemanticRoleLabelingLoader::ConllHandler cHandler(m_language, analysis, tokenList);
  LimaString predicateTypeAnnotation="predicate";
  LimaString roleTypeAnnotation="semantic role";
  for (std::map<int,QString>::iterator it=sentences.begin(); it!=sentences.end(); ++it){
    int sentenceIndex=it->first;
    QString sentence=it->second;
    if(cHandler.extractSemanticInformation(sentenceIndex, limaConllMapping,sentence)){
      LDEBUG << "there is/are " << cHandler.m_verbalClassNb << "verbal class(es) for this sentence " << LENDL;
      for (int vClassIndex=0;vClassIndex<cHandler.m_verbalClassNb;vClassIndex++){
        LinguisticGraphVertex posGraphPredicateVertex=cHandler.m_verbalClasses[vClassIndex].first;
        LimaString verbalClass=cHandler.m_verbalClasses[vClassIndex].second;

        AnnotationGraphVertex annotPredicateVertex=annotationData->createAnnotationVertex();
        annotationData->addMatching("PosGraph", posGraphPredicateVertex, "annot", annotPredicateVertex);
        annotationData->annotate(annotPredicateVertex, Common::Misc::utf8stdstring2limastring("predicate"), verbalClass);


        LDEBUG << " A vertex was created for the verbal class "<< annotationData->stringAnnotation(annotPredicateVertex, predicateTypeAnnotation)<< LENDL;
        std::vector <pair<LinguisticGraphVertex,QString>>::iterator semRoleIt;
        for (semRoleIt=cHandler.m_semanticRoles[vClassIndex].begin();         semRoleIt!=cHandler.m_semanticRoles[vClassIndex].end();semRoleIt++){
          LinguisticGraphVertex posGraphRoleVertex=(*semRoleIt).first;
          LimaString semanticRole=(*semRoleIt).second;

          AnnotationGraphVertex annotRoleVertex=annotationData->createAnnotationVertex();
          annotationData->addMatching("PosGraph", posGraphRoleVertex, "annot", annotRoleVertex);
          AnnotationGraphEdge roleEdge=annotationData->createAnnotationEdge(annotPredicateVertex, annotRoleVertex);
          annotationData->annotate(roleEdge, roleTypeAnnotation,semanticRole);

          LDEBUG << " An edge annotated " << annotationData->stringAnnotation(roleEdge, roleTypeAnnotation)<< "was created between " << verbalClass << " and the Lima vertex " << posGraphRoleVertex << LENDL;
        }
      }
    }
  }
  return SUCCESS_ID;
}



SemanticRoleLabelingLoader::ConllHandler::ConllHandler(MediaId language, AnalysisContent& analysis, LinguisticAnalysisStructure::AnalysisGraph* graph):
m_language(language),
m_analysis(analysis),
m_graph(graph),
m_descriptorSeparator("\t+"),
m_tokenSeparator("\n+"),
m_verbalClasses(),
m_semanticRoles(),
m_verbalClassNb()
{
  SEMANTICANALYSISLOGINIT;
  LDEBUG << "SemanticRoleLabelingLoader::ConllHandler constructor";
}
SemanticRoleLabelingLoader::ConllHandler::~ConllHandler(){
  delete [] m_semanticRoles;
  delete [] m_verbalClasses;
}

// designed to be repeated on each sentence
bool SemanticRoleLabelingLoader::ConllHandler::extractSemanticInformation(int sentenceI, LimaConllTokenIdMapping* limaConllMapping, const QString & sent){
  SEMANTICANALYSISLOGINIT;
  SemanticRoleLabelingLoader::ConllHandler cHandler(m_language, m_analysis, m_graph);
  QStringList sentenceTokens=cHandler.splitSegment(sent, m_tokenSeparator);
  QStringList::const_iterator tokensIterator;
  QString firstSentenceToken=(*sentenceTokens.constBegin());
  int descriptorsNb=cHandler.splitSegment(firstSentenceToken, m_descriptorSeparator).size();
  m_verbalClassNb=descriptorsNb-11;
  if (m_verbalClassNb!=0){
    LDEBUG << sentenceI << " : \n" << sent << LENDL;
    int classIndex=0;
    m_verbalClasses=new std::pair<LinguisticGraphVertex, QString>[m_verbalClassNb];
    m_semanticRoles=new std::vector<std::pair<LinguisticGraphVertex,QString>>[m_verbalClassNb];
    //repeated on each token of the sentence, that is on each line
    for (tokensIterator = sentenceTokens.constBegin(); tokensIterator != sentenceTokens.constEnd();
            ++tokensIterator){
      int  roleNumbers=0;
      QStringList descriptors=cHandler.splitSegment((*tokensIterator),m_descriptorSeparator);
      if (descriptors.size()>=10){
        int conllTokenId=descriptors[0].toInt();
        QString conllToken=descriptors[1];
        if(descriptors[10]!="-"){
            QString verbalClass=descriptors[10];
            QString vClass=descriptors[10];
            LinguisticGraphVertex limaTokenId=cHandler.getLimaTokenId(conllTokenId, sentenceI, limaConllMapping);
            m_verbalClasses[classIndex]=make_pair(limaTokenId, vClass);
            classIndex++;
          }
          for (int roleTargetFieldIndex=0; roleTargetFieldIndex<m_verbalClassNb;roleTargetFieldIndex++){
            if (descriptors[11+roleTargetFieldIndex]!="-"){
              QString semanticRoleLabel=descriptors[11+roleTargetFieldIndex];
              LinguisticGraphVertex limaTokenId=cHandler.getLimaTokenId(conllTokenId, sentenceI,   limaConllMapping);
              LDEBUG << "The Lima token id matching the conll token id " << conllTokenId << " is " << limaTokenId<< LENDL;
              std::vector<std::pair<LinguisticGraphVertex,QString>> sRoles;
              m_semanticRoles[roleTargetFieldIndex].push_back(make_pair(limaTokenId,semanticRoleLabel));
              roleNumbers++;
            }
          }
        }
      }
    return classIndex;
  }
}



QStringList SemanticRoleLabelingLoader::ConllHandler::splitSegment(const QString & segment, QRegExp separator){
  QStringList segmentsSplited;
  segmentsSplited =segment.split(QRegExp(separator),QString::SkipEmptyParts);
  return segmentsSplited;
}

LinguisticGraphVertex SemanticRoleLabelingLoader::ConllHandler::getLimaTokenId(int conllTokenId, int sentenceI, LimaConllTokenIdMapping* limaConllMapping){
  SEMANTICANALYSISLOGINIT;
  std::map< int,std::map< int,LinguisticGraphVertex>>::iterator limaConllMappingIt;
  limaConllMappingIt=limaConllMapping->find(sentenceI);
  if (limaConllMappingIt == limaConllMapping->end()) {
    LERROR << "Sentence " << sentenceI << " not found";
    // TODO exit or raise an exception
  }
  std::map< int,LinguisticGraphVertex> limaConllId=(*limaConllMappingIt).second;
  std::map< int,LinguisticGraphVertex>::iterator limaConllIdIt=limaConllId.find(conllTokenId);
  if (limaConllIdIt==limaConllId.end()) {
    LERROR << "Conll token id " << conllTokenId << " not found";
    // TODO exit or raise an exception
  }
  LinguisticGraphVertex limaTokenId=limaConllIdIt->second;
  return limaTokenId;
}



}
}
} // end namespace

