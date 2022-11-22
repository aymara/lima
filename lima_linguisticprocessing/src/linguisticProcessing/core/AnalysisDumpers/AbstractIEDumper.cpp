// Copyright (C) 2016 by CEA - LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "AbstractIEDumper.h"
#include "linguisticProcessing/core/Automaton/SpecificEntityAnnotation.h"

#include "common/MediaticData/mediaticData.h"
#include "common/Data/LimaString.h"
#include "common/Data/strwstrtools.h"
#include "common/time/traceUtils.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/core/LinguisticResources/AbstractResource.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/Token.h"
#include "linguisticProcessing/core/SemanticAnalysis/SemanticAnnotation.h"

#include <boost/algorithm/string.hpp>
#include <fstream>
#include <queue>
#include <tuple>

using namespace std;
using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::Common::MediaticData;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::LinguisticProcessing::SpecificEntities;
using namespace Lima::LinguisticProcessing::EventAnalysis;
using namespace Lima::LinguisticProcessing::SemanticAnalysis;
using namespace boost;

namespace Lima {
namespace LinguisticProcessing {
namespace AnalysisDumpers {

// a class to hold information about current state of the output,
// when the output is used in append mode (e.g. when the analysis is done
// separately on each paragraph): store the number of elements printed to get ids right

struct AbstractIEDumper::IEDumperMetaData {
  uint nbEntities;
  uint nbAttributes;
  uint nbRelations;
  uint nbEvents;

  IEDumperMetaData():
  nbEntities(0),
  nbAttributes(0),
  nbRelations(0),
  nbEvents(0)
  {}

  IEDumperMetaData(LinguisticMetaData* metadata):
  nbEntities(0),
  nbAttributes(0),
  nbRelations(0),
  nbEvents(0)
  {
    if (metadata->hasMetaData("IEDumper.nbEntities"))
      nbEntities=std::stoi(metadata->getMetaData("IEDumper.nbEntities"));
    if (metadata->hasMetaData("IEDumper.nbAttributes"))
      nbAttributes=std::stoi(metadata->getMetaData("IEDumper.nbAttributes"));
    if (metadata->hasMetaData("IEDumper.nbRelations"))
      nbRelations=std::stoi(metadata->getMetaData("IEDumper.nbRelations"));
    if (metadata->hasMetaData("IEDumper.nbEvents"))
      nbEvents=std::stoi(metadata->getMetaData("IEDumper.nbEvents"));

    //std::cerr << *this << std::endl;
  }

  void save(LinguisticMetaData* metadata) {
    metadata->setMetaData("IEDumper.nbEntities",std::to_string(nbEntities));
    metadata->setMetaData("IEDumper.nbAttributes",std::to_string(nbAttributes));
    metadata->setMetaData("IEDumper.nbRelations",std::to_string(nbRelations));
    metadata->setMetaData("IEDumper.nbEvents",std::to_string(nbEvents));
  }

  friend std::ostream& operator<<(std::ostream& os, IEDumperMetaData& m) {
    os << "DumperMetaData: nbEntities="<< m.nbEntities << ",nbRelations=" << m.nbRelations << ",nbEvents="<< m.nbEvents;
    return os;
  }

};

AbstractIEDumper::AbstractIEDumper() :
AbstractTextualAnalysisDumper(),
m_language(0),
m_graph("PosGraph"),
m_followGraph(false),
m_domains(),
m_ignore(),
m_attributes(),
m_all_attributes(false),
m_templateDefinitions(),
m_templateNames(),
m_offsetMapping(nullptr),
m_outputGroups(false),
m_posOffset(-1)
{
  DUMPERLOGINIT;
  LDEBUG << "AbstractIEDumper::AbstractIEDumper()";
}

AbstractIEDumper::~AbstractIEDumper()
{}

void AbstractIEDumper::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  DUMPERLOGINIT;
  LDEBUG << "AbstractIEDumper::init";
  AbstractTextualAnalysisDumper::init(unitConfiguration,manager);
  std::deque<std::string> eventTemplates;

  m_language=manager->getInitializationParameters().media;

  try
  {
    m_graph=unitConfiguration.getParamsValueAtKey("graph");
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    DUMPERLOGINIT;
    LWARN << "No 'graph' parameter in unit configuration '"
          << unitConfiguration.getName() << "' ; using PosGraph";
    m_graph=string("PosGraph");
  }

  try
  {
    // single domain
    string val=unitConfiguration.getParamsValueAtKey("domain");
    if (! val.empty()) {
      m_domains.insert(val);
    }
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    m_domains=set<string>();
    DUMPERLOGINIT;
    LDEBUG << "no parameter 'domain' in AbstractIEDumper: entities from all domains will be printed";
  } // if empty set, all domains are printed

  try
  {
    deque< string > vals=unitConfiguration.getListsValueAtKey("domains");
    for (const auto& v: vals) {
      m_domains.insert(v);
    }
  }
  catch (Common::XMLConfigurationFiles::NoSuchList& )
  {
    LDEBUG << "no list 'domains' in AbstractIEDumper";
  } // if empty set, all domains are printed

  try
  {
    deque< string > vals=unitConfiguration.getListsValueAtKey("ignore");
    for (const auto& v: vals) {
      m_ignore.insert(v);
    }
  }
  catch (Common::XMLConfigurationFiles::NoSuchList& )
  {
    LDEBUG << "no list 'ignore' in AbstractIEDumper: all entity types of authorized domains are printed";
  }

  try
  {
    string val=unitConfiguration.getParamsValueAtKey("outputAllAttributes");
    if (val=="1" || val=="true" || val=="yes") {
      m_all_attributes=true;
    }
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {}// keep default value (false)

  if(!m_all_attributes){
      try
      {
        deque< string > vals=unitConfiguration.getListsValueAtKey("attributes");
        m_attributes=vals;
      }
      catch (Common::XMLConfigurationFiles::NoSuchList& )
      {
        m_attributes=std::deque<std::string>();
        DUMPERLOGINIT;
        LDEBUG << "no list 'attributes' in AbstractIEDumper: no attributes will be printed";
      } // no attributes are printed
  }

  try
  {
    string val=unitConfiguration.getParamsValueAtKey("outputGroups");
    if (val=="1" || val=="true" || val=="yes") {
      m_outputGroups=true;
    }
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {} // keep default value (false)


  try
  {
    eventTemplates=unitConfiguration.getListsValueAtKey("eventTemplates");
    //std::cout << "eventTemplates.size()=" << eventTemplates.size() << "\n";

    MediaId language=manager->getInitializationParameters().media;
    for(std::deque<std::string>::const_iterator it=eventTemplates.begin();it!=eventTemplates.end();it++)
    {
      std::string templateResource=*it;
      //std::cout << "templateResource=" << templateResource << "\n";
      AbstractResource* res=LinguisticResources::single().getResource(language,templateResource);
      if (res)
      {
        //std::cout << " La ressource est lue \n";
        EventTemplateDefinitionResource* templateDefinitions=static_cast<EventTemplateDefinitionResource*>(res);
        m_templateDefinitions.insert(std::make_pair(templateResource,templateDefinitions));
      }
    }
    for (const auto& t:m_templateDefinitions) {
      m_templateNames.insert(t.second->getMention());
    }
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {} // do nothing: optional

  try {
    std::string str=unitConfiguration.getParamsValueAtKey("followGraph");
    if (str=="1" || str=="true" || str=="yes") {
      m_followGraph=true;
    }
    else {
      m_followGraph=false;
    }
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {} // keep default value

  try {
    std::string str=unitConfiguration.getParamsValueAtKey("posOffset");
    m_posOffset=std::stoi(str);
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {} // keep default value

}

LimaStatusCode AbstractIEDumper::process(
  AnalysisContent& analysis) const
{
  DUMPERLOGINIT;
  LDEBUG << "AbstractIEDumper::process";
  TimeUtils::updateCurrentTime();
/// map< tuple< pos, len, entityType > , entityId >
  std::map<std::tuple<std::uint64_t,std::uint64_t,std::string>, std::size_t > mapEntities;
  /// map< tuple< entityId, featName, featValue > , featId >
  std::map <std::tuple <std::size_t, std::string , std::string >,  std::size_t > mapAttributes;

  std::string sourceFile;

  auto originalText = std::dynamic_pointer_cast<LimaStringText>(analysis.getData("Text"));

  auto metadata = std::dynamic_pointer_cast<LinguisticMetaData>(analysis.getData("LinguisticMetaData"));
  if (metadata == 0) {
    DUMPERLOGINIT;
    LERROR << "no LinguisticMetaData ! abort";
    return MISSING_DATA;
  }

  sourceFile=metadata->getMetaData("FileName");

  IEDumperMetaData* dumperMetadata=new IEDumperMetaData(metadata.get());

  auto annotationData = std::dynamic_pointer_cast< AnnotationData >(analysis.getData("AnnotationData"));
  if (annotationData == 0) {
    DUMPERLOGINIT;
    LERROR << "no annotationData ! abort";
    return MISSING_DATA;
  }


  auto graphp = std::dynamic_pointer_cast<LinguisticAnalysisStructure::AnalysisGraph>(analysis.getData(m_graph));
  if (graphp == 0) {
    DUMPERLOGINIT;
    LERROR << "no graph "<< m_graph <<" ! abort";
    return MISSING_DATA;
  }

  auto eventData = std::dynamic_pointer_cast<EventTemplateData>(analysis.getData("EventTemplateData"));
  if (eventData==0) {
    DUMPERLOGINIT;
    LDEBUG << "No data 'EventTemplateData'";
  }

  auto offsetData = analysis.getData("OffsetMapping");
  if (offsetData!=0) {
    m_offsetMapping = std::dynamic_pointer_cast<OffsetMapping>(offsetData).get();
  }

  const LinguisticAnalysisStructure::AnalysisGraph& graph = *graphp;
  LinguisticGraph* lingGraph = const_cast<LinguisticGraph*>(graph.getGraph());
  VertexTokenPropertyMap tokenMap = get(vertex_token, *lingGraph);



  auto dstream = initialize(analysis);
  ostream& out=dstream->out();

  uint64_t offset(0);
  try {
    offset=QString::fromStdString(metadata->getMetaData("StartOffset")).toUInt();
  }
  catch (LinguisticProcessingException& ) {
    // do nothing: not set in analyzeText (only in analyzeXmlDocuments)
  }

  /*uint64_t offsetIndexingNode(0);
    try {
    offsetIndexingNode=atoi(metadata->getMetaData("StartOffsetIndexingNode").c_str());
    }
    catch (LinguisticProcessingException& ) {
    // do nothing: not set in analyzeText (only in analyzeXmlDocuments)
    }*/

  std::string docId("");
  try {
    docId=metadata->getMetaData("DocId");
  }
  catch (LinguisticProcessingException& ) {
    // do nothing: not set in analyzeText (only in analyzeXmlDocuments)
  }
  outputGlobalHeader(out,sourceFile,*originalText);

  outputEntitiesHeader(out);

  if (m_followGraph) {
    // instead of looking to all annotations, follow the graph (in
    // morphological graph, some vertices are not related to main graph:
    // idiomatic expressions parts and named entity parts)
    // -> this will not include nested entities

    auto tokenList = std::dynamic_pointer_cast<AnalysisGraph>(analysis.getData(m_graph));
    if (tokenList==0) {
      LERROR << "graph " << m_graph << " has not been produced: check pipeline";
      return MISSING_DATA;
    }
    LinguisticGraph* graph=tokenList->getGraph();
    //const FsaStringsPool& sp=Common::MediaticData::MediaticData::single().stringsPool(m_language);

    std::queue<LinguisticGraphVertex> toVisit;
    std::set<LinguisticGraphVertex> visited;
    toVisit.push(tokenList->firstVertex());

    LinguisticGraphOutEdgeIt outItr,outItrEnd;
    while (!toVisit.empty()) {
      LinguisticGraphVertex v=toVisit.front();
      toVisit.pop();
      if (v == tokenList->lastVertex()) {
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
      const SpecificEntityAnnotation* annot=getSpecificEntityAnnotation(v,annotationData.get());
      if (annot != 0) {
        outputEntity(out,v,annot,tokenMap,offset,*originalText,mapEntities,mapAttributes,dumperMetadata);
      }
    }
  }
  else {
    // take all annotations
    AnnotationGraphVertexIt itv, itv_end;
    boost::tie(itv, itv_end) = vertices(annotationData->getGraph());
    for (; itv != itv_end; itv++)
    {
      //     LDEBUG << "AbstractIEDumper on annotation vertex " << *itv;
      if (annotationData->hasAnnotation(*itv,QString::fromUtf8("SpecificEntity")))
      {
        //       LDEBUG << "    it has SpecificEntityAnnotation";
        const SpecificEntityAnnotation* annot = 0;
        try
        {
          annot = annotationData->annotation(*itv,QString::fromUtf8("SpecificEntity"))
            .pointerValue<SpecificEntityAnnotation>();
        }
        catch (const boost::bad_any_cast& )
        {
          DUMPERLOGINIT;
          LERROR << "This annotation is not a SpecificEntity; SE not logged";
          continue;
        }

        // recuperer l'id du vertex morph cree
        LinguisticGraphVertex v;
        if (!annotationData->hasIntAnnotation(*itv,QString::fromStdString(m_graph)))
        {
          //         DUMPERLOGINIT;
          //         LDEBUG << *itv << " has no " << m_graph << " annotation. Skeeping it.";
          continue;
        }
        v = annotationData->intAnnotation(*itv,QString::fromStdString(m_graph));
        outputEntity(out,v,annot,tokenMap,offset,*originalText,mapEntities,mapAttributes,dumperMetadata);
      }
    }
  }
  // update number of entities/attributes
  dumperMetadata->nbEntities+=mapEntities.size();
  dumperMetadata->nbAttributes+=mapAttributes.size();
  //std::cerr << *dumperMetadata << std::endl;
  outputEntitiesFooter(out);

  outputEventsHeader(out);
  if (eventData!=0) {
    uint nbEvents=outputEventData(out,eventData.get(),annotationData.get(),tokenMap,mapEntities,offset,
                                  *originalText, dumperMetadata);
    dumperMetadata->nbEvents+=nbEvents;
  }
  outputEventsFooter(out);

  outputRelationsHeader(out);
  uint nbRelations=outputSemanticRelations( out, annotationData.get(), tokenMap, mapEntities, offset, dumperMetadata);
  dumperMetadata->nbRelations+=nbRelations;
  outputRelationsFooter(out);

  outputGlobalFooter(out);

  // save current state for future output
  dumperMetadata->save(metadata.get());

  TimeUtils::logElapsedTime("AbstractIEDumper");
  return SUCCESS_ID;

}

// due to text expansion, the same event may have been recognized in several parts of the text: after
// changing back the positions, this creates duplicate events: need to remove those duplicates
// use an internal class to store event infos, with a comparison operator that allows to find duplicates
struct EventInfos {
public:
  EventInfos(): eventMentionId(0),eventMentionType(""),eventRoleId(),eventRoleType(),
                eventMentionString(""),eventMentionPosition(0),eventMentionLength(0) {}
  mutable unsigned int eventMentionId; // mutable because does not count for set<> in comparison operator
  std::string eventMentionType;
  std::vector<unsigned int> eventRoleId;
  std::vector<std::string> eventRoleType;
  // infos on the mention to add entity if needed
  mutable std::string eventMentionString; // mutable because does not count for set<> in comparison operator
  uint64_t eventMentionPosition;
  uint64_t eventMentionLength;

  bool operator<(const EventInfos& other) const {
    // use type and string of event mention for comparison
    // + all events roles (ids of entities)
    // do not use entityMentionId because it is not fixed (id of existing entity or new entity to be added)
    // or entityMentionPosition because it is the position before changing back to original offsets
    if (eventMentionType!=other.eventMentionType) {
      return (eventMentionType<other.eventMentionType);
    }
    else if (eventMentionString!=other.eventMentionString) {
      return (eventMentionString<other.eventMentionString);
    }
    else {
      return (eventRoleId<other.eventRoleId);
    }
  }

  bool isIncluded(const EventInfos& other) const {
    if (eventMentionType!=other.eventMentionType) {
      return false;
    }
    if (eventMentionString!=other.eventMentionString) {
      return false;
    }
    for (const auto& role: eventRoleId) {
      if (std::find(other.eventRoleId.begin(), other.eventRoleId.end(), role) == other.eventRoleId.end()) {
        return false;
      }
    }
    return true;
  }

  // for debug
  friend std::ostream& operator<<(std::ostream& os, const EventInfos e) {
    os << "id=" << e.eventMentionId << "/type=" << e.eventMentionType
       << "/mention=" << e.eventMentionString << "/roles=";
    for (unsigned int i(0); i<e.eventRoleId.size(); i++) {
      os << "[" << e.eventRoleType[i] << ":" << e.eventRoleId[i] << "]";
    }
    return os;
  }
  friend QDebug& operator<<(QDebug& os, const EventInfos e) {
    ostringstream oss; oss << e; os << oss.str(); return os;
  }

};

uint AbstractIEDumper::outputEventData(std::ostream& out,
                                       const EventAnalysis::EventTemplateData* eventData,
                                       const Common::AnnotationGraphs::AnnotationData* /*annotationData*/,
                                       const VertexTokenPropertyMap& tokenMap,
                                       std::map<std::tuple<std::uint64_t,std::uint64_t,std::string>,std::size_t > mapEntities,
                                       uint64_t offset,
                                       LimaStringText originalText,
                                       IEDumperMetaData* metadata
  ) const
{
  // output of events is based on the idea of elements of information to extraction: if the elements
  // coming from different parts of the text are the same, printing them several times is not relevant
  // (differs from an annotation task, maybe @todo have a parameter to handle this difference)
  // Here, the rules for the output of events are:
  // - equal events are kept only once
  // - if an event is included in another event, it is not kept
  // from these rules: most single event mentions (without roles) are not kept: for each different mention, one
  // could be kept if there is no other event with roles that has the same mention

  DUMPERLOGINIT;
  //return the number of events
  // use a set of EventInfos to remove duplicates
  set<EventInfos> events;
  // use a bufferEvents to possibly have all event mentions as entities before all events
  stringstream bufferEvents;
  for (std::vector<EventTemplate>::const_iterator it= eventData->begin(); it!= eventData->end();it++)
  {
    const TemplateElements& templateElements=(*it).getTemplateElements();
    if (! templateElements.empty()) {

      const std::string& templateName=it->getType();
      LDEBUG << "templateName:" << templateName;

      std::string templateMention="";
      for(const auto& t:m_templateDefinitions)
      {
        if (t.second->getName()==templateName) templateMention=t.second->getMention();
      }
      LDEBUG << "templateMention" << templateMention;

      EventInfos eventInfos;

      for(map<string,EventTemplateElement>::const_iterator it1= templateElements.begin(); it1!= templateElements.end();it1++)
      {
        const string typeName=it1->first;
        LDEBUG << "typeName='" << typeName << "',templateMention='" << templateMention << "'";

        LinguisticGraphVertex v=(*it1).second.getVertex();
        LinguisticAnalysisStructure::Token* vToken = tokenMap[v];
        std::uint64_t position= vToken->position();
        std::uint64_t length= vToken->length();
        string stringForm = originalText.mid( position+m_posOffset,length).toUtf8().data();

        if (addEventMentionAsEntity() && templateMention.compare(typeName)==0)
        {
          // specific treatment for event mention: add a new entity with the corresponding type
          LDEBUG << "add event mention as entity typeName=" << typeName;
          eventInfos.eventMentionString=stringForm;
          eventInfos.eventMentionPosition=position;
          eventInfos.eventMentionLength=length;
          eventInfos.eventMentionType=typeName;
        }
        else {

          // entities in map are stored with original positions
          adjustPosition(position,offset);

          // get corresponding entity
          string entityType=Common::Misc::limastring2utf8stdstring(Common::MediaticData::MediaticData::single().getEntityName((*it1).second.getType()));
          const auto itE=mapEntities.find(make_tuple(position,length,entityType));
          uint64_t id(0);
          if (itE!=mapEntities.end()) {
            id=(*itE).second;
            LDEBUG << "AbstractIEDumper: found index" << id << "for entity (" << position << "," << length << "," << entityType << ")";
          }
          else {
            LERROR << "AbstractIEDumper: cannot find index of entity (" << position << "," << length << "," << entityType << ")";
            // push something to ensure both vectors are aligned (eventRoleType and eventRoleId)
          }

          if (templateMention.compare(typeName)==0) {
            eventInfos.eventMentionType=typeName;
            eventInfos.eventMentionId=id;
            // save string because used in comparison operator (for set)
            eventInfos.eventMentionString=stringForm;
          }
          else {
            eventInfos.eventRoleType.push_back(typeName);
            eventInfos.eventRoleId.push_back(id);
          }
        }
      }
      LDEBUG << "Add event infos" << eventInfos;
      events.insert(eventInfos);
      LDEBUG << "=>" << events.size() << "events";
      // use a bufferEvents to have all event mentions as entities before all events
    }
  }
  // output event mentions as entities
  if (addEventMentionAsEntity()) {
    unsigned int idEntity=mapEntities.size()+1+metadata->nbEntities;
    for (auto& e: events) {
      e.eventMentionId=idEntity; // new entity id
      std::vector<pair<uint64_t,uint64_t> > positions;
      LimaString eventMentionString=QString::fromStdString(e.eventMentionString);
      computePositions(positions,eventMentionString,e.eventMentionPosition,e.eventMentionLength);
      // if (m_outputGroups && !m_domain.empty()) {
      //   outputEntityString(out, e.eventMentionId, m_domain+"."+e.eventMentionType, eventMentionString.toUtf8().data(), positions, Automaton::EntityFeatures(), true);
      // }
      // else {
      outputEntityString(out, e.eventMentionId, e.eventMentionType, eventMentionString.toUtf8().data(), positions, Automaton::EntityFeatures(), true);
      // }
      idEntity++;
    }
  }

  // filter events: remove events that are included in other events (possible because of text expansion)
  for (set<EventInfos>::iterator it=events.begin();it!=events.end();) {
    bool isIncluded(false);
    for (set<EventInfos>::iterator it2=std::next(it); it2!=events.end(); it2++) {
      if ((*it).isIncluded(*it2)) {
        LDEBUG << "=> filter event" << *it << "(included in" << *it2 << ")";
        isIncluded=true;
        it=events.erase(it);
        break;
      }
    }
    if (! isIncluded) {
      it++;
    }
  }

  unsigned int eventId(1+metadata->nbEvents);
  for (const auto& e: events) {
    LDEBUG << "=> output event"<< eventId << "/" << events.size();
    outputEventString(out, eventId, e.eventMentionId, e.eventMentionType, e.eventRoleId, e.eventRoleType);
    eventId++;
  }
  return events.size();
}

void AbstractIEDumper::
computePositions(std::vector<pair<uint64_t,uint64_t> >& positions,
                 LimaString& stringForm,
                 uint64_t pos,
                 uint64_t len) const
{
  // if string contains \n, have to set several position intervals around these characters (brat-style)
  positions.push_back(make_pair(pos+m_posOffset,pos+len+m_posOffset));
  string::size_type prev(0),i=stringForm.indexOf("\n");
  vector<unsigned int> toErase;
  while (i!=string::npos) {
//    LimaString debugString(stringForm);
//    debugString.replace(' ','_');
//    debugString.replace('\n','N');
//    cout << "--prev=" << prev << ",i=" << i << " [" << debugString.toUtf8().data() << "] positions=";
//    for (const auto& p:positions) {cout << "(" << p.first << "," << p.second << ") "; }; cout << "--" << endl;
    // replace by space
    //stringForm[i]=' ';
    stringForm.replace(i,1,' ');
    if (prev!=0 && i==prev+1) { // case of several consecutive \n
      positions.back().first++;
      toErase.push_back(i); // erase it on output string, but do it later to avoid changing positions in original string
    }
    else {
      // add an interval
      unsigned int nbCharsIgnored=1; // the carriage return
      // ignore spaces before
      /*while (stringForm.size()>0 && stringForm[i-1]==' ') {
        // erase space in the string
        stringForm.erase(i-1,1);
        i--;
        nbCharsIgnored++;
      }*/
      // ignore spaces after the \n
      /*while (i< stringForm.size() && stringForm[i+1]==' ') {
          i++;
          nbCharsIgnored++;
      }*/
      uint64_t previousEnd=positions.back().second;
      if (prev==0) {
        positions.back().second=positions.back().first+i;
      }
      else {
        positions.back().second=positions.back().first+(i-prev-1);
      }
      positions.push_back(make_pair(positions.back().second+nbCharsIgnored,previousEnd));
//       cout << "-->positions="; for (const auto& p:positions) {cout << "(" << p.first << "," << p.second << ") "; }; cout << endl;
    }
    // find next
    prev=i;
    i=stringForm.indexOf("\n",prev+1);
  }
  // change string form to remove consecutive spaces due to \n
  unsigned int shift=0;
  for (const auto i: toErase) {
    stringForm.remove(i-shift,1); // must change positions to follow string with removed chars
    shift++;
  }
}

bool AbstractIEDumper::
outputEntity(std::ostream& out,
             LinguisticGraphVertex v,
             const SpecificEntityAnnotation* annot,
             const VertexTokenPropertyMap& tokenMap,
             uint64_t offset,
             LimaStringText originalText,
             std::map<std::tuple<std::uint64_t,std::uint64_t,std::string>,std::size_t>& mapEntities,
             std::map <std::tuple <std::size_t, std::string , std::string >,  std::size_t >& mapAttributes,
             IEDumperMetaData* metadata
  ) const
{
  LinguisticAnalysisStructure::Token* vToken = tokenMap[v];
  //       LDEBUG << "AbstractIEDumper tokenMap[" << v << "] = " << vToken;
  if (vToken == 0)
  {
    DUMPERLOGINIT;
    LERROR << "Vertex " << v << " has no entry in the analysis graph token map. This should not happen !!";
  }
  else
  {
    std::uint64_t pos=annot->getPosition();
    std::uint64_t len=annot->getLength();
    //string stringForm=originalText.mid( pos-1,len).toUtf8().data();
    auto stringForm=originalText.mid( pos+m_posOffset,len);
    std::vector<pair<uint64_t,uint64_t> > positions;
//     cerr << "computePositions("<< stringForm.toUtf8().data() << ") " << pos << ":" << len << " -> ";
    computePositions(positions,stringForm,pos,len);
//     for (const auto& p: positions) { cerr << p.first << ":" << p.second << " "; }; cerr << endl;

    // get entity type and group type
    EntityType eType=annot->getType();
    std::string domainType=MediaticData::single().getEntityGroupName(eType.getGroupId()).toUtf8().data();
    std::string entityName= MediaticData::single().getEntityName(eType).toUtf8().data();
    std::string entityType(entityName);
    std::size_t posG=entityType.find(".");
    if (posG!=std::string::npos && ! m_outputGroups){
      entityType = entityType.substr(posG+1);
    }
    if (m_ignore.find(entityType)!=m_ignore.end()) {
      //LDEBUG << "AbstractIEDumper: ignored entity type" << entityType;
      return false;
    }

    // back to the original offset if text has been expanded
    // do this before inserting in mapEntities to find real duplicates
    adjustPosition(pos,offset);
    adjustPositions(positions,offset);
    // if non-contiguous position (especially after adjustment to original positions), need to take real length to check for duplicates
    if (positions.size()>0) {
      len=positions.back().second-pos+1;
    }

    auto entity=std::make_tuple(pos,len,entityName);

    if (mapEntities.find(entity)!=mapEntities.end())
    {
      // entity already exists
      DUMPERLOGINIT;
      LWARN << "AbstractIEDumper: duplicate entity: " << pos << "," << len << " " << entityName;
      return false;
    }

    std::size_t index = mapEntities.size()+1 + metadata->nbEntities;

    if (! m_domains.empty() && m_domains.find(domainType)==m_domains.end())
    {
      // entity is not in considered domain: do not print it (nor store it in the map)
      DUMPERLOGINIT;
      LDEBUG << "AbstractIEDumper: ignore entity of domain" << domainType;
      return false;
    }

    //std::cout << "m_domain " << m_domain << " m_domain.size " << m_domain.size() << std::endl;
    //std::cout << "domainType " << domainType << " domainType.size " << domainType.size() << std::endl;
    //std::cout << "Entity is printed " << entityType << "domain=" << domainType << " / targetDomain=" << m_domain << std::endl;
    const Automaton::EntityFeatures& entityFeatures = annot->getFeatures();
    bool forceNoNorm=false;
    if (m_templateNames.find(entityType)!=m_templateNames.end()) {
      // if the entity is a template mention, must not have a normalization (according to Brat)
      forceNoNorm=true;
    }
    outputEntityString(out, index, entityType, stringForm.toUtf8().data(), positions, entityFeatures,forceNoNorm);

    // identify attributes as entityFeatures having a "POSITION" (or parts of a numex) and being in the declared list of attributes to display
    for(Automaton::EntityFeatures::const_iterator featureItr=entityFeatures.cbegin(),features_end=entityFeatures.cend();
        featureItr!=features_end; featureItr++) {
      const std::string featName = featureItr->getName();
      if( (featureItr->getPosition() != UNDEFPOSITION) ||
          (entityName=="Numex.NUMBER" && featName == "numvalue") ||
          (entityName=="Numex.NUMEX" && featName == "numvalue") ||
          (entityName=="Numex.NUMEX" && featName == "unit") ) {
        if( m_all_attributes || std::find(m_attributes.begin(), m_attributes.end(), featName) != m_attributes.end() ){
          std::size_t feat_index = mapAttributes.size()+1 +metadata->nbAttributes;
          std::string featValue = featureItr->getValueString();
          auto feature = std::make_tuple(index,featName,featValue);
          mapAttributes.insert( std::make_pair(feature, feat_index ) );
        }
      }
    }
    outputAttributesString(out, index, mapAttributes);

    DUMPERLOGINIT;
    LDEBUG << "AbstractIEDumper: add entity (" << get<0>(entity) << "," << get<1>(entity) << "," << get<2>(entity) << "): index=" << index;

    mapEntities.insert(std::make_pair(entity,index));
    return true;
  }
  return false;
}

const SpecificEntityAnnotation* AbstractIEDumper::
getSpecificEntityAnnotation(LinguisticGraphVertex v,
                            const Common::AnnotationGraphs::AnnotationData* annotationData) const
{

  const SpecificEntityAnnotation* se=0;

  // check only entity found in current graph

  std::set< AnnotationGraphVertex > matches = annotationData->matches(m_graph,v,"annot");
  for (std::set< AnnotationGraphVertex >::const_iterator it = matches.begin();
       it != matches.end(); it++)
  {
    AnnotationGraphVertex vx=*it;
    if (annotationData->hasAnnotation(vx, QString::fromUtf8("SpecificEntity")))
    {
      //BoWToken* se = createSpecificEntity(v,*it, annotationData, anagraph, posgraph, offsetBegin);
      se = annotationData->annotation(vx, QString::fromUtf8("SpecificEntity")).
        pointerValue<SpecificEntityAnnotation>();
      if (se!=0) {
        return se;
      }
    }
  }

  // special case: if specified graph is posgraph, allows to search in analysis graph
  // (entities found before pos-tagging)
  if (m_graph=="PosGraph") {
    std::set< AnnotationGraphVertex > anaVertices = annotationData->matches("PosGraph",v,"AnalysisGraph");

    // note: anaVertices size should be 0 or 1
    for (const auto& anaVertex : anaVertices)  {

      std::set< AnnotationGraphVertex > matches = annotationData->matches("AnalysisGraph",anaVertex,"annot");

      for (const auto& vx: matches)
      {
        if (annotationData->hasAnnotation(vx, QString::fromUtf8("SpecificEntity")))
        {
          se = annotationData->annotation(vx, QString::fromUtf8("SpecificEntity")).
          pointerValue<SpecificEntityAnnotation>();
          if (se!=0) {
            return se;
          }
        }
      }
    }
  }

  return se;

}

std::string AbstractIEDumper::
outputSemanticRelationArg(const std::string& /*vertexRole*/,
                          const AnnotationGraphVertex& vertex,
                          const VertexTokenPropertyMap& tokenMap,
                          std::map<std::tuple<std::uint64_t,std::uint64_t,std::string>,std::size_t >  mapEntities,
                          const Common::AnnotationGraphs::AnnotationData* annotationData,
                          uint64_t offset) const
{
  ostringstream oss;

  // get id of the corresponding vertex in analysis graph
  LinguisticGraphVertex v;
  if (!annotationData->hasIntAnnotation(vertex,QString::fromStdString(m_graph)))
  {
    // DUMPERLOGINIT;
    // LDEBUG << *itv << " has no " << m_graph << " annotation. Skeeping it.";
    return "";
  }
  v = annotationData->intAnnotation(vertex,QString::fromStdString(m_graph));
  LinguisticAnalysisStructure::Token* vToken = tokenMap[v];
  //       LDEBUG << "SemanticRelationsXmlLogger tokenMap[" << v << "] = " << vToken;
  if (vToken == 0)
  {
    return "";
  }

  // get annotation : element in relation can be an entity => get entity type
  // otherwise, its type is "token"
  //EntityT type("token");

  //std::set< uint32_t > matches = annotationData->matches(m_graph,v,"annot");
  std::set< AnnotationGraphVertex > matches = annotationData->matches(m_graph,v,"annot");
  for (std::set< AnnotationGraphVertex >::const_iterator it = matches.begin();
       it != matches.end(); it++)
  {
    if (annotationData->hasAnnotation(*it,QString::fromUtf8("SpecificEntity"))) {
      const SpecificEntityAnnotation* annot = 0;
      try {
        annot = annotationData->annotation(*it,QString::fromUtf8("SpecificEntity"))
          .pointerValue<SpecificEntityAnnotation>();
      }
      catch (const boost::bad_any_cast& e) {

        continue;
      }
      LimaString typeName=Common::MediaticData::MediaticData::single().getEntityName(annot->getType());
      std::uint64_t position= offset+vToken->position() ;
      std::uint64_t length= vToken->length();
      for(std::map<std::tuple<std::uint64_t,std::uint64_t,std::string>,std::size_t > ::const_iterator itE=mapEntities.begin();itE!=mapEntities.end();itE++)
      {
        std::tuple<std::uint64_t,std::uint64_t,std::string> entityIndex= itE->first;
        if (std::get<0>(entityIndex)==position &&
            std::get<1>(entityIndex)==length &&
            typeName.compare(LimaString(std::get<2>(entityIndex).c_str()))==0) oss << itE->second;

      }
      break;
    }
  }


  return oss.str();
}

uint AbstractIEDumper::
outputSemanticRelations(std::ostream& out,
                        const Common::AnnotationGraphs::AnnotationData* annotationData,
                        const VertexTokenPropertyMap& tokenMap,
                        std::map<std::tuple<std::uint64_t,std::uint64_t,std::string>,std::size_t > mapEntities,
                        uint64_t offset,
                        IEDumperMetaData* metadata
  ) const
{
  // return the number of relations produced in ouptut
  uint nbRelations(0);

  AnnotationGraphEdgeIt it,it_end;
  std::uint64_t index=1+metadata->nbRelations;
  const AnnotationGraph& annotGraph=annotationData->getGraph();
  boost::tie(it, it_end) = edges(annotGraph);
  for (; it != it_end; it++) {

    if (annotationData->hasAnnotation(*it,QString::fromUtf8("SemanticRelation")))
    {

      const SemanticRelationAnnotation* annot = 0;
      try
      {
        annot = annotationData->annotation(*it,QString::fromUtf8("SemanticRelation"))
          .pointerValue<SemanticRelationAnnotation>();
      }
      catch (const boost::bad_any_cast& e)
      {

        continue;
      }

      std::string annotType( annot->type() );
      std::size_t posG=annotType.find(".");
      if (posG!=std::string::npos && ! m_outputGroups){
          annotType = annotType.substr(posG+1);
      }
      //output
      DUMPERLOGINIT;
      LDEBUG << "AbstractIEDumper: add relation (type" << annotType << ", source" << source(*it,annotGraph) << ", target " << target(*it,annotGraph) << "): index=" << index;
      index += outputRelationString(out, index, annotType,
                           outputSemanticRelationArg("source",source(*it,annotGraph),tokenMap,mapEntities,annotationData,offset),
                           outputSemanticRelationArg("target",target(*it,annotGraph),tokenMap,mapEntities,annotationData,offset));
      nbRelations++;
    }
  }
  return nbRelations;
}

const SemanticAnalysis::SemanticRelationAnnotation* AbstractIEDumper::
getSemanticRelationAnnotation(LinguisticGraphVertex v,
                              const Common::AnnotationGraphs::AnnotationData* annotationData) const
{

  const SemanticRelationAnnotation* sr=0;

  // check only entity found in current graph (not previous graph such as AnalysisGraph)

  std::set< AnnotationGraphVertex > matches = annotationData->matches(m_graph,v,"annot");
  for (std::set< AnnotationGraphVertex >::const_iterator it = matches.begin();
       it != matches.end(); it++)
  {
    AnnotationGraphVertex vx=*it;
    if (annotationData->hasAnnotation(vx, QString::fromUtf8("SemanticRelation")))
    {
      //BoWToken* se = createSpecificEntity(v,*it, annotationData, anagraph, posgraph, offsetBegin);
      sr = annotationData->annotation(vx, QString::fromUtf8("SemanticRelation")).
        pointerValue<SemanticRelationAnnotation>();
      if (sr!=0) {
        return sr;
      }
    }
  }
  return sr;

}

void AbstractIEDumper::adjustPosition(std::uint64_t& position, uint64_t offset) const
{
  //DUMPERLOGINIT;
//   std::uint64_t prevPos(position);
  if (m_offsetMapping!=0) {
    position=m_offsetMapping->getOriginalOffset(position)+offset;
  }
  if (offset) {
    position+=offset;
  }

  //LDEBUG << "AbstractIEDumper::adjustPosition" << prevPos << "->" << position;
}

void AbstractIEDumper::adjustPositions(std::vector<pair<uint64_t,uint64_t> >& positions, uint64_t offset) const
{
  DUMPERLOGINIT;
  ostringstream prevPos;
  for (const auto& p: positions) { prevPos << "(" << p.first << "," << p.second << ") "; }
  if (m_offsetMapping) {
    for (auto& p: positions) {
      p.first=m_offsetMapping->getOriginalOffset(p.first);
      p.second=m_offsetMapping->getOriginalOffset(p.second);
    }
  }
  if (offset) {
    for (auto& p: positions) {
      p.first+=offset;
      p.second+=offset;
    }
  }

  ostringstream newPos;
  for (const auto& p: positions) { newPos << "(" << p.first << "," << p.second << ") "; }
  LDEBUG << "AbstractIEDumper::adjustPositions" << prevPos.str() << "->" << newPos.str();
}


} // AnalysisDumpers
} // LinguisticProcessing
} // Lima
