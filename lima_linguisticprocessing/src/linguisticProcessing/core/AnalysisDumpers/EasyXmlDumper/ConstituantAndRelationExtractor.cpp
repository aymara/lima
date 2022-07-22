// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/**
 *
 * @file       ConstituantAndRelationExtractor.h
 * @author     Damien Nouvel <Damien.Nouvel@cea.fr>

 *             Copyright (C) 2004 by CEA LIST
 * @author     Gael de Chalendar <Gael.de-Chalendar@cea.fr>
 * @author     Benoit Mathieu <mathieub@zoe.cea.fr>
 * @date       Mon Oct 07 2008
 * Project     s2lp
 *
 * @brief      extracts forms and relations from boost graph (origninally, from XML file)
 *
 */

#include "ConstituantAndRelationExtractor.h"
#include "easyXmlDumper.h"

#include "common/LimaCommon.h"
#include "common/Data/strwstrtools.h"
#include "common/time/traceUtils.h"
#include "linguisticProcessing/common/PropertyCode/PropertyManager.h"
#include "linguisticProcessing/common/PropertyCode/PropertyAccessor.h"
#include "common/MediaticData/mediaticData.h"
#include "linguisticProcessing/core/SyntacticAnalysis/SyntacticData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticDataUtils.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <set>

#include <boost/tokenizer.hpp>

using namespace Lima;

using namespace std;
//using namespace boost;
using namespace boost::tuples;


using namespace Lima::Common::PropertyCode;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::LinguisticProcessing::SyntacticAnalysis;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::LinguisticProcessing::MorphologicAnalysis;
using namespace Lima::LinguisticProcessing::SpecificEntities;

typedef boost::color_traits<boost::default_color_type> Color;

namespace Lima {
namespace LinguisticProcessing {
namespace AnalysisDumpers {
namespace EasyXmlDumper {

ConstituantAndRelationExtractor::ConstituantAndRelationExtractor(
    const Common::PropertyCode::PropertyCodeManager* propertyCodeManager) :
  m_macroA(propertyCodeManager->getPropertyAccessor("MACRO")),
  m_macroPm(propertyCodeManager->getPropertyManager("MACRO")),
  m_microA(propertyCodeManager->getPropertyAccessor("MICRO")),
  m_microPm(propertyCodeManager->getPropertyManager("MICRO"))
{}

ConstituantAndRelationExtractor::~ConstituantAndRelationExtractor()
{}

#undef min
#undef max

void ConstituantAndRelationExtractor::visitBoostGraph(const LinguisticGraphVertex& v,
                                                      const LinguisticGraphVertex& end,
                                                      const LinguisticGraph& anaGraph,
                                                      const LinguisticGraph& posGraph,
                                                      const AnnotationData& annotationData,
                                                      const SyntacticData& syntacticData,
                                                      std::map< LinguisticAnalysisStructure::Token*, uint64_t >& fullTokens,
                                                      std::vector< bool >& alreadyDumpedTokens,
                                                      const MediaId& language)
{

  DUMPERLOGINIT;
  LDEBUG << "ConstituantAndRelationExtractor:: begin visiting vertex " << v;

  LinguisticGraphOutEdgeIt itLing, itLing_end;
  boost::tie(itLing, itLing_end) = out_edges(v, posGraph);
  if(v != end)
  {
    for (; itLing != itLing_end; itLing++)
    {
      const auto& nextVertex = target(*itLing, posGraph);
      LDEBUG << "ConstituantAndRelationExtractor:: visiting out edge " << nextVertex;
      visitBoostGraph(nextVertex,
                      end,
                      anaGraph,
                      posGraph,
                      annotationData,
                      syntacticData,
                      fullTokens,
                      alreadyDumpedTokens,
                      language);
    }
  }

  auto forme = extractVertex(v, posGraph, true, fullTokens,
                             alreadyDumpedTokens, language);

  if (forme == 0)
    return;

  LDEBUG << "ConstituantAndRelationExtractor:: insert form in index "
          << forme->id;
  m_formesIndex[forme->id] = forme;
  m_positionsFormsIds.insert(std::make_pair(forme->poslong.position,
                                            forme->id));
  m_vertexToFormeIds.insert(std::make_pair(v,forme->id));
  m_formeIdsToVertex.insert(std::make_pair(forme->id,v));

  // Looking for compound tense in annotation data
  std::set<AnnotationGraphVertex> vAnnot = annotationData.matches("PosGraph",
                                                                  v, "annot");
  if (!vAnnot.empty())
  {
    std::set<AnnotationGraphVertex>::const_iterator vAnnotIt, vAnnotIt_end;
    vAnnotIt = vAnnot.begin();
    vAnnotIt_end = vAnnot.end();
    for (; vAnnotIt != vAnnotIt_end; vAnnotIt++)
    {
      // if corresponding AnnotationGraph vertex, link it
      m_posAnnotMatching.insert(std::make_pair(forme->poslong.position,*vAnnotIt));
      m_annotPosMatching.insert(std::make_pair(*vAnnotIt,forme->poslong.position));
      if(annotationData.hasIntAnnotation(*vAnnotIt, QString::fromUtf8("CpdTense")))
      {
        bool foundCpdAux = false, foundCpdPp = false;
        AnnotationGraphVertex auxVertex, ppVertex;
        auxVertex = ppVertex = std::numeric_limits< AnnotationGraphVertex >::max();
        AnnotationGraphOutEdgeIt vAnnotOutIt, vAnnotOutIt_end;
        boost::tie(vAnnotOutIt, vAnnotOutIt_end) = boost::out_edges(*vAnnotIt, annotationData.getGraph());
        for (; vAnnotOutIt != vAnnotOutIt_end; vAnnotOutIt++)
        {
          if (annotationData.hasIntAnnotation(*vAnnotOutIt, QString::fromUtf8("Aux")))
          {
            auxVertex = boost::target(*vAnnotOutIt, annotationData.getGraph());
            if(auxVertex != 0) foundCpdAux = true;
          }
          else if(annotationData.hasIntAnnotation(*vAnnotOutIt, QString::fromUtf8("PastPart")))
          {
            ppVertex = boost::target(*vAnnotOutIt, annotationData.getGraph());
            if(ppVertex != 0) foundCpdPp = true;          }
        }
        if(foundCpdAux && foundCpdPp)
        {
          auto auxV = *(annotationData.matches("annot", auxVertex, "PosGraph").begin());
          visitBoostGraph(auxV,
                          end,
                          anaGraph,
                          posGraph,
                          annotationData,
                          syntacticData,
                          fullTokens,
                          alreadyDumpedTokens,
                          language);
          auto ppV = *(annotationData.matches("annot", ppVertex, "PosGraph").begin());
          visitBoostGraph(ppV,
                          end,
                          anaGraph,
                          posGraph,
                          annotationData,
                          syntacticData,
                          fullTokens,
                          alreadyDumpedTokens,
                          language);
          if(m_formesIndex[auxV] != 0 && m_formesIndex[ppV] != 0)
          {
            LDEBUG << "ConstituantAndRelationExtractor:: register compound tense "
                    << *vAnnotIt << ", " << forme->forme;
            m_compoundTenses[*vAnnotIt] = std::make_pair(auxV, ppV);
          }
        }
      }
    }
  }

  // Looking for idiomatic expressions in analysis data
  vAnnot = annotationData.matches("PosGraph", v, "AnalysisGraph");
  if (!vAnnot.empty())
  {
    std::set<AnnotationGraphVertex>::const_iterator vAnnotIt, vAnnotIt_end;
    vAnnotIt = vAnnot.begin();
    vAnnotIt_end = vAnnot.end();
    for (; vAnnotIt != vAnnotIt_end; vAnnotIt++)
    {

      LimaString idiomExprLimaString = QString::fromUtf8("IdiomExpr");
      if(annotationData.hasAnnotation(*vAnnotIt, idiomExprLimaString))
      {
        LDEBUG << "ConstituantAndRelationExtractor:: found idiomatic " << *vAnnotIt << ", " << forme->forme;
        splitCompoundAnalysisAnnotation<IdiomaticExpressionAnnotation>(
          *vAnnotIt,
          *forme,
          idiomExprLimaString,
          annotationData,
          anaGraph,
          fullTokens,
          alreadyDumpedTokens,
          language);
      }

      LimaString seLimaString = QString::fromUtf8("SpecificEntity");
      if(annotationData.hasAnnotation(*vAnnotIt, seLimaString))
      {
        LDEBUG << "ConstituantAndRelationExtractor:: found specific entity " << *vAnnotIt << ", " << forme->forme;
        splitCompoundAnalysisAnnotation<SpecificEntityAnnotation>(
          *vAnnotIt,
          *forme,
          seLimaString,
          annotationData,
          anaGraph,
          fullTokens,
          alreadyDumpedTokens,
          language);
      }

    }
  }

  const DependencyGraph* depGraph = syntacticData.dependencyGraph();
  DependencyGraphVertex depv = syntacticData.depVertexForTokenVertex(v);
  DependencyGraphOutEdgeIt itDep, itDep_end;
  boost::tie(itDep, itDep_end) = out_edges(depv, *depGraph);
  for (; itDep != itDep_end; itDep++)
  {
    Relation* relation = extractEdge(*itDep, posGraph, *depGraph, fullTokens, syntacticData, language);
    if(relation != 0)
    {
      forme->m_outRelations.push_back(relation);
    }
  }

  LDEBUG << "ConstituantAndRelationExtractor:: end visiting vertex " << v;

}

//***********************************************************************
// extract functions
//***********************************************************************

Forme* ConstituantAndRelationExtractor::extractVertex(const LinguisticGraphVertex& v,
                                                      const LinguisticGraph& graph,
                                                      bool checkFullTokens,
                                                      std::map< LinguisticAnalysisStructure::Token*, uint64_t >& fullTokens,
                                                      std::vector< bool >& alreadyDumpedTokens,
                                                      MediaId language)
{

  FsaStringsPool& sp = Common::MediaticData::MediaticData::changeable().stringsPool(language);

  Token* token = get(vertex_token, graph, v);
  DUMPERLOGINIT;
  if(token == 0)
    return 0;
  if(checkFullTokens)
  {
    LDEBUG << "ConstituantAndRelationExtractor:: check token " << v  << "("
           << token->stringForm() << ")";
    auto tokenIter = fullTokens.find(token);
    if(tokenIter != fullTokens.end())
    {
      uint64_t tokenId = tokenIter->second;
      if(alreadyDumpedTokens[tokenId])
        return 0;
      alreadyDumpedTokens[tokenId] = true;
    }
  }

  LDEBUG << "ConstituantAndRelationExtractor:: extract vertex " << v << "("
         << token->stringForm() << ")";
  Forme* forme = new Forme();
  forme->id = v;
  forme->forme = Common::Misc::limastring2utf8stdstring(token->stringForm());

  MorphoSyntacticData* data = get(vertex_data, graph, v);
  if(data != 0)
  {

    const PropertyCodeManager pcm = static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager();
    const PropertyAccessor macroA = pcm.getPropertyAccessor("MACRO");
    const PropertyManager macroPm = pcm.getPropertyManager("MACRO");
    const PropertyAccessor microA = pcm.getPropertyAccessor("MICRO");
    const PropertyManager microPm = pcm.getPropertyManager("MICRO");

    std::vector<LinguisticElement>::const_iterator itForms = (*data).begin();
    if(itForms != (*data).end())
    {

      LDEBUG << "ConstituantAndRelationExtractor:: found morphosyntactic data ";
      forme->inflForme = Common::Misc::limastring2utf8stdstring(sp[itForms->inflectedForm]);

      forme->poslong.position = token->position();
      forme->poslong.longueur = token->length();

      forme->macro = macroPm.getPropertySymbolicValue(macroA.readValue(itForms->properties));
      forme->micro = microPm.getPropertySymbolicValue(microA.readValue(itForms->properties));

    }

    itForms++;
    if(itForms != (*data).end())
    {
      LWARN << "ConstituantAndRelationExtractor:: ommitting multiple morphosyntactic data ";
    }

  }

  return forme;

}

Relation* ConstituantAndRelationExtractor::extractEdge(const LinguisticGraphEdge& e,
                                                       const LinguisticGraph& /*posGraph*/,
                                                       const DependencyGraph& depGraph,
                                                       std::map< LinguisticAnalysisStructure::Token*, uint64_t >& /*fullTokens*/,
                                                       const SyntacticData& syntacticData,
                                                       MediaId language)
{

  CEdgeDepRelTypePropertyMap relTypeMap = get(edge_deprel_type, depGraph);
  std::string relationName = static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getSyntacticRelationName(relTypeMap[e]);

  DUMPERLOGINIT;
  LDEBUG << "ConstituantAndRelationExtractor:: extract dependency " << relationName;

  uint64_t srcTokenId = syntacticData.tokenVertexForDepVertex(source(e, depGraph));
  if(srcTokenId == 0)
    return 0;

  uint64_t tgtTokenId = syntacticData.tokenVertexForDepVertex(target(e, depGraph));
  if(tgtTokenId == 0)
    return 0;

  // Check for duplicate relations
  for(std::vector<Relation*>::iterator relationsIt = m_outRelations.begin(), relationsIt_end = m_outRelations.end();
      relationsIt != relationsIt_end;
      relationsIt++)
  {
    Relation* rel = *relationsIt;
    if(rel->type == relationName &&
       rel->srcVertex == srcTokenId &&
       (rel->tgtVertex == tgtTokenId ||
        (rel->secondaryVertex != 0 && rel->secondaryVertex == tgtTokenId)
       )
      )
    {
      LDEBUG << "ConstituantAndRelationExtractor:: found duplicate relation";
      return 0;
    }
  }

  Relation* relation = new Relation;
  relation->srcVertex = srcTokenId;
  relation->tgtVertex = tgtTokenId;
  relation->type = relationName;

  // Special case for coordination
  if(relation->type == "COORD1" || relation->type == "COORD2")
  {
    std::vector<Relation*>::iterator it, it_toErase;
    it_toErase = m_outRelations.end();
    for(it = m_outRelations.begin(); it != m_outRelations.end(); it++)
    {
      if( ( (*it)->type == "COORD1" &&  relation->type == "COORD2" )
      || ( (*it)->type == "COORD2" &&  relation->type == "COORD1") )
      {
        if((*it)->srcVertex == relation->tgtVertex)
        {
          LDEBUG << "ConstituantAndRelationExtractor:: found coord second edge";
          (*it)->secondaryVertex = relation->srcVertex;
          delete relation;
          return 0;
        }
        else if((*it)->tgtVertex == relation->srcVertex)
        {
          LDEBUG << "ConstituantAndRelationExtractor:: found coord second edge";
          relation->secondaryVertex = (*it)->srcVertex;
          it_toErase = it;
        }
      }
    }
    if(it_toErase != m_outRelations.end())
    {
      Forme* srcForme = m_formesIndex[(*it_toErase)->srcVertex];
      if(srcForme != 0)
      {
        std::vector<Relation*>::iterator formIt, formIt_toErase;
        formIt_toErase = srcForme->m_outRelations.end();
        for(formIt = srcForme->m_outRelations.begin(); formIt != srcForme->m_outRelations.end(); formIt++)
        {
          if(*formIt == *it_toErase)
          {
            formIt_toErase = formIt;
          }
        }
        if(formIt_toErase != srcForme->m_outRelations.end())
        {
          srcForme->m_outRelations.erase(formIt_toErase);
        }
      }
      m_outRelations.erase(it_toErase);
      LDEBUG << "ConstituantAndRelationExtractor:: erased coord second edge";
    }
  }

  m_outRelations.push_back(relation);
  return relation;

}


/*
char* annotId = Common::Misc::limastring2utf8stdstring(attrs.getValue(Common::Misc::limastring2utf8stdstring("k")));
char* posId = Common::Misc::limastring2utf8stdstring(attrs.getValue(Common::Misc::limastring2utf8stdstring("v")));
std::cout << "pos annot matching: " << atoi(posId) << " / " << atoi(annotId) << std::endl;
m_posAnnotMatching.insert(std::make_pair(atoi(posId),atoi(annotId)));
m_annotPosMatching.insert(std::make_pair(atoi(annotId),atoi(posId)));
*/

/**
  * @brief construit les groupes syntaxiques apres la lecture du graphe
  */
void ConstituantAndRelationExtractor::constructionDesGroupes()
{
  std::set< uint64_t > formesDone;
  std::map<uint64_t, uint64_t>::const_iterator formesIt, formesIt_end;
  formesIt = m_positionsFormsIds.begin();
  formesIt_end = m_positionsFormsIds.end();

  DUMPERLOGINIT;
  LDEBUG << "ConstituantAndRelationExtractor:: construction des groupes";

  for (; formesIt != formesIt_end; formesIt++)
  {
    Groupe* newGrp = 0;
    uint64_t formeId = (*formesIt).second;
    if (formesDone.find(formeId) == formesDone.end())
    {
      formesDone.insert(formeId);
      Forme* forme = m_formesIndex[formeId];
      if(forme == 0)
      {
        LWARN << "ConstituantAndRelationExtractor:: form not found in index: " << formeId;
        continue;
      }
      LDEBUG << "ConstituantAndRelationExtractor:: construction des groupes " << forme->forme << "(" << forme->macro << "/" << forme->micro << ")";
      if (addToGroupIfIsInsideAGroup(forme))
      {
        LDEBUG << "ConstituantAndRelationExtractor:: already inserted ";
        continue;
      }
      // always follow SECOMPOUND as it is a specific entity composition
      std::set<std::string> relationsToFollow;
      relationsToFollow.insert("SECOMPOUND");
      std::set<std::string> relationsToFollow2;
      relationsToFollow2.insert("SECOMPOUND");
      // verbe non pointe par une relation SujInv
      if ( (forme->macro == "V") && !(forme->hasInRelation("SujInv") )  && !(forme->hasOutRelation("SUBADJPOST") ) )
      {
        LDEBUG << "ConstituantAndRelationExtractor:: verbe non pointe par une relation SujInv";
        createGroupe(forme, relationsToFollow, "NV", true);
      }
      else if ( forme->micro == "ADV" )
      {
        createGroupe(forme, relationsToFollow, "GR", true);
      }
      else if (forme->micro == "CLS")
      {
        relationsToFollow.insert("SujInv");
        relationsToFollow.insert("TIl");
        // relationsToFollow.insert("aux");
        newGrp = createGroupe(forme, relationsToFollow, "NV");
        if(newGrp == 0)
        {
          newGrp = createGroupe(forme, relationsToFollow, "GN", true);
        }
      }
      else if (forme->macro == "DET"  || forme->macro == "ADJ" )
      {
        relationsToFollow.insert("CodAnaph"); // "le veut" dans "a qui le veut."
        LDEBUG << "ConstituantAndRelationExtractor:: construction de groupe déterminant";
        newGrp = createGroupe(forme, relationsToFollow, "NV");
        if (newGrp == 0)
        {
          relationsToFollow.insert("PREPSUB");
          relationsToFollow.insert("PrepPron");
          relationsToFollow.insert("PrepDetInt");
          relationsToFollow.insert("DetIntSub");
          relationsToFollow.insert("DETSUB");
          relationsToFollow.insert("ADJPRENSUB");
          relationsToFollow.insert("ADVADJ");
          relationsToFollow.insert("ADVADV");
          relationsToFollow.insert("PrepPronRelCa");
          relationsToFollow.insert("SUBSUBJUX;NPP;NPP");
//           relationsToFollow.insert("SUBSUBJUX;U;NC");
          LDEBUG << "ConstituantAndRelationExtractor:: insert '" << forme->forme << "' in GP group";
          newGrp = createGroupe(forme, relationsToFollow, "GP");

          if(newGrp == 0)
          {
            relationsToFollow2.insert("DETSUB");
            relationsToFollow2.insert("DetAdj");
            relationsToFollow2.insert("ADJPRENSUB");
            relationsToFollow2.insert("ADVADJ");
            relationsToFollow2.insert("ADVADV");
            relationsToFollow2.insert("COORD1;CC;ADJ");
            relationsToFollow2.insert("COORD2;CC;ADJ");
            LDEBUG << "ConstituantAndRelationExtractor:: insert '" << forme->forme << "' in GN group";
            createGroupe(forme, relationsToFollow2, "GN");
          }

        }
      }
/*
      else if ( forme->micro == "ADV" && forme->hasOutRelation("AdvSub") )
      {
        relationsToFollow.insert("AdvSub");
        // relationsToFollow.insert("aux");
        newGrp = createGroupe(forme, relationsToFollow, "GP");
        if (newGrp == 0)
        {
          LDEBUG << "ConstituantAndRelationExtractor:: insert '" << forme->forme << "' in GR group";
          newGrp = createGroupe(forme, relationsToFollow, "GR");
        }
      }
*/
      else if ( forme->micro == "ADV" )
      {
        newGrp = createGroupe(forme, relationsToFollow, "GR", true);
      }
      else if ( forme->macro == "NP" )
      {
        relationsToFollow.insert("SUBSUBJUX;NP;NP");
        relationsToFollow.insert("SUBADJPOST");
        newGrp = createGroupe(forme, relationsToFollow, "GN", true);
      }
      else if ( forme->micro == "DETWH" )
      {
        relationsToFollow.insert("DETSUB");
        relationsToFollow.insert("DetIntSub");
        relationsToFollow.insert("ADJPRENSUB");
        newGrp = createGroupe(forme, relationsToFollow, "GN", true);
      }
      else if ( forme->micro == "NC" && forme->hasInRelation("SUBSUBJUX") )
      {
        relationsToFollow.insert("SUBSUBJUX");
        // relationsToFollow.insert("aux");
        newGrp = createGroupe(forme, relationsToFollow, "GN", true);
      }
      else if ( forme->micro == "PROREL" )
      {
        relationsToFollow.insert("COMPADV");
        newGrp = createGroupe(forme, relationsToFollow, "GN", true);
      }
      else if ( forme->micro == "PROREL"
                || forme->micro == "NC" || forme->micro == "CLS" )
      {
        newGrp = createGroupe(forme, relationsToFollow, "GN", true);
      }
      else if ( forme->micro == "P" && forme->hasOutRelation("PrepPronCliv")
                && !forme->hasOutRelation("PREPSUB") )
      {
        newGrp = createGroupe(forme, relationsToFollow, "NV", true);
      }
      else if (forme->macro == "PREP" || forme->macro == "DET" )
      {
        relationsToFollow.insert("PREPSUB");
        relationsToFollow.insert("PrepPron");
        relationsToFollow.insert("PrepDetInt");
        relationsToFollow.insert("DetIntSub");
        relationsToFollow.insert("DETSUB");
        relationsToFollow.insert("ADJPRENSUB");
        relationsToFollow.insert("ADVADJ");
        relationsToFollow.insert("ADVADV");
        relationsToFollow.insert("PrepPronRelCa");
        relationsToFollow.insert("PrepPronRel");
        relationsToFollow.insert("PrepAdv");
        relationsToFollow.insert("COORD1");
        relationsToFollow.insert("COORD2");
        relationsToFollow.insert("SUBSUBJUX;NPP;NPP");
//         relationsToFollow.insert("SUBSUBJUX;U;U");
        newGrp = createGroupe(forme, relationsToFollow, "GP");

        if(newGrp == 0)
        {
          relationsToFollow2.insert("PrepInf");
          relationsToFollow2.insert("Neg");
          relationsToFollow2.insert("NePas");
          relationsToFollow2.insert("ADVADV");
          relationsToFollow2.insert("AdvVerbe");
          relationsToFollow2.insert("PronReflVerbe");
          relationsToFollow2.insert("CodPrev");
          relationsToFollow2.insert("AuxCplPrev");
          newGrp = createGroupe(forme, relationsToFollow2, "PV");
        }

      }
      else if (forme->micro == "ADV" || forme->micro == "CLS" || forme->micro == "CLO" )
      {
        relationsToFollow.insert("Neg");
        relationsToFollow.insert("NePas");
        relationsToFollow.insert("PronSujVerbe");
        relationsToFollow.insert("PronReflVerbe");
        relationsToFollow.insert("CodPrev");
        relationsToFollow.insert("CoiPrev");
        relationsToFollow.insert("AuxCplPrev");
        newGrp = createGroupe(forme, relationsToFollow, "NV");
      }
      else if ( forme->micro == "NC" )
      {
        relationsToFollow.insert("SUBSUBJUX");
        newGrp = createGroupe(forme, relationsToFollow, "GN", true);
      }
      else if (forme->micro == "VPP" )
      {
        newGrp = createGroupe(forme, relationsToFollow, "NV", true);
      }
      else if ( forme->macro == "ADJ" )
      {
        newGrp = createGroupe(forme, relationsToFollow, "GA", true);
      }
      else if (forme->micro == "CLR")
      {
        relationsToFollow.insert("PronReflVerbe");
        relationsToFollow.insert("AuxCplPrev");
        // relationsToFollow.insert("aux");
        newGrp = createGroupe(forme, relationsToFollow, "NV");
      }
      else if ( forme->micro == "PROREL" )
      {
        newGrp = createGroupe(forme, relationsToFollow, "GP", true);
      }
      else if ( forme->micro == "CS" && forme->hasInRelation("PrepPronCliv") )
      {
        newGrp = createGroupe(forme, relationsToFollow, "GP", true);
      }
      else if ( forme->micro == "PRO" || forme->micro == "PROWH" )
      {
        newGrp = createGroupe(forme, relationsToFollow, "GN", true);
      }
/*
      else if ( forme->macro == "V" )
      {
        newGrp = createGroupe(forme, relationsToFollow, "NV", true);
      }
*/
      if(newGrp != 0)
      {
        LDEBUG << "ConstituantAndRelationExtractor:: inserted " << forme->forme << " in " << newGrp->type() << " group";
      }
    }
  }
}

/**
  * @brief construit les relations entrantes des formes
  */
void ConstituantAndRelationExtractor::constructionDesRelationsEntrantes()
{
  DUMPERLOGINIT;
  LDEBUG << "ConstituantAndRelationExtractor:: constructionDesRelationsEntrantes";
  std::map<uint64_t,Forme*>::iterator formesIt, formesIt_end;
  formesIt = m_formesIndex.begin();
  formesIt_end = m_formesIndex.end();
  for (; formesIt != formesIt_end; formesIt++)
  {
    Forme* forme = formesIt->second;
    if(forme != 0)
    {
      std::vector<Relation*>::iterator relsIt, relsIt_end;
      relsIt = forme->m_outRelations.begin();
      relsIt_end = forme->m_outRelations.end();
      for (; relsIt != relsIt_end; relsIt++)
      {
        Relation* rel = (*relsIt);
        uint64_t tgtFormeId = m_vertexToFormeIds[rel->tgtVertex];
        if(tgtFormeId != 0)
        {
          Forme* tgtForme = m_formesIndex[tgtFormeId];
          if(tgtForme != 0)
          {
            tgtForme->m_inRelations.push_back(rel);
          }
        }
      }
    }
  }
}

Groupe* ConstituantAndRelationExtractor::createGroupe(
    const Forme* forme,
    const std::set<std::string>& theRelationsToFollow,
    const std::string& groupType)
{
  return createGroupe(forme, theRelationsToFollow, groupType, false);
}

Groupe* ConstituantAndRelationExtractor::createGroupe(
    const Forme* forme,
    const std::set<std::string>& theRelationsToFollow,
    const std::string& groupType,
    bool mayBeUnique = false)
{
  DUMPERLOGINIT;
  LDEBUG << "ConstituantAndRelationExtractor:: createGroupe " << forme->forme << ", " << groupType << " : ";
  std::set<std::string> relationsToFollow;
  std::map< std::string, std::set< std::pair< std::string, std::string > > > followConds;
  std::set<std::string>::const_iterator fit, fit_end;
  fit = theRelationsToFollow.begin(); fit_end = theRelationsToFollow.end();
  for (; fit != fit_end; fit++)
  {
    if ( (*fit).find(';') != std::string::npos )
    {
      size_t first = (*fit).find(";");
      size_t second = (*fit).find(";", first+1);
      std::string rel = (*fit).substr(0, first);
      std::string srcCond = (*fit).substr(first+1, second-first-1);
      std::string targCond = (*fit).substr(second+1);
      if ((srcCond != "*") || (targCond != "*"))
      {
        if (followConds.find(rel) == followConds.end())
        {
          std::set< std::pair< std::string, std::string > > conds;
          conds.insert(std::make_pair(srcCond, targCond));
          followConds.insert(std::make_pair(rel, conds));
        }
        else
          followConds[rel].insert(std::make_pair(srcCond, targCond));
      }
      relationsToFollow.insert(rel);
    }
    else
    {
      relationsToFollow.insert(*fit);
    }
  }
  LDEBUG << "ConstituantAndRelationExtractor:: collected relations to insert";
  Groupe* newGrp = new Groupe();
  newGrp->type(groupType);
  std::vector< uint64_t > formsToLookup;
  std::set< uint64_t > formsAlreadyLookuped;
  formsToLookup.push_back(forme->id);
  while (formsToLookup.size() > 0)
  {
    Forme* currentForm = m_formesIndex[formsToLookup.back()];
    LDEBUG << "ConstituantAndRelationExtractor:: current form is " << currentForm->forme;
    formsToLookup.pop_back();
    if (m_inGroupFormsPositions.find(currentForm->poslong.position) != m_inGroupFormsPositions.end())
      continue;
    formsAlreadyLookuped.insert(currentForm->id);

    std::vector<Relation*>::iterator inIt, inIt_end;
    inIt = currentForm->m_inRelations.begin();
    inIt_end = currentForm->m_inRelations.end();
    for (; inIt != inIt_end; inIt++)
    {
      Relation* rel = *inIt;
      LDEBUG << "ConstituantAndRelationExtractor:: looking at in rel " << rel->type;
      if(relationsToFollow.find(rel->type) != relationsToFollow.end() &&
          formsAlreadyLookuped.find(m_vertexToFormeIds[rel->srcVertex]) == formsAlreadyLookuped.end() &&
          m_inGroupFormsPositions.find(m_formesIndex[m_vertexToFormeIds[rel->srcVertex]]->poslong.position) == m_inGroupFormsPositions.end()
        )
      {
        const Forme* srcForme = m_formesIndex[m_vertexToFormeIds[rel->srcVertex]];
        const Forme* tgtForme = m_formesIndex[m_vertexToFormeIds[rel->tgtVertex]];
        std::pair< std::string, std::string > pair1 = std::make_pair(srcForme->macro, tgtForme->macro);
        std::pair< std::string, std::string > pair2 = std::make_pair(srcForme->macro, tgtForme->micro);
        std::pair< std::string, std::string > pair3 = std::make_pair(srcForme->macro, "*");
        std::pair< std::string, std::string > pair4 = std::make_pair(srcForme->micro, tgtForme->macro);
        std::pair< std::string, std::string > pair5 = std::make_pair(srcForme->micro, tgtForme->micro);
        std::pair< std::string, std::string > pair6 = std::make_pair(srcForme->micro, "*");
        std::pair< std::string, std::string > pair7 = std::make_pair("*", tgtForme->macro);
        std::pair< std::string, std::string > pair8 = std::make_pair("*", tgtForme->micro);
        if ((followConds.find(rel->type) == followConds.end())
             || (followConds[rel->type].find(pair1) != followConds[rel->type].end())
             || (followConds[rel->type].find(pair2) != followConds[rel->type].end())
             || (followConds[rel->type].find(pair3) != followConds[rel->type].end())
             || (followConds[rel->type].find(pair4) != followConds[rel->type].end())
             || (followConds[rel->type].find(pair5) != followConds[rel->type].end())
             || (followConds[rel->type].find(pair6) != followConds[rel->type].end())
             || (followConds[rel->type].find(pair7) != followConds[rel->type].end())
             || (followConds[rel->type].find(pair8) != followConds[rel->type].end()) )
        {
          formsToLookup.push_back( m_vertexToFormeIds[rel->srcVertex]);
          formsAlreadyLookuped.insert(m_vertexToFormeIds[rel->srcVertex]);
          LDEBUG << "ConstituantAndRelationExtractor:: ins src form '" << srcForme->forme << "' in " << newGrp->type();
          newGrp->insert( std::make_pair(srcForme->poslong.position, srcForme->id) );
        }
      }
    }
    std::vector<Relation*>::iterator outIt, outIt_end;
    outIt = currentForm->m_outRelations.begin();
    outIt_end = currentForm->m_outRelations.end();
    for (; outIt != outIt_end; outIt++)
    {
      Relation& rel = **outIt;
      LDEBUG << "ConstituantAndRelationExtractor:: looking at out rel " << rel.type;
      if (
          rel.doFollow &&
          (m_vertexToFormeIds[rel.tgtVertex] != 0) &&
          (m_formesIndex[m_vertexToFormeIds[rel.tgtVertex]] != 0) &&
          (relationsToFollow.find(rel.type) != relationsToFollow.end()) &&
          (formsAlreadyLookuped.find(m_vertexToFormeIds[rel.tgtVertex]) == formsAlreadyLookuped.end() ) &&
          (m_inGroupFormsPositions.find(m_formesIndex[m_vertexToFormeIds[rel.tgtVertex]]->poslong.position) == m_inGroupFormsPositions.end() ) )
      {
        LDEBUG << "ConstituantAndRelationExtractor:: first condition fullfilled";
        const Forme* srcForme = m_formesIndex[m_vertexToFormeIds[rel.srcVertex]];
        const Forme* tgtForme = m_formesIndex[m_vertexToFormeIds[rel.tgtVertex]];
        std::pair< std::string, std::string > pair1 = std::make_pair(srcForme->macro, tgtForme->macro);
        std::pair< std::string, std::string > pair2 = std::make_pair(srcForme->macro, tgtForme->micro);
        std::pair< std::string, std::string > pair3 = std::make_pair(srcForme->macro, "*");
        std::pair< std::string, std::string > pair4 = std::make_pair(srcForme->micro, tgtForme->macro);
        std::pair< std::string, std::string > pair5 = std::make_pair(srcForme->micro, tgtForme->micro);
        std::pair< std::string, std::string > pair6 = std::make_pair(srcForme->micro, "*");
        std::pair< std::string, std::string > pair7 = std::make_pair("*", tgtForme->macro);
        std::pair< std::string, std::string > pair8 = std::make_pair("*", tgtForme->micro);
        if ((followConds.find(rel.type) == followConds.end())
              || (followConds[rel.type].find(pair1) != followConds[rel.type].end())
              || (followConds[rel.type].find(pair2) != followConds[rel.type].end())
              || (followConds[rel.type].find(pair3) != followConds[rel.type].end())
              || (followConds[rel.type].find(pair4) != followConds[rel.type].end())
              || (followConds[rel.type].find(pair5) != followConds[rel.type].end())
              || (followConds[rel.type].find(pair6) != followConds[rel.type].end())
              || (followConds[rel.type].find(pair7) != followConds[rel.type].end())
              || (followConds[rel.type].find(pair8) != followConds[rel.type].end()) )
        {
          LDEBUG << "ConstituantAndRelationExtractor:: second condition fullfilled";
          formsToLookup.push_back( m_vertexToFormeIds[rel.tgtVertex]);
          formsAlreadyLookuped.insert(m_vertexToFormeIds[rel.tgtVertex]);
          LDEBUG << "ConstituantAndRelationExtractor:: insert tgt form '" << tgtForme->forme << "' in " << newGrp->type() << " group";
          newGrp->insert( std::make_pair(tgtForme->poslong.position, tgtForme->id) );
        }
      }
    }
  }
  if (mayBeUnique || !newGrp->empty())
  {
    LDEBUG << "ConstituantAndRelationExtractor:: insert forme '" << forme->forme << "' in " << newGrp->type() << " group";
    newGrp->insert( std::make_pair(forme->poslong.position, forme->id) );
    insertGroup(*newGrp);
    return newGrp;
  }
  return 0;
}

void ConstituantAndRelationExtractor::insertGroup(const Groupe& groupe)
{
  uint64_t grpPos = m_formesIndex[(*(groupe.begin())).second]->poslong.position;
  DUMPERLOGINIT;
  LDEBUG << "ConstituantAndRelationExtractor:: inserting group " << grpPos << " : ";
  m_groupes.insert(std::make_pair(grpPos,groupe));
  Groupe::const_iterator grpsIt, grpsIt_end;
  grpsIt = groupe.begin(); grpsIt_end = groupe.end();
  for (; grpsIt != grpsIt_end; grpsIt++)
  {
    m_inGroupFormsPositions.insert((*grpsIt).first);
  }
}

bool ConstituantAndRelationExtractor::addToGroupIfIsInsideAGroup(const Forme* forme)
{
  uint64_t position = forme->poslong.position;

  std::map<uint64_t, Groupe>::const_iterator itg, itg_end;
  itg =  m_groupes.begin(); itg_end = m_groupes.end();
  uint64_t pos = 0;
  for (; itg != itg_end ; itg++)
  {
    if ( (*itg).first > pos && position >= (*itg).first )
    {
      pos = (*itg).first;

    }
    else if ( (*itg).first > pos && (*itg).first > position )
      break;
  }

  if (m_groupes.find(pos) != m_groupes.end() && !m_groupes[pos].empty())
  {
    if (m_groupes[pos].rbegin() == m_groupes[pos].rend())
    {
      return false;
    }
    else if ( ( (*(m_groupes[pos].rbegin())).first >= position ) &&
            ( (*(m_groupes[pos].begin())).first <= position ) )
    {
      DUMPERLOGINIT;
      LDEBUG << "ConstituantAndRelationExtractor:: insert '" << forme->forme << "' in " << m_groupes[pos].type() << " group";
      m_groupes[pos].insert( std::make_pair(position, forme->id) );
      m_inGroupFormsPositions.insert(position);
      return true;
    }
    else
    {
      return false;
    }
  }
  return false;
}

void ConstituantAndRelationExtractor::addLastFormsInGroups()
{
  DUMPERLOGINIT;
  LDEBUG << "ConstituantAndRelationExtractor:: add last forms in groups";
  std::map<uint64_t, uint64_t>formsInGroups;
  std::map<uint64_t, Groupe>::iterator itGr, itGr_end;
  itGr = m_groupes.begin();
  itGr_end = m_groupes.end();
  for (;itGr!=itGr_end;itGr++)
  {
    std::map<uint64_t, uint64_t>::iterator itForms, itForms_end;
    itForms = ((*itGr).second).begin();
    itForms_end = ((*itGr).second).end();

    for (;itForms!=itForms_end;itForms++)
    {
      formsInGroups.insert(std::make_pair((*itForms).first, (*itForms).second));
    }
  }
  std::map<uint64_t, uint64_t>::iterator it, it_end;
  it = m_positionsFormsIds.begin();
  it_end = m_positionsFormsIds.end();
  for (;it!=it_end;it++)
  {
    if (formsInGroups.find((*it).first) == formsInGroups.end() )
    {
      if(m_formesIndex[(*it).second] != 0)
      {
        addToGroupIfIsInsideAGroup(m_formesIndex[(*it).second]);
      }
    }
  }
}

void ConstituantAndRelationExtractor::splitCompoundTenses()
{
  DUMPERLOGINIT;
  LDEBUG << "ConstituantAndRelationExtractor:: splitCompoundTenses";
  std::map<uint64_t, uint64_t>::iterator it, it_end;
  it = m_positionsFormsIds.begin(); it_end = m_positionsFormsIds.end();
  uint64_t compoundSplitted = 0;
  for (; it != it_end; it++)
  {
    LDEBUG << "ConstituantAndRelationExtractor:: pos/id/annot="<<(*it).first<<"/"<<(*it).second<<"/"<<m_posAnnotMatching[(*it).first];
    // le noeud a la position courante definit un temps compose
    if (m_compoundTenses.find(m_posAnnotMatching[(*it).first]) != m_compoundTenses.end() )
    {
      uint64_t position = (*it).first;

      // ids are in pos graph space
      uint64_t cpdtenseid = m_positionsFormsIds[position];
      uint64_t auxid = m_compoundTenses[m_posAnnotMatching[(*it).first]].first;
      uint64_t pastpartid = m_compoundTenses[m_posAnnotMatching[(*it).first]].second;

      LDEBUG << "ConstituantAndRelationExtractor:: cpd tense: "<<cpdtenseid<<"->("<<auxid << "," << pastpartid << ")";
      Forme* cpdtenseForme = m_formesIndex[cpdtenseid];
      Forme* auxForme = m_formesIndex[auxid];
      Forme* pastpartForme = m_formesIndex[pastpartid];

      if(cpdtenseForme != 0 && auxForme != 0 && pastpartForme != 0){

        // remplacer la forme a la position courante par celle de l'auxiliaire
        LDEBUG << "ConstituantAndRelationExtractor:: replacing at " << position <<" by " << auxForme->forme << " (" << auxid << ")";
        m_positionsFormsIds[position] = auxid;

        // pour chaque relation entrante sur temps compose
        // - si sujinv ou suj ou advv, la repointer sur la forme de l'aux
        // - si cod ou coi (cplv), la repointer sur la forme du past part
        std::vector<Relation*>::iterator cpdTenseInRelsIt, cpdTenseInRelsIt_end;
        cpdTenseInRelsIt = cpdtenseForme->m_inRelations.begin();
        cpdTenseInRelsIt_end = cpdtenseForme->m_inRelations.end();
        for (;cpdTenseInRelsIt != cpdTenseInRelsIt_end; cpdTenseInRelsIt++)
        {
          Relation* cpdTenseInRel = *cpdTenseInRelsIt;
          LDEBUG << "ConstituantAndRelationExtractor:: compound tense input relation = " << cpdTenseInRel->type;
          if (cpdTenseInRel->type == "SujInv" || cpdTenseInRel->type == "SUJ_V" || cpdTenseInRel->type == "Neg" || cpdTenseInRel->type == "PronSujVerbe")
          {
            LDEBUG << "ConstituantAndRelationExtractor:: change it from (" << cpdTenseInRel->srcVertex << "-> " << cpdTenseInRel->tgtVertex << ") to (" << cpdTenseInRel->srcVertex << "->" << auxForme->forme << ")";
            cpdTenseInRel->tgtVertex = m_formeIdsToVertex[auxForme->id];
            auxForme->m_inRelations.push_back(cpdTenseInRel);
          }
          else // at least COD_V and CPL_V
          {
            LDEBUG << "ConstituantAndRelationExtractor:: change it from (" << cpdTenseInRel->srcVertex << "-> " << cpdTenseInRel->tgtVertex << ") to (" << cpdTenseInRel->srcVertex << "->" << pastpartForme->forme << ")";
            cpdTenseInRel->tgtVertex = m_formeIdsToVertex[pastpartForme->id];
            pastpartForme->m_inRelations.push_back(cpdTenseInRel);
            // dans ce cas attention �  ne pas suivre dans la construction des groupes, mais �  suivre AuxCplPrev
            if (cpdTenseInRel->type == "CodPrev" || cpdTenseInRel->type == "CoiPrev" || cpdTenseInRel->type == "PronSujVerbe" || cpdTenseInRel->type == "PronReflVerbe" || cpdTenseInRel->type == "PrepInf")
            {
              Forme* srcForme = m_formesIndex[m_vertexToFormeIds[cpdTenseInRel->srcVertex]];
              if(srcForme != 0){
                LDEBUG << "ConstituantAndRelationExtractor:: specific compound tense case: " << srcForme->forme;
                cpdTenseInRel->doFollow = false;
                Relation* cplRel = new Relation();
                cplRel->srcVertex = cpdTenseInRel->srcVertex;
                cplRel->tgtVertex = m_formeIdsToVertex[auxForme->id];
                cplRel->type = "AuxCplPrev";
                m_outRelations.push_back(cplRel);
                srcForme->m_outRelations.push_back(cplRel);
                auxForme->m_inRelations.push_back(cplRel);
              }
            }
          }
        }

        // pour chaque relation sortante d'un temps compose
        // - si sujinv ou suj ou advv, la repointer sur la forme de l'aux
        // - si cod, la repointer sur la forme du past part
        std::vector<Relation*>::iterator cpdTenseOutRelsIt, cpdTenseOutRelsIt_end;
        cpdTenseOutRelsIt = cpdtenseForme->m_outRelations.begin();
        cpdTenseOutRelsIt_end = cpdtenseForme->m_outRelations.end();
        for (;cpdTenseOutRelsIt != cpdTenseOutRelsIt_end; cpdTenseOutRelsIt++)
        {
          Relation* cpdTenseOutRel = *cpdTenseOutRelsIt;
          LDEBUG << "ConstituantAndRelationExtractor:: compound tense output relation = " << cpdTenseOutRel->type;
          LDEBUG << "ConstituantAndRelationExtractor:: change it from (" << cpdTenseOutRel->srcVertex << "-> " << cpdTenseOutRel->tgtVertex << ") to (" << pastpartForme->forme << "->" << cpdTenseOutRel->tgtVertex << ")";
          cpdTenseOutRel->srcVertex = m_formeIdsToVertex[pastpartForme->id];
          pastpartForme->m_outRelations.push_back(cpdTenseOutRel);
        }

        compoundSplitted++;

      }
      else
      {
        LDEBUG << "ConstituantAndRelationExtractor:: compound tense not found part";
      }
    }
  }
  LDEBUG << "ConstituantAndRelationExtractor:: splitCompoundTenses DONE";

  if(compoundSplitted > 0){
    LDEBUG << "ConstituantAndRelationExtractor:: trying recursive splitCompoundTenses";
    //splitCompoundTenses();
  }

}

//Fonction qui permet de remplacer une entité nommée par les éléments qui la composent
void ConstituantAndRelationExtractor::replaceSEWithCompounds()
{
  std::vector<uint64_t> formsToErase;

  if (m_formesIndex.empty())
  {
    return;
  }
  std::vector<Relation*>::iterator relIt, relIt_end;

  std::map<uint64_t,Forme*>::iterator It, It_end;
  It = m_formesIndex.begin();
  It_end = m_formesIndex.end();

  for (;It!=It_end;It++)
  {
    uint64_t position = (*It).first;
    Forme* forme = (*It).second;
    // Pour toutes les formes de m_formesIndex, on regarde si leur id est présent dans les ids des entités nommées.
    if (m_namedEntitiesVertices.find(position) != m_namedEntitiesVertices.end())
    {
      DUMPERLOGINIT;
      LDEBUG << "ConstituantAndRelationExtractor:: se at " << position << " for " << forme->forme ;
      //on récupère l'id du vertex dans l'analysis graph qui correspond �  l'id du posgraph
      uint64_t matchingVertex = m_posAnaMatching[((*It).first)];
      //on stocke dans un vecteur les composants de l'entité nommée
      std::vector<uint64_t> tmpVector = m_seCompounds[matchingVertex];
      std::vector<uint64_t>::iterator vectIt, vectIt_end;
      vectIt = tmpVector.begin(); vectIt_end = tmpVector.end();

      //ici on ajoute l'id de l'entité nommée afin de pouvoir la supprimer de m_formesIndex par la suite
      formsToErase.push_back(position);
      bool firstPassage = true;
      Forme* precForm = 0;
      uint64_t maxVertex = (*(m_formesIndex.rbegin())).first;
      for (;vectIt!=vectIt_end;vectIt++)
      {
        //pour chacune des composantes de l'entité nommée, on en extrait la forme sur laquelle on fait quelques modification
        Forme* tmpForme = m_anaGraphVertices[*vectIt];
        LDEBUG << "ConstituantAndRelationExtractor:: se compound: " << tmpForme->forme;
        //afin qu'il n'y ait pas de conflit dans la numérotation des vertex, l'id de la première composante est égale �  l'id la plus grande de m_formesIndex que l'on incrémente de 1
        tmpForme->id = maxVertex+1;
        maxVertex++;
        // Il faut alors ajouter  cette forme dans le tableau des formes
        m_formesIndex[tmpForme->id]=tmpForme;
        tmpForme->macro = m_namedEntitiesVertices[(*It).first].macro;
        tmpForme->micro = m_namedEntitiesVertices[(*It).first].micro;

        // si la composante traitée est la première composante de l'entité nommmée, on copie les relations de dépendances si elles existent de l'entité nommée sur celle ci (tout en modifiant srcVertex pour qu'il corresponde �  son id) et on met �  jour m_outRelations.
        if (firstPassage)
        {

          //copie des relations de l'entité nommée
          tmpForme->m_outRelations = (*It).second->m_outRelations;
          relIt = tmpForme->m_outRelations.begin();
          relIt_end = tmpForme->m_outRelations.end();

          // pour toute les relations copiées, modifie la source de la dépendance vers l'id de la composante
          for (;relIt != relIt_end; relIt++)
          {
            (*relIt)->srcVertex = tmpForme->id;
          }

          // on met �  jour des relations dans m_out_relations, qui ont pour sources ou cible l'entité nommée traitée
          relIt = m_outRelations.begin();
          relIt_end = m_outRelations.end();
          for (;relIt != relIt_end;relIt++)
          {
            if ((*relIt)->srcVertex == (*It).first)
              (*relIt)->srcVertex = tmpForme->id;
            else if ((*relIt)->tgtVertex == (*It).first)
              (*relIt)->tgtVertex = tmpForme->id;
          }

          // ici, on met �  jour m_formesIndex pour que toutes les relations qui pointaient vers l'entité nommée pointent désormais vers la composante.
          relIt = m_outRelations.begin();
          relIt_end = m_outRelations.end();
          for(; relIt!=relIt_end;relIt++)
          {
            if ((*relIt)->tgtVertex == position)
            {
              LDEBUG << "ConstituantAndRelationExtractor:: update relation target " << (*relIt)->tgtVertex;
              (*relIt)->tgtVertex = tmpForme->id;
            }
          }
          firstPassage = false;
        }
        else if(precForm != 0)
        {
          Relation* rel = new Relation;
          rel->srcVertex = precForm->id;
          rel->tgtVertex = tmpForme->id;
          rel->type = "SECOMPOUND";
          precForm->m_outRelations.push_back(rel);
          m_outRelations.push_back(rel);
        }

        //on insère la forme créée dans les différents conteneurs
        LDEBUG << "ConstituantAndRelationExtractor:: adding compound " << tmpForme->forme << ", "  << tmpForme->id << "(" << tmpForme->poslong.position << ")";
        std::map< LinguisticAnalysisStructure::Token*, uint64_t >::const_iterator tokenIter;
        m_positionsFormsIds.erase(tmpForme->poslong.position);
        m_positionsFormsIds.insert(std::make_pair(tmpForme->poslong.position, tmpForme->id));
        m_vertexToFormeIds.insert(std::make_pair(tmpForme->id, tmpForme->id));
        m_formeIdsToVertex.insert(std::make_pair(tmpForme->id, tmpForme->id));

        precForm = tmpForme;

      }

    }
  }

  //pour finir, on supprime les entités nommées traitées de m_formesIndex
  std::vector<uint64_t>::const_iterator eraseIt, eraseIt_end;
  eraseIt = formsToErase.begin();
  eraseIt_end = formsToErase.end();
  for (;eraseIt!=eraseIt_end;eraseIt++)
  {
    DUMPERLOGINIT;
    LDEBUG << "ConstituantAndRelationExtractor:: erase compound " << *eraseIt;
    m_formesIndex.erase(*eraseIt);
  }
}

} // end namespace EasyXmlDumper
} // end namespace AnalysisDumpers
} // end namespace LinguisticProcessings
} // end namespace Lima
