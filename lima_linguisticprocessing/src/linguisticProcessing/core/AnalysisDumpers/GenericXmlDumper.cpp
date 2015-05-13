/*
    Copyright 2002-2013 CEA LIST

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
/***************************************************************************
 *   Copyright (C) 2010 by CEA LIST                              *
 *                                                                         *
 ***************************************************************************/
#include "GenericXmlDumper.h"
#include "TextDumper.h" // for lTokenPosition comparison function to order tokens

// #include "linguisticProcessing/core/LinguisticProcessors/HandlerStreamBuf.h"
#include "common/MediaProcessors/HandlerStreamBuf.h"
#include "common/time/traceUtils.h"
#include "common/Data/strwstrtools.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/MediaticData/mediaticData.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "linguisticProcessing/common/BagOfWords/bowNamedEntity.h"
#include "linguisticProcessing/common/BagOfWords/bowToken.h"
#include "linguisticProcessing/common/BagOfWords/bowTerm.h"
#include "linguisticProcessing/common/BagOfWords/bowTokenIterator.h"
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "linguisticProcessing/core/TextSegmentation/SegmentationData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticDataUtils.h"
#include "common/Handler/AbstractAnalysisHandler.h"

#include <boost/graph/properties.hpp>

#include <fstream>
#include <deque>
#include <queue>
#include <iostream>

using namespace std;
//using namespace boost;
using namespace boost::tuples;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::Common::BagOfWords;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::LinguisticProcessing::SyntacticAnalysis;
using namespace Lima::LinguisticProcessing::SpecificEntities;

namespace Lima {
namespace LinguisticProcessing {
namespace AnalysisDumpers {

SimpleFactory<MediaProcessUnit,GenericXmlDumper> genericXmlDumperFactory(GENERICXMLDUMPER_CLASSID);

GenericXmlDumper::GenericXmlDumper():
AbstractTextualAnalysisDumper(),
m_graph("PosGraph"),
m_features(),
m_bowFeatures(),
m_featureNames(),
m_featureTags(),
m_defaultFeatures(),
m_outputWords(true),
m_outputSentenceBoundaries(false),
m_outputSpecificEntities(false),
m_outputSpecificEntityParts(false),
m_outputCompounds(false),
m_outputCompoundParts(false),
m_outputAllCompounds(false),
m_wordTag("w"),
m_sentenceBoundaryTag("s"),
m_specificEntityTag("e"),
m_compoundTag("c"),
m_bowGenerator(0)
{
  // default features
  m_defaultFeatures["p"]="position";
  m_defaultFeatures["inf"]="word";
  m_defaultFeatures["pos"]="property:MICRO";
  m_defaultFeatures["lemma"]="lemma";
}

//@todo : copy constructor: deal with copy of pointers

GenericXmlDumper::~GenericXmlDumper()
{
  clearFeatures();
  if (m_bowGenerator!=0) {
    delete m_bowGenerator;
    m_bowGenerator=0;
  }
}

void GenericXmlDumper::clearFeatures() {
  for (WordFeatures::const_iterator it=m_features.begin(),it_end=m_features.end();it!=it_end;it++) {
    if (*it!=0) {
      delete *it;
    }
  }
  m_features.clear();
  m_bowFeatures.clear();
  m_featureTags.clear();
}

void GenericXmlDumper::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  DUMPERLOGINIT;
  AbstractTextualAnalysisDumper::init(unitConfiguration,manager);

  try
  {
    m_graph=unitConfiguration.getParamsValueAtKey("graph");
  }
  catch (NoSuchParam& ) {} // keep default value

  m_features.setLanguage(m_language);
  m_bowFeatures.setLanguage(m_language);
  try { 
    // if some features are specified, all must be specified: do not keep any default ones
    map<string,string> featuresMap=unitConfiguration.getMapAtKey("features"); 
    try {
      // order can be specified (not in map: map is unordered)
      deque<string> featureOrder=unitConfiguration.getListsValueAtKey("featureOrder"); 
      initializeFeatures(featuresMap,featureOrder);
    }
    catch (NoSuchList& ) { // no order specified: keep order from map
      initializeFeatures(featuresMap);
    }
  }
  catch (NoSuchMap& ) {
    //initialize with default values
    initializeFeatures(m_defaultFeatures);
  }

  try {
    string str=unitConfiguration.getParamsValueAtKey("words");
    if (str=="no" || str=="false" || str=="") {
      m_outputWords=false;
    }
    else {
      m_outputWords=true;
      m_wordTag=str;
      LDEBUG << "GenericXmlDumper: outputSpecificEntities set to true (tag is " << str << ")";
    }
  }
  catch (NoSuchParam& ) {// optional : do not output entities if param not specified
  } 

  try {
    string str=unitConfiguration.getParamsValueAtKey("specificEntities");
    if (str!="no" && str!="false" && str!="") {
      m_outputSpecificEntities=true;
      m_specificEntityTag=str;
      LDEBUG << "GenericXmlDumper: outputSpecificEntities set to true (tag is " << str << ")";
    }
  }
  catch (NoSuchParam& ) {// optional : do not output entities if param not specified
  } 

  try {
    string str=unitConfiguration.getParamsValueAtKey("specificEntityParts");
    if (str!="no" && str!="false" && str!="") {
      m_outputSpecificEntityParts=true;
    }
  }
  catch (NoSuchParam& ) { }// optional : do not output entities if param not specified
  
  try {
    string str=unitConfiguration.getParamsValueAtKey("sentenceBoundaries");
    if (str!="no" && str!="false" && str!="") {
      m_outputSentenceBoundaries=true;
      m_sentenceBoundaryTag=str;
      LDEBUG << "GenericXmlDumper: outputSentenceBoundaries set to true (tag is " << str << ")";
    }
  }
  catch (NoSuchParam& ) {} // optional : do not output entities if param not specified

  try {
    string str=unitConfiguration.getParamsValueAtKey("compounds");
    if (str!="no" && str!="false" && str!="") {
      m_outputCompounds=true;
      m_compoundTag=str;
      // initialize compound creator
      m_bowGenerator=new Compounds::BowGenerator();
      m_bowGenerator->init(unitConfiguration, m_language);
      LDEBUG << "GenericXmlDumper: outputCompounds set to true (tag is " << str << ")";
    }
  }
  catch (NoSuchParam& ) {} // optional : do not output entities if param not specified

  try {
    string str=unitConfiguration.getParamsValueAtKey("compoundParts");
    if (str!="no" && str!="false" && str!="") {
      m_outputCompoundParts=true;
    }
  }
  catch (NoSuchParam& ) { }// optional : do not output entities if param not specified

  try {
    string str=unitConfiguration.getParamsValueAtKey("allCompounds");
    if (str!="no" && str!="false" && str!="") {
      m_outputAllCompounds=true;
    }
  }
  catch (NoSuchParam& ) { }// optional : do not output entities if param not specified

  
/*  const Common::PropertyCode::PropertyCodeManager& codeManager=static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager();
  m_propertyAccessor=&codeManager.getPropertyAccessor(m_property);
  m_propertyManager=&codeManager.getPropertyManager(m_property);

  try { 
    std::string str=unitConfiguration.getParamsValueAtKey("outputTStatus"); 
    if (str=="yes" || str=="1") {
      m_outputTStatus=true;
    }
  }
  catch (NoSuchParam& ) {} // keep default value
 
 try { 
   std::string str=unitConfiguration.getParamsValueAtKey("outputVerbTense"); 
   if (str=="yes" || str=="1") {
     m_outputVerbTense=true;
     m_tenseAccessor=&codeManager.getPropertyAccessor("TIME");
     m_tenseManager=&codeManager.getPropertyManager("TIME");
   }
 }
 catch (NoSuchParam& ) {} // keep default value
*/ 
}

void GenericXmlDumper::initializeFeatures(const map<string,string>& featuresMap, 
                                          const std::deque<std::string>& featureOrder) 
{
  clearFeatures();
  bool useMapOrder(false);
  if (! featureOrder.empty()) {
    // use specified order 
    DUMPERLOGINIT;
    LDEBUG << "GenericXmlDumper: initialize features: use order";
    for (deque<string>::const_iterator it=featureOrder.begin(),it_end=featureOrder.end();it!=it_end;it++) {
      LDEBUG << "GenericXmlDumper: --"<< (*it);
      const std::string& featureTag=(*it);
      m_featureTags.push_back(featureTag);
      map<string,string>::const_iterator f=featuresMap.find(featureTag);
      if (f==featuresMap.end()) {
        DUMPERLOGINIT;
        LWARN << "GenericXmlDumper: 'featureOrder' parameter mentions a feature '" << featureTag
              << "' not in feature map parameter: order ignored";
        useMapOrder=true;
        m_featureNames.clear();
        m_featureTags.clear();
        break;
      }
      m_featureNames.push_back((*f).second);
    }
  }
  else {
    useMapOrder=true;
  }
  
  if (useMapOrder) {
    for (map<string,string>::const_iterator it=featuresMap.begin(),it_end=featuresMap.end();it!=it_end; it++) {
      const std::string& featureName=(*it).second;
      const std::string& featureTag=(*it).first;
      m_featureNames.push_back(featureName);
      m_featureTags.push_back(featureTag);
    }
  }
  m_features.initialize(m_featureNames);
  m_bowFeatures.initialize(m_featureNames);
  if (m_features.size()!=m_featureTags.size() || m_bowFeatures.size()!=m_featureTags.size()) {
    DUMPERLOGINIT;
    LERROR << "GenericXmlDumper: error: failed to initialize all features";
    throw InvalidConfiguration();
  }
}

LimaStatusCode GenericXmlDumper::
process(AnalysisContent& analysis) const
{
  TimeUtils::updateCurrentTime();
  DUMPERLOGINIT;
  LDEBUG << "GenericXmlDumper::process";
  
  LinguisticMetaData* metadata=static_cast<LinguisticMetaData*>(analysis.getData("LinguisticMetaData"));
  if (metadata == 0)
  {
    LERROR << "no LinguisticMetaData ! abort";
    return MISSING_DATA;
  }

  AnalysisGraph* anagraph=static_cast<AnalysisGraph*>(analysis.getData("AnalysisGraph"));
  if (anagraph==0)
  {
    LERROR << "no graph 'AnaGraph' available !";
    return MISSING_DATA;
  }
  AnalysisGraph* posgraph=static_cast<AnalysisGraph*>(analysis.getData("PosGraph"));
  if (posgraph==0)
  {
    LERROR << "no graph 'PosGraph' available !";
    return MISSING_DATA;
  }
  AnnotationData* annotationData = static_cast< AnnotationData* >(analysis.getData("AnnotationData"));
  if (annotationData==0)
  {
    LERROR << "no annotation graph available !";
    return MISSING_DATA;
  }

  SyntacticData* syntacticData = 0;
  if(m_outputCompounds) {
    syntacticData=static_cast< SyntacticData* >(analysis.getData("SyntacticData"));
    if (annotationData==0)
    {
      LWARN << "compounds are supposed to be printed in output but no syntactic data available !";
    }
  }

  DumperStream* dstream=initialize(analysis);
  xmlOutput(dstream->out(), analysis, anagraph, posgraph, annotationData,syntacticData);
  delete dstream;

  TimeUtils::logElapsedTime("GenericXmlDumper");
  return SUCCESS_ID;
}

void GenericXmlDumper::
xmlOutput(std::ostream& out,
          AnalysisContent& analysis,
          AnalysisGraph* anagraph,
          AnalysisGraph* posgraph,
          const Common::AnnotationGraphs::AnnotationData* annotationData,
          const SyntacticAnalysis::SyntacticData* syntacticData) const
{
  DUMPERLOGINIT;

  out << "<text>" << endl;
  
  LinguisticMetaData* metadata=static_cast<LinguisticMetaData*>(analysis.getData("LinguisticMetaData"));

  const FsaStringsPool& sp=Common::MediaticData::MediaticData::single().stringsPool(m_language);

  SegmentationData* sb(0);
  if (m_outputSentenceBoundaries) {
    sb=static_cast<SegmentationData*>(analysis.getData("SentenceBoundaries"));
    if (sb==0) {
      LWARN << "GenericXmlDumper:: no SentenceBoundaries";
    }
  }

  if (sb==0)
  {
    // no sentence bounds : dump all text at once
    xmlOutputVertices(out,
                      anagraph,
                      posgraph,
                      annotationData,
                      syntacticData,
                      anagraph->firstVertex(),
                      anagraph->lastVertex(),
                      sp,
                      metadata->getStartOffset());
  }
  else
  {
    // ??OME2 uint64_t nbSentences(sb->size());
    uint64_t nbSentences((sb->getSegments()).size());
    LDEBUG << "GenericXmlDumper: "<< nbSentences << " sentences found";
    for (uint64_t i=0; i<nbSentences; i++)
    {
      // ??OME2 LinguisticGraphVertex sentenceBegin=(*sb)[i].getFirstVertex();
      // LinguisticGraphVertex sentenceEnd=(*sb)[i].getLastVertex();
      LinguisticGraphVertex sentenceBegin=(sb->getSegments())[i].getFirstVertex();
      LinguisticGraphVertex sentenceEnd=(sb->getSegments())[i].getLastVertex();
      
      // if (sentenceEnd==posgraph->lastVertex()) {
      //   continue;
      // }
      
      LDEBUG << "dump sentence between " << sentenceBegin << " and " << sentenceEnd;
      LDEBUG << "dump simple terms for this sentence";
      
      ostringstream oss;
      xmlOutputVertices(oss,
                        anagraph,
                        posgraph,
                        annotationData,
                        syntacticData,
                        sentenceBegin,
                        sentenceEnd,
                        sp,
                        metadata->getStartOffset());
      string str=oss.str();
      if (str.empty()) {
        LDEBUG << "nothing to dump in this sentence";
      }
      else {
        out << "<" << m_sentenceBoundaryTag << " id=\"" << i << "\">" << endl
            << str
            << "</"<< m_sentenceBoundaryTag << ">" << endl;
      }
    }
  }
  out << "</text>" << endl;
}

void GenericXmlDumper::
xmlOutputVertices(std::ostream& out,
                  AnalysisGraph* anagraph,
                  AnalysisGraph* posgraph,
                  const Common::AnnotationGraphs::AnnotationData* annotationData,
                  const SyntacticAnalysis::SyntacticData* syntacticData,
                  const LinguisticGraphVertex begin,
                  const LinguisticGraphVertex end,
                  const FsaStringsPool& sp,
                  const uint64_t offset) const
{

  DUMPERLOGINIT;
  LDEBUG << "GenericXmlDumper: ========================================";
  LDEBUG << "GenericXmlDumper: outputXml from vertex "  << begin << " to vertex " << end;

  LinguisticGraph* graph=posgraph->getGraph();
  LinguisticGraphVertex lastVertex=posgraph->lastVertex();
  
  map<Token*, vector<LinguisticGraphVertex>, lTokenPosition> sortedTokens;

  std::queue<LinguisticGraphVertex> toVisit;
  std::set<LinguisticGraphVertex> visited;
  
  LinguisticGraphOutEdgeIt outItr,outItrEnd;
 
  // output vertices between begin and end,
  // but do not include begin (beginning of text or previous end of sentence) and include end (end of sentence)
  toVisit.push(begin);

  bool first=true;
  bool last=false;
  while (!toVisit.empty()) {
    LinguisticGraphVertex v=toVisit.front();
    toVisit.pop();
    if (last || v == lastVertex) {
      continue;
    }
    if (v == end) {
      last=true;
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
    
    if (first) {
      first=false;
    }
    else {
      Token* t=get(vertex_token,*graph,v);
      if( t!=0) {
        sortedTokens[t].push_back(v);
      }
    }
  }
  
  // for compounds
  std::set<LinguisticGraphVertex> alreadyStoredVertices;
  
  // store outputs, sorting them by their positions (useful for compounds)
  map<uint64_t,vector<string> > xmlOutputs;
  
  for (map< Token*,vector<LinguisticGraphVertex>,lTokenPosition >::const_iterator
         it=sortedTokens.begin(),it_end=sortedTokens.end(); it!=it_end; it++)
  {
    const vector<LinguisticGraphVertex>& vertices=(*it).second;
    if (vertices.size()==0) {
      DUMPERLOGINIT;
      LERROR << "GenericXmlDumper: no vertices for token " << (*it).first->stringForm();
      continue;
    }
    
    for (vector<LinguisticGraphVertex>::const_iterator d=vertices.begin(),
      d_end=vertices.end(); d!=d_end; d++) {
      
      /*if (alreadyStoredVertices.find(*d)!=alreadyStoredVertices.end()) {
        // if already printed as a compound part
        continue;
      }*/
      ostringstream oss;
      xmlOutputVertex(oss,(*d),anagraph,posgraph,annotationData,syntacticData,
                      sp,offset,visited,alreadyStoredVertices);
      uint64_t pos=(*it).first->position();
      xmlOutputs[pos].push_back(oss.str());
    }
  }

  for (map<uint64_t,vector<string> >::const_iterator it=xmlOutputs.begin(),it_end=xmlOutputs.end();it!=it_end;it++) {
    for (vector<string>::const_iterator s=(*it).second.begin(),s_end=(*it).second.end();s!=s_end;s++) {
      out << *s;
    }
  }
  
}

void GenericXmlDumper::
xmlOutputVertex(std::ostream& out, 
                LinguisticGraphVertex v,
                AnalysisGraph* anagraph,
                AnalysisGraph* posgraph,
                const Common::AnnotationGraphs::AnnotationData* annotationData,
                const SyntacticAnalysis::SyntacticData* syntacticData,
                const FsaStringsPool& sp,
                uint64_t offset,
                set<LinguisticGraphVertex>& visited,
                std::set<LinguisticGraphVertex>& alreadyStoredVertices) const
{
  DUMPERLOGINIT;
  LDEBUG << "GenericXmlDumper: output vertex " << v;

  // check if specific entity, event if m_outputSpecificEntities is false, to access parts
  // and print them as simple words
  std::pair<const SpecificEntityAnnotation*,AnalysisGraph*>
  se=checkSpecificEntity(v,anagraph,posgraph,annotationData);
  if (se.first!=0) {
    LDEBUG << "GenericXmlDumper: -- is a specific entity ";
    if (xmlOutputSpecificEntity(out,se.first,se.second,sp,offset)) {
      return;
    }
    else {
      DUMPERLOGINIT;
      LERROR << "failed to output specific entity for vertex " << v;
    }
  }

  if (m_outputCompounds) {
    // check if the word is head of a compound
    std::vector<BoWToken*> compoundTokens=
    checkCompound(v, anagraph, posgraph, annotationData, syntacticData, offset, visited);
    if (compoundTokens.size()!=0) {
      for (std::vector<BoWToken*>::const_iterator it=compoundTokens.begin(),
        it_end=compoundTokens.end();it!=it_end;it++) {
        
        xmlOutputCompound(out,(*it),anagraph,posgraph,annotationData,sp,offset);
        std::set<uint64_t> bowTokenVertices = (*it)->getVertices();
        alreadyStoredVertices.insert(bowTokenVertices.begin(), bowTokenVertices.end());
      }
    }
    // clean
    for (std::vector<BoWToken*>::iterator it=compoundTokens.begin(),
        it_end=compoundTokens.end();it!=it_end;it++) {
      delete (*it);
      (*it)=0;
    }
  }

  LDEBUG << "GenericXmlDumper: -- is simple word ";
  // if not a specific entity nor a compound, output simple word infos
  if (m_outputWords) {
    xmlOutputVertexInfos(out, v, posgraph, offset);
  }
}

std::pair<const SpecificEntityAnnotation*,AnalysisGraph*>
GenericXmlDumper::checkSpecificEntity(LinguisticGraphVertex v,
                                      LinguisticAnalysisStructure::AnalysisGraph* anagraph,
                                      AnalysisGraph* posgraph,
                                      const Common::AnnotationGraphs::AnnotationData* annotationData) const
{
  // first, check if vertex corresponds to a specific entity found before pos tagging (i.e. in analysis graph)
  std::set< AnnotationGraphVertex > anaVertices = annotationData->matches("PosGraph",v,"AnalysisGraph");
  // note: anaVertices size should be 0 or 1
  for (std::set< AnnotationGraphVertex >::const_iterator anaVerticesIt = anaVertices.begin();
       anaVerticesIt != anaVertices.end(); anaVerticesIt++)
  {
    std::set< AnnotationGraphVertex > matches = annotationData->matches("AnalysisGraph",*anaVerticesIt,"annot");
    for (std::set< AnnotationGraphVertex >::const_iterator it = matches.begin();
         it != matches.end(); it++)
    {
      AnnotationGraphVertex vx=*it;
      if (annotationData->hasAnnotation(vx, Common::Misc::utf8stdstring2limastring("SpecificEntity")))
      {
        const SpecificEntityAnnotation* se =
          annotationData->annotation(vx, Common::Misc::utf8stdstring2limastring("SpecificEntity")).
          pointerValue<SpecificEntityAnnotation>();
          return make_pair(se,anagraph);
      }
    }
  }

  // then check if vertex corresponds to a specific entity found after POS tagging
  std::set< AnnotationGraphVertex > matches = annotationData->matches("PosGraph",v,"annot");
  for (std::set< AnnotationGraphVertex >::const_iterator it = matches.begin();
       it != matches.end(); it++)
  {
    AnnotationGraphVertex vx=*it;
    if (annotationData->hasAnnotation(vx, Common::Misc::utf8stdstring2limastring("SpecificEntity")))
    {
      //BoWToken* se = createSpecificEntity(v,*it, annotationData, anagraph, posgraph, offsetBegin);
      const SpecificEntityAnnotation* se =
        annotationData->annotation(vx, Common::Misc::utf8stdstring2limastring("SpecificEntity")).
        pointerValue<SpecificEntityAnnotation>();
      return make_pair(se,posgraph);
    }
  }  
  return std::pair<const SpecificEntityAnnotation*,AnalysisGraph*>((const SpecificEntityAnnotation*)0,(AnalysisGraph*)0);
}

bool GenericXmlDumper::
xmlOutputSpecificEntity(std::ostream& out,
                        const SpecificEntities::SpecificEntityAnnotation* se,
                        LinguisticAnalysisStructure::AnalysisGraph* graph,
                        const FsaStringsPool& sp,
                        uint64_t offset) const
{
  if (se == 0) {
    DUMPERLOGINIT;
    LERROR << "missing specific entity annotation";
    return false;
  }
  
  // output enclosing tag for entity with associated information
  if (m_outputSpecificEntities) {
    out << "<" << m_specificEntityTag;
    for (unsigned int i=0,size=m_featureNames.size();i<size;i++) {
      // try to get value directly from specific entity (defined for some features)
      string value=xmlString(specificEntityFeature(se,m_featureNames[i],sp,offset));
      if (value.empty()) {
        // otherwise, get features from head
        value=xmlString(m_features[i]->getValue(graph,se->getHead()));
      }
      out << " " << m_featureTags[i] << "=\"" << value << "\"";
    }
    //<< " inf=\"" << xmlString(Common::Misc::limastring2utf8stdstring(sp[se->getString()])) << "\""

    if (m_outputSpecificEntityParts) {
      // output tag as enclosing tag, with parts enclosed
      out << ">" << endl;
      for (std::vector< LinguisticGraphVertex>::const_iterator m(se->m_vertices.begin());
           m != se->m_vertices.end(); m++)
      {
        xmlOutputVertexInfos(out,(*m),graph,offset);
      }
      out << "</" << m_specificEntityTag << ">" << endl;
    }
    else {
      // output only the named entity tag
      out << "/>" << endl;
    }
  }
  else {
    // output parts as simple words
    for (std::vector< LinguisticGraphVertex>::const_iterator m(se->m_vertices.begin());
         m != se->m_vertices.end(); m++)
    {
      xmlOutputVertexInfos(out,(*m),graph,offset);
    }
  }
  
  // take as category for parts the category for the named entity
  /*LinguisticCode category=m_propertyAccessor->readValue(data->begin()->properties);
  DUMPERLOGINIT;
  LDEBUG << "Using category " << m_propertyManager->getPropertySymbolicValue(category) << " for specific entity of type " << typeName;
  */

  return true;

}

std::vector<BoWToken*> GenericXmlDumper::
checkCompound(LinguisticGraphVertex v,
              AnalysisGraph* anagraph,
              AnalysisGraph* posgraph,
              const Common::AnnotationGraphs::AnnotationData* annotationData,
              const SyntacticAnalysis::SyntacticData* syntacticData,
              uint64_t offset,
              set<LinguisticGraphVertex>& visited) const
{
  DUMPERLOGINIT;
  LDEBUG << "GenericXmlDumper: check if compound for vertex " << v; 
  
  std::set< AnnotationGraphVertex > cpdsHeads = annotationData->matches("PosGraph", v, "cpdHead");
  if (cpdsHeads.empty())
  {
    // not a compound
    return std::vector<BoWToken*>();
  }
  
  LDEBUG << "GenericXmlDumper: -- is head of a compound ";
  std::vector<BoWToken*> tokens;
  std::set< std::string > alreadyStored;
  for (std::set< AnnotationGraphVertex >::const_iterator it=cpdsHeads.begin(), it_end=cpdsHeads.end(); 
       it!=it_end; it++)
  {
    const AnnotationGraphVertex& agv=*it;

    // create compound using BoWGeneration : store in BoW
    std::vector<std::pair<BoWRelation*, BoWToken*> > bowTokens = 
    m_bowGenerator->buildTermFor(agv, agv, *(anagraph->getGraph()), *(posgraph->getGraph()), offset, 
                                 syntacticData, annotationData, visited);
    for (std::vector<std::pair<BoWRelation*, BoWToken*> >::const_iterator bowItr=bowTokens.begin();
         bowItr!=bowTokens.end(); bowItr++)
    {
      std::string elem = (*bowItr).second->getIdUTF8String();
      if (alreadyStored.find(elem) != alreadyStored.end())
      {  
        // already stored
        //          LDEBUG << "BuildBoWTokenListVisitor: BoWToken already stored. Skipping it.";
        delete (*bowItr).first;
        delete (*bowItr).second;
      }
      else {
        tokens.push_back((*bowItr).second);
        alreadyStored.insert(elem);
      }
    }
  }
  return tokens;
}

void GenericXmlDumper::
xmlOutputCompound(std::ostream& out, 
                  Common::BagOfWords::AbstractBoWElement* token,
                  LinguisticAnalysisStructure::AnalysisGraph* anagraph,
                  LinguisticAnalysisStructure::AnalysisGraph* posgraph,
                  const AnnotationData* annotationData,
                  const FsaStringsPool& sp,
                  uint64_t offset) const
{
  DUMPERLOGINIT;
  LDEBUG << "GenericXmlDumper: output BoWToken [" << token->getOutputUTF8String() << "]";
  switch (token->getType()) {
    case BOW_PREDICATE:{
      // FIXME To implement
      LERROR << "GenericXmlDumper: BOW_PREDICATE support not implemented";
      break;
    }
    case BOW_TERM: {
      LDEBUG << "GenericXmlDumper: output BoWTerm";
      // compound informations
      out << "<" << m_compoundTag;
      xmlOutputBoWInfos(out,token,offset);
      
      if (m_outputCompoundParts) {
        // close opening tag, then parts, then closing tag
        out << ">" << endl;
      }
      else {
        //single tag
        out << "/>" << endl;
      }
      
      // go through parts in any case, at least to get enclosed compounds
      if (m_outputAllCompounds) {
        // use iterator to create all partial compounds
        BoWText t;
        t.push_back(token);
        BoWTokenIterator bit(t);
        if (! bit.isAtEnd()) {
          LDEBUG << "first token=" << bit.getElement()->getOutputUTF8String();
          bit++; // first one is same BoWTerm
        }
        while (! bit.isAtEnd()) {
          AbstractBoWElement* tok=const_cast<AbstractBoWElement*>(bit.getElement());
          LDEBUG << "next token=" << tok->getOutputUTF8String();
          xmlOutputCompound(out,tok,anagraph,posgraph,annotationData,sp,offset);
          bit++;
        }
      }
      else {
        // output only enclosed compounds
        BoWTerm* term=static_cast<BoWTerm*>(token);
        const std::deque< BoWComplexToken::Part >& parts=term->getParts();
        for (std::deque<BoWComplexToken::Part>::const_iterator p=parts.begin(),p_end=parts.end();p!=p_end;p++) {
          xmlOutputCompound(out,(*p).getBoWToken(),anagraph,posgraph,annotationData,sp,offset);
        }
      }

      if (m_outputCompoundParts) {
        out << "</" << m_compoundTag << ">" << endl;
      }
      break;
    }
    case BOW_NAMEDENTITY: {
      if (m_outputCompoundParts) {
        LinguisticGraphVertex v=dynamic_cast<BoWNamedEntity*>(token)->getVertex();
        LDEBUG << "GenericXmlDumper: output BoWNamedEntity of vertex " << v;
        std::pair<const SpecificEntityAnnotation*,AnalysisGraph*>
        se=checkSpecificEntity(v,anagraph,posgraph,annotationData);
        if (se.first==0) {
        DUMPERLOGINIT;
        LERROR << "GenericXmlDumper: for vertex " << v << ": specific entity not found";
        }
        else {
          xmlOutputSpecificEntity(out,se.first,se.second,sp,offset);
        }
      }
      break;
    }
    case BOW_TOKEN: {
      if  (m_outputCompoundParts) {
        LinguisticGraphVertex v=dynamic_cast<BoWToken*>(token)->getVertex();
        LDEBUG << "GenericXmlDumper: output BoWToken of vertex " << v;
        xmlOutputVertexInfos(out,v,posgraph,offset);
      }
      break;
    }
    default: {
      DUMPERLOGINIT;
      LERROR << "GenericXmlDumper: Error: BowToken has type BOW_NOTYPE";
      
    }
  }
}

void GenericXmlDumper::xmlOutputVertexInfos(std::ostream& out, 
                                           LinguisticGraphVertex v,
                                           LinguisticAnalysisStructure::AnalysisGraph* graph,
                                           uint64_t offset) const
{
  out << "<" << m_wordTag;
  for (unsigned int i=0,size=m_features.size();i<size;i++) {
    std::string value;
    // for position, correct with offset : hard coded name
    if (m_features[i]->getName()=="position") {
      unsigned int pos=atoi(m_features[i]->getValue(graph,v).c_str());
      pos+=offset;
      ostringstream oss;
      oss << pos;
      value=oss.str();
    }
    else {
      value=xmlString(m_features[i]->getValue(graph,v));
    }
    out << " " << m_featureTags[i] << "=\"" << value << "\"";
  }
  out << "/>" << endl;
}                          

void GenericXmlDumper::xmlOutputBoWInfos(ostream& out, AbstractBoWElement* token, uint64_t offset) const
{
  for (unsigned int i=0,size=m_bowFeatures.size();i<size;i++) {
    std::string value;
    // for position, correct with offset : hard coded name
    if (m_bowFeatures[i]->getName()=="position") {
      unsigned int pos=atoi(m_bowFeatures[i]->getValue(token).c_str());
      pos+=offset;
      ostringstream oss;
      oss << pos;
      value=oss.str();
    }
    else {
      value=xmlString(m_bowFeatures[i]->getValue(token));
    }
    out << " " << m_featureTags[i] << "=\"" << value << "\"";
  }
}

std::string GenericXmlDumper::
specificEntityFeature(const SpecificEntities::SpecificEntityAnnotation* se,
                      const std::string& featureName,
                      const FsaStringsPool& sp,
                      uint64_t offset) const
{
  // all hard-coded feature names : not really clean, but a clean definition of all features for 
  // a specialized class such as SpecificEntityAnnotation seems a bit too much...
  if (featureName=="position") { 
    uint64_t pos=se->getPosition();
    pos+=offset;
    ostringstream oss;
    oss << pos;
    return oss.str();
  }
  if (featureName.find("property:MACRO")==0) { // put entity type in category
    std::string typeName("");
    try {
      LimaString str= MediaticData::single().getEntityName(se->getType());
      typeName=Common::Misc::limastring2utf8stdstring(str);
    }
    catch (std::exception& ) {
      DUMPERLOGINIT;
    LERROR << "Undefined entity type " << se->getType();
      return "";
    }
    return typeName;
  }
  else if (featureName=="lemma") {
    return Common::Misc::limastring2utf8stdstring(sp[se->getNormalizedForm()]);
  }
  else if (featureName=="word") {
    return Common::Misc::limastring2utf8stdstring(sp[se->getString()]);
  }
  return "";
}

//--------------------------------------------------------------------------------------
// string manipulation functions to protect XML entities
std::string GenericXmlDumper::xmlString(const std::string& inputStr) const
{
  // protect XML entities
  std::string str(inputStr);
  replace(str,"&", "&amp;");
  replace(str,"<", "&lt;");
  replace(str,">", "&gt;");
  replace(str,"\"", "&quot;");
  replace(str,"\n", "\\n");
  return str;
}

void GenericXmlDumper::replace(std::string& str, 
                              const std::string& toReplace, 
                              const std::string& newValue) const
{
  string::size_type oldLen=toReplace.size();
  string::size_type newLen=newValue.size();
  string::size_type i=str.find(toReplace);
  while (i!=string::npos) {
    str.replace(i,oldLen,newValue);
    i+=newLen;
    i=str.find(toReplace,i);
  }
}


} // AnalysisDumper
} // LinguisticProcessing
} // Lima
