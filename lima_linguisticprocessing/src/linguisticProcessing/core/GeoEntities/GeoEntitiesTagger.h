// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/**
  *
  * @file        geoEntitiesTagger.h
  * @author      Faiza GARA (faiza.gara@cea.fr) 

  *              Copyright (c) 2010 by CEA
  * @date        Created on January, 15 2010
  * @version     $Id: eventAnalyzer.h 
  *
  */

#ifndef LIMA_GEOENTITIES_GEOENTITIESTAGGER_H
#define LIMA_GEOENTITIES_GEOENTITIESTAGGER_H

#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "linguisticProcessing/core/TextSegmentation/SegmentationData.h"
#include "common/MediaProcessors/MediaProcessUnit.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/common/PropertyCode/PropertyAccessor.h"
#include "common/ProcessUnitFramework/AnalysisContent.h"
#include "common/Data/LimaString.h"
#include "linguisticProcessing/core/FlatTokenizer/Tokenizer.h"
#include "GeoDbContext.h"

#include "boost/tuple/tuple.hpp"


#include <string>
#include <map>
#include <set>

namespace Lima
{

namespace LinguisticProcessing
{

namespace GeoEntities
{ 

#define GEOENTITIESTAGGERPU_CLASSID "GeoEntitiesTagger"

struct lTokenMap
{
  inline bool operator()(std::string token1, std::string token2) const
  {
    return strcmp(token1.c_str(), token2.c_str()) < 0;

  }
};

class GeoEntitiesTagger : public MediaProcessUnit
{
public:
  GeoEntitiesTagger();

  virtual ~GeoEntitiesTagger();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager)
  ;

  LimaStatusCode process(
    AnalysisContent& analysis) const;


  
private:  
  std::string m_graphId;
  MediaId m_language;
  unsigned short m_maxlength;
  const Common::PropertyCode::PropertyAccessor* m_macroAccessor;
  const Common::PropertyCode::PropertyAccessor* m_microAccessor;
  std::map< Lima::LinguisticCode, boost::tuple<std::set<Lima::LinguisticCode>, std::set<std::string> ,bool > > m_TriggerMicro;
  std::map< std::string, boost::tuple<std::set<Lima::LinguisticCode>, std::set<std::string> , bool> > m_TriggerStatus;
  std::map< Lima::LinguisticCode, boost::tuple<std::set<Lima::LinguisticCode>, std::set<std::string> ,bool> > m_EndWordMicro;
  std::map< std::string, boost::tuple<std::set<Lima::LinguisticCode>, std::set<std::string> ,bool> > m_EndWordStatus;


  FlatTokenizer::CharChart*  m_charChart;
  GeoDbContext *m_db;
  std::string m_field;
  std::string m_db_connect;
  std::string m_dbms;
protected:
  void compute_entities(LinguisticGraph*,LinguisticAnalysisStructure::AnalysisGraph*,Common::AnnotationGraphs::AnnotationData*) const;
  bool isTrigger(LinguisticGraphVertex,LinguisticGraph* graph,LinguisticGraphVertex,LinguisticAnalysisStructure::AnalysisGraph*) const;
  bool isEndWord(LinguisticGraphVertex,LinguisticGraph* graph) const;
  bool hasMicro(LinguisticGraphVertex,LinguisticGraph*,std::set< Lima::LinguisticCode >) const;
  std::set<std::string> getSet(std::string) const;
  bool hasStatus(LinguisticGraphVertex,LinguisticGraph*,std::set< std::string >) const;
  std::string Normalize(const Lima::LimaString ) const;
  std::string Normalize(std::string ) const;
  bool DichoFind(std::string, std::vector<std::string>) const;
  std::map<std::string,std::vector<uint64_t>, lTokenMap> getLocationMap(std::string) const;
  std::vector<boost::tuple<std::string,std::string,std::string> > getAllLocationClass(uint64_t ) const;
  std::vector<std::string>  getLocationField(uint64_t ,std::string) const;
};


} // closing namespace GeoEntities
} // closing namespace LinguisticProcessing
} // closing namespace Lima

#endif // LIMA_GEOENTITIES_GEOENTITIESTAGGER_H
