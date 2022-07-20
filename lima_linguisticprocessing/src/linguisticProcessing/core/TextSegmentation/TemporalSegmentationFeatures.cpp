// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       TemporalSegmentationFeatures.cpp
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Thu Feb 10 2011
 * copyright   Copyright (C) 2011 by CEA LIST
 * 
 ***********************************************************************/

#include "TemporalSegmentationFeatures.h"
#include "linguisticProcessing/core/Automaton/SpecificEntityAnnotation.h"
#include "common/Data/strwstrtools.h"
#include <sstream>
#include <queue>

using namespace std;
using namespace boost;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::Common::AnnotationGraphs;

namespace Lima {
namespace LinguisticProcessing {

//***********************************************************************
// factories
SegmentFeatureExtractorFactory<SegmentFeatureEntity> SegmentFeatureEntityFactory(SegmentFeatureEntity_ID);
SegmentFeatureExtractorFactory<SegmentFeatureEntityInData> SegmentFeatureEntityInDataFactory(SegmentFeatureEntityInData_ID);
SegmentFeatureExtractorFactory<SegmentFeatureRank> SegmentFeatureRankFactory(SegmentFeatureRank_ID);
SegmentFeatureExtractorFactory<SegmentFeatureCheckProperty> SegmentFeatureCheckPropertyFactory(SegmentFeatureCheckProperty_ID);
SegmentFeatureExtractorFactory<SegmentFeatureInSegment> SegmentFeatureInSegmentFactory(SegmentFeatureInSegment_ID);

//***********************************************************************
// feature accessors
//***********************************************************************
SegmentFeatureEntity::SegmentFeatureEntity(MediaId language, const std::string& complement):
AbstractSegmentFeatureExtractor(language,complement),
m_entityName(complement),
m_entityType(),
m_annotationData(0)
{
  m_entityType=Common::MediaticData::MediaticData::single().
  getEntityType(Common::Misc::utf8stdstring2limastring(complement));
}

void SegmentFeatureEntity::
update(const AnalysisContent& analysis) {
  m_annotationData = static_cast<const AnnotationData*>(analysis.getData("AnnotationData"));
  if (m_annotationData==0)
  {
    LOGINIT("LP::Segmentation");
    LERROR << "no annotation graph available !";
  }
}

std::string SegmentFeatureEntity::
getValue(const AnalysisGraph* anagraph,
         const Segment& seg) const
{
  uint64_t nbEntities(0);
  if (m_annotationData!=0) {
  
    bool isPosGraph=(anagraph->getGraphId()=="PosGraph");
    
    // check if an entity of a given type exists in the segment : 
    // go through the graph inside the segment
    const LinguisticGraph* graph=anagraph->getGraph();
    LinguisticGraphVertex lastVertex=anagraph->lastVertex();

    std::queue<LinguisticGraphVertex> toVisit;
    std::set<LinguisticGraphVertex> visited;
    
    LinguisticGraphOutEdgeIt outItr,outItrEnd;
  
    toVisit.push(seg.getFirstVertex());

    bool first=true;
    bool last=false;
    while (!toVisit.empty()) {
      LinguisticGraphVertex v=toVisit.front();
      toVisit.pop();
      if (last || v == lastVertex) {
        continue;
      }
      if (v == seg.getLastVertex()) {
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
        // check for entity
        if (isPosGraph) {
          if (isVertexInPosGraphAnEntityOfType(v, m_annotationData, m_entityType)) {
            nbEntities++;
          }
          else {
            if (isAnEntityOfType(v, m_annotationData, m_entityType, "AnalysisGraph")) {
              nbEntities++;
            }
          }
        }
      }
    }
  }
  ostringstream oss;
  oss << m_entityName << "_" << nbEntities;
  return oss.str();
}

bool SegmentFeatureEntity::
isAnEntityOfType(LinguisticGraphVertex v, 
                 const Common::AnnotationGraphs::AnnotationData* annotationData,
                 Common::MediaticData::EntityType type,
                 const std::string& graphId) const
{
  std::set< AnnotationGraphVertex > matches = annotationData->matches(graphId,v,"annot");
  for (std::set< AnnotationGraphVertex >::const_iterator it = matches.begin();
  it != matches.end(); it++)
  {
    AnnotationGraphVertex vx=*it;
    if (annotationData->hasAnnotation(vx, Common::Misc::utf8stdstring2limastring("SpecificEntity")))
    {
      const SpecificEntities::SpecificEntityAnnotation* se =
      annotationData->annotation(vx, Common::Misc::utf8stdstring2limastring("SpecificEntity")).
      pointerValue<SpecificEntities::SpecificEntityAnnotation>();
      if (se->getType()==type) {
        return true;
      }
    }
  }
  return false;
}
                                   
                                   
bool SegmentFeatureEntity::
isVertexInPosGraphAnEntityOfType(LinguisticGraphVertex v, 
                                 const Common::AnnotationGraphs::AnnotationData* annotationData,
                                 Common::MediaticData::EntityType type) const
{

  // first, check if vertex corresponds to a specific entity found before pos tagging (i.e. in analysis graph)
  std::set< AnnotationGraphVertex > anaVertices = annotationData->matches("PosGraph",v,"AnalysisGraph");
  // note: anaVertices size should be 0 or 1
  for (std::set< AnnotationGraphVertex >::const_iterator anaVerticesIt = anaVertices.begin();
       anaVerticesIt != anaVertices.end(); anaVerticesIt++)
  {
    if (isAnEntityOfType(*anaVerticesIt,annotationData,type,"AnalysisGraph")) {
      return true;
    }
  }

  // check annotations directly in PosGraph
  if (isAnEntityOfType(v,annotationData,type,"PosGraph")) {
    return true;
  }
  
  return false;
}

//***********************************************************************
SegmentFeatureEntityInData::SegmentFeatureEntityInData(MediaId language, const std::string& complement):
AbstractSegmentFeatureExtractor(language,complement),
m_entityName(),
m_dataName(),
m_entityType()
{
  // complement contains the name of the entity + the name of the data
  // in which it is stored (separated by a comma)
  string::size_type i=complement.find(",");
  if (i==string::npos) {
    LOGINIT("LP::Segmentation");
    LERROR << "No data name provided for entityInData";
    m_entityName=complement;
  }
  else {
    m_entityName=string(complement,0,i);
    m_dataName=string(complement,i+1);
  }
  m_entityType=Common::MediaticData::MediaticData::single().
  getEntityType(Common::Misc::utf8stdstring2limastring(m_entityName));
}

void SegmentFeatureEntityInData::
update(const AnalysisContent& analysis) {

  // get result data
  const AnalysisData* resultData=analysis.getData(m_dataName);
  if (resultData == 0) {
    LOGINIT("LP::Segmentation");
    LERROR << "no data " << m_data << "in AnalysisContent";
  }
  m_data=dynamic_cast<const ApplyRecognizer::RecognizerResultData*>(resultData);
  if (m_data == 0) {
    LOGINIT("LP::Segmentation");
    LERROR << "data " << m_data << "in AnalysisContent is not a RecognizerResultData";
  }
  
}

std::string SegmentFeatureEntityInData::
getValue(const AnalysisGraph* /*anagraph*/,
         const Segment& seg) const
{
  if (m_data==0) {
    return "";
  }
  uint64_t nbEntities(0);
  for (ApplyRecognizer::RecognizerResultData::const_iterator it=m_data->begin(),it_end=m_data->end();
       it!=it_end; it++) {
    for (std::vector< Automaton::RecognizerMatch >::const_iterator m=(*it).begin(),
           m_end=(*it).end(); m!=m_end; m++) {
      if ((*m).getType()==m_entityType && 
          (*m).positionBegin()>=seg.getPosBegin() && 
          (*m).positionEnd()<=seg.getPosEnd()) {
        nbEntities++;
      }
    }
  }
  ostringstream oss;
  oss << m_entityName << "_" << nbEntities;
  return oss.str();
}

//***********************************************************************
SegmentFeatureRank::SegmentFeatureRank(MediaId language, const std::string& complement):
AbstractSegmentFeatureExtractor(language,complement),
m_data(),
m_segmData(0)
{
  m_data=complement;
}

void SegmentFeatureRank::
update(const AnalysisContent& analysis) {
  // store information from segmentation data to know at which segment we are
  const AnalysisData* data=analysis.getData(m_data);
  if (data==0) {
    return;
  }
  m_segmData=static_cast<const SegmentationData*>(data);
}

std::string SegmentFeatureRank::
getValue(const AnalysisGraph* /*graph*/,
         const Segment& seg) const
{
  // give rank of segment in segmentation data
  uint64_t rank(0);
  uint64_t i(0);
  for (vector<Segment>::const_iterator it=m_segmData->getSegments().begin(),
    it_end=m_segmData->getSegments().end();it!=it_end;it++) {
    if ((*it).getPosBegin()==seg.getPosBegin() && (*it).getLength() == seg.getLength()) {
      rank=i;
      break;
    }
    i++;
  }
  ostringstream oss;
  oss << "rank_" << rank;
  return oss.str();
}

//***********************************************************************
SegmentFeatureCheckProperty::SegmentFeatureCheckProperty(MediaId language, const std::string& complement):
AbstractSegmentFeatureExtractor(language,complement),
m_propertyName(""),
m_propertyValue(""),
m_value(0),
m_propertyAccessor(0)
{
  string::size_type i=complement.find("=");
  if (i==string::npos) {
    LOGINIT("LP::Segmentation");
    LERROR << SegmentFeatureCheckProperty_ID << ": no property value (format must be PROPERTY=VALUE)";
  }
  else {
    m_propertyName=string(complement,0,i);
    m_propertyValue=string(complement,i+1);
    const Common::PropertyCode::PropertyCodeManager& codeManager=
      static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager();
    m_propertyAccessor=&codeManager.getPropertyAccessor(m_propertyName);

    const Common::PropertyCode::PropertyManager* propertyManager=&codeManager.getPropertyManager(m_propertyName);
    
    m_value=propertyManager->getPropertyValue(m_propertyValue);
  }
}

void SegmentFeatureCheckProperty::
update(const AnalysisContent& /*analysis*/) {}

std::string SegmentFeatureCheckProperty::
getValue(const AnalysisGraph* anagraph,
         const Segment& seg) const
{
  // check if initialization has been successful
  if (m_propertyAccessor==0) {
    return "";
  }

  uint64_t nbProp(0);

  // go through the graph inside the segment
  const LinguisticGraph* graph=anagraph->getGraph();
  LinguisticGraphVertex lastVertex=anagraph->lastVertex();
  
  std::queue<LinguisticGraphVertex> toVisit;
  std::set<LinguisticGraphVertex> visited;
  
  LinguisticGraphOutEdgeIt outItr,outItrEnd;
  
  toVisit.push(seg.getFirstVertex());
  
  bool first=true;
  bool last=false;
  while (!toVisit.empty()) {
    LinguisticGraphVertex v=toVisit.front();
    toVisit.pop();
    if (last || v == lastVertex) {
      continue;
    }
    if (v == seg.getLastVertex()) {
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
      // check for entity
      if (isProperty(v, graph, m_value)) {
        nbProp++;
      }
    }
  }
 
  ostringstream oss;
  oss << m_propertyValue << "_" << nbProp;
  return oss.str();
}

bool SegmentFeatureCheckProperty::
isProperty(LinguisticGraphVertex v, 
           const LinguisticGraph* graph,
           LinguisticCode propertyValue) const
{
  const MorphoSyntacticData* data=get(vertex_data,*graph,v);
  for (MorphoSyntacticData::const_iterator m=data->begin(),m_end=data->end();m!=m_end;m++) 
  {
    LinguisticCode code=m_propertyAccessor->readValue((*m).properties);
    if (code==propertyValue) {
      return true;
      }
  }
  return false;
}

//***********************************************************************
SegmentFeatureInSegment::SegmentFeatureInSegment(MediaId language, const std::string& complement):
AbstractSegmentFeatureExtractor(language,complement),
m_segmentData(""),
m_data(0)
{
  m_segmentData=complement;
}

void SegmentFeatureInSegment::
update(const AnalysisContent& analysis) 
{
  const AnalysisData* data=analysis.getData(m_segmentData);
  if (data==0) {
    LOGINIT("LP::Segmentation");
    LERROR << SegmentFeatureInSegment_ID << ": No data " << m_segmentData;
    m_data=0;
  }
  else {
    m_data=static_cast<const SegmentationData*>(data);
  }
}

std::string SegmentFeatureInSegment::
getValue(const AnalysisGraph* /*anagraph*/,
         const Segment& seg) const
{
  if (m_data==0) {
    return "";
  }
  std::string segmentType;
  //find corresponding segment in given segmentation data
  // ??OME2 for (SegmentationData::const_iterator s=m_data->begin(),s_end=m_data->end();s!=s_end;s++)
  for (std::vector<Segment>::const_iterator s=(m_data->getSegments()).begin(),s_end=(m_data->getSegments()).end();s!=s_end;s++)
  {
    if ((*s).getPosBegin() <= seg.getPosBegin() && (*s).getPosEnd() >= seg.getPosEnd()) {
      segmentType=(*s).getType();
      break;
    }
  }
  if (segmentType.empty()) {
    return "";
  }
  ostringstream oss;
  oss << segmentType << "_1";
  return oss.str();
}
           
} // end namespace
} // end namespace
