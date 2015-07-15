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
 *   Copyright (C) 2004-2012 by CEA LIST                              *
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
    bool operator()(const std::pair<QSharedPointer< Common::BagOfWords::BoWRelation >, QSharedPointer< Common::BagOfWords::BoWToken > > t1, const std::pair<QSharedPointer< Common::BagOfWords::BoWRelation >, QSharedPointer< Common::BagOfWords::BoWToken > > t2) const
    {
      return (t1.second->getPosition()<t2.second->getPosition()) ;
    }
  };


BowGenerator::BowGenerator():
    m_language(0),
    m_stopList(0),
    m_useStopList(true),
    m_useEmptyMacro(true),
    m_useEmptyMicro(true),
    m_properNounCategory(0),
    m_commonNounCategory(0),
    m_keepAllNamedEntityParts(false),
    m_macroAccessor(0),
    m_microAccessor(0),
    m_NEnormalization(NORMALIZE_NE_INFLECTED)
{
}

BowGenerator::~BowGenerator()
{
}

void BowGenerator::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  MediaId language)

{
  DUMPERLOGINIT;
  m_language=language;
  m_macroAccessor=&static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyAccessor("MACRO");
  m_microAccessor=&static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyAccessor("MICRO");
  try
  {
    std::string use=unitConfiguration.getParamsValueAtKey("useStopList");
    m_useStopList=(use=="true");
  }
  catch (NoSuchParam& )
  {
    LWARN << "No param 'useStopList' in "<<unitConfiguration.getName()<<" configuration group for language " << (int)m_language;
    LWARN << "use default value : true";
  }
  if (m_useStopList)
  {
    try
    {
      std::string stoplist=unitConfiguration.getParamsValueAtKey("stopList");
      m_stopList=static_cast<StopList*>(LinguisticResources::single().getResource(m_language,stoplist));
      LDEBUG << "BowGenerator.init(): STOPLIST:";
      for( StopList::const_iterator wordIt = m_stopList->begin() ; wordIt != m_stopList->end() ; wordIt++ ) {
        LDEBUG << "BowGenerator.init(): " << *wordIt;
      }
    }
    catch (NoSuchParam& )
    {
      LERROR << "No param 'stopList' in "<<unitConfiguration.getName()<<" configuration group for language " << (int)m_language;
//       throw InvalidConfiguration();
    }
  }
  try
  {
    std::string use=unitConfiguration.getParamsValueAtKey("useEmptyMacro");
    m_useEmptyMacro=(use=="true");
  }
  catch (NoSuchParam& )
  {
    LWARN << "No param 'useEmptyMacro' in "<<unitConfiguration.getName()<<" configuration group for language " << (int)m_language;
    LWARN << "use default value : true";
  }
  try
  {
    std::string use=unitConfiguration.getParamsValueAtKey("useEmptyMicro");
    m_useEmptyMicro=(use=="true");
  }
  catch (NoSuchParam& )
  {
    LWARN << "No param 'useEmptyMicro' in "<<unitConfiguration.getName()<<" configuration group for language " << (int)m_language;
    LWARN << "use default value : true";
  }
  try
  {
    std::string np=unitConfiguration.getParamsValueAtKey("properNounCategory");
    m_properNounCategory=static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyManager("MACRO").getPropertyValue(np);
  }
  catch (NoSuchParam& )
  {
    LERROR << "No param 'properNounCategory' in "<<unitConfiguration.getName()<<" configuration group for language " << (int)m_language;
//     throw InvalidConfiguration();
  }
  try
  {
    std::string cn=unitConfiguration.getParamsValueAtKey("commonNounCategory");
    m_commonNounCategory=static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyManager("MACRO").getPropertyValue(cn);
  }
  catch (NoSuchParam& )
  {
    LERROR << "No param 'commonNounCategory' in "<<unitConfiguration.getName()<<" configuration group for language " << (int)m_language;
//     throw InvalidConfiguration();
  }

  try
  {
    std::string value=unitConfiguration.getParamsValueAtKey("keepAllNamedEntityParts");
    if (value == "yes" || value == "true" || value == "1")
    {
      m_keepAllNamedEntityParts=true;
    }
    else
    {
      m_keepAllNamedEntityParts=false;
    }
  }
  catch (NoSuchParam& ) { /* optional */ }

  try
  {
    std::string value=unitConfiguration.getParamsValueAtKey("NEnormalization");
    if (value == "useInflectedForm")
    {
      m_NEnormalization=NORMALIZE_NE_INFLECTED;
    }
    else if (value == "useLemma")
    {
      m_NEnormalization=NORMALIZE_NE_LEMMA;
    }
    else if (value == "useNENormalizedForm")
    {
      m_NEnormalization=NORMALIZE_NE_NORMALIZEDFORM;
    }
    else if (value == "useNEType")
    {
      m_NEnormalization=NORMALIZE_NE_NETYPE;
    }
  }
  catch (NoSuchParam& ) { /* optional */ }
}


std::vector< std::pair< QSharedPointer< BoWRelation >, QSharedPointer< BoWToken > > > BowGenerator::buildTermFor(
      const AnnotationGraphVertex& vx,
      const AnnotationGraphVertex& tgt,
      const LinguisticGraph& anagraph,
      const LinguisticGraph& posgraph,
      const uint64_t offset,
      const SyntacticData* syntacticData,
      const AnnotationData* annotationData,
      std::set< LinguisticGraphVertex >& visited) const
{
  DUMPERLOGINIT;
  LDEBUG << "BowGenerator::buildTermFor annot:" << vx << "; pointing on annot:"<<tgt;

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

  std::vector< std::pair<QSharedPointer< BoWRelation >, QSharedPointer< AbstractBoWElement > > > vxBoWTokens = createAbstractBoWElement(vxTokVertex, anagraph, posgraph, offset, annotationData,visited);

  LDEBUG << "BowGenerator: There is " << vxBoWTokens.size() << " bow tokens";
  if (vxGovernors.empty())
  {
    std::vector< std::pair<QSharedPointer< BoWRelation >, QSharedPointer< BoWToken > > > vxBoWTk;
    LDEBUG << "BowGenerator: == DONE buildTermFor " << vx << " (pointing on "<<tgt<<"):empty governors ";
    QSharedPointer< BoWRelation > relation = createBoWRelationFor(vx, tgt, annotationData, posgraph,syntacticData);
    if (relation)
    {
      auto vxBoWTokensIt= vxBoWTokens.begin(), vxBoWTokensIt_end= vxBoWTokens.end();
      for (; vxBoWTokensIt != vxBoWTokensIt_end; vxBoWTokensIt++)
      {
        (*vxBoWTokensIt).first = relation;
      }
    }
    auto vxBoWTokensIt = vxBoWTokens.begin(), vxBoWTokensIt_end = vxBoWTokens.end();
    for (; vxBoWTokensIt != vxBoWTokensIt_end; vxBoWTokensIt++)
    {
      if (qSharedPointerDynamicCast<BoWToken>((*vxBoWTokensIt).second) != 0)
      {
        vxBoWTk.push_back(std::make_pair((*vxBoWTokensIt).first,qSharedPointerDynamicCast<BoWToken>((*vxBoWTokensIt).second)));
      }
      else
      {
        LERROR << "BowGenerator::buildTermFor vxBoWTokensIt is not a BoWToken";
      }
    }
    return vxBoWTk;
  }

  std::vector< std::pair< QSharedPointer< BoWRelation >, QSharedPointer< BoWToken > > > result;
  std::vector< std::vector< std::pair< QSharedPointer< BoWRelation > , QSharedPointer< BoWToken > > > > termsForVxGovernors;
  std::vector<DependencyGraphVertex>::const_iterator govsIt, govsIt_end;
  govsIt = vxGovernors.begin(); govsIt_end = vxGovernors.end();
  for (uint64_t i = 0; govsIt != govsIt_end; govsIt++, i++)
  {

    auto pairs = buildTermFor(*govsIt, vx, anagraph, posgraph, offset, syntacticData, annotationData,visited);
    termsForVxGovernors.push_back(pairs);
    LDEBUG << "BowGenerator: For governor " << i << ", there is " << pairs.size() << " terms.";
  }


  std::vector< std::vector< std::pair< QSharedPointer< BoWRelation >, QSharedPointer< BoWToken > > >::iterator > begins;
  std::vector< std::vector< std::pair< QSharedPointer< BoWRelation >, QSharedPointer< BoWToken > > >::iterator > ends;
  std::vector< std::vector< std::pair< QSharedPointer< BoWRelation >, QSharedPointer< BoWToken > > >::iterator > stack;
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
    LDEBUG << "BowGenerator: Stack is empty ! Returning bow tokens of " << vxTokVertex;
    LDEBUG << "BowGenerator: == DONE buildTermFor " << vx << " (pointing on "<<tgt<<"):stack governors ";
    QSharedPointer< BoWRelation> relation = createBoWRelationFor(vx, tgt, annotationData, posgraph,syntacticData);
    if (relation)
    {
      auto vxBoWTokensIt = vxBoWTokens.begin(), vxBoWTokensIt_end = vxBoWTokens.end();
      for (; vxBoWTokensIt != vxBoWTokensIt_end; vxBoWTokensIt++)
      {
        (*vxBoWTokensIt).first = relation;
      }
    }
    std::vector< std::pair< QSharedPointer< BoWRelation >, QSharedPointer< BoWToken > > > vxBoWTk;
    auto vxBoWTokensIt = vxBoWTokens.begin(), vxBoWTokensIt_end = vxBoWTokens.end();
    for (; vxBoWTokensIt != vxBoWTokensIt_end; vxBoWTokensIt++)
    {
      if (qSharedPointerDynamicCast<BoWToken>((*vxBoWTokensIt).second) != 0)
      {
        vxBoWTk.push_back(std::make_pair((*vxBoWTokensIt).first,qSharedPointerDynamicCast<BoWToken>((*vxBoWTokensIt).second)));
      }
      else
      {
        LERROR << "BowGenerator::buildTermFor vxBoWTokensIt is not a BoWToken";
      }
    }
    return vxBoWTk;
  }
  std::vector< std::pair< QSharedPointer< BoWRelation >, QSharedPointer< BoWToken > > >::iterator t;
  while (!stack.empty())
  {
    LDEBUG << "BowGenerator: There is " << vxBoWTokens.size() << " heads, " << vxGovernors.size() << " governors and stack size is " << stack.size();
    for (auto vxBoWToken=vxBoWTokens.begin();
         vxBoWToken!=vxBoWTokens.end();
         vxBoWToken++)
    {
      QSharedPointer< BoWToken > head = qSharedPointerDynamicCast<BoWToken>((*vxBoWToken).second);
      if (head == 0)
      {
        LERROR << "BowGenerator::buildTermFor head" << (*vxBoWToken).second << "is not a BoWToken";
        continue;
      }
      LDEBUG << "BowGenerator: Working on head " << *head << "(" << head << ")";

      std::set< std::pair< QSharedPointer< BoWRelation >, QSharedPointer< BoWToken > >, A > extensions;
      auto govsIt = stack.begin(), govsIt_end = stack.end();
      for (; govsIt != govsIt_end; govsIt++)
      {
        LDEBUG << "BowGenerator: Entering loop body";
        QSharedPointer< BoWRelation > relation( (**govsIt).first);
        QSharedPointer< BoWToken > bt = (**govsIt).second;
        LDEBUG << "BowGenerator:     ... done.";
//         LDEBUG << "BowGenerator: Inserting extension...";
        extensions.insert(std::make_pair(relation,bt));
//         LDEBUG << "BowGenerator:     ... done.";
      }

      LimaString lemma;
      LimaString infl;
      uint64_t position=0;
      uint64_t length=0;
      TokenPositions headPositions;
      bowTokenPositions(headPositions, head);
      TokenPositions extensionPositions;

      LDEBUG << "BowGenerator: Working on extensions";
      auto extensionsIt = extensions.begin(),
      extensionsIt_end = extensions.end();
      for (; extensionsIt != extensionsIt_end; extensionsIt++)
      {
        QSharedPointer< BoWToken > extension = (*extensionsIt).second;
        LDEBUG << "BowGenerator:     extension: " << extension;
        LDEBUG << "BowGenerator:     extension: " << ((qSharedPointerDynamicCast< BoWTerm >(extension)==0)?(*extension):(*(qSharedPointerDynamicCast< BoWTerm >(extension))));

        bowTokenPositions(extensionPositions, extension);

      }

      LDEBUG << "BowGenerator: Building term";
      // position is the min of head min position and extension min position
      position=headPositions.begin()->first;
      if (position > extensionPositions.begin()->first)
      {
        position = extensionPositions.begin()->first;
      }
      LDEBUG << "BowGenerator:     position: " << position;

        // length is the length in original text: take end of term
      length=computeCompoundLength(headPositions,extensionPositions);
      LDEBUG << "BowGenerator:     length  : " << length;

      QSharedPointer< BoWTerm > complex( new BoWTerm( lemma, head->getCategory(), position, length) );
      complex->setVertex(head->getVertex());
      complex->setInflectedForm(infl);
      complex->setCategory(head->getCategory());
      complex->addPart(head);
//       delete head; head = 0;

      extensionsIt = extensions.begin();
      extensionsIt_end = extensions.end();
      for (; extensionsIt != extensionsIt_end; extensionsIt++)
      {
        QSharedPointer< BoWToken > extension = (*extensionsIt).second;
//         LDEBUG << "BowGenerator:     extension: " << (*(dynamic_cast< const BoWTerm* >(extension)));
        if ((*extensionsIt).first == 0)
          complex->addPart(extension);
        else
          complex->addPart((*extensionsIt).first,extension);
//         LDEBUG << "Built complex: " << ((dynamic_cast< BoWTerm* >(complex)==0)?(*complex):(*(dynamic_cast< BoWTerm* >(complex))));
        LDEBUG << "BowGenerator: Built complex: " << *complex;
      }

      QSharedPointer< BoWRelation > relation = createBoWRelationFor(vx, tgt, annotationData, posgraph,syntacticData);

      LDEBUG << "BowGenerator: Filling result with: " << *complex;
      result.push_back(std::make_pair(relation,complex));
    }

    // Mise a joueur de la pile d'iterateurs pour produire une nouvelle serie d'extensions
    LDEBUG << "BowGenerator: Stack updating...";
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
        LDEBUG << "BowGenerator: Stack filling...";
        for (uint64_t i = stack.size(); i < begins.size();i++)
        {
          stack.push_back(begins[i]);
        }
      }
    }
  }

  LDEBUG << "BowGenerator: == DONE buildTermFor " << vx << " (pointing on "<<tgt<<")";
  return result;
}

QSharedPointer< BoWRelation > BowGenerator::createBoWRelationFor(
    const AnnotationGraphVertex& vx,
    const AnnotationGraphVertex& tgt,
    const AnnotationData* annotationData,
    const LinguisticGraph& posgraph,
    const SyntacticData* syntacticData) const
{
  LIMA_UNUSED(posgraph);
  const DependencyGraph* depGraph = syntacticData->dependencyGraph();
  DUMPERLOGINIT;
  LDEBUG << "BowGenerator::createBoWRelationFor" << vx << tgt;
  QSharedPointer< BoWRelation > relation;
  if (vx != tgt 
      && annotationData->hasAnnotation(vx, tgt,
          Common::Misc::utf8stdstring2limastring("CompoundTokenAnnotation")) )
  {
    LDEBUG << "BowGenerator:     working on relation";
    const CompoundTokenAnnotation* annot = annotationData->annotation(vx,tgt, Common::Misc::utf8stdstring2limastring("CompoundTokenAnnotation")).pointerValue<CompoundTokenAnnotation>();
    if (annot != 0 && !annot->empty())
    {
      const ConceptModifier& modifier = (*annot)[0];
      StringsPoolIndex realizationIdx = modifier.getRealization();
      LimaString realization = Common::MediaticData::MediaticData::changeable().stringsPool(m_language)[realizationIdx];
      int type = modifier.getConceptType();
      relation = QSharedPointer< BoWRelation >(new BoWRelation(realization, type));
    }
    else
    {
      relation = QSharedPointer< BoWRelation >(new BoWRelation());
    }
    LinguisticGraphVertex vxTokVertex = *(annotationData->matches("cpd", vx, "PosGraph").begin());
    LinguisticGraphVertex tgtTokVertex = *(annotationData->matches("cpd", tgt, "PosGraph").begin());
    LDEBUG << "BowGenerator:     working vx  " << vxTokVertex;
    LDEBUG << "BowGenerator:     working tgt  " << tgtTokVertex;
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
  if (relation !=0)
  {
    LDEBUG << "BowGenerator:     relation : " << *relation;
  }
  return relation;
}


std::vector< std::pair< QSharedPointer< BoWRelation >, QSharedPointer< AbstractBoWElement > > > BowGenerator::createAbstractBoWElement(const LinguisticGraphVertex v,
  const LinguisticGraph& anagraph,
  const LinguisticGraph& posgraph,
  const uint64_t offsetBegin,
  const AnnotationData* annotationData,
  std::set<LinguisticGraphVertex>& visited,
  bool keepAnyway) const
{
  DUMPERLOGINIT;
  LDEBUG << "BowGenerator::createAbstractBoWElement for " << v;
  std::vector<std::pair< QSharedPointer< BoWRelation >, QSharedPointer< AbstractBoWElement > > > abstractBowEl;
  // Create bow tokens for specific entities created on the before PoS tagging 
  // analysis graph
  //std::set< uint64_t > anaVertices = annotationData->matches("PosGraph",v,"AnalysisGraph"); portage 32 64
  std::set< AnnotationGraphVertex > anaVertices = annotationData->matches("PosGraph",v,"AnalysisGraph");
  LDEBUG << "BowGenerator::createAbstractBoWElement " << v << " has " << anaVertices.size() << " matching vertices in analysis graph";

  // note: anaVertices size should be 0 or 1
  //for (std::set< uint64_t >::const_iterator anaVerticesIt = anaVertices.begin(); portage 32 64
  for (std::set< AnnotationGraphVertex >::const_iterator anaVerticesIt = anaVertices.begin();
  anaVerticesIt != anaVertices.end(); anaVerticesIt++)
  {
    LDEBUG << "BowGenerator::createAbstractBoWElement Looking at analysis graph vertex " << *anaVerticesIt;
    //std::set< uint64_t > matches = annotationData->matches("AnalysisGraph",*anaVerticesIt,"annot"); portage 32 64
    std::set< AnnotationGraphVertex > matches = annotationData->matches("AnalysisGraph",*anaVerticesIt,"annot");
    //for (std::set< uint64_t >::const_iterator it = matches.begin(); portage 32 64
    for (std::set< AnnotationGraphVertex >::const_iterator it = matches.begin();
         it != matches.end(); it++)
    {
      LDEBUG << "BowGenerator::createAbstractBoWElement Looking at annotation graph vertex " << *it;
      if (annotationData->hasAnnotation(*it, Common::Misc::utf8stdstring2limastring("SpecificEntity")))
      {
        QSharedPointer< BoWToken  > se = createSpecificEntity(v,*it, annotationData, anagraph, posgraph, offsetBegin, false);
        if (se != 0)
        {
          LDEBUG << "BowGenerator::createAbstractBoWElement created specific entity: " << QString::fromUtf8(se->getOutputUTF8String().c_str());
          se->setVertex(v);
          abstractBowEl.push_back(std::make_pair(QSharedPointer< BoWRelation >(0),se));
//           visited.insert(v);
          break;
        }
      }
    }
  }
  LDEBUG << "BowGenerator::createAbstractBoWElement move on the PosGraph annot matching test";

  // check if there is specific entities or compound tenses associated to v.
  // return them if any
  std::set< AnnotationGraphVertex > matches = annotationData->matches("PosGraph",v,"annot");
  LDEBUG << "BowGenerator::createAbstractBoWElement there are " << matches.size() << " annotation graph vertices matching the current PsGraph vertex " << v;
  for (std::set< AnnotationGraphVertex >::const_iterator it = matches.begin();
       it != matches.end(); it++)
  {
    AnnotationGraphVertex vx=*it;
    LDEBUG << "BowGenerator::createAbstractBoWElement Looking at annotation graph vertex " << vx;
    if (annotationData->hasAnnotation(*it, Common::Misc::utf8stdstring2limastring("SpecificEntity")))
    {
      QSharedPointer< BoWToken > se = createSpecificEntity(v,*it, annotationData, anagraph, posgraph, offsetBegin);
      if (se != 0)
      {
        LDEBUG << "BowGenerator::createAbstractBoWElement created specific entity: " << QString::fromUtf8(se->getOutputUTF8String().c_str());
        se->setVertex(v);
        abstractBowEl.push_back(std::make_pair(QSharedPointer< BoWRelation >(0),se));
//         visited.insert(v);
        return abstractBowEl;
      }
    }
    else if (annotationData->hasIntAnnotation(*it, Common::Misc::utf8stdstring2limastring("CpdTense")))
    {
      QSharedPointer< BoWToken > ct = createCompoundTense(*it, annotationData, anagraph, posgraph, offsetBegin, visited);
      if (ct != 0)
      {
        LDEBUG << "BowGenerator::createAbstractBoWElement created compound tense: " << *ct;
        ct->setVertex(v);
        abstractBowEl.push_back(std::make_pair(QSharedPointer< BoWRelation >(0),ct));
//         visited.insert(v);
        return abstractBowEl;
      }
    }
    else if (annotationData->hasStringAnnotation(*it, Common::Misc::utf8stdstring2limastring("Predicate")))
    {

      LDEBUG << "BowGenerator::createAbstractBoWElement Found a predicate in the PosGraph annnotation graph matching";
      QSharedPointer< BoWPredicate > bP=createPredicate(v, *it, annotationData, anagraph, posgraph, offsetBegin, visited, keepAnyway);
      if (bP!=0){
        LDEBUG << "BowGenerator::createAbstractBoWElement created a predicate" ;
        abstractBowEl.push_back(std::make_pair(QSharedPointer< BoWRelation >(0),bP));
  //         visited.insert(v);
        return abstractBowEl;
      }
    }
    else
    {
      LDEBUG << "BowGenerator::createAbstractBoWElement No SpecificEntity nor CpdTense nor Predicate found";
    }
  }


  // bow tokens have been created for specific entities on the before PoS 
  // tagging graph. return them
  if (!abstractBowEl.empty())
  {
    return abstractBowEl;
  }

  const FsaStringsPool& sp=Common::MediaticData::MediaticData::single().stringsPool(m_language);

  MorphoSyntacticData* data = get(vertex_data, posgraph, v);
  Token* token = get(vertex_token, posgraph, v);

  std::set<std::pair<StringsPoolIndex,LinguisticCode> > alreadyCreated;
  std::pair<StringsPoolIndex,LinguisticCode> predNormCode = std::make_pair(StringsPoolIndex(0),LinguisticCode(0));

  if (data!=0)
  {
    for (auto it=data->begin(); it!=data->end(); it++)
    {
      std::pair<StringsPoolIndex,LinguisticCode> normCode=std::make_pair(it->normalizedForm,m_microAccessor->readValue(it->properties));
      if (normCode != predNormCode)
      {
        if (alreadyCreated.find(normCode)==alreadyCreated.end()) 
        {
          if (keepAnyway || shouldBeKept(*it))
          {
            QSharedPointer< BoWToken > newbowtok( new BoWToken(sp[it->normalizedForm],
                                               m_macroAccessor->readValue(it->properties),
                                               offsetBegin+token->position(),
                                               token->length()));
            newbowtok->setVertex(v);
            newbowtok->setInflectedForm(token->stringForm());
            LDEBUG << "BowGenerator::createAbstractBoWElement created bow token: " << *newbowtok;
            abstractBowEl.push_back(std::make_pair(QSharedPointer< BoWRelation >(0),newbowtok));
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



bool BowGenerator::shouldBeKept(const LinguisticAnalysisStructure::LinguisticElement& elem) const
{
/*
  Critical function : comment logging messages
*/
  DUMPERLOGINIT;
  //  bool result = false;

  const FsaStringsPool& sp=Common::MediaticData::MediaticData::single().stringsPool(m_language);
  const LanguageData& ldata=static_cast<const LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language));
  if (m_useEmptyMacro && ldata.isAnEmptyMacroCategory(m_macroAccessor->readValue(elem.properties)))
  {
    LDEBUG << "BowGenerator: token ("
      << sp[elem.lemma] << "|"
      << elem.properties << ") not kept : macro category is empty ";
    return false;
  }


  if (m_useEmptyMicro && ldata.isAnEmptyMicroCategory(m_microAccessor->readValue(elem.properties)))
  {
    LDEBUG << "BowGenerator: token ("
      << sp[elem.lemma] << "|"
      << elem.properties << ") not kept : micro category is empty ";
    return false;
  }

   LDEBUG << "BowGenerator: check token (" << sp[elem.normalizedForm] << ")";
   if (m_useStopList && m_stopList!=0 && (m_stopList->find(sp[elem.normalizedForm]) != m_stopList->end()))
   {
     LDEBUG << "BowGenerator: token (" 
       << sp[elem.lemma] << "|" 
       << elem.properties << ") not kept : normalization " 
       << sp[elem.normalizedForm] 
       << " is in stoplist";
     return false;
   }

   LDEBUG << "BowGenerator: token (" 
     << sp[elem.lemma] << "|" 
     << elem.properties << "), normalization " 
     << sp[elem.normalizedForm] << " kept";

  return true;
}

bool BowGenerator::checkStopWordInCompound(
    QSharedPointer< Common::BagOfWords::BoWToken >& tok,
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

void BowGenerator::bowTokenPositions(BowGenerator::TokenPositions& res,
                                     const QSharedPointer< Common::BagOfWords::BoWToken > tok) const
{
  Common::Misc::PositionLengthList poslenlist=tok->getPositionLengthList();
  res.insert(poslenlist.begin(),poslenlist.end());
}

uint64_t BowGenerator::computeCompoundLength(
    const BowGenerator::TokenPositions& headTokPositions,
    const BowGenerator::TokenPositions& extensionPositions) const
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


QSharedPointer< BoWNamedEntity > BowGenerator::createSpecificEntity(
  const LinguisticGraphVertex& vertex,
  const AnnotationGraphVertex& v,
  const AnnotationData* annotationData,
  const LinguisticGraph& anagraph,
  const LinguisticGraph& posgraph,
  const uint64_t offset,
  bool frompos) const
{
  DUMPERLOGINIT;
  LINFO << "BowGenerator: createSpecificEntity ling:" << vertex <<"; annot:"<< v;
  if (!annotationData->hasAnnotation(v, Common::Misc::utf8stdstring2limastring("SpecificEntity")))
  {
    return QSharedPointer< BoWNamedEntity >(0);
  }
  const FsaStringsPool& sp=Common::MediaticData::MediaticData::single().stringsPool(m_language);

  const SpecificEntityAnnotation* se =
    annotationData->annotation(v, Common::Misc::utf8stdstring2limastring("SpecificEntity")).
      pointerValue<SpecificEntityAnnotation>();

  LDEBUG << "BowGenerator: specific entity type is " << se->getType();

  std::set< std::string > alreadyStored;

  // build BoWNamedEntity
  LimaString typeName("");
  try {
    typeName = MediaticData::single().getEntityName(se->getType());
  }
  catch (std::exception& e) {
    LERROR << "Undefined entity type " << se->getType();
    return QSharedPointer< BoWNamedEntity >(0);
  }
  LDEBUG << "BowGenerator: specific entity type name is " << typeName;
  // get the macro-category to use for this named entity 
  MorphoSyntacticData* data = get(vertex_data, posgraph, vertex);
  if (data->empty())
  {
    LERROR << "Empty data for vertex " << vertex << " at " << __FILE__ << ", line " << __LINE__;
    LERROR << "This is a bug. Returning null entity for" << se->getString() << typeName;
    return QSharedPointer< BoWNamedEntity >(0);

  }
  LinguisticCode category=m_macroAccessor->readValue(data->begin()->properties);

  QSharedPointer< BoWNamedEntity > bowNE( new
                          BoWNamedEntity(sp[getNamedEntityNormalization(v, annotationData)],
                                          category,
                                          se->getType(),
                                          offset+se->getPosition(),
                                          se->getLength()) );
  // add named entity parts
  std::vector<BowGenerator::NamedEntityPart> neParts = createNEParts(v, annotationData, anagraph, posgraph,frompos);
  if (neParts.empty())
  {
    LWARN << "No parts kept for named entity " << (*se).getString();
    // set named entity itself as part
    QSharedPointer< BoWToken > bowToken(new
      BoWToken(sp[getNamedEntityNormalization(v, annotationData)],
              category,
              offset+(*se).getPosition(),
              (*se).getLength()));
    Token* token = get(vertex_token, posgraph, vertex);
    bowToken->setInflectedForm(token->stringForm());

    bowNE->addPart(bowToken);
  }
  else {

    for (std::vector<BowGenerator::NamedEntityPart>::const_iterator
          p=neParts.begin(); p!=neParts.end(); p++) {
      //create a new BoWToken
      QSharedPointer< BoWToken > bowToken(new BoWToken((*p).lemma,(*p).category,
                                      offset+(*p).position,
                                      (*p).length));
      bowToken->setInflectedForm((*p).inflectedForm);
      LDEBUG << "BowGenerator: specific entity part infl " << (*p).inflectedForm;

      bowNE->addPart(bowToken);

    }
  }

  // add the features
  const Automaton::EntityFeatures& features=(*se).getFeatures();
  for (Automaton::EntityFeatures::const_iterator
        f=features.begin(),f_end=features.end();
      f!=f_end; f++)
  {
    bowNE->addFeature((*f).getName(),
                      (*f).getValueLimaString());
  }
  LDEBUG << "CreateSpecificEntity: created features " << QString::fromUtf8(bowNE->getFeaturesUTF8String().c_str());

  std::string elem = bowNE->getIdUTF8String();
  if (alreadyStored.find(elem) != alreadyStored.end())
  { // already stored
    LDEBUG << "BowGenerator: BoWNE already stored. Skipping it.";
    return QSharedPointer< BoWNamedEntity >(0);
  }
  else
  {
//     LDEBUG << "BowGenerator: created BoWNamedEntity " << bowNE->getOutputUTF8String();
    alreadyStored.insert(elem);
    return bowNE;
  }
  return bowNE;
}


QSharedPointer< BoWPredicate > BowGenerator::createPredicate(
    const LinguisticGraphVertex& lgv, const AnnotationGraphVertex& agv, const AnnotationData* annotationData, const LinguisticGraph& anagraph, const LinguisticGraph& posgraph, const uint64_t offset, std::set< LinguisticGraphVertex >& visited, bool keepAnyway) const
{
  DUMPERLOGINIT;
  LDEBUG << "BowGenerator::createPredicate ling:" << lgv << "; annot:" << agv;
  QSharedPointer< BoWPredicate > bowP(new BoWPredicate());

  Token* token = get(vertex_token, posgraph, lgv);
  bowP->setPosition(offset+token->position());
  bowP->setLength(token->length());

  // FIXME handle the ambiguous case when there is several class values for the predicate
  QStringList predicateIds=annotationData->stringAnnotation(agv,Common::Misc::utf8stdstring2limastring("Predicate")).split("|");
  if (predicateIds.size()>1)
  {
    LERROR << "BowGenerator::createPredicate Predicate has" << predicateIds.size() << "values:" << predicateIds;
  }
  // FIXME replace the hardcoded VerbNet by a value from configuration
  LWARN << "BowGenerator::createPredicate FIXME replace the hardcoded VerbNet by a value from configuration at" << __FILE__ << ", line"<< __LINE__;
  LimaString predicate=LimaString("VerbNet.%1").arg(predicateIds.first());
  try
  {
    EntityType predicateEntity= Common::MediaticData::MediaticData::single().getEntityType(predicate);
    LDEBUG << "BowGenerator::createPredicate  The role(s) related to "<< predicate << " is/are ";
    AnnotationGraph annotGraph=annotationData->getGraph();
    AnnotationGraphOutEdgeIt outIt, outIt_end;
    boost::tie(outIt, outIt_end) = boost::out_edges(agv, annotationData->getGraph());
    QMultiMap<Common::MediaticData::EntityType, QSharedPointer< AbstractBoWElement > > roles;
    const LimaString typeAnnot="SemanticRole";
    for (; outIt != outIt_end; outIt++)
    {
      // FIXME handle the ambiguous case when there is several values for each role
      const AnnotationGraphVertex semRoleVx=boost::target(*outIt, annotGraph);
      QStringList semRoleIds = annotationData->stringAnnotation(agv,semRoleVx,typeAnnot).split("|");
      if (semRoleIds.size()>1)
      {
        LERROR << "BowGenerator::createPredicate Role has" << semRoleIds.size() << "values:" << semRoleIds;
      }
      // FIXME replace the hardcoded VerbNet by a value from configuration
      LimaString semRole = LimaString("VerbNet.%1").arg(semRoleIds.first());
      LDEBUG << semRole;
      try
      {
        EntityType semRoleEntity = Common::MediaticData::MediaticData::single().getEntityType(semRole);
        std::set< LinguisticGraphVertex > posGraphSemRoleVertices = annotationData->matches("annot", semRoleVx,  "PosGraph");
        if (!posGraphSemRoleVertices.empty())
        {
          LinguisticGraphVertex posGraphSemRoleVertex = *(posGraphSemRoleVertices.begin());
          if (posGraphSemRoleVertex == lgv)
          {
            LERROR << "BowGenerator::createPredicate role vertex is the same as the trigger vertex. Abort this role.";
            continue;
          }
          LDEBUG << "BowGenerator::createPredicate Calling createAbstractBoWElement on PoS graph vertex" << posGraphSemRoleVertex;
          std::vector<std::pair<QSharedPointer< BoWRelation >, QSharedPointer< AbstractBoWElement > > > semRoleTokens = createAbstractBoWElement(posGraphSemRoleVertex, anagraph,posgraph, offset, annotationData, visited, keepAnyway);
          LDEBUG << "BowGenerator::createPredicate Created "<< semRoleTokens.size()<<"token for the role associated to " << predicate;
//               if (semRoleTokens[0].second!="")
          if (!semRoleTokens.empty())
          {
            roles.insert(semRoleEntity, semRoleTokens[0].second);
          }
        }
        else
        {
          LDEBUG << "BowGenerator::createPredicate Found no matching for the semRole in the annot graph";
        }
      }
      catch (const Lima::LimaException& e)
      {
        LDEBUG << "BowGenerator::createPredicate Unknown semantic role" << semRole << ";" << e.what();
      }
    }
    bowP->setPredicateType(predicateEntity);
    Common::MediaticData::EntityType pEntityType=bowP->getPredicateType();
    LDEBUG << "BowGenerator::createPredicate Created a Predicate for the verbal class " << Common::MediaticData::MediaticData::single().getEntityName(pEntityType);
    if (!roles.empty())
    {
      bowP->setRoles(roles);
      QMultiMap<Common::MediaticData::EntityType, QSharedPointer< AbstractBoWElement > >pRoles=bowP->roles();
      for (auto it = pRoles.begin();
            it != pRoles.end(); it++)
      {
        QSharedPointer< BoWToken> outputRoles=qSharedPointerDynamicCast<BoWToken>(it.value());
        if (outputRoles != 0)
        {
          LimaString roleLabel=Common::MediaticData::MediaticData::single().getEntityName(it.key());
          LDEBUG << "BowGenerator::createPredicate Associated "<< QString::fromUtf8(outputRoles->getOutputUTF8String().c_str()) << " to it" << "via the semantic role label "<< roleLabel ;
        }
      }
    }
    return bowP;
  }
  catch (const Lima::LimaException& e)
  {
    LERROR << "BowGenerator::createPredicate Unknown predicate" << predicate << ";" << e.what();
    return QSharedPointer< BoWPredicate >(0);
  }
}

QSharedPointer< BoWPredicate > BowGenerator::createPredicate(
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
  DUMPERLOGINIT;
  LDEBUG << "BowGenerator::createPredicate " << lgvs << agvs << agvt << annot.type().c_str();
  QSharedPointer< BoWPredicate > bowP( new BoWPredicate() );

  EntityType predicateEntity = Common::MediaticData::MediaticData::single().getEntityType(QString::fromUtf8(annot.type().c_str()));
  bowP->setPredicateType(predicateEntity);

  bowP->setPosition(0);
  bowP->setLength(0);

  std::vector<AnnotationGraphVertex> vertices;
  vertices.push_back(agvs);
  vertices.push_back(agvt);
  LDEBUG << "BowGenerator::createPredicate  The role(s) related to "<< annot.type().c_str() << " is/are ";
  QMultiMap<Common::MediaticData::EntityType, QSharedPointer< AbstractBoWElement > > roles;
  const LimaString typeAnnot="SemanticRole";
  for (auto verticesIt = vertices.begin(); verticesIt != vertices.end(); verticesIt++)
  {
    const AnnotationGraphVertex semRoleVx = *verticesIt;
    std::set< LinguisticGraphVertex > anaGraphSemRoleVertices = annotationData->matches("annot", semRoleVx,  "AnalysisGraph");
    if (!anaGraphSemRoleVertices.empty())
    {
      LinguisticGraphVertex anaGraphSemRoleVertex = *anaGraphSemRoleVertices.begin();
      std::set< LinguisticGraphVertex > posGraphSemRoleVertices = annotationData->matches("AnalysisGraph", anaGraphSemRoleVertex,  "PosGraph");
      if (!posGraphSemRoleVertices.empty())
      {
        LinguisticGraphVertex posGraphSemRoleVertex = *(posGraphSemRoleVertices.begin());
        if (posGraphSemRoleVertex == lgvs)
        {
          LERROR << "BowGenerator::createPredicate role vertex is the same as the trigger vertex. Abort this role.";
          continue;
        }
        LDEBUG << "BowGenerator::createPredicate Calling createAbstractBoWElement on PoS graph vertex" << posGraphSemRoleVertex;
        std::vector<std::pair<QSharedPointer< BoWRelation >, QSharedPointer< AbstractBoWElement > > > semRoleTokens = createAbstractBoWElement(posGraphSemRoleVertex, anagraph,posgraph, offset, annotationData, visited, keepAnyway);
        LDEBUG << "BowGenerator::createPredicate Created "<< semRoleTokens.size()<<"token for the role associated to " << annot.type().c_str();
        if (!semRoleTokens.empty())
        {
          EntityType semRoleEntity;
          roles.insert(semRoleEntity, semRoleTokens[0].second);
        }
      }
      else
      {
        LDEBUG << "BowGenerator::createPredicate Found no matching for the semRole in the annot graph";
      }
    }
  }
  LDEBUG << "BowGenerator::createPredicate Created a Predicate for the semantic relation"
      << predicateEntity << Common::MediaticData::MediaticData::single().getEntityName(predicateEntity);
  if (!roles.empty())
  {
    bowP->setRoles(roles);
    for (auto it = roles.begin(); it != roles.end(); it++)
    {
      QSharedPointer< BoWToken > outputRoles=qSharedPointerDynamicCast<BoWToken>(it.value());
      if (outputRoles != 0)
      {
        LimaString roleLabel = it.key().isNull() ? QString() : Common::MediaticData::MediaticData::single().getEntityName(it.key());
        LDEBUG << "BowGenerator::createPredicate Associated "<< QString::fromUtf8(outputRoles->getOutputUTF8String().c_str()) << " to it" << "via the semantic role label "<< roleLabel ;
      }
    }
  }
  return bowP;
}


StringsPoolIndex BowGenerator::getNamedEntityNormalization(
    const AnnotationGraphVertex& v,
    const AnnotationData* annotationData) const
{
  if (!annotationData->hasAnnotation(v, Common::Misc::utf8stdstring2limastring("SpecificEntity")))
  {
    DUMPERLOGINIT;
    LDEBUG << "BowGenerator::getNamedEntityNormalization: no SpecificEntity annotation for " << v << " ; return 0";
    return static_cast<StringsPoolIndex>(0);
  }
  DUMPERLOGINIT;
  LDEBUG << "BowGenerator::getNamedEntityNormalization: m_NEnormalization is " << m_NEnormalization;
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
  LDEBUG << "BowGenerator::getNamedEntityNormalization return " << normalizedForm;
  return normalizedForm;
}

std::vector<BowGenerator::NamedEntityPart> BowGenerator::createNEParts(
    const AnnotationGraphVertex& v,
    const AnnotationData* annotationData,
    const LinguisticGraph& anagraph,
    const LinguisticGraph& posgraph,
    bool frompos) const
{
  DUMPERLOGINIT;
  const LinguisticGraph& graph = (frompos?posgraph:anagraph);
  const FsaStringsPool& sp=Common::MediaticData::MediaticData::single().stringsPool(m_language);

  const SpecificEntities::SpecificEntityAnnotation* namedEntity =
    annotationData->annotation(v,Common::Misc::utf8stdstring2limastring("SpecificEntity"))
      .pointerValue< SpecificEntityAnnotation >();
  std::vector<BowGenerator::NamedEntityPart> parts;


  bool useOnePart(false);      // use one part, tagged as proper noun
  bool useDefaultParts(true); // use parts of named entity as they are in the graph
  uint64_t useCategory((uint64_t)-1);   // use this category for each of the parts (NP or NC)
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
    for (std::vector< LinguisticGraphVertex>::const_iterator m(namedEntity->m_vertices.begin());
         m != namedEntity->m_vertices.end(); m++)
    {
      const Token* token = get(vertex_token, graph, *m);
      const MorphoSyntacticData* data = get(vertex_data, graph, *m);

      if (data!=0 && !data->empty())
      {
        const LinguisticElement& elem=*(data->begin());

        if (! m_keepAllNamedEntityParts &&
              ! shouldBeKept(elem))
        {
          LDEBUG << "BowGenerator: part of named entity not kept: " << token->stringForm();
          continue;
        }

        uint64_t category(0);
        if (useCategory != (uint64_t)-1)
        {
          category=useCategory;
        }
        else
        {
          category=m_macroAccessor->readValue(elem.properties);
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

QSharedPointer< BoWToken > BowGenerator::createCompoundTense(
    const AnnotationGraphVertex& v,
    const AnnotationData* annotationData,
    const LinguisticGraph& anagraph,
    const LinguisticGraph& posgraph,
    const uint64_t offset,
    std::set<LinguisticGraphVertex>& visited) const
{
  DUMPERLOGINIT;
  LDEBUG << "BowGenerator: createCompoundTense " << v;
  if (!annotationData->hasIntAnnotation(v, Common::Misc::utf8stdstring2limastring("CpdTense")))
  {
    return QSharedPointer< BoWToken >(0);
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
    return QSharedPointer< BoWToken >(0);
  }
  //   parcourir les liens entrants su v annotes par Aux et PastPart
  //   si la source est annotee CpdTense, rappeler recursivement
  //   createCompoundTense ; sinon, creer un BoWToken simple
  // creer eventuellement un compound tense pour chaque ;
  QSharedPointer< BoWToken> head (0), extension (0);
  if (annotationData->hasIntAnnotation(ppVertex, Common::Misc::utf8stdstring2limastring("CpdTense")))
  {
    head = createCompoundTense(ppVertex, annotationData, anagraph, posgraph, offset, visited);
  }
  else
  {
    LinguisticGraphVertex ppTokVertex =
      *(annotationData->matches("annot", ppVertex, "PosGraph").begin());

    std::vector< std::pair<QSharedPointer< BoWRelation >, QSharedPointer< AbstractBoWElement > > > ppBoWTokens = createAbstractBoWElement(ppTokVertex, anagraph, posgraph, offset, annotationData, visited, true);
    if (ppBoWTokens.empty())
    {
      return QSharedPointer< BoWToken >(0);
    }
    else
    {
      head = qSharedPointerDynamicCast<BoWToken>(ppBoWTokens.back().second);
      ppBoWTokens.pop_back();
    }
  }
  if (head == 0)
  {
    return QSharedPointer< BoWToken >(0);
  }
  if (annotationData->hasIntAnnotation(auxVertex, Common::Misc::utf8stdstring2limastring("CpdTense")))
  {
    extension = createCompoundTense(auxVertex, annotationData, anagraph, posgraph, offset, visited);
  }
  else
  {
    LinguisticGraphVertex auxTokVertex =
      *(annotationData->matches("annot", auxVertex, "PosGraph").begin());

    std::vector<std::pair<QSharedPointer< BoWRelation >, QSharedPointer< AbstractBoWElement > > > auxBoWTokens = createAbstractBoWElement(auxTokVertex, anagraph, posgraph, offset, annotationData, visited, true);
    if (auxBoWTokens.empty())
    {
      return QSharedPointer< BoWToken >(0);
    }
    else
    {
      extension = qSharedPointerDynamicCast<BoWToken>(auxBoWTokens.back().second);
      auxBoWTokens.pop_back();
    }
  }
  if (extension == 0 && head != 0)
  {
    return QSharedPointer< BoWToken >(0);
  }
  // construire un BoWTerm avec le pp pour tete et l'aux pour extension
  LimaString lemma, infl;
  
  QSharedPointer< BoWToken > complex(
    new BoWToken( 
                  head->getLemma(), 
                  head->getCategory(), 
                  extension->getPosition(), 
                  (head->getPosition()+head->getLength()-extension->getPosition())));
  complex->setVertex(head->getVertex());
  complex->setInflectedForm(head->getInflectedForm());
  LDEBUG << "BowGenerator: Built complex: " << *complex;

  return complex;
}

} // AnalysisDumper

} // LinguisticProcessing

} // Lima
