// Copyright 2002-2022 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "CompoundsBuilderFromSyntacticData.h"
#include "DepGraphCompoundsBuildVisitor.h"
#include "common/time/timeUtilsController.h"
#include "common/MediaticData/mediaticData.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/core/SyntacticAnalysis/SyntacticData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/TextSegmentation/SegmentationData.h"

using namespace Lima::Common::XMLConfigurationFiles;

//using namespace boost;
using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::LinguisticProcessing::SyntacticAnalysis;

namespace Lima
{
namespace LinguisticProcessing
{
namespace Compounds
{

SimpleFactory<MediaProcessUnit,CompoundsBuilderFromSyntacticData> compoundsBuilderFromSyntacticDataFactory(COMPOUNDSBUILDERFROMSYNTACTICDATAFACTORY_CLASSID);


class CompoundsBuilderFromSyntacticDataPrivate
{
  friend class CompoundsBuilderFromSyntacticData;

  CompoundsBuilderFromSyntacticDataPrivate();

  ~CompoundsBuilderFromSyntacticDataPrivate();

  void initConceptTypes(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration) ;
  void initMacros2ConceptsMapping(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration);
  void initCompoundRels(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration);

  uint64_t m_depGraphMaxBranchingFactor;
  MediaId m_language;
  std::map< std::string, Lima::Common::MediaticData::ConceptType > m_conceptTypes;
  std::map< Lima::Common::MediaticData::ConceptType, std::string > m_conceptNames;

  std::map< LinguisticCode, Lima::Common::MediaticData::ConceptType > m_macro2ConceptMapping;
  std::set< std::string > m_compoundsRels;
  bool m_useChains = false;
};

CompoundsBuilderFromSyntacticDataPrivate::CompoundsBuilderFromSyntacticDataPrivate()
{}


CompoundsBuilderFromSyntacticDataPrivate::~CompoundsBuilderFromSyntacticDataPrivate()
{}

CompoundsBuilderFromSyntacticData::CompoundsBuilderFromSyntacticData() :
    MediaProcessUnit(),
    m_d(new CompoundsBuilderFromSyntacticDataPrivate())
{}


CompoundsBuilderFromSyntacticData::~CompoundsBuilderFromSyntacticData()
{
  delete m_d;
}

void CompoundsBuilderFromSyntacticData::init(
  Lima::Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
//  LIMA_UNUSED(unitConfiguration);
//   MORPHOLOGINIT;
  m_d->m_language=manager->getInitializationParameters().media;
  m_d->initConceptTypes(unitConfiguration);
  m_d->initCompoundRels(unitConfiguration);
  m_d->initMacros2ConceptsMapping(unitConfiguration);
  try {
    m_d->m_useChains = unitConfiguration.getBooleanParameter("useChains");
  } catch (const NoSuchParam&)
  {}

}

void CompoundsBuilderFromSyntacticDataPrivate::initCompoundRels(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration)
{
  MORPHOLOGINIT;
  LINFO << "init compounds relations";
  try
  {
    // loads syntactic analysis resources
    std::deque< std::string > compoundsRelsList = unitConfiguration.getListsValueAtKey("CompoundsRels");
    m_compoundsRels.insert(compoundsRelsList.begin(), compoundsRelsList.end());
  }
  catch (NoSuchList& )
  {
    LERROR << "No list 'CompoundsRels' in 'SyntacticAnalysis' group for language " << (int)m_language;
    throw InvalidConfiguration();
  }
  catch (NoSuchGroup& )
  {
    LERROR << "No group 'SyntacticAnalysis' in common language configuration file for language " << (int)m_language;
    throw InvalidConfiguration();
  }
}

bool CompoundsBuilderFromSyntacticData::isACompoundRel(const std::string& rel) const
{
  return (m_d->m_compoundsRels.find(rel) != m_d->m_compoundsRels.end());
}

void CompoundsBuilderFromSyntacticDataPrivate::initConceptTypes(
    GroupConfigurationStructure& unitConfiguration)
{
#ifdef DEBUG_LP
  MDATALOGINIT;
  LINFO << "CompoundsBuilderFromSyntacticDataPrivate::initConceptTypes";
#endif

  try
  {
   const auto& mapping= unitConfiguration.getMapAtKey("conceptTypes");
#if __cplusplus >= 201703L // Only available with c++17 extensions : -std=c++1z
    for (const auto& [key, val] : mapping)
    {
#else
    for (std::map<std::string,std::string>::const_iterator it=mapping.begin();
         it!=mapping.end();
         it++)
    {
      const std::string key = it->first;
      const std::string val = it->second;
#endif
      auto type = static_cast<Lima::Common::MediaticData::ConceptType>(atoi(val.c_str()));
#ifdef DEBUG_LP
      LDEBUG << "read concept type " << key.c_str() << " -> " << type;
#endif
      m_conceptTypes[key] = type;
      m_conceptNames[type] = key;
    }

  }
  catch (NoSuchMap& e)
  {
    MDATALOGINIT;
    LERROR << "No map 'conceptTypes' in configuration file:" << e.what();
    throw InvalidConfiguration();
  }
}

Lima::Common::MediaticData::ConceptType CompoundsBuilderFromSyntacticData::getConceptType(const std::string& typeName) const
{
  if (m_d->m_conceptTypes.find(typeName)==m_d->m_conceptTypes.end())
  {
    MDATALOGINIT;
    LERROR << "Concept type name " << typeName.c_str() << " not found. Returning value for LatticeDown (should be '0').";
        return (*(m_d->m_conceptTypes.find("LatticeDown"))).second;
  }
  else
  {
    return (*(m_d->m_conceptTypes.find(typeName))).second;
  }
}


const std::string& CompoundsBuilderFromSyntacticData::getConceptName(const Lima::Common::MediaticData::ConceptType& type) const
{
  if (m_d->m_conceptNames.find(type)==m_d->m_conceptNames.end())
  {
    MDATALOGINIT;
    LERROR << "Concept type " << type << " not found. Returning for 0 (should be 'LatticeDown').";
    return (*(m_d->m_conceptNames.find(static_cast<Lima::Common::MediaticData::ConceptType>(0)))).second;
  }
  else
  {
    return (*(m_d->m_conceptNames.find(type))).second;
  }
}

void CompoundsBuilderFromSyntacticDataPrivate::initMacros2ConceptsMapping(
        Common::XMLConfigurationFiles::GroupConfigurationStructure& macro2ConceptsGroup)
{
#ifdef DEBUG_LP
  LDATALOGINIT;
  LDEBUG << "Initializing macro to concepts mapping:";
#endif
  try
  {
    const std::map<std::string,std::string>& mapping = macro2ConceptsGroup.getMapAtKey("Macros2ConceptTypes");

    const Lima::Common::MediaticData::LanguageData& languageData = static_cast<const Lima::Common::MediaticData::LanguageData&>(Lima::Common::MediaticData::MediaticData::single().mediaData(m_language));
    const Lima::Common::PropertyCode::PropertyCodeManager& propCodeManager = languageData.getPropertyCodeManager();

    const Lima::Common::PropertyCode::PropertyManager& macroManager =
        propCodeManager.getPropertyManager("MACRO");

    for(auto it = mapping.begin(), it_end = mapping.end(); it != it_end; it++)
    {
      LinguisticCode macro = macroManager.getPropertyValue((*it).first);
      std::string typeName = (*it).second;
      Lima::Common::MediaticData::ConceptType concept = (*(m_conceptTypes.find("LatticeDown"))).second;
      if (m_conceptTypes.find(typeName) == m_conceptTypes.end())
      {
        MDATALOGINIT;
        LERROR << "Concept type name " << typeName.c_str() << " not found. Keeping LatticeDown (should be '0').";
      }
      else
      {
        concept = (*(m_conceptTypes.find(typeName))).second;
      }

#ifdef DEBUG_LP
      LDEBUG << "    " << (*it).first << " (" << macro << ") <-> "
          << (*it).second << " (" << concept << ")";
#endif
      m_macro2ConceptMapping.insert(std::make_pair(macro,concept));
    }

  }
  catch (NoSuchMap& )
  {
    MDATALOGINIT;
    LERROR << "No map 'Macros2ConceptTypes' for group 'SemanticData' in common "
        "language configuration file for language " << m_language;
    throw InvalidConfiguration();
  }
  catch (NoSuchGroup& )
  {
    MDATALOGINIT;
    LERROR << "No group 'SemanticData' in common language configuration file "
        "for language " << m_language;
    throw InvalidConfiguration();
  }
}


Lima::Common::MediaticData::ConceptType CompoundsBuilderFromSyntacticData::getConceptForMacro(const LinguisticCode& macro) const
{
#ifdef DEBUG_LP
  MORPHOLOGINIT;
  LDEBUG << "CompoundsBuilderFromSyntacticData::getConceptForMacro" << macro;
#endif
  const Lima::Common::MediaticData::LanguageData& languageData = static_cast<const Lima::Common::MediaticData::LanguageData&>(Lima::Common::MediaticData::MediaticData::single().mediaData(m_d->m_language));
  Lima::Common::PropertyCode::PropertyCodeManager propertycodeManager = languageData.getPropertyCodeManager();

  if (m_d->m_macro2ConceptMapping.find(macro) == m_d->m_macro2ConceptMapping.end())
  {
#ifdef DEBUG_LP
    LDATALOGINIT;
    LDEBUG << "CompoundsBuilderFromSyntacticData::getConceptForMacro" << macro << "not found. Returning for" << SYMBOLIC_NONE_1 << "(should be 0, 'LatticeDown').";
#endif
    const Lima::Common::PropertyCode::PropertyManager& macroManager = propertycodeManager.getPropertyManager("MACRO");
    LinguisticCode none = macroManager.getPropertyValue(SYMBOLIC_NONE_1);
    if (m_d->m_macro2ConceptMapping.find(none) == m_d->m_macro2ConceptMapping.end())
    {
#ifdef DEBUG_LP
      LDATALOGINIT;
      LDEBUG << "SYMBOLIC_NONE_1 Not found !!! Returning 0";
#endif
      return static_cast<Lima::Common::MediaticData::ConceptType>(0);
    }
    else
    {
#ifdef DEBUG_LP
      LDEBUG << "SYMBOLIC_NONE_1 Found !!! Returning " << (*(m_d->m_macro2ConceptMapping.find(none))).second;
#endif
      return (*(m_d->m_macro2ConceptMapping.find(none))).second;
    }
  }
  else
  {
#ifdef DEBUG_LP
    LDEBUG << "CompoundsBuilderFromSyntacticData::getConceptForMacro Found !!! Returning " << (*(m_d->m_macro2ConceptMapping.find(macro))).second;
#endif
    return (*(m_d->m_macro2ConceptMapping.find(macro))).second;
  }

}

LimaStatusCode CompoundsBuilderFromSyntacticData::process(
  AnalysisContent& analysis) const
{
  Lima::TimeUtilsController timer("CompoundsBuilderFromSyntacticData");
  MORPHOLOGINIT;
#ifdef DEBUG_LP
  LDEBUG << "CompoundsBuilderFromSyntacticData::process : start";
#endif

  auto tmp=analysis.getData("SyntacticData");
  if (tmp==0)
  {
    LERROR << "Can't Process CompoundsBuilderFromSyntacticData : missing data 'SyntacticData'";
    return MISSING_DATA;
  }
  auto data = std::dynamic_pointer_cast<SyntacticData>(tmp);

  tmp = analysis.getData("PosGraph");
  if (tmp==0)
  {
    LERROR << "Can't Process CompoundsBuilderFromSyntacticData : missing data 'AnalysisGraph'";
    return MISSING_DATA;
  }
  auto anagraph = std::dynamic_pointer_cast<AnalysisGraph>(tmp);

  tmp=analysis.getData("SentenceBoundaries");
  if (tmp==0)
  {
    LERROR << "can't process CompoundsBuilderFromSyntacticData : missing SentenceBounds !";
    return MISSING_DATA;
  }
  auto sb = std::dynamic_pointer_cast<SegmentationData>(tmp);
  if (sb->getGraphId() != "PosGraph") {
    LERROR << "SentenceBounds have been computed on " << sb->getGraphId() << " !";
    LERROR << "SyntacticAnalyzer-deps needs SentenceBounds on PosGraph";
    return INVALID_CONFIGURATION;
  }
  auto annotationData = std::dynamic_pointer_cast< AnnotationData >(analysis.getData("AnnotationData"));
  if (annotationData==0)
  {
    annotationData = std::make_shared<AnnotationData>();
    if (std::dynamic_pointer_cast<AnalysisGraph>(analysis.getData("AnalysisGraph")) != 0)
    {
      std::dynamic_pointer_cast<AnalysisGraph>(analysis.getData("AnalysisGraph"))->populateAnnotationGraph(
        annotationData.get(), "AnalysisGraph");
    }
    if (std::dynamic_pointer_cast<AnalysisGraph>(analysis.getData("PosGraph")) != 0)
    {
      std::dynamic_pointer_cast<AnalysisGraph>(analysis.getData("PosGraph"))->populateAnnotationGraph(
        annotationData.get(), "PosGraph");
    }

    analysis.setData("AnnotationData",annotationData);
  }

  const LinguisticGraph& graph=*(anagraph->getGraph());

  //LinguisticGraphVertex beginSentence=sb->getStartVertex();
  // ??OME2 for (SegmentationData::const_iterator boundItr=sb->begin();
  //     boundItr!=sb->end();
  for (const auto& bound: sb->getSegments())
  {
    LinguisticGraphVertex beginSentence=bound.getFirstVertex();
    LinguisticGraphVertex endSentence=bound.getLastVertex();

#ifdef DEBUG_LP
    LDEBUG << "process sentence from " << beginSentence << " to " << endSentence;
 #endif
   DepGraphCompoundsBuildVisitor vis(
        this,
        m_d->m_language,
        data.get(),
        data->dependencyGraph() ,
        anagraph.get(),
        beginSentence,
        endSentence,
        annotationData.get(),
        m_d->m_useChains);

    // don't use STL algorithm to avoid throwing exceptions
    LinguisticGraphVertex firstVx = anagraph->firstVertex();
    LinguisticGraphVertex lastVx = anagraph->lastVertex();

    std::set<LinguisticGraphVertex> visited;
    std::queue<LinguisticGraphVertex> toVisit;
    toVisit.push(beginSentence);

    LinguisticGraphOutEdgeIt outItr,outItrEnd;
    while (!toVisit.empty())
    {
      LinguisticGraphVertex v=toVisit.front();
      toVisit.pop();

      if (v != firstVx &&
          v != lastVx)
      {
        vis.discover_vertex(v);
      }

      if (v == endSentence) {
        continue;
      }

      for (boost::tie(outItr,outItrEnd)=out_edges(v,graph);
          outItr!=outItrEnd;
          outItr++)
      {
        LinguisticGraphVertex next=target(*outItr,graph);
        if (visited.find(next)==visited.end())
        {
          visited.insert(next);
          toVisit.push(next);
        }
      }
    }

//     LDEBUG << compoundsItr->size() << " compounds built ! ";

    //beginSentence=*boundItr;
  }
  return SUCCESS_ID;
}


}

}

}
