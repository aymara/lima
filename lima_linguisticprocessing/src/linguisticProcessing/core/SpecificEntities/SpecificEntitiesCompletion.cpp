/*
    Copyright 2002-2021 CEA LIST

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

#include "SpecificEntitiesCompletion.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/common/linguisticData/LimaStringText.h"
#include "linguisticProcessing/core/Automaton/SpecificEntityAnnotation.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "linguisticProcessing/core/Automaton/recognizerData.h"
#include "common/Data/strwstrtools.h"
#include "common/time/timeUtilsController.h"
#include <queue>

using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::Misc;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::LinguisticProcessing::Automaton;
using namespace Lima::LinguisticProcessing::ApplyRecognizer;
using namespace std;

namespace Lima {
namespace LinguisticProcessing {
namespace SpecificEntities {

SimpleFactory<MediaProcessUnit,SpecificEntitiesCompletion> specificEntitiesCompletion(SPECIFICENTITIESCOMPLETION_CLASSID);

SpecificEntitiesCompletion::SpecificEntitiesCompletion():
MediaProcessUnit(),
m_language(),
m_graph("AnalysisGraph"),
m_entityTypes()
{}


SpecificEntitiesCompletion::~SpecificEntitiesCompletion()
{}

void SpecificEntitiesCompletion::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  SELOGINIT;
  m_language=manager->getInitializationParameters().media;
  try
  {
    deque<string> types=unitConfiguration.getListsValueAtKey("entityTypes");
    for (const auto& s: types) {
      m_entityTypes.insert(MediaticData::single().getEntityType(utf8stdstring2limastring(s)));
    }
  }
  catch (Common::XMLConfigurationFiles::NoSuchList& )
  {
    LDEBUG << "No param 'entityTypes' in SpecificEntitiesCompletion group: nothing to do";
  }

  try
  {
    m_graph=unitConfiguration.getParamsValueAtKey("graph");
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {} // keep default value

}

// internal class holding information of found entities that is
// useful for searching other occurrences
class EntityInfo {
public:
  EntityType entityType;
  LimaString entityString;
  LimaString entityNorm;
  mutable std::set<std::pair<unsigned int,unsigned int> > occurrences;

  EntityInfo(const EntityType& t, const LimaString& e, const LimaString& n):
    entityType(t),
    entityString(e),
    entityNorm(n),
    occurrences() {}

  void addOccurrence(unsigned int posBegin, unsigned int posEnd)
  {
    occurrences.insert(make_pair(posBegin,posEnd));
  }

  LimaString regex() const {
    // simplest for the moment: string equality
    return entityString;
  }

  // comparison operator use equality on string and type (norm may be empty)
  bool operator<(const EntityInfo& other) const {
    if (this->entityType<other.entityType) {
      return true;
    }
    else if (this->entityType==other.entityType) {
      return this->entityString<other.entityString;
    }
    return false;
  }

  // debug
  friend QDebug& operator<<(QDebug& os, const EntityInfo& ent) {
    os <<  MediaticData::single().getEntityName(ent.entityType).toUtf8().constData()
    << "/" << ent.entityString.toUtf8().constData()
    << "/" << ent.entityNorm.toUtf8().constData()
    << "/";
    for (const auto& occ: ent.occurrences) {
      os << "[" << occ.first << "-" << occ.second << "]";
    }
    return os;
  }
};

class SpecificEntitiesCompletion::Entities: public set<EntityInfo>
{
public:
  Entities():set<EntityInfo>() {}

  void add(LinguisticGraphVertex v,
           const SpecificEntityAnnotation* annot,
           const VertexTokenPropertyMap& tokenMap)
  {
    unsigned int posBegin=annot->getPosition();
    unsigned int posEnd=posBegin+annot->getLength();
    EntityType entityType=annot->getType();
    auto token=tokenMap[v];
    if (token==0) {
      SELOGINIT;
      LERROR << "Empty token for vertex" << v;
      return;
    }
    LimaString entityString=token->stringForm();
    // get entity normalization in features
    LimaString entityNorm;
    for (const auto& f: annot->getFeatures())
    {
      if (f.getName()=="value") {
        entityNorm=f.getValueLimaString();
        break;
      }
    }
    EntityInfo ent(entityType,entityString,entityNorm);
    Entities::iterator it=find(ent);
    if (it==end()) {
      ent.addOccurrence(posBegin,posEnd);
      insert(ent);
    }
    else {
      // can't use addOccurrence (because iterator in a set is always const)
      // byt can modify occurences member directly because I made it mutable
      // (it is not used in the comparison function)
      (*it).occurrences.insert(make_pair(posBegin,posEnd));
    }
  }

  // debug
  friend QDebug& operator<<(QDebug& os, const Entities& ent) {
    for (const auto& e: ent) {
      os << e << Qt::endl;
    }
    return os;
  }

}; // end of class


// internal class holding information of new entity occurrences
// do not use the same EntityInfo class because, for new occurrence, having individual occurence is more efficient
class SpecificEntitiesCompletion::EntityOccurrence {
public:
  EntityType entityType;
  LimaString entityString;
  LimaString entityNorm;
  unsigned int posBegin;
  unsigned int posEnd;

  EntityOccurrence(const EntityType& t,
                   const LimaString& s,
                   const LimaString& n,
                   const pair<unsigned int,unsigned int>& p):
    entityType(t),entityString(s),entityNorm(n),posBegin(p.first),posEnd(p.second)
  {}

  // debug
  friend QDebug& operator<<(QDebug& os, const EntityOccurrence& occ) {
    os <<  MediaticData::single().getEntityName(occ.entityType).toUtf8().constData()
    << "/" << occ.entityString.toUtf8().constData()
    << "/" << occ.entityNorm.toUtf8().constData()
    << "/[" << occ.posBegin << "-" << occ.posEnd << "]";
    return os;
  }

  friend QDebug& operator<<(QDebug& os, const std::vector<EntityOccurrence>& occ) {
    for (const auto& o: occ) { os << o << Qt::endl; };
    return os;
  }

};


LimaStatusCode SpecificEntitiesCompletion::process(
  AnalysisContent& analysis) const
{
  Lima::TimeUtilsController timer("SpecificEntitiesCompletion");
  SELOGINIT;
  LINFO << "start process";

  // possible implementations:
  // - create dynamic recognizers and apply them
  // - use string matching on the text and report results on the graph
  //
  // use second approach

  auto graphp = static_cast<LinguisticAnalysisStructure::AnalysisGraph*>(analysis.getData(m_graph));
  if (graphp == nullptr)
  {
    SELOGINIT;
    LERROR << "no graph "<< m_graph <<" ! abort";
    return MISSING_DATA;
  }
  const auto& graph = *graphp;
  auto lingGraph = const_cast<LinguisticGraph*>(graph.getGraph());
  auto tokenMap = get(vertex_token, *lingGraph);

  // gather found entities of the selected types
  Entities foundEntities;
  getEntities(analysis,foundEntities,tokenMap);
  LDEBUG << "SpecificEntitiesCompletion: found entities" << Qt::endl << foundEntities;

  // find entities in text
  std::vector<EntityOccurrence> newEntities;
  findOccurrences(foundEntities,analysis,newEntities);
  LDEBUG << "SpecificEntitiesCompletion: found new occurrences" << Qt::endl << newEntities;

  // report found entities in graph data
  // use existing CreateSpecificEntity function, create RecognizerMatch
  updateAnalysis(newEntities,analysis);

  return SUCCESS_ID;
}

void SpecificEntitiesCompletion::getEntities(AnalysisContent& analysis, Entities& foundEntities,
                                             const VertexTokenPropertyMap& tokenMap) const
{
  AnnotationData* annotationData = static_cast< AnnotationData* >(analysis.getData("AnnotationData"));
  if (annotationData==0) {
    SELOGINIT;
    LDEBUG << "SpecificEntitiesCompletion: no annotation data";
    return;
  }

  // take all annotations
  AnnotationGraphVertexIt itv, itv_end;
  boost::tie(itv, itv_end) = vertices(annotationData->getGraph());
  for (; itv != itv_end; itv++)
  {
    if (annotationData->hasAnnotation(*itv,Common::Misc::utf8stdstring2limastring("SpecificEntity")))
    {
      const SpecificEntityAnnotation* annot = 0;
      try
      {
        annot = annotationData->annotation(*itv,Common::Misc::utf8stdstring2limastring("SpecificEntity"))
          .pointerValue<SpecificEntityAnnotation>();
      }
      catch (const boost::bad_any_cast& )
      {
        SELOGINIT;
        LERROR << "This annotation is not a SpecificEntity; SE not logged";
        continue;
      }

      // recuperer l'id du vertex morph cree
      LinguisticGraphVertex v;
      if (!annotationData->hasIntAnnotation(*itv,Common::Misc::utf8stdstring2limastring(m_graph)))
      {
        continue;
      }
      v = annotationData->intAnnotation(*itv,Common::Misc::utf8stdstring2limastring(m_graph));
      if (m_entityTypes.find(annot->getType())!=m_entityTypes.end()) {
        foundEntities.add(v,annot,tokenMap);
      }
    }
  }
}

void SpecificEntitiesCompletion::
findOccurrences(Entities& foundEntities,
                AnalysisContent& analysis,
                std::vector<EntityOccurrence>& newEntities) const
{
  LimaStringText* text=static_cast<LimaStringText*>(analysis.getData("Text"));
  for (const auto& e: foundEntities) {
    QRegExp rx(e.regex());
    int pos = 0;
    while ((pos = rx.indexIn(*text, pos)) != -1) {
      pair<unsigned int, unsigned int> matchpos(pos+1,pos+1+rx.matchedLength());
      if (e.occurrences.find(matchpos)==e.occurrences.end()) {
        // found a new occurrence
        newEntities.push_back(EntityOccurrence(e.entityType,rx.cap(0),e.entityString,matchpos));
      }
      pos += rx.matchedLength();
    }

  }
}

void SpecificEntitiesCompletion::
updateAnalysis(std::vector<EntityOccurrence>& occurrences,
               AnalysisContent& analysis) const
{
  // use existing functions to create the entities:
  // go through the graph to identify the vertices corresponding
  // to the occurrences, build RecognizerMatch objects and use
  // CreateSpecificEntity function
  SELOGINIT;

  // needs recognizerdata to make CreateSpecificEntity work
  RecognizerData* recoData=static_cast<RecognizerData*>(analysis.getData("RecognizerData"));
  if (recoData == 0)
  {
    recoData = new RecognizerData();
    analysis.setData("RecognizerData", recoData);
  }
  // resultData is mandatory in recognizerData
  RecognizerResultData* resultData=new RecognizerResultData(m_graph);
  recoData->setResultData(resultData);

  // create constraint functions
  map<EntityType,std::unique_ptr<CreateSpecificEntity> > entityCreator;
  for (auto t: m_entityTypes) {
    LimaString entityName=MediaticData::single().getEntityName(t);
    entityCreator[t]=std::make_unique<CreateSpecificEntity>(m_language,entityName);
  }

  AnalysisGraph* anagraph=static_cast<AnalysisGraph*>(analysis.getData(m_graph));

  LinguisticGraph* graph=anagraph->getGraph();
  VertexTokenPropertyMap tokenMap=get(vertex_token,*graph);
  std::queue<LinguisticGraphVertex> toVisit;
  std::set<LinguisticGraphVertex> visited;

  const LinguisticGraphVertex firstVx = anagraph->firstVertex();
  const LinguisticGraphVertex lastVx = anagraph->lastVertex();

  RecognizerMatch currentMatch(anagraph);
  int currentOccurrence=-1;

  // todo: ensure the process is robust when the analysis graph is not linear...
  try
  {
    toVisit.push(firstVx);

    LinguisticGraphOutEdgeIt outItr,outItrEnd;
    while (!toVisit.empty())
    {
      LinguisticGraphVertex v=toVisit.front();
      toVisit.pop();
      if (v == lastVx) {
        continue;
      }

      for (boost::tie(outItr,outItrEnd)=out_edges(v,*graph); outItr!=outItrEnd; outItr++)
      {
        LinguisticGraphVertex next=target(*outItr,*graph);
        if (visited.find(next)==visited.end())
        {
          visited.insert(next);
          toVisit.push(next);
        }
      }

      if (v != firstVx && v != lastVx)
      {
        processVertex(v,tokenMap,occurrences,currentOccurrence,currentMatch, entityCreator, anagraph, analysis);
      }
    }
  }
  catch (std::exception &exc)
  {
    SELOGINIT;
    LWARN << "Exception in SpecificEntitiesCompletion : " << exc.what();
  }

  // effective graph update
  recoData->removeVertices(analysis);
  recoData->clearVerticesToRemove();
  recoData->removeEdges(analysis);
  recoData->clearEdgesToRemove();
  // clean recognizer data (used internally in the process unit)
  recoData->deleteResultData();
  analysis.removeData("RecognizerData");

}

void SpecificEntitiesCompletion::processVertex(LinguisticGraphVertex currentVertex,
                                               const VertexTokenPropertyMap& tokenMap,
                                               std::vector<EntityOccurrence>& occurrences,
                                               int& currentOccurrence,
                                               RecognizerMatch& currentMatch,
                                               map<EntityType,std::unique_ptr<CreateSpecificEntity> >& entityCreator,
                                               AnalysisGraph* anagraph,
                                               AnalysisContent& analysis) const
{
  SELOGINIT;
  Token* currentToken=tokenMap[currentVertex];
  if (currentToken)
  {
    // process current Token
    unsigned int pos=currentToken->position();
    unsigned int posEnd=pos+currentToken->length();
    //LDEBUG << "Exploring vertex" << currentVertex << "at position" << pos;
    if (currentOccurrence!=-1) {
      // in the process of matching the entity
      if (pos>occurrences[currentOccurrence].posEnd) {
        // we missed it
        LDEBUG << "--at pos" << pos << ": missed occurrence" << occurrences[currentOccurrence];
       currentMatch=RecognizerMatch(anagraph);
       currentOccurrence=-1;
      }
      else {
        // in the process: add this token to the match
        LDEBUG << "--at pos" << pos << ":     in occurrence" << occurrences[currentOccurrence];
        currentMatch.addBackVertex(currentVertex);
      }
    }
    else {
      // does the position of the token matches the position of an occurrence
      for (unsigned int i(0);i<occurrences.size();i++) {
        if (pos==occurrences[i].posBegin) {
          currentOccurrence=i;
          currentMatch.addBackVertex(currentVertex);
          LDEBUG << "--at pos" << pos << ":  start occurrence" << occurrences[currentOccurrence];
          break;
        }
      }
    }
    if (currentOccurrence!=-1 && posEnd==occurrences[currentOccurrence].posEnd) {
      LDEBUG << "--at pos" << pos << ": -> end occurrence" << occurrences[currentOccurrence];
      // found the end: success !
      // add relevant info
      EntityOccurrence& occ=occurrences[currentOccurrence];
      currentMatch.setType(occ.entityType);
      currentMatch.features().setFeature("value",occ.entityNorm);
      // create the entity
      LDEBUG << "found match for occcurrence" << occ << ":" << currentMatch;
      (*entityCreator[occ.entityType])(currentMatch,analysis);
      //reinit currents
      currentMatch=RecognizerMatch(anagraph);
      currentOccurrence=-1;
    }
  }
}

} // end namespace
} // end namespace
} // end namespace
