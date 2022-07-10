// Copyright 2002-2020 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2020 by CEA LIST                                   *
 *                                                                         *
 ***************************************************************************/

#include "BowGeneration.h"
#include "common/time/traceUtils.h"
#include "common/Data/strwstrtools.h"
#include "common/Data/genericDocumentProperties.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/MediaticData/mediaticData.h"
#include "common/MediaticData/EntityType.h"
#include "linguisticProcessing/common/BagOfWords/bowToken.h"
#include "linguisticProcessing/common/BagOfWords/BoWRelation.h"
#include "linguisticProcessing/common/BagOfWords/bowNamedEntity.h"
#include "linguisticProcessing/common/BagOfWords/bowTerm.h"
#include "linguisticProcessing/common/BagOfWords/bowText.h"
#include "linguisticProcessing/common/BagOfWords/bowDocument.h"
#include "linguisticProcessing/common/BagOfWords/BoWPredicate.h"
#include "linguisticProcessing/common/linguisticData/languageData.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/core/Automaton/recognizerMatch.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/TextSegmentation/SegmentationData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/Token.h"
#include "linguisticProcessing/core/SyntacticAnalysis/SyntacticData.h"
#include "linguisticProcessing/core/Automaton/SpecificEntityAnnotation.h"
#include "linguisticProcessing/core/Compounds/CompoundTokenAnnotation.h"
#include "linguisticProcessing/core/SemanticAnalysis/SemanticRelationAnnotation.h"

#include <boost/graph/properties.hpp>

#include <fstream>
#include <deque>
#include <iostream>
#include <QStringList>


using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::BagOfWords;
using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::LinguisticProcessing::Automaton;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
// using namespace Lima::LinguisticProcessing::Compounds;
using namespace Lima::LinguisticProcessing::SpecificEntities;
using namespace Lima::LinguisticProcessing::SyntacticAnalysis;
using namespace Lima::LinguisticProcessing::AnalysisDumpers;
using namespace Lima::LinguisticProcessing::SemanticAnalysis;

namespace Lima
{

namespace LinguisticProcessing
{

namespace Compounds
{

  struct A
  {
    bool operator()(const std::pair<boost::shared_ptr< Common::BagOfWords::BoWRelation >, boost::shared_ptr< Common::BagOfWords::BoWToken > > t1, const std::pair<boost::shared_ptr< Common::BagOfWords::BoWRelation >, boost::shared_ptr< Common::BagOfWords::BoWToken > > t2) const
    {
      return (t1.second->getPosition()<t2.second->getPosition()) ;
    }
  };


class LIMA_ANALYSISDUMPERS_EXPORT BowGeneratorPrivate
{
  friend class BowGenerator;

  BowGeneratorPrivate();

  ~BowGeneratorPrivate() = default;
  BowGeneratorPrivate(const BowGeneratorPrivate&) = delete;
  BowGeneratorPrivate& operator=(const BowGeneratorPrivate&) = delete;

  std::vector< std::pair< boost::shared_ptr< Common::BagOfWords::BoWRelation >,
               boost::shared_ptr< Common::BagOfWords::AbstractBoWElement > > >
      createAbstractBoWElement(
    const LinguisticGraphVertex v,
    const LinguisticGraph& anagraph,
    const LinguisticGraph& posgraph,
    const uint64_t offsetBegin,
    const Common::AnnotationGraphs::AnnotationData* annotationData,
    std::set<LinguisticGraphVertex>& visited,
    bool keepAnyway = false) const;

  boost::shared_ptr< Common::BagOfWords::BoWRelation > createBoWRelationFor(
    const AnnotationGraphVertex& vx,
    const AnnotationGraphVertex& tgt,
    const Common::AnnotationGraphs::AnnotationData* annotationData,
    const LinguisticGraph& posgraph,
    const SyntacticAnalysis::SyntacticData* syntacticData) const;

  class NamedEntityPart
  {
    public:
      NamedEntityPart(): inflectedForm(), lemma(), position(0),
      length(0) {}
      NamedEntityPart(const LimaString& fl, const LimaString& l,
                      const LinguisticCode cat, const uint64_t pos,
                      const uint64_t len):
          inflectedForm(fl), lemma(l), category(cat), position (pos),
          length(len) {}

      LimaString inflectedForm;
      LimaString lemma;
      LinguisticCode category;
      uint64_t position;
      uint64_t length;
  };

  typedef std::set< std::pair<uint64_t,uint64_t> > TokenPositions;

  MediaId m_language;
  AnalysisDumpers::StopList* m_stopList;
  bool m_useStopList;
  bool m_useEmptyMacro;
  bool m_useEmptyMicro;
  LinguisticCode m_properNounCategory;
  LinguisticCode m_commonNounCategory;
  bool m_keepAllNamedEntityParts;
  const Common::PropertyCode::PropertyAccessor* m_macroAccessor;
  const Common::PropertyCode::PropertyAccessor* m_microAccessor;

  // what to assign to the BoWNamedEntity lemma
  typedef enum {
    NORMALIZE_NE_INFLECTED, // assign inflected form
    NORMALIZE_NE_LEMMA,     // assign lemma
    NORMALIZE_NE_NORMALIZEDFORM, // assign normalized form from NE
    NORMALIZE_NE_NETYPE // assign type of NE
  } NENormalization;
  NENormalization m_NEnormalization;

  boost::shared_ptr< Common::BagOfWords::BoWNamedEntity > createSpecificEntity(
    const LinguisticGraphVertex& vertex,
    const AnnotationGraphVertex& v,
    const Common::AnnotationGraphs::AnnotationData* annotationData,
    const LinguisticGraph& anagraph,
    const LinguisticGraph& posgraph,
    const uint64_t offset,
    bool frompos = true) const;

  boost::shared_ptr< Common::BagOfWords::BoWToken > createCompoundTense(
    const AnnotationGraphVertex& v,
    const Common::AnnotationGraphs::AnnotationData* annotationData,
    const LinguisticGraph& anagraph,
    const LinguisticGraph& posgraph,
    const uint64_t offset,
    std::set<LinguisticGraphVertex>& visited) const;

//   Common::BagOfWords::BoWPredicate* createPredicate(const Common::MediaticData::EntityType& t, QMultiMap<Common::MediaticData::EntityType, Common::BagOfWords::AbstractBoWElement*> roles) const;

  /**
   * Creates the BoWPredicates corresponding to SRL predicates
   */
  QList< boost::shared_ptr< Common::BagOfWords::BoWPredicate > > createPredicate(
    const LinguisticGraphVertex& lgv,
    const AnnotationGraphVertex& agv,
    const Common::AnnotationGraphs::AnnotationData* annotationData,
    const LinguisticGraph& anagraph,
    const LinguisticGraph& posgraph,
    const uint64_t offset,
    std::set<LinguisticGraphVertex>& visited,
    bool keepAnyway)const;

    bool checkStopWordInCompound(
    boost::shared_ptr< Common::BagOfWords::BoWToken>&,
    uint64_t offset,
    std::set< std::string >& alreadyStored,
    Common::BagOfWords::BoWText& bowText) const;

  StringsPoolIndex getNamedEntityNormalization(
      const AnnotationGraphVertex& v,
      const Common::AnnotationGraphs::AnnotationData* annotationData) const;

  bool shouldBeKept(const LinguisticAnalysisStructure::LinguisticElement& elem) const;

  /**
     * create the parts of a named entity (depends on its type,
     * but may be independant from its components, that can be
     * erroneously tagged)
     *
     * @param v the annotation graph vertex handling the named entity
     *
     * @return a list of the parts to consider, composed of an inflected
     * form, a lemma and a category
   */
  std::vector<NamedEntityPart> createNEParts(
      const AnnotationGraphVertex& v,
      const Common::AnnotationGraphs::AnnotationData* annotationData,
      const LinguisticGraph& anagraph,
      const LinguisticGraph& posgraph,
      bool frompos = true) const;

  void bowTokenPositions(TokenPositions& res,
                         const boost::shared_ptr< Common::BagOfWords::BoWToken > tok) const;

  uint64_t computeCompoundLength(const TokenPositions& headTokPositions,
                                const TokenPositions& extensionPositions) const;
};

BowGeneratorPrivate::BowGeneratorPrivate():
    m_language(0),
    m_stopList(0),
    m_useStopList(true),
    m_useEmptyMacro(true),
    m_useEmptyMicro(true),
    m_keepAllNamedEntityParts(false),
    m_macroAccessor(0),
    m_microAccessor(0),
    m_NEnormalization(NORMALIZE_NE_INFLECTED)
{
}

BowGenerator::BowGenerator():
    m_d(new BowGeneratorPrivate())
{
}

BowGenerator::~BowGenerator()
{
  delete m_d;
}

void BowGenerator::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  MediaId language)
{
  DUMPERLOGINIT;
  m_d->m_language=language;
  m_d->m_macroAccessor=&static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_d->m_language)).getPropertyCodeManager().getPropertyAccessor("MACRO");
  m_d->m_microAccessor=&static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_d->m_language)).getPropertyCodeManager().getPropertyAccessor("MICRO");
  try
  {
    auto use = unitConfiguration.getParamsValueAtKey("useStopList");
    m_d->m_useStopList = (use=="true");
  }
  catch (NoSuchParam& )
  {
    LWARN << "No param 'useStopList' in"<<unitConfiguration.getName()
          << "configuration group for language " << (int)m_d->m_language;
    LWARN << "use default value : true";
  }
  if (m_d->m_useStopList)
  {
    try
    {
      auto stoplist = unitConfiguration.getParamsValueAtKey("stopList");
      m_d->m_stopList = static_cast<StopList*>(LinguisticResources::single().getResource(m_d->m_language, stoplist));
#ifdef DEBUG_LP
      LDEBUG << "BowGenerator.init(): STOPLIST:";
      for(const auto& word: *m_d->m_stopList)
      {
        LDEBUG << "BowGenerator.init(): " << word;
      }
#endif
    }
    catch (NoSuchParam& )
    {
        LWARN << "No param 'stopList' in" << unitConfiguration.getName()
              << "configuration group for language "
              << (int)m_d->m_language;
//       throw InvalidConfiguration();
    }
  }
  try
  {
    auto use = unitConfiguration.getParamsValueAtKey("useEmptyMacro");
    m_d->m_useEmptyMacro = (use=="true");
  }
  catch (NoSuchParam& )
  {
    LWARN << "No param 'useEmptyMacro' in" << unitConfiguration.getName()
          << "configuration group for language " << (int)m_d->m_language;
    LWARN << "use default value : true";
  }
  try
  {
    auto use = unitConfiguration.getParamsValueAtKey("useEmptyMicro");
    m_d->m_useEmptyMicro = (use=="true");
  }
  catch (NoSuchParam& )
  {
    LWARN << "No param 'useEmptyMicro' in" << unitConfiguration.getName()
          << "configuration group for language " << (int)m_d->m_language;
    LWARN << "use default value : true";
  }
  try
  {
    auto np = unitConfiguration.getParamsValueAtKey("properNounCategory");
    m_d->m_properNounCategory = static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_d->m_language)).getPropertyCodeManager().getPropertyManager("MACRO").getPropertyValue(np);
  }
  catch (NoSuchParam& )
  {
    LERROR << "No param 'properNounCategory' in" << unitConfiguration.getName()
            << "configuration group for language " << (int)m_d->m_language;
//     throw InvalidConfiguration();
  }
  try
  {
    auto cn = unitConfiguration.getParamsValueAtKey("commonNounCategory");
    m_d->m_commonNounCategory = static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_d->m_language)).getPropertyCodeManager().getPropertyManager("MACRO").getPropertyValue(cn);
  }
  catch (NoSuchParam& )
  {
    LERROR << "No param 'commonNounCategory' in" << unitConfiguration.getName()
            << "configuration group for language " << (int)m_d->m_language;
//     throw InvalidConfiguration();
  }

  try
  {
    auto value = unitConfiguration.getParamsValueAtKey("keepAllNamedEntityParts");
    m_d->m_keepAllNamedEntityParts = (value == "yes" || value == "true" || value == "1");
  }
  catch (NoSuchParam& ) { /* optional */ }

  try
  {
    auto value = unitConfiguration.getParamsValueAtKey("NEnormalization");
    if (value == "useInflectedForm")
    {
      m_d->m_NEnormalization = BowGeneratorPrivate::NORMALIZE_NE_INFLECTED;
    }
    else if (value == "useLemma")
    {
      m_d->m_NEnormalization = BowGeneratorPrivate::NORMALIZE_NE_LEMMA;
    }
    else if (value == "useNENormalizedForm")
    {
      m_d->m_NEnormalization = BowGeneratorPrivate::NORMALIZE_NE_NORMALIZEDFORM;
    }
    else if (value == "useNEType")
    {
      m_d->m_NEnormalization = BowGeneratorPrivate::NORMALIZE_NE_NETYPE;
    }
  }
  catch (NoSuchParam& ) { /* optional */ }
}


std::vector< std::pair< boost::shared_ptr< BoWRelation >, boost::shared_ptr< BoWToken > > > BowGenerator::buildTermFor(
      const AnnotationGraphVertex& vx,
      const AnnotationGraphVertex& tgt,
      const LinguisticGraph& anagraph,
      const LinguisticGraph& posgraph,
      const uint64_t offset,
      const SyntacticData* syntacticData,
      const AnnotationData* annotationData,
      std::set< LinguisticGraphVertex >& visited) const
{

#ifdef DEBUG_LP
  DUMPERLOGINIT;
  LDEBUG << "BowGenerator::buildTermFor annot:" << vx << "; pointing on annot:"<<tgt;
#endif

  LinguisticGraphVertex vxTokVertex =
      *(annotationData->matches("cpd", vx, "PosGraph").begin());

  // recuperation des noeuds tetes de relations pointant sur vx
  std::vector<AnnotationGraphVertex> vxGovernors;
  AnnotationGraphInEdgeIt inIt, inIt_end;
  boost::tie(inIt, inIt_end) = boost::in_edges(vx, annotationData->getGraph());
  for (; inIt != inIt_end; inIt++)
  {
    vxGovernors.push_back(source(*inIt, annotationData->getGraph()));
  }

  auto vxBoWTokens = createAbstractBoWElement(vxTokVertex, anagraph, posgraph,
                                              offset, annotationData, visited);

// #ifdef DEBUG_LP
//   LDEBUG << "BowGenerator::buildTermFor, line"<<__LINE__<<","<<vx<<vxTokVertex<<" There is " << vxBoWTokens.size() << " bow tokens";
// #endif
  if (vxGovernors.empty())
  {

    std::vector< std::pair<boost::shared_ptr< BoWRelation >,
                           boost::shared_ptr< BoWToken > > > vxBoWTk;
// #ifdef DEBUG_LP
//     LDEBUG << "BowGenerator::buildTermFor empty governors ";
// #endif
    auto relation = m_d->createBoWRelationFor(vx, tgt, annotationData,
                                              posgraph, syntacticData);

    if (relation)
    {
      for (auto& vxBoWToken: vxBoWTokens)
      {
        vxBoWToken.first = relation;
      }
    }
    for (auto& vxBoWToken: vxBoWTokens)
    {
      if (boost::dynamic_pointer_cast<BoWToken>(vxBoWToken.second) != 0)
      {
        vxBoWTk.push_back(std::make_pair(vxBoWToken.first,
                                         boost::dynamic_pointer_cast<BoWToken>(vxBoWToken.second)));
      }
      else
      {
        DUMPERLOGINIT;
        LERROR << "BowGenerator::buildTermFor vxBoWTokensIt is not a BoWToken";
      }
    }
// #ifdef DEBUG_LP
//     LDEBUG << "BowGenerator::buildTermFor == DONE buildTermFor " << vx << " (pointing on "<<tgt<<"):empty governors ";
//     LDEBUG << "BowGenerator::buildTermFor return result of size" << vxBoWTk.size();
// #endif
    return vxBoWTk;
  }

  std::vector< std::pair< boost::shared_ptr< BoWRelation >, boost::shared_ptr< BoWToken > > > result;
  std::vector< std::vector< std::pair< boost::shared_ptr< BoWRelation > , boost::shared_ptr< BoWToken > > > > termsForVxGovernors;
  for (const auto& gov: vxGovernors)
  {
    auto pairs = buildTermFor(gov, vx, anagraph, posgraph, offset,
                              syntacticData, annotationData, visited);
    termsForVxGovernors.push_back(pairs);
// #ifdef DEBUG_LP
//     LDEBUG << "BowGenerator::buildTermFor"<<vx<<vxTokVertex<<" For governor "
//            << i <<*govsIt<< ", there is " << pairs.size() << " terms.";
// #endif
  }


  std::vector< std::vector< std::pair< boost::shared_ptr< BoWRelation >, boost::shared_ptr< BoWToken > > >::iterator > begins;
  std::vector< std::vector< std::pair< boost::shared_ptr< BoWRelation >, boost::shared_ptr< BoWToken > > >::iterator > ends;
  std::vector< std::vector< std::pair< boost::shared_ptr< BoWRelation >, boost::shared_ptr< BoWToken > > >::iterator > stack;
  for (auto termsForVxGovernorsIt = termsForVxGovernors.begin();
       termsForVxGovernorsIt != termsForVxGovernors.end();
       termsForVxGovernorsIt++)
  {
    if (!(*termsForVxGovernorsIt).empty())
    {
      begins.push_back((*termsForVxGovernorsIt).begin());
      ends.push_back((*termsForVxGovernorsIt).end());
      stack.push_back((*termsForVxGovernorsIt).begin());
    }
  }

  if (stack.empty())
  {
// #ifdef DEBUG_LP
//     LDEBUG << "BowGenerator::buildTermFor Stack is empty ! Returning bow tokens of " << vxTokVertex;
//     LDEBUG << "BowGenerator::buildTermFor == DONE buildTermFor " << vx << " (pointing on "<<tgt<<"):stack governors ";
// #endif
    auto relation = m_d->createBoWRelationFor(vx, tgt, annotationData,
                                              posgraph, syntacticData);
    if (relation)
    {
      for (auto& vxBoWToken: vxBoWTokens)
      {
        vxBoWToken.first = relation;
      }
    }
    std::vector< std::pair< boost::shared_ptr< BoWRelation >, boost::shared_ptr< BoWToken > > > vxBoWTk;
    auto vxBoWTokensIt = vxBoWTokens.begin(), vxBoWTokensIt_end = vxBoWTokens.end();
    for (; vxBoWTokensIt != vxBoWTokensIt_end; vxBoWTokensIt++)
    {
      if (boost::dynamic_pointer_cast<BoWToken>((*vxBoWTokensIt).second) != 0)
      {
        vxBoWTk.push_back(std::make_pair((*vxBoWTokensIt).first,boost::dynamic_pointer_cast<BoWToken>((*vxBoWTokensIt).second)));
      }
      else
      {
        DUMPERLOGINIT;
        LERROR << "BowGenerator::buildTermFor vxBoWTokensIt is not a BoWToken";
      }
    }
// #ifdef DEBUG_LP
//     LDEBUG << "BowGenerator::buildTermFor return result of size" << vxBoWTk.size();
// #endif
    return vxBoWTk;
  }
  std::vector< std::pair< boost::shared_ptr< BoWRelation >, boost::shared_ptr< BoWToken > > >::iterator t;
  while (!stack.empty())
  {
// #ifdef DEBUG_LP
//     LDEBUG << "BowGenerator::buildTermFor There is " << vxBoWTokens.size() << " heads, " << vxGovernors.size() << " governors and stack size is " << stack.size();
// #endif
    for (auto vxBoWToken=vxBoWTokens.begin();
         vxBoWToken!=vxBoWTokens.end();
         vxBoWToken++)
    {
      boost::shared_ptr< BoWToken > head = boost::dynamic_pointer_cast<BoWToken>((*vxBoWToken).second);
      if (head == 0)
      {
        DUMPERLOGINIT;
        LERROR << "BowGenerator::buildTermFor head" << &*(*vxBoWToken).second << "is not a BoWToken";
        continue;
      }
// #ifdef DEBUG_LP
//       LDEBUG << "BowGenerator::buildTermFor Working on head " << *head << "(" << *head << ")";
// #endif

      std::set< std::pair< boost::shared_ptr< BoWRelation >, boost::shared_ptr< BoWToken > >, A > extensions;
      auto govsIt = stack.begin(), govsIt_end = stack.end();
      for (; govsIt != govsIt_end; govsIt++)
      {
// #ifdef DEBUG_LP
//         LDEBUG << "BowGenerator::buildTermFor Entering loop body";
// #endif
        boost::shared_ptr< BoWRelation > relation( (**govsIt).first);
        boost::shared_ptr< BoWToken > bt = (**govsIt).second;
// #ifdef DEBUG_LP
//         LDEBUG << "BowGenerator::buildTermFor     ... done.";
// #endif
//         LDEBUG << "BowGenerator: Inserting extension...";
        extensions.insert(std::make_pair(relation,bt));
//         LDEBUG << "BowGenerator:     ... done.";
      }

      LimaString lemma;
      LimaString infl;
      uint64_t position=0;
      uint64_t length=0;
      BowGeneratorPrivate::TokenPositions headPositions;
      m_d->bowTokenPositions(headPositions, head);
      BowGeneratorPrivate::TokenPositions extensionPositions;

// #ifdef DEBUG_LP
//       LDEBUG << "BowGenerator::buildTermFor Working on extensions";
// #endif
      auto extensionsIt = extensions.begin(),
      extensionsIt_end = extensions.end();
      for (; extensionsIt != extensionsIt_end; extensionsIt++)
      {
        boost::shared_ptr< BoWToken > extension = (*extensionsIt).second;
// #ifdef DEBUG_LP
//         LDEBUG << "BowGenerator::buildTermFor     extension: " << *extension;
//         LDEBUG << "BowGenerator::buildTermFor     extension: " << ((boost::dynamic_pointer_cast< BoWTerm >(extension)==0)?(*extension):(*(boost::dynamic_pointer_cast< BoWTerm >(extension))));
// #endif
        m_d->bowTokenPositions(extensionPositions, extension);
      }

// #ifdef DEBUG_LP
//       LDEBUG << "BowGenerator::buildTermFor Building term";
// #endif
      // position is the min of head min position and extension min position
      position=headPositions.begin()->first;
      if (position > extensionPositions.begin()->first)
      {
        position = extensionPositions.begin()->first;
      }
// #ifdef DEBUG_LP
//       LDEBUG << "BowGenerator::buildTermFor     position: " << position;
// #endif

        // length is the length in original text: take end of term
      length=m_d->computeCompoundLength(headPositions,extensionPositions);
// #ifdef DEBUG_LP
//       LDEBUG << "BowGenerator::buildTermFor     length  : " << length;
// #endif

      boost::shared_ptr< BoWTerm > complex( new BoWTerm( lemma, head->getCategory(), position, length) );
      complex->setVertex(head->getVertex());
      complex->setInflectedForm(infl);
      complex->setCategory(head->getCategory());
      complex->addPart(head);
//       delete head; head = 0;

      extensionsIt = extensions.begin();
      extensionsIt_end = extensions.end();
      for (; extensionsIt != extensionsIt_end; extensionsIt++)
      {
        boost::shared_ptr< BoWToken > extension = (*extensionsIt).second;
// #ifdef DEBUG_LP
//         LDEBUG << "BowGenerator::buildTermFor     extension: " << ((boost::dynamic_pointer_cast< BoWTerm >(extension)==0)?(*extension):(*(boost::dynamic_pointer_cast< BoWTerm >(extension))));
// #endif
        if ((*extensionsIt).first == 0)
          complex->addPart(extension);
        else
          complex->addPart((*extensionsIt).first,extension);
//         LDEBUG << "Built complex: " << ((dynamic_cast< BoWTerm* >(complex)==0)?(*complex):(*(dynamic_cast< BoWTerm* >(complex))));
// #ifdef DEBUG_LP
//         LDEBUG << "BowGenerator::buildTermFor Built complex: " << *complex;
// #endif
      }

      auto relation = m_d->createBoWRelationFor(vx, tgt, annotationData,
                                                posgraph, syntacticData);

// #ifdef DEBUG_LP
//       LDEBUG << "BowGenerator::buildTermFor Filling result with: " << *complex;
// #endif
      result.push_back(std::make_pair(relation,complex));
    }

    // Mise a joueur de la pile d'iterateurs pour produire une nouvelle serie d'extensions
// #ifdef DEBUG_LP
//     LDEBUG << "BowGenerator::buildTermFor Stack updating...";
// #endif
    {
      t = *stack.rbegin();
      t++;
      while ( t == ends[stack.size()-1] )
      {
        stack.pop_back();
        if (stack.empty())
        {
          break;
        }
        t = *stack.rbegin();
        t++;
      }
      if (!stack.empty())
      {
        stack.pop_back();
        stack.push_back(t);
// #ifdef DEBUG_LP
//         LDEBUG << "BowGenerator::buildTermFor Stack filling...";
// #endif
        for (uint64_t i = stack.size(); i < begins.size();i++)
        {
          stack.push_back(begins[i]);
        }
      }
    }
  }

// #ifdef DEBUG_LP
//   LDEBUG << "BowGenerator::buildTermFor == DONE" << vx << "(pointing on" << tgt << ")";
//   LDEBUG << "BowGenerator::buildTermFor, line"<<__LINE__<<", return result of size" << result.size();
// #endif
  return result;
}

boost::shared_ptr< BoWRelation > BowGeneratorPrivate::createBoWRelationFor(
    const AnnotationGraphVertex& vx,
    const AnnotationGraphVertex& tgt,
    const AnnotationData* annotationData,
    const LinguisticGraph& posgraph,
    const SyntacticData* syntacticData) const
{
  LIMA_UNUSED(posgraph);
  const DependencyGraph* depGraph = syntacticData->dependencyGraph();
#ifdef DEBUG_LP
  DUMPERLOGINIT;
  LDEBUG << "BowGenerator::createBoWRelationFor" << vx << tgt;
#endif
  boost::shared_ptr<BoWRelation> relation;
  if (vx != tgt
      && annotationData->hasAnnotation(vx, tgt,
          Common::Misc::utf8stdstring2limastring("CompoundTokenAnnotation")) )
  {
#ifdef DEBUG_LP
    LDEBUG << "BowGenerator:     working on relation";
#endif
    const CompoundTokenAnnotation* annot = annotationData->annotation(vx,tgt, Common::Misc::utf8stdstring2limastring("CompoundTokenAnnotation")).pointerValue<CompoundTokenAnnotation>();
    if (annot != 0 && !annot->empty())
    {
      const ConceptModifier& modifier = (*annot)[0];
      StringsPoolIndex realizationIdx = modifier.getRealization();
      LimaString realization = Common::MediaticData::MediaticData::changeable().stringsPool(m_language)[realizationIdx];
      int type = modifier.getConceptType();
      relation = boost::shared_ptr< BoWRelation >(new BoWRelation(realization, type));
    }
    else
    {
      relation = boost::shared_ptr< BoWRelation >(new BoWRelation());
    }
    LinguisticGraphVertex vxTokVertex = *(annotationData->matches("cpd", vx, "PosGraph").begin());
    LinguisticGraphVertex tgtTokVertex = *(annotationData->matches("cpd", tgt, "PosGraph").begin());
#ifdef DEBUG_LP
    LDEBUG << "BowGenerator:     working vx  " << vxTokVertex;
    LDEBUG << "BowGenerator:     working tgt  " << tgtTokVertex;
#endif
    DependencyGraphVertex depV = syntacticData->depVertexForTokenVertex(vxTokVertex);
    if (out_degree(depV, *depGraph) > 0){
      DependencyGraphOutEdgeIt depIt, depIt_end;
      boost::tie(depIt, depIt_end) = out_edges(depV, *depGraph);
      for (; depIt != depIt_end; depIt++)
      {
        DependencyGraphVertex depTargV = target(*depIt, *depGraph);
        LinguisticGraphVertex targV = syntacticData-> tokenVertexForDepVertex(depTargV);
        if (targV == tgtTokVertex){
            CEdgeDepRelTypePropertyMap relTypeMap = get(edge_deprel_type, *depGraph);
            relation->setSynType(relTypeMap[*depIt]);
        }
      }
    }
  }
#ifdef DEBUG_LP
  if (relation !=0)
  {
    LDEBUG << "BowGenerator:     relation : " << *relation;
  }
#endif
  return relation;
}


std::vector< std::pair< boost::shared_ptr< BoWRelation >,
             boost::shared_ptr< AbstractBoWElement > > >
    BowGenerator::createAbstractBoWElement(
  const LinguisticGraphVertex v,
  const LinguisticGraph& anagraph,
  const LinguisticGraph& posgraph,
  const uint64_t offsetBegin,
  const AnnotationData* annotationData,
  std::set<LinguisticGraphVertex>& visited,
  bool keepAnyway) const
{
  return m_d->createAbstractBoWElement(v, anagraph, posgraph, offsetBegin,
                                       annotationData, visited, keepAnyway);
}

std::vector< std::pair< boost::shared_ptr< BoWRelation >,
              boost::shared_ptr< AbstractBoWElement > > >
    BowGeneratorPrivate::createAbstractBoWElement(
  const LinguisticGraphVertex v,
  const LinguisticGraph& anagraph,
  const LinguisticGraph& posgraph,
  const uint64_t offsetBegin,
  const AnnotationData* annotationData,
  std::set<LinguisticGraphVertex>& visited,
  bool keepAnyway) const
{
#ifdef DEBUG_LP
  DUMPERLOGINIT;
  LDEBUG << "BowGenerator::createAbstractBoWElement for " << v;
#endif
  std::vector<std::pair< boost::shared_ptr< BoWRelation >, boost::shared_ptr< AbstractBoWElement > > > abstractBowEl;
  // Create bow tokens for specific entities created on the before PoS tagging
  // analysis graph
  //std::set< uint64_t > anaVertices = annotationData->matches("PosGraph",v,"AnalysisGraph"); portage 32 64
  std::set< AnnotationGraphVertex > anaVertices = annotationData->matches("PosGraph",v,"AnalysisGraph");
#ifdef DEBUG_LP
  LDEBUG << "BowGenerator::createAbstractBoWElement " << v << " has " << anaVertices.size() << " matching vertices in analysis graph";
#endif

  bool createdSpecificEntity(false);

  // note: anaVertices size should be 0 or 1
  for (auto anaVertex = anaVertices.begin(); anaVertex != anaVertices.end(); ++anaVertex)
  {
#ifdef DEBUG_LP
   LDEBUG << "BowGenerator::createAbstractBoWElement Looking at analysis graph vertex " << *anaVertex;
#endif
    std::set< AnnotationGraphVertex > matches = annotationData->matches("AnalysisGraph",*anaVertex,"annot");
    for (auto matchVertex = matches.begin(); matchVertex != matches.end(); ++matchVertex)
    {
#ifdef DEBUG_LP
      LDEBUG << "BowGenerator::createAbstractBoWElement Looking at annotation graph vertex " << *matchVertex;
#endif
      if (annotationData->hasAnnotation(*matchVertex, Common::Misc::utf8stdstring2limastring("SpecificEntity")))
      {
        // corresponding vertex from analysis graph 
        LinguisticGraphVertex matchv= annotationData->intAnnotation(*matchVertex,"AnalysisGraph");
        boost::shared_ptr< BoWToken  > se = createSpecificEntity(matchv,*matchVertex, annotationData, anagraph, posgraph, offsetBegin, false);
        //boost::shared_ptr< BoWToken  > se = createSpecificEntity(v,*matchVertex, annotationData, anagraph, posgraph, offsetBegin, false);
        if (se != 0)
        {
#ifdef DEBUG_LP
          LDEBUG << "BowGenerator::createAbstractBoWElement created specific entity: " << QString::fromUtf8(se->getOutputUTF8String().c_str());
#endif
          se->setVertex(v);
          abstractBowEl.push_back(std::make_pair(boost::shared_ptr< BoWRelation >(),se));
//           visited.insert(v);
          createdSpecificEntity=true;
          break;
        }
      }
    }
  }
#ifdef DEBUG_LP
  LDEBUG << "BowGenerator::createAbstractBoWElement move on the PosGraph annot matching test";
#endif

  // check if there is specific entities or compound tenses associated to v.
  // return them if any
  std::set< AnnotationGraphVertex > matches = annotationData->matches("PosGraph",v,"annot");
#ifdef DEBUG_LP
  LDEBUG << "BowGenerator::createAbstractBoWElement there are " << matches.size() << " annotation graph vertices matching the current PsGraph vertex " << v;
#endif
  for (auto it = matches.begin(); it != matches.end(); ++it)
  {
    AnnotationGraphVertex vx = *it;
#ifdef DEBUG_LP
    LDEBUG << "BowGenerator::createAbstractBoWElement Looking at annotation graph vertex " << vx;
#endif
    if (annotationData->hasAnnotation(vx, Common::Misc::utf8stdstring2limastring("SpecificEntity")))
    {
      boost::shared_ptr< BoWToken > se = createSpecificEntity(v,vx, annotationData, anagraph, posgraph, offsetBegin);
      if (se != 0)
      {
#ifdef DEBUG_LP
        LDEBUG << "BowGenerator::createAbstractBoWElement created specific entity: " << QString::fromUtf8(se->getOutputUTF8String().c_str());
#endif
        se->setVertex(v);
        abstractBowEl.push_back(std::make_pair(boost::shared_ptr< BoWRelation >(),se));
//         visited.insert(v);
        return abstractBowEl;
      }
    }
    else if (annotationData->hasIntAnnotation(vx, Common::Misc::utf8stdstring2limastring("CpdTense")))
    {
      boost::shared_ptr< BoWToken > ct = createCompoundTense(vx, annotationData, anagraph, posgraph, offsetBegin, visited);
      if (ct != 0)
      {
 #ifdef DEBUG_LP
       LDEBUG << "BowGenerator::createAbstractBoWElement created compound tense: " << *ct;
#endif
        ct->setVertex(v);
        abstractBowEl.push_back(std::make_pair(boost::shared_ptr< BoWRelation >(),ct));
//         visited.insert(v);
        return abstractBowEl;
      }
    }
    else if (annotationData->hasStringAnnotation(vx, Common::Misc::utf8stdstring2limastring("Predicate")))
    {

#ifdef DEBUG_LP
      LDEBUG << "BowGenerator::createAbstractBoWElement Found a predicate in the PosGraph annnotation graph matching";
#endif

      MorphoSyntacticData* data = get(vertex_data, posgraph, v);
      bool toKeep = true;
      if (data!=0)
      {
        for (auto elem = data->begin(); elem != data->end(); ++elem)
        {
          if (!keepAnyway && !shouldBeKept(*elem))
          {
            toKeep = false;
            break;
          }
        }
      }
      if (toKeep)
      {
		auto pred = createPredicate(v, vx, annotationData, anagraph, posgraph,
                                    offsetBegin, visited, keepAnyway);
        for (auto bP = pred.begin(); bP != pred.end(); ++bP)
        {
          if (*bP!=0)
          {
  #ifdef DEBUG_LP
            LDEBUG << "BowGenerator::createAbstractBoWElement created a predicate" ;
  #endif
            abstractBowEl.push_back(std::make_pair(boost::shared_ptr< BoWRelation >(),*bP));
      //         visited.insert(v);
  //           return abstractBowEl;
          }
        }
      }
    }
    else
    {
#ifdef DEBUG_LP
      LDEBUG << "BowGenerator::createAbstractBoWElement No SpecificEntity nor CpdTense nor Predicate found";
#endif
    }
  }

  // bow tokens have been created for specific entities on the before PoS
  // tagging graph. return them
  if (!abstractBowEl.empty())
  {
//     return abstractBowEl;
  }

  const FsaStringsPool& sp=Common::MediaticData::MediaticData::single().stringsPool(m_language);

  MorphoSyntacticData* data = get(vertex_data, posgraph, v);
  Token* token = get(vertex_token, posgraph, v);

  std::set<std::pair<StringsPoolIndex,LinguisticCode> > alreadyCreated;
  std::pair<StringsPoolIndex,LinguisticCode> predNormCode = std::make_pair(StringsPoolIndex(0),L_NONE);

  if (createdSpecificEntity) {
    // a specific entity has been created on the analysis graph: do not output a token
    // (RB: do that here so that the vertex on the posgraph can also be analyzed: should test is this is
    // needed or if we only need to place the return just after the creation of the named entity)
    return abstractBowEl;
  }

  if (data!=0)
  {
    for (auto it=data->begin(); it!=data->end(); it++)
    {
      auto normCode=std::make_pair(it->normalizedForm, m_microAccessor->readValue(it->properties));
      if (normCode != predNormCode)
      {
        if (alreadyCreated.find(normCode)==alreadyCreated.end())
        {
          if (keepAnyway || shouldBeKept(*it))
          {
            boost::shared_ptr< BoWToken > newbowtok( new BoWToken(sp[it->normalizedForm],
                                               m_macroAccessor->readValue(it->properties),
                                               offsetBegin+token->position(),
                                               token->length()));
            newbowtok->setVertex(v);
            newbowtok->setInflectedForm(token->stringForm());
#ifdef DEBUG_LP
            LDEBUG << "BowGenerator::createAbstractBoWElement created bow token: " << *newbowtok;
#endif
            abstractBowEl.push_back(std::make_pair(boost::shared_ptr< BoWRelation >(),newbowtok));
          }
          alreadyCreated.insert(normCode);
        }
        predNormCode=normCode;
      }
    }
  }
/*  if (!bowTokens.empty())
  {
    visited.insert(v);
  }*/
  return abstractBowEl;
}



bool BowGeneratorPrivate::shouldBeKept(const LinguisticAnalysisStructure::LinguisticElement& elem) const
{
/*
  Critical function : comment logging messages
*/
#ifdef DEBUG_LP
  DUMPERLOGINIT;
#endif
  //  bool result = false;

  const FsaStringsPool& sp=Common::MediaticData::MediaticData::single().stringsPool(m_language);
  const LanguageData& ldata=static_cast<const LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language));
  if (m_useEmptyMacro && ldata.isAnEmptyMacroCategory(m_macroAccessor->readValue(elem.properties)))
  {
#ifdef DEBUG_LP
    LDEBUG << "BowGenerator::shouldBeKept token ("
      << sp[elem.lemma] << "|"
      << elem.properties << ") not kept : macro category is empty ";
#endif
    return false;
  }


  if (m_useEmptyMicro && ldata.isAnEmptyMicroCategory(m_microAccessor->readValue(elem.properties)))
  {
#ifdef DEBUG_LP
    LDEBUG << "BowGenerator::shouldBeKept token ("
      << sp[elem.lemma] << "|"
      << elem.properties << ") not kept : micro category is empty ";
#endif
    return false;
  }

#ifdef DEBUG_LP
   LDEBUG << "BowGenerator::shouldBeKept check token (" << sp[elem.normalizedForm] << ")";
#endif
   if (m_useStopList && m_stopList!=0 && (m_stopList->find(sp[elem.normalizedForm]) != m_stopList->end()))
   {
#ifdef DEBUG_LP
     LDEBUG << "BowGenerator::shouldBeKept token ("
       << sp[elem.lemma] << "|"
       << elem.properties << ") not kept : normalization "
       << sp[elem.normalizedForm]
       << " is in stoplist";
#endif
     return false;
   }

#ifdef DEBUG_LP
   LDEBUG << "BowGenerator::shouldBeKept token ("
     << sp[elem.lemma] << "|"
     << elem.properties << "), normalization "
     << sp[elem.normalizedForm] << " kept";
#endif

  return true;
}

bool BowGeneratorPrivate::checkStopWordInCompound(
    boost::shared_ptr< Common::BagOfWords::BoWToken >& tok,
    uint64_t offset,
    std::set< std::string >& alreadyStored,
    Common::BagOfWords::BoWText& bowText) const
{
  LIMA_UNUSED(tok);
  LIMA_UNUSED(offset);
  LIMA_UNUSED(alreadyStored);
  LIMA_UNUSED(bowText);
  /*  DUMPERLOGINIT;
  LDEBUG << "BowGenerator: checkStopWord : " << tok->getIdUTF8String();
  BoWTerm* bowTerm=dynamic_cast<BoWTerm*>(tok);
  if (bowTerm != 0)
  {
    LDEBUG << "BowGenerator: is a bowTerm";
      // is a bowTerm : check parts and rearrange if necessary
    std::deque< BoWComplexToken::Part >& parts=bowTerm->getParts();
    uint64_t partIndex=0;
    std::vector<bool> partToRemove;
    bool removeHead=false;

      // get part to remove
    for (std::deque< BoWComplexToken::Part >::iterator partItr=parts.begin();
         partItr!=parts.end();
         partItr++, partIndex++)
    {

      if (checkStopWordInCompound(partItr->get<1>(),offset,alreadyStored,bowText))
      {
          // have to remove part and rearrange bowTerm
        if (partIndex == bowTerm->getHead())
        {
          removeHead=true;
        }
          // stop word is dependant add to remove list
        partToRemove.push_back(true);
      }
      else
      {
        partToRemove.push_back(false);
      }
    }

    if (removeHead)
    {
      LDEBUG << "BowGenerator: remove head";
        // stop word is head : index other parts and return true
      std::vector<bool>::iterator toRemove=partToRemove.begin();
      for (std::deque< BoWComplexToken::Part >::iterator partItr=parts.begin();
           partItr!=parts.end();
           partItr++,toRemove++)
      {
        BoWToken* t=partItr->get<1>();
        if (*toRemove || (alreadyStored.find(t->getIdUTF8String())!=alreadyStored.end()) )
        {
          LDEBUG << "BowGenerator: remove part " << t->getIdUTF8String();
          if (!partItr->get<2>())
          {
            delete t;
          }
        }
        else
        {
            // add part to bowText
          LDEBUG << "BowGenerator: write part " << t->getIdUTF8String();
          t->addToPosition(offset);
          bowText.push_back(t);
          alreadyStored.insert(t->getIdUTF8String());
        }
      }
      parts.clear();
      return true;
    }
    else
    {
      LDEBUG << "BowGenerator: check part to remove";
        // check part to remove, and rearrange if only one part left
      std::vector<bool>::iterator toRemove=partToRemove.begin();
      uint64_t index=parts.size()-1;
      for (std::deque< BoWComplexToken::Part >::iterator partItr=parts.begin();
           partItr!=parts.end();
           toRemove++)
      {
        if (*toRemove)
        {
          LDEBUG << "BowGenerator: remove part " << partItr->get<1>()->getIdUTF8String();
          if (index < bowTerm->getHead())
          {
            bowTerm->setHead(bowTerm->getHead()-1);
          }
          if (!partItr->get<2>())
          {
            delete partItr->get<1>();
          }
          parts.erase(partItr);
        }
        else
        {
          partItr++;
        }
      }
        // size cannot be null, there should at least be the head
      if (parts.size()==1)
      {
        LDEBUG << "BowGenerator: replace bowTerm " << tok->getIdUTF8String() << " by his only part ";
          // replace bowToken by its only part
        tok=parts.begin()->get<1>()->clone();
        delete bowTerm;
        LDEBUG << "BowGenerator: bowTerm becomes " << tok->getIdUTF8String();
      }
    }
    return false;
  }
  else
  {
      // is a bowToken : check if stopword and return
    if (m_stopList->find(tok->getLemma()) != m_stopList->end())
    {
      LINFO << "found stopword " << tok->getIdUTF8String() << " in coumpound !";
      return true;
    }
    else
    {
      return false;
    }
  }*/
  return true;
}

void BowGeneratorPrivate::bowTokenPositions(
    TokenPositions& res,
    const boost::shared_ptr< Common::BagOfWords::BoWToken > tok) const
{
  Common::Misc::PositionLengthList poslenlist=tok->getPositionLengthList();
  res.insert(poslenlist.begin(),poslenlist.end());
}

uint64_t BowGeneratorPrivate::computeCompoundLength(
    const TokenPositions& headTokPositions,
    const TokenPositions& extensionPositions) const
{
  // extent (end-begin) is not a good measure: in "nice little cat",
  // nice_cat and nice_little_cat would have same length

  // sum of length of part is not a good measure: in
  // in "products available in Minnesota",
  // products_available and products_Minnesota would
  // have same length
  //

  // => keep extent and let further treatments
  // decide if they need to use more complex comparisons
  // (use complete PositionLengthList)

  uint64_t length=0;

  // extent
  uint64_t positionBegin=headTokPositions.begin()->first;
  if (positionBegin>extensionPositions.begin()->first)
  {
    positionBegin=extensionPositions.begin()->first;
  }
  uint64_t positionEnd=headTokPositions.rbegin()->first+
      headTokPositions.rbegin()->second;
  uint64_t positionEndExt=extensionPositions.rbegin()->first+
      extensionPositions.rbegin()->second;
  if (positionEndExt>positionEnd)
  {
    positionEnd=positionEndExt;
  }
  length=positionEnd-positionBegin;

  // sum of lengths of components
  //   TokenPositions::const_iterator
  //     it=headTokPositions.begin(),
  //     it_end=headTokPositions.end();
  //   for (; it!=it_end; it++) {
  //     length+=(*it).second;
  //   }
  //   it=extensionPositions.begin();
  //   it_end=extensionPositions.end();
  //   for (; it!=it_end; it++) {
  //     length+=(*it).second;
  //   }

  return length;
}

boost::shared_ptr< BoWNamedEntity > BowGenerator::createSpecificEntity(
  const LinguisticGraphVertex& vertex,
  const AnnotationGraphVertex& v,
  const AnnotationData* annotationData,
  const LinguisticGraph& anagraph,
  const LinguisticGraph& posgraph,
  const uint64_t offset,
  bool frompos) const
{
  return m_d->createSpecificEntity(vertex,v,annotationData,anagraph,posgraph,offset,frompos);
}

boost::shared_ptr< BoWNamedEntity > BowGeneratorPrivate::createSpecificEntity(
  const LinguisticGraphVertex& vertex,
  const AnnotationGraphVertex& v,
  const AnnotationData* annotationData,
  const LinguisticGraph& anagraph,
  const LinguisticGraph& posgraph,
  const uint64_t offset,
  bool frompos) const
{
  if (!annotationData->hasAnnotation(v, Common::Misc::utf8stdstring2limastring("SpecificEntity")))
  {
    return boost::shared_ptr< BoWNamedEntity >();
  }
#ifdef DEBUG_LP
  DUMPERLOGINIT;
  LINFO << "BowGenerator: createSpecificEntity ling:" << vertex
        << "; annot:" << v << offset << frompos;
#endif
  const LinguisticGraph& graph = (frompos?posgraph:anagraph);
  const FsaStringsPool& sp=Common::MediaticData::MediaticData::single().stringsPool(m_language);

  const SpecificEntityAnnotation* se =
    annotationData->annotation(v, Common::Misc::utf8stdstring2limastring("SpecificEntity")).
      pointerValue<SpecificEntityAnnotation>();

#ifdef DEBUG_LP
  LINFO << "BowGenerator: specific entity type is " << se->getType();
#endif

  std::set< std::string > alreadyStored;

  // build BoWNamedEntity
  LimaString typeName("");
  try {
    typeName = MediaticData::single().getEntityName(se->getType());
  }
  catch (std::exception& e) {
    DUMPERLOGINIT;
    LERROR << "Undefined entity type " << se->getType();
    return boost::shared_ptr< BoWNamedEntity >();
  }
#ifdef DEBUG_LP
  LINFO << "BowGenerator: specific entity type name is " << typeName;
#endif
  // get the macro-category to use for this named entity
  MorphoSyntacticData* data = get(vertex_data, graph, vertex);
  if (data->empty())
  {
    DUMPERLOGINIT;
    LERROR << "Empty data for vertex " << vertex << " at " << __FILE__ << ", line " << __LINE__;
    LERROR << "This is a bug. Returning null entity for" << se->getString() << typeName;
    return boost::shared_ptr< BoWNamedEntity >();
  }

  LinguisticCode category=m_macroAccessor->readValue(data->begin()->properties);

  boost::shared_ptr< BoWNamedEntity > bowNE( new
                          BoWNamedEntity(sp[getNamedEntityNormalization(v, annotationData)],
                                          category,
                                          se->getType(),
                                          offset+se->getPosition(),
                                          se->getLength()) );
  // add named entity parts
  auto neParts = createNEParts(v, annotationData, anagraph, posgraph, frompos);

  if (neParts.empty())
  {
    DUMPERLOGINIT;
    LWARN << "No parts kept for named entity " << (*se).getString();
    // set named entity itself as part
    boost::shared_ptr< BoWToken > bowToken(new
      BoWToken(sp[getNamedEntityNormalization(v, annotationData)],
              category,
              offset+(*se).getPosition(),
              (*se).getLength()));
    Token* token = get(vertex_token, graph, vertex);
    bowToken->setInflectedForm(token->stringForm());

    bowNE->addPart(bowToken);
  }
  else
  {
    for (auto p=neParts.begin(); p!=neParts.end(); p++)
    {
      //create a new BoWToken
      boost::shared_ptr< BoWToken > bowToken(new BoWToken((*p).lemma,(*p).category,
                                      offset+(*p).position,
                                      (*p).length));
      bowToken->setInflectedForm((*p).inflectedForm);
#ifdef DEBUG_LP
      LDEBUG << "BowGenerator: specific entity part infl " << (*p).inflectedForm;
#endif
      bowNE->addPart(bowToken);
    }
  }

  // add the features
  const auto& features=(*se).getFeatures();
  for (auto f=features.begin(), f_end=features.end(); f!=f_end; f++)
  {
    bowNE->setFeature((*f).getName(),
                      (*f).getValueLimaString());
  }
#ifdef DEBUG_LP
  LDEBUG << "CreateSpecificEntity: created features " << QString::fromUtf8(bowNE->getFeaturesUTF8String().c_str());
#endif

  auto elem = bowNE->getIdUTF8String();
  if (alreadyStored.find(elem) != alreadyStored.end())
  { // already stored
#ifdef DEBUG_LP
    LDEBUG << "BowGenerator: BoWNE already stored. Skipping it.";
#endif
    return boost::shared_ptr< BoWNamedEntity >();
  }
  else
  {
//     LDEBUG << "BowGenerator: created BoWNamedEntity " << bowNE->getOutputUTF8String();
    alreadyStored.insert(elem);
    return bowNE;
  }
  return bowNE;
}


QList< boost::shared_ptr< BoWPredicate > > BowGeneratorPrivate::createPredicate(
    const LinguisticGraphVertex& lgv,
    const AnnotationGraphVertex& agv,
    const AnnotationData* annotationData,
    const LinguisticGraph& anagraph,
    const LinguisticGraph& posgraph,
    const uint64_t offset,
    std::set< LinguisticGraphVertex >& visited,
    bool keepAnyway) const
{
#ifdef DEBUG_LP
  DUMPERLOGINIT;
  LINFO << "BowGenerator::createPredicate ling:" << lgv << "; annot:" << agv;
#endif
  QList< boost::shared_ptr< BoWPredicate > > result;

  Token* token = get(vertex_token, posgraph, lgv);

  // FIXME handle the ambiguous case when there is several class values for the predicate
  QStringList predicateIds=annotationData->stringAnnotation(agv,Common::Misc::utf8stdstring2limastring("Predicate")).split(",");
#ifdef DEBUG_LP
  if (predicateIds.size()>1)
  {
    LDEBUG << "BowGenerator::createPredicate Predicate has"
            << predicateIds.size() << "values:" << predicateIds;
  }
#endif


  // FIXED replace the hardcoded VerbNet by a value from configuration
  // LimaString predicate=predicateIds.first();
  // The fix should work only with FrameNet annotations. VerbNet does not assure to have the same
  // number of roles in each list as the number of predicates
  for (int i = 0 ; i < predicateIds.size(); i++)
  {
    auto predicate = predicateIds[i];
    try
    {
      auto predicateEntity = Common::MediaticData::MediaticData::single().getEntityType(predicate);
#ifdef DEBUG_LP
      LDEBUG << "BowGenerator::createPredicate  The role(s) related to "
              << predicate << " is/are ";
#endif
      AnnotationGraph annotGraph = annotationData->getGraph();
      AnnotationGraphOutEdgeIt outIt, outIt_end;
      boost::tie(outIt, outIt_end) = boost::out_edges(agv, annotationData->getGraph());
      QMultiMap<Common::MediaticData::EntityType,
                boost::shared_ptr< AbstractBoWElement > > roles;
      const LimaString typeAnnot = "SemanticRole";
      for (; outIt != outIt_end; outIt++)
      {
        // FIXME handle the ambiguous case when there is several values for each role
        auto semRoleVx = boost::target(*outIt, annotGraph);
        auto semRoleIds = annotationData->stringAnnotation(agv,
                                                           semRoleVx,
                                                           typeAnnot).split("|");
        if (predicateIds.size() != semRoleIds.size())
        {
          DUMPERLOGINIT;
          LERROR << "BowGenerator::createPredicate predicateIds and semRoleIds sizes are different:"
                  << predicateIds.size() << "and" << semRoleIds.size();
          LERROR << "BowGenerator::createPredicate abort this predicate creation";
          return result;
        }
        Q_ASSERT(predicateIds.size() == semRoleIds.size());
        LimaString semRole = semRoleIds[i];
#ifdef DEBUG_LP
        LDEBUG << semRole;
#endif
        if (semRole.isEmpty()) continue;
        try
        {
          auto semRoleEntity = Common::MediaticData::MediaticData::single().getEntityType(semRole);
          auto posGraphSemRoleVertices = annotationData->matches("annot", semRoleVx,
                                                                 "PosGraph");
          if (!posGraphSemRoleVertices.empty())
          {
            auto posGraphSemRoleVertex = *(posGraphSemRoleVertices.begin());
            if (posGraphSemRoleVertex == lgv)
            {
              DUMPERLOGINIT;
              LERROR << "BowGenerator::createPredicate role vertex is the same as the trigger vertex. Abort this role.";
              continue;
            }
#ifdef DEBUG_LP
            LDEBUG << "BowGenerator::createPredicate Calling createAbstractBoWElement on PoS graph vertex"
                    << posGraphSemRoleVertex;
#endif
            auto semRoleTokens = createAbstractBoWElement(posGraphSemRoleVertex,
                                                          anagraph,
                                                          posgraph,
                                                          offset,
                                                          annotationData,
                                                          visited,
                                                          keepAnyway);
#ifdef DEBUG_LP
            LDEBUG << "BowGenerator::createPredicate Created "
                    << semRoleTokens.size()
                    << "token for the role associated to" << predicate;
#endif
  //               if (semRoleTokens[0].second!="")
            if (!semRoleTokens.empty())
            {
              roles.insert(semRoleEntity, semRoleTokens[0].second);
            }
          }
          else
          {
#ifdef DEBUG_LP
            LDEBUG << "BowGenerator::createPredicate Found no matching for the semRole in the annot graph";
#endif
          }
        }
        catch (const Lima::LimaException& e)
        {
          DUMPERLOGINIT;
          LERROR << "BowGenerator::createPredicate Unknown semantic role"
                  << semRole << ";" << e.what();
          throw;
        }
      }
      boost::shared_ptr< BoWPredicate > bowP(new BoWPredicate());
      bowP->setPosition(offset+token->position());
      bowP->setLength(token->length());
      bowP->setPredicateType(predicateEntity);
      auto pEntityType = bowP->getPredicateType();
#ifdef DEBUG_LP
      LDEBUG << "BowGenerator::createPredicate Created a Predicate for the verbal class "
              << Common::MediaticData::MediaticData::single().getEntityName(pEntityType);
#endif
      if (!roles.empty())
      {
        bowP->setRoles(roles);
        auto pRoles = bowP->roles();
        for (auto it = pRoles.begin(); it != pRoles.end(); it++)
        {
          auto outputRoles = boost::dynamic_pointer_cast<BoWToken>(it.value());
          if (outputRoles != 0)
          {
            auto roleLabel = Common::MediaticData::MediaticData::single().getEntityName(it.key());
#ifdef DEBUG_LP
            LDEBUG << "BowGenerator::createPredicate Associated "
                    << QString::fromStdString(outputRoles->getOutputUTF8String())
                    << " to it" << "via the semantic role label "<< roleLabel ;
#endif
          }
        }
      }
      result.append(bowP);
    }
    catch (const Lima::LimaException& e)
    {
      DUMPERLOGINIT;
      LERROR << "BowGenerator::createPredicate Unknown predicate"
              << predicate << ";" << e.what();
      return QList< boost::shared_ptr< BoWPredicate > >();
    }
  }
  return result;
}

QList< boost::shared_ptr< Common::BagOfWords::BoWPredicate > > BowGenerator::createSemanticRelationPredicate(
    const LinguisticGraphVertex& lgvs,
    const AnnotationGraphVertex& agvs,
    const AnnotationGraphVertex& agvt,
    const SemanticRelationAnnotation& annot ,
    const AnnotationData* annotationData,
    const LinguisticGraph& anagraph,
    const LinguisticGraph& posgraph,
    uint64_t offset,
    std::set< LinguisticGraphVertex >& visited,
    bool keepAnyway) const
{
#ifdef DEBUG_LP
  DUMPERLOGINIT;
  LINFO << "BowGenerator::createSemanticRelationPredicate " << lgvs << ", src:" << agvs << ", trgt:"<< agvt
          << ", rel:" << annot.type().c_str();
#endif
  QList< boost::shared_ptr< BoWPredicate > > result;

    // FIXME handle the ambiguous case when there is several class values for the predicate
  auto predicateIds = QString::fromStdString(annot.type()).split("|");
#ifdef DEBUG_LP
  if (predicateIds.size()>1)
  {
    LDEBUG << "BowGenerator::createSemanticRelationPredicate Semantic relation has"
            << predicateIds.size() << "values:" << predicateIds;
  }
#endif

  // FIXED replace the hardcoded VerbNet by a value from configuration
  // LimaString predicate=predicateIds.first();
  // The fix should work only with FrameNet annotations. VerbNet does not assure to have the same
  // number of roles in each list as the number of predicates
  for (int i = 0 ; i < predicateIds.size(); i++)
  {
    auto predicate = predicateIds[i];
    try
    {
      auto predicateEntity = Common::MediaticData::MediaticData::single().getEntityType(predicate);

      boost::shared_ptr< BoWPredicate > bowP( new BoWPredicate() );

      bowP->setPredicateType(predicateEntity);

      bowP->setPosition(0);
      bowP->setLength(0);

      std::vector<AnnotationGraphVertex> vertices;
      vertices.push_back(agvs);
      vertices.push_back(agvt);
    #ifdef DEBUG_LP
      LDEBUG << "BowGenerator::createSemanticRelationPredicate  The role(s) related to "
              << annot.type() << " is/are ";
    #endif
      QMultiMap<Common::MediaticData::EntityType,
                boost::shared_ptr< AbstractBoWElement > > roles;
    //   const LimaString typeAnnot="SemanticRole";
      for (const auto& semRoleVx: vertices)
      {
        auto anaGraphSemRoleVertices = annotationData->matches("annot", semRoleVx,
                                                              "AnalysisGraph");
        if (!anaGraphSemRoleVertices.empty())
        {
          auto anaGraphSemRoleVertex = *anaGraphSemRoleVertices.begin();
          auto posGraphSemRoleVertices = annotationData->matches("AnalysisGraph",
                                                                anaGraphSemRoleVertex,
                                                                "PosGraph");
          if (!posGraphSemRoleVertices.empty())
          {
            auto posGraphSemRoleVertex = *(posGraphSemRoleVertices.begin());
            if (posGraphSemRoleVertex == lgvs)
            {
    #ifdef DEBUG_LP
              LERROR << "BowGenerator::createSemanticRelationPredicate role vertex is the same as the trigger vertex. Abort this role.";
    #endif
              continue;
            }
    #ifdef DEBUG_LP
            LDEBUG << "BowGenerator::createSemanticRelationPredicate Calling createAbstractBoWElement on PoS graph vertex"
                    << posGraphSemRoleVertex;
    #endif
            auto semRoleTokens = m_d->createAbstractBoWElement(posGraphSemRoleVertex,
                                                          anagraph,
                                                          posgraph,
                                                          offset,
                                                          annotationData,
                                                          visited,
                                                          keepAnyway);
    #ifdef DEBUG_LP
            LDEBUG << "BowGenerator::createSemanticRelationPredicate Created "
                    << semRoleTokens.size() << "token for the role associated to "
                    << annot.type().c_str();
    #endif
            if (!semRoleTokens.empty())
            {
              EntityType semRoleEntity;
              roles.insert(semRoleEntity, semRoleTokens[0].second);
            }
          }
#ifdef DEBUG_LP
          else
          {
            LDEBUG << "BowGenerator::createSemanticRelationPredicate Found no matching for the semRole in the annot graph";
          }
#endif
        }
      }
#ifdef DEBUG_LP
      LDEBUG << "BowGenerator::createSemanticRelationPredicate Created a Predicate for the semantic relation"
              << predicateEntity
              << Common::MediaticData::MediaticData::single().getEntityName(predicateEntity);
#endif
      if (!roles.empty())
      {
        bowP->setRoles(roles);
#ifdef DEBUG_LP
        for (auto it = roles.begin(); it != roles.end(); it++)
        {
          auto outputRoles=boost::dynamic_pointer_cast<BoWToken>(it.value());
          if (outputRoles != 0)
          {
            auto roleLabel = it.key().isNull() ? QString()
                              : Common::MediaticData::MediaticData::single().getEntityName(it.key());
            LDEBUG << "BowGenerator::createSemanticRelationPredicate Associated "
                    << QString::fromStdString(outputRoles->getOutputUTF8String())
                    << " to it" << "via the semantic role label "<< roleLabel ;
          }
        }
#endif
      }
      result.append(bowP);
    }
    catch (const Lima::LimaException& e)
    {
      DUMPERLOGINIT;
      LERROR << "BowGenerator::createSemanticRelationPredicate Unknown predicate"
              << predicate << ";" << e.what();
      return QList< boost::shared_ptr< BoWPredicate > >();
    }
  }
  return result;
}


StringsPoolIndex BowGeneratorPrivate::getNamedEntityNormalization(
    const AnnotationGraphVertex& v,
    const AnnotationData* annotationData) const
{
  if (!annotationData->hasAnnotation(v, Common::Misc::utf8stdstring2limastring("SpecificEntity")))
  {
#ifdef DEBUG_LP
    DUMPERLOGINIT;
    LDEBUG << "BowGenerator::getNamedEntityNormalization: no SpecificEntity annotation for " << v << " ; return 0";
#endif
    return static_cast<StringsPoolIndex>(0);
  }
#ifdef DEBUG_LP
  DUMPERLOGINIT;
  LINFO << "BowGenerator::getNamedEntityNormalization: m_NEnormalization is " << m_NEnormalization;
#endif
  StringsPoolIndex normalizedForm(0);
  switch (m_NEnormalization)
  {
    case NORMALIZE_NE_INFLECTED:
    normalizedForm=annotationData->annotation(v,Common::Misc::utf8stdstring2limastring("SpecificEntity"))
          .pointerValue< SpecificEntityAnnotation >()->getString();
      break;
    case NORMALIZE_NE_LEMMA:
    normalizedForm=annotationData->annotation(v,Common::Misc::utf8stdstring2limastring("SpecificEntity"))
          .pointerValue< SpecificEntityAnnotation >()->getNormalizedString();
      break;
    case NORMALIZE_NE_NORMALIZEDFORM:
      // list of "attribute=value" elements (not pretty but generic)
      //  normalizedForm=se.getNormalizedForm().str();
    normalizedForm=annotationData->annotation(v,Common::Misc::utf8stdstring2limastring("SpecificEntity"))
          .pointerValue< SpecificEntityAnnotation >()->getNormalizedForm();
      break;
    case NORMALIZE_NE_NETYPE:
    FsaStringsPool& sp = Common::MediaticData::MediaticData::changeable().stringsPool(m_language);
    const SpecificEntityAnnotation* annot = annotationData->annotation(v,Common::Misc::utf8stdstring2limastring("SpecificEntity"))
          .pointerValue< SpecificEntityAnnotation >();

    LimaString typeStr = Common::MediaticData::MediaticData::single().getEntityName(annot->getType());

    normalizedForm=sp[typeStr];
    break;
  }
#ifdef DEBUG_LP
  LDEBUG << "BowGenerator::getNamedEntityNormalization return " << normalizedForm;
#endif
  return normalizedForm;
}

std::vector<BowGeneratorPrivate::NamedEntityPart> BowGeneratorPrivate::createNEParts(
    const AnnotationGraphVertex& v,
    const AnnotationData* annotationData,
    const LinguisticGraph& anagraph,
    const LinguisticGraph& posgraph,
    bool frompos) const
{
#ifdef DEBUG_LP
  DUMPERLOGINIT;
#endif
  const LinguisticGraph& graph = (frompos?posgraph:anagraph);
  const FsaStringsPool& sp=Common::MediaticData::MediaticData::single().stringsPool(m_language);

  const auto namedEntity =
    annotationData->annotation(v,Common::Misc::utf8stdstring2limastring("SpecificEntity"))
      .pointerValue< SpecificEntityAnnotation >();
  std::vector<BowGeneratorPrivate::NamedEntityPart> parts;


  bool useOnePart(false);      // use one part, tagged as proper noun
  bool useDefaultParts(true); // use parts of named entity as they are in the graph
  LinguisticCode useCategory = LinguisticCode::fromUInt((uint64_t)-1);   // use this category for each of the parts (NP or NC)
  uint64_t position(0);
  uint64_t length(0);

  /// @todo make this configurable
  /*
  LimaString typeName=MediaticData::single().getEntityName(namedEntity->getType());
  if (it==m_entityNames.end())
  {
    LERROR << "Undefined entity type " << namedEntity->getType();
  }
  else
  {
    typeName=(*it).second;
  }
  const Automaton::EntityFeatures& features=namedEntity->getFeatures();
  EntityFeatures::const_iterator featureIt;

  if (typeName == "PERSON")
  {
    // could force two parts, firstname and lastname, both tagged as proper noun
    // but firstname in features can be added by normalization -> not found in text
    // => use real parts, but each with proper noun category
    useDefaultParts=true;
    useCategory=m_properNounCategory;
  }
  else if (typeName == "TIMEX")
  {
    // problems to get the positions and length for day/month/year features
    // => use real parts, each with common noun category
    useDefaultParts=true;
    useCategory=m_commonNounCategory;
  }
  else if (typeName == "NUMEX")
  {
    // problems to get the positions and length for value/unit features
    // => use real parts, each with common noun category
    useDefaultParts=true;
    useCategory=m_commonNounCategory;
  }
  else if (typeName == "EVENT")
  {
    // use default parts, with category assigned by analysis
    useDefaultParts=true;
  }
  else if (typeName == "LOCATION" ||
           typeName == "ORGANIZATION" ||
           typeName == "PRODUCT")
  {
    // only one part, tagged as proper noun
    useOnePart=true;
  }
  else
  { // other types
    LWARN << "unexpected type of NE to dump: use default treatment";
    useDefaultParts=true;
  }
  */

  if (useOnePart)
  {
    position=namedEntity->getPosition();
    length=namedEntity->getLength();
    LimaString normalizedForm =  sp[getNamedEntityNormalization(v, annotationData)];
    LimaString str = sp[namedEntity->getNormalizedString()];
    parts.
      push_back(NamedEntityPart(normalizedForm,
                                str,
                                m_properNounCategory,
                                position,length));
  }
  else if (useDefaultParts)
  {
    // get the parts of the named entity match
    for (auto m = namedEntity->vertices().cbegin();
         m != namedEntity->vertices().cend(); m++)
    {
      const auto token = get(vertex_token, graph, *m);
      const auto data = get(vertex_data, graph, *m);

      if (data != nullptr && !data->empty())
      {
        const auto& elem = *(data->begin());

        if (! m_keepAllNamedEntityParts && ! shouldBeKept(elem))
        {
#ifdef DEBUG_LP
          LDEBUG << "BowGenerator: part of named entity not kept: " << token->stringForm();
#endif
          continue;
        }

        LinguisticCode category;
        if (useCategory != LinguisticCode::fromUInt((uint64_t)-1))
        {
          category = useCategory;
        }
        else
        {
          category = m_macroAccessor->readValue(elem.properties);
        }
        parts.push_back(NamedEntityPart(token->stringForm(),
                        sp[elem.normalizedForm],
                        category,
                        token->position(),
                        token->length()));
      }
    }
  }

  return parts;
}

boost::shared_ptr< BoWToken > BowGeneratorPrivate::createCompoundTense(
    const AnnotationGraphVertex& v,
    const AnnotationData* annotationData,
    const LinguisticGraph& anagraph,
    const LinguisticGraph& posgraph,
    const uint64_t offset,
    std::set<LinguisticGraphVertex>& visited) const
{
#ifdef DEBUG_LP
  DUMPERLOGINIT;
  LINFO << "BowGenerator: createCompoundTense " << v;
#endif
  if (!annotationData->hasIntAnnotation(v, Common::Misc::utf8stdstring2limastring("CpdTense")))
  {
    return boost::shared_ptr< BoWToken >();
  }

  // chercher l'aux et le pp ;
  AnnotationGraphVertex auxVertex, ppVertex;
  auxVertex = ppVertex = std::numeric_limits< AnnotationGraphVertex >::max();
  AnnotationGraphOutEdgeIt it, it_end;
  boost::tie(it, it_end) = boost::out_edges(v, annotationData->getGraph());
  for (; it != it_end; it++)
  {
    if (annotationData->hasIntAnnotation(*it,Common::Misc::utf8stdstring2limastring("Aux")))
    {
      auxVertex = boost::target(*it, annotationData->getGraph());
    }
    else if(annotationData->hasIntAnnotation(*it,Common::Misc::utf8stdstring2limastring("PastPart")))
    {
      ppVertex = boost::target(*it, annotationData->getGraph());
    }
  }
  if ( auxVertex == std::numeric_limits< AnnotationGraphVertex >::max()
       || ppVertex == std::numeric_limits< AnnotationGraphVertex >::max())
  {
    return boost::shared_ptr< BoWToken >();
  }
  //   parcourir les liens entrants su v annotes par Aux et PastPart
  //   si la source est annotee CpdTense, rappeler recursivement
  //   createCompoundTense ; sinon, creer un BoWToken simple
  // creer eventuellement un compound tense pour chaque ;
  boost::shared_ptr< BoWToken> head, extension;
  if (annotationData->hasIntAnnotation(ppVertex, Common::Misc::utf8stdstring2limastring("CpdTense")))
  {
    head = createCompoundTense(ppVertex, annotationData, anagraph, posgraph, offset, visited);
  }
  else
  {
    LinguisticGraphVertex ppTokVertex =
      *(annotationData->matches("annot", ppVertex, "PosGraph").begin());

    std::vector< std::pair<boost::shared_ptr< BoWRelation >, boost::shared_ptr< AbstractBoWElement > > > ppBoWTokens = createAbstractBoWElement(ppTokVertex, anagraph, posgraph, offset, annotationData, visited, true);
    if (ppBoWTokens.empty())
    {
      return boost::shared_ptr< BoWToken >();
    }
    else
    {
      head = boost::dynamic_pointer_cast<BoWToken>(ppBoWTokens.back().second);
      ppBoWTokens.pop_back();
    }
  }
  if (head == 0)
  {
    return boost::shared_ptr< BoWToken >();
  }
  if (annotationData->hasIntAnnotation(auxVertex, Common::Misc::utf8stdstring2limastring("CpdTense")))
  {
    extension = createCompoundTense(auxVertex, annotationData, anagraph, posgraph, offset, visited);
  }
  else
  {
    LinguisticGraphVertex auxTokVertex =
      *(annotationData->matches("annot", auxVertex, "PosGraph").begin());

    std::vector<std::pair<boost::shared_ptr< BoWRelation >, boost::shared_ptr< AbstractBoWElement > > > auxBoWTokens = createAbstractBoWElement(auxTokVertex, anagraph, posgraph, offset, annotationData, visited, true);
    if (auxBoWTokens.empty())
    {
      return boost::shared_ptr< BoWToken >();
    }
    else
    {
      extension = boost::dynamic_pointer_cast<BoWToken>(auxBoWTokens.back().second);
      auxBoWTokens.pop_back();
    }
  }
  if (extension == 0 && head != 0)
  {
    return boost::shared_ptr< BoWToken >();
  }
  // Build a BoWTerm with the preposition group as head and the aux as extension

  boost::shared_ptr< BoWToken > complex(
    new BoWToken(
                  head->getLemma(),
                  head->getCategory(),
                  extension->getPosition(),
                  (head->getPosition()+head->getLength()-extension->getPosition())));
  complex->setVertex(head->getVertex());
  complex->setInflectedForm(head->getInflectedForm());
#ifdef DEBUG_LP
  LDEBUG << "BowGenerator: Built complex: " << *complex;
#endif

  return complex;
}

} // AnalysisDumper

} // LinguisticProcessing

} // Lima
