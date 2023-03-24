// Copyright 2002-2022 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT
/**
  * @author      Gael de Chalendar <Gael.de-Chalendar@cea.fr>
  * @date        Created on Jan, 20 2003
  */

#include "DepGraphCompoundsBuildVisitor.h"
#include "CompoundsBuilderFromSyntacticData.h"
#include "CompoundsCompatibilityBuilder.h"

#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/common/PropertyCode/PropertyManager.h"
#include "common/MediaticData/mediaticData.h"
#include "common/Data/strwstrtools.h"
#include "linguisticProcessing/core/SyntacticAnalysis/SyntacticData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"

#include <stdexcept>
#include <iostream>

namespace Lima
{
using namespace Common;
using namespace Common::AnnotationGraphs;

namespace LinguisticProcessing
{
using namespace LinguisticAnalysisStructure;
using namespace SyntacticAnalysis;

namespace Compounds
{

class DepGraphCompoundsBuildVisitorPrivate
{
  friend class DepGraphCompoundsBuildVisitor;

  DepGraphCompoundsBuildVisitorPrivate(
                              const CompoundsBuilderFromSyntacticData* cpbfsd,
                              MediaId language,
                              SyntacticAnalysis::SyntacticData* syntacticData,
                              DependencyGraph* depGraph,
                              LinguisticAnalysisStructure::AnalysisGraph* iter,
                              LinguisticGraphVertex startVertex,
                              LinguisticGraphVertex stopVertex,
//                                 uint64_t depGraphMaxBranchingFactor,
                              Common::AnnotationGraphs::AnnotationData* annotationData,
                              bool useChains);

  ~DepGraphCompoundsBuildVisitorPrivate() {}

  bool isDepEdgeACompoundNounRelation(const DependencyGraphEdge& e) const;

  std::vector< DependencyGraphVertex > getNodes(const std::vector< DependencyGraphEdge >& relations);

  // calculer les incompatibilites binaires
  std::map< uint64_t, std::set< uint64_t > > computeExclusions(
      const std::vector< DependencyGraphEdge >& relations);

  // remplir le tableau des tailles
  std::map< uint64_t, bool > computeSizes();

  // calculer l'ordre
  std::vector< uint64_t > computeOrder(const std::map< uint64_t, std::set< uint64_t > >& exclusions);

  // construire les termes correspondants et les enregistrer
  void buildTermsFromCompoundsSets(std::list< std::set< uint64_t > >& compoundsSets);

  DependencyGraphVertex findFinalSourceOfCompoundInRelationsSet(std::set< uint64_t >& relations);

  AnnotationGraphVertex buildTermFor(DependencyGraphVertex head,
                    const std::set< uint64_t > relations,
                    bool theHead = false);

  void buildCompatibilityTermsFromWithChains(DependencyGraphVertex from);
  void buildCompatibilityTermsFromNoChains(DependencyGraphVertex from);

  /** @note On ne gere pas ici les differents lemmes possibles. Ce sera fait au
   * niveau du BoWDumper. Ideallement, il devrait y avoir une phase de
   * desambiguisation semantique permettant de reduire le nombre de cas.
   */
  AnnotationGraphVertex createAnnotationVertex(const LinguisticGraphVertex v);

  std::vector< DependencyGraphEdge > getCompoundsRelationsFromWithChain(
      DependencyGraphVertex from,
      const LinguisticAnalysisStructure::ChainIdStruct& chId);

  std::map< uint64_t, std::set< uint64_t > > computeExclusionsWithChain(
      const std::vector< DependencyGraphEdge >& relations,
      const LinguisticAnalysisStructure::ChainIdStruct& chId);

  std::vector< DependencyGraphEdge > getCompoundsRelationsFromNoChain(
      DependencyGraphVertex from);

  std::map< uint64_t, std::set< uint64_t > > computeExclusionsNoChain(
      const std::vector< DependencyGraphEdge >& relations);

  ConceptModifier createModifierFor(LinguisticGraphVertex inSrcMorph);


  const CompoundsBuilderFromSyntacticData* m_context;
  MediaId m_language;
  SyntacticAnalysis::SyntacticData* m_syntacticData;
  DependencyGraphVertex m_depFailedVertex;
  std::set< DependencyGraphVertex > m_visited;
  std::set< std::pair< DependencyGraphVertex, LinguisticAnalysisStructure::ChainIdStruct > > m_visitedWithChains;
  std::set< std::string > m_alreadyInserted;
  DependencyGraph* m_depGraph;
  LinguisticAnalysisStructure::AnalysisGraph* m_iter;
  const Common::PropertyCode::PropertyAccessor* m_macroAccessor;
  const Common::PropertyCode::PropertyAccessor* m_microAccessor;
  Common::AnnotationGraphs::AnnotationData* m_annotationData;

  /** vecteur de representations de relations
    * 1 relation = une paire source, cible
    * 1 source ou cible = tuple de position, vertex, liste des chaines, type de la relation
    */
  std::vector<
      std::pair<
        boost::tuple<uint64_t,DependencyGraphVertex, std::set< uint64_t>, Common::MediaticData::SyntacticRelationId >,
        boost::tuple<uint64_t,DependencyGraphVertex, std::set<uint64_t>, Common::MediaticData::SyntacticRelationId >
      >
  > m_relsmap;

  std::vector< boost::tuple<uint64_t,DependencyGraphVertex, std::set<uint64_t> > > m_nodesmap;

  bool m_useChains = false;
};

DepGraphCompoundsBuildVisitorPrivate::DepGraphCompoundsBuildVisitorPrivate(const
        CompoundsBuilderFromSyntacticData* cpbfsd,
        MediaId language,
        SyntacticAnalysis::SyntacticData* syntacticData,
        DependencyGraph* depGraph,
        LinguisticAnalysisStructure::AnalysisGraph* iter,
        LinguisticGraphVertex startVertex,
        LinguisticGraphVertex stopVertex,
        Common::AnnotationGraphs::AnnotationData* annotationData,
        bool useChains) :
    m_context(cpbfsd),
    m_language(language),
    m_syntacticData(syntacticData),
    m_depFailedVertex(syntacticData->depVertexForTokenVertex(iter->firstVertex())),
    m_visited(),
    m_depGraph(depGraph),
    m_iter(iter),
    m_annotationData(annotationData),
    m_useChains(useChains)
{
  m_macroAccessor=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyAccessor("MACRO"));
  m_microAccessor=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyAccessor("MICRO"));
  if (m_annotationData->dumpFunction("CompoundTokenAnnotation") == 0)
  {
    m_annotationData->dumpFunction("CompoundTokenAnnotation", new DumpCompoundTokenAnnotation(Common::MediaticData::MediaticData::changeable().stringsPool(m_language)));
  }
}


DepGraphCompoundsBuildVisitor::DepGraphCompoundsBuildVisitor(const CompoundsBuilderFromSyntacticData* cpbfsd,
        MediaId language,
        SyntacticAnalysis::SyntacticData* syntacticData,
        DependencyGraph* depGraph,
        LinguisticAnalysisStructure::AnalysisGraph* iter,
        LinguisticGraphVertex startVertex,
        LinguisticGraphVertex stopVertex,
        Common::AnnotationGraphs::AnnotationData* annotationData,
        bool useChains) :
    m_d(new DepGraphCompoundsBuildVisitorPrivate(cpbfsd,
                                                 language,
                                                 syntacticData,
                                                 depGraph,
                                                 iter,
                                                 startVertex,
                                                 stopVertex,
                                                 annotationData,
                                                 useChains))
{
}

DepGraphCompoundsBuildVisitor::~DepGraphCompoundsBuildVisitor()
{
  delete m_d;
}

void DepGraphCompoundsBuildVisitor::discover_vertex(LinguisticGraphVertex v)
{
#ifdef DEBUG_LP
  MORPHOLOGINIT;
  LDEBUG << "DepGraphCompoundsBuildVisitor::discover_vertex" << v;
#endif
  if ( (v != m_d->m_syntacticData-> iterator()-> firstVertex()) &&
        (v != m_d->m_syntacticData-> iterator()-> lastVertex()) )
  {
    DependencyGraphVertex vd = m_d->m_syntacticData->depVertexForTokenVertex(v);
    if (m_d->m_visited.find(vd) == m_d->m_visited.end())
    {
      if (m_d->m_useChains)
        m_d->buildCompatibilityTermsFromWithChains(vd);
      else
        m_d->buildCompatibilityTermsFromNoChains(vd);
    }
#ifdef DEBUG_LP
    else
    {
      LDEBUG << "DepGraphCompoundsBuildVisitor::discover_vertex" << v << "already visited";
    }
#endif
  }
#ifdef DEBUG_LP
  else
  {
    LDEBUG << "DepGraphCompoundsBuildVisitor::discover_vertex" << v << "is first or last vertex";
  }
#endif
}

void DepGraphCompoundsBuildVisitorPrivate::buildCompatibilityTermsFromWithChains(DependencyGraphVertex from)
{
  //ChainIdStruct
#ifdef DEBUG_LP
  MORPHOLOGINIT;
  LDEBUG << "Building compatibility terms with chains from " << from;
#endif
  VertexChainIdPropertyMap chainsMap = get(vertex_chain_id, *(m_syntacticData->graph()));
  LinguisticGraphVertex fromTokVertex = m_syntacticData->tokenVertexForDepVertex(from);
  VertexChainIdProp::const_iterator fromChainsIt, fromChainsIt_end;
  fromChainsIt = chainsMap[fromTokVertex].begin(); fromChainsIt_end = chainsMap[fromTokVertex].end();
  for (; fromChainsIt != fromChainsIt_end; fromChainsIt++)
  {
#ifdef DEBUG_LP
    LDEBUG << "Building compatibility terms with chains from " << from << " on chain " << *fromChainsIt;
#endif
    if (m_visitedWithChains.find(std::make_pair(from,*fromChainsIt)) == m_visitedWithChains.end())
    {
      // recuperer l'ensemble des relations accessibles depuis from
      std::vector< DependencyGraphEdge > relations = getCompoundsRelationsFromWithChain(from, *fromChainsIt);
#ifdef DEBUG_LP
      LDEBUG << "Got " << relations.size() << " compound relations";
#endif
      std::vector< DependencyGraphVertex > nodes = getNodes(relations);
#ifdef DEBUG_LP
      LDEBUG << "Got " << nodes.size() << " compound nodes";
#endif
      // calculer les incompatibilites binaires
      std::map< uint64_t, std::set< uint64_t > > exclusions = computeExclusionsWithChain(relations, *fromChainsIt);
#ifdef DEBUG_LP
      LDEBUG << "Got " << exclusions.size() << " exclusions";
#endif
      // calculer l'ordre
      std::map< uint64_t, bool > sizes = computeSizes();
#ifdef DEBUG_LP
      LDEBUG << "Got " << sizes.size() << " sizes";
#endif
      // remplir le tableau des tailles
      std::vector< uint64_t > order = computeOrder(exclusions);
#ifdef DEBUG_LP
      LDEBUG << "Got " << order.size() << " order elements";
#endif
      // appeler le calcul des compatibilites
      CompoundsCompatibilityBuilder builder;
      std::list< std::set< uint64_t > > compoundsSets = builder.computeCompatibilitiesWithChain(exclusions, sizes, order);
#ifdef DEBUG_LP
      LDEBUG << "Got " << compoundsSets.size() << " compounds sets" ;
#endif
      std::list< std::set< uint64_t > >::const_iterator compsIt, compsIt_end;
      compsIt = compoundsSets.begin(); compsIt_end = compoundsSets.end();
      for (; compsIt  != compsIt_end; compsIt++)
      {
        std::set< uint64_t >::const_iterator compIt, compIt_end;
        compIt = (*compsIt).begin(); compIt_end = (*compsIt).end();
        for (; compIt != compIt_end; compIt++)
        {
// #ifdef DEBUG_LP
//           LDEBUG << "("<<m_relsmap[*compIt].first.get<1>()<<"->"
//                   < <m_relsmap[*compIt].second.get<1>()<<"), ";
// #endif
        }
#ifdef DEBUG_LP
        LDEBUG;
#endif
      }

      // construire les termes correspondants et les enregistrer
#ifdef DEBUG_LP
      LDEBUG << "Building terms from compatibility results";
#endif
      buildTermsFromCompoundsSets(compoundsSets);
    }
  }
#ifdef DEBUG_LP
  LDEBUG << "Finished Building compatibility terms with chains from " << from;
#endif
}

void DepGraphCompoundsBuildVisitorPrivate::buildCompatibilityTermsFromNoChains(
    DependencyGraphVertex from)
{
  //ChainIdStruct
#ifdef DEBUG_LP
  MORPHOLOGINIT;
  LDEBUG << "Building compatibility terms no chains from " << from;
#endif
  {
#ifdef DEBUG_LP
    LDEBUG << "Building compatibility terms no chains from " << from ;
#endif
    {
      // recuperer l'ensemble des relations accessibles depuis from
      std::vector< DependencyGraphEdge > relations = getCompoundsRelationsFromNoChain(from);
#ifdef DEBUG_LP
      LDEBUG << "Got " << relations.size() << " compound relations";
#endif
      std::vector< DependencyGraphVertex > nodes = getNodes(relations);
#ifdef DEBUG_LP
      LDEBUG << "Got " << nodes.size() << " compound nodes";
#endif
      // calculer les incompatibilites binaires
      std::map< uint64_t, std::set< uint64_t > > exclusions = computeExclusionsNoChain(relations);
#ifdef DEBUG_LP
      LDEBUG << "Got " << exclusions.size() << " exclusions";
#endif
      // calculer l'ordre
      std::map< uint64_t, bool > sizes = computeSizes();
#ifdef DEBUG_LP
      LDEBUG << "Got " << sizes.size() << " sizes";
#endif
      // remplir le tableau des tailles
      std::vector< uint64_t > order = computeOrder(exclusions);
#ifdef DEBUG_LP
      LDEBUG << "Got " << order.size() << " order elements";
#endif
      // appeler le calcul des compatibilites
      CompoundsCompatibilityBuilder builder;
      std::list< std::set< uint64_t > > compoundsSets = builder.computeCompatibilitiesWithChain(exclusions, sizes, order);
#ifdef DEBUG_LP
      LDEBUG << "Got " << compoundsSets.size() << " compounds sets" ;
#endif
      std::list< std::set< uint64_t > >::const_iterator compsIt, compsIt_end;
      compsIt = compoundsSets.begin(); compsIt_end = compoundsSets.end();
      for (; compsIt  != compsIt_end; compsIt++)
      {
        std::set< uint64_t >::const_iterator compIt, compIt_end;
        compIt = (*compsIt).begin(); compIt_end = (*compsIt).end();
        for (; compIt != compIt_end; compIt++)
        {
// #ifdef DEBUG_LP
//           LDEBUG << "("<<m_relsmap[*compIt].first.get<1>()<<"->"
//                   < <m_relsmap[*compIt].second.get<1>()<<"), ";
// #endif
        }
#ifdef DEBUG_LP
        LDEBUG;
#endif
      }

      // construire les termes correspondants et les enregistrer
#ifdef DEBUG_LP
      LDEBUG << "Building terms from compatibility results";
#endif
      buildTermsFromCompoundsSets(compoundsSets);
    }
  }
#ifdef DEBUG_LP
  LDEBUG << "Finished Building compatibility terms no chains from " << from;
#endif
}

// construire les termes correspondants et les enregistrer
void DepGraphCompoundsBuildVisitorPrivate::buildTermsFromCompoundsSets(
    std::list< std::set< uint64_t > >& compoundsSets)
{
/*
  Critical function : comment logging messages
*/
//  MORPHOLOGINIT;
  for (auto currentSet: compoundsSets)
  {
//    LDEBUG << "Building a Compound";
    DependencyGraphVertex head = findFinalSourceOfCompoundInRelationsSet(currentSet);
    if (head == 0)
    {
      MORPHOLOGINIT;
      LWARN << "No final source of compound found => no term built here.";
      continue;
    }
//    LDEBUG << "Compound head is " << head;
    buildTermFor(head,currentSet,true);
  }
}


bool DepGraphCompoundsBuildVisitorPrivate::isDepEdgeACompoundNounRelation(const DependencyGraphEdge& e) const
{
  EdgeDepRelTypePropertyMap relTypeMap =
      get(edge_deprel_type, *m_depGraph);

  Common::MediaticData::SyntacticRelationId relType=relTypeMap[e];
  std::string relName = static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getSyntacticRelationName(relType);

#ifdef DEBUG_LP
  MORPHOLOGINIT;
  LDEBUG << "isDepEdgeACompoundNounRelation: Relation name" << relName;
#endif

  if (relName.empty())
  {
    MORPHOLOGINIT;
    LERROR << "Unknown type " << relType
        << " while testing edge for compound relation.";
    return false;
  }
  if (m_context->isACompoundRel(relName))
  {
    return true;
  }
  else
  {
    return false;
  }
}

std::vector< DependencyGraphVertex > DepGraphCompoundsBuildVisitorPrivate::getNodes(
    const std::vector< DependencyGraphEdge >& relations)
{
/*
  Critical function : comment logging messages
*/
//  MORPHOLOGINIT;
//  LDEBUG << "Getting nodes";
  std::vector< DependencyGraphVertex > nodes;
  std::set< DependencyGraphVertex > snodes;
  std::vector< DependencyGraphEdge >::const_iterator it, it_end;
  it = relations.begin(); it_end = relations.end();
  for (; it != it_end; it++)
  {
    if (snodes.find(source(*it, *m_depGraph)) == snodes.end())
    {
      snodes.insert(source(*it, *m_depGraph));
      nodes.push_back(source(*it, *m_depGraph));
    }
    if (snodes.find(target(*it, *m_depGraph)) == snodes.end())
    {
      snodes.insert(target(*it, *m_depGraph));
      nodes.push_back(target(*it, *m_depGraph));
    }
  }
  return nodes;
}

// calculer les incompatibilites binaires
std::map< uint64_t, std::set< uint64_t > > DepGraphCompoundsBuildVisitorPrivate::computeExclusions(
    const std::vector< DependencyGraphEdge >& relations)
{
#ifdef DEBUG_LP
  MORPHOLOGINIT;
  LDEBUG << "Computing exclusions";
#endif
  std::map< uint64_t, std::set< uint64_t > > exclusions;
  EdgeDepRelTypePropertyMap relTypeMap = get(edge_deprel_type, *m_depGraph);
  VertexChainIdPropertyMap chainsMap = get(vertex_chain_id, *(m_syntacticData->graph()));
  m_relsmap.clear();
  m_nodesmap.clear();
  std::vector< DependencyGraphEdge >::const_iterator relsit, relsit_end;
  relsit = relations.begin(); relsit_end = relations.end();
  //preparation des donnees
  for (; relsit != relsit_end; relsit++)
  {
    DependencyGraphVertex src = source(*relsit, *m_depGraph);
    DependencyGraphVertex tgt = target(*relsit, *m_depGraph);
    LinguisticGraphVertex srcTokVertex = m_syntacticData->tokenVertexForDepVertex(src);
    LinguisticGraphVertex tgtTokVertex = m_syntacticData->tokenVertexForDepVertex(tgt);
    const Token* srcToken = get(vertex_token, *(m_iter->getGraph()), src);
    const Token* tgtToken = get(vertex_token, *(m_iter->getGraph()), tgt);
    uint64_t srcPos = srcToken->position();
    uint64_t tgtPos = tgtToken->position();
    std::set<uint64_t> srcChains;
    VertexChainIdProp::const_iterator srcChainsIt, srcChainsIt_end;
    srcChainsIt = chainsMap[srcTokVertex].begin(); srcChainsIt_end = chainsMap[srcTokVertex].end();
    for ( ; srcChainsIt != srcChainsIt_end; srcChainsIt++)
    {
      srcChains.insert((*srcChainsIt).chainId());
    }
    std::set<uint64_t> tgtChains;
    VertexChainIdProp::const_iterator tgtChainsIt, tgtChainsIt_end;
    tgtChainsIt = chainsMap[tgtTokVertex].begin(); tgtChainsIt_end = chainsMap[tgtTokVertex].end();
    for ( ; tgtChainsIt != tgtChainsIt_end; tgtChainsIt++)
    {
      tgtChains.insert((*tgtChainsIt).chainId());
    }
    //    VertexWordPropertyMap dwsMap = get(vertex_word, *(m_iter->getGraph()));
    //    const DicoWord& word = dwsMap[v];
    //    const LimaString& lemma = word.lemma();
    //    uint64_t macroCategory=word.category();
    Common::MediaticData::SyntacticRelationId relType = relTypeMap[boost::edge(src,tgt,*m_depGraph).first];
    m_relsmap.push_back(std::make_pair(boost::make_tuple(srcPos,srcTokVertex,srcChains,relType),boost::make_tuple(tgtPos,tgtTokVertex,tgtChains,relType)));
    m_nodesmap.push_back(boost::make_tuple(srcPos,srcTokVertex,srcChains));
    m_nodesmap.push_back(boost::make_tuple(tgtPos,tgtTokVertex,tgtChains));
  }
  for (uint64_t i = 0; i < m_relsmap.size(); i++)
  {
    exclusions.insert(std::make_pair(i, std::set< uint64_t >()));
  }
  for (uint64_t i = m_relsmap.size(); i < m_relsmap.size()+m_nodesmap.size(); i++)
  {
    exclusions.insert(std::make_pair(i, std::set< uint64_t >()));
  }
  return exclusions;
  /*
  // calcul des exclusions
  for (uint64_t i = 0; i < m_relsmap.size(); i++)
  {
    for (uint64_t j = i+1; j < m_relsmap.size(); j++)
    {
      std::set< uint64_t > intersec;

      std::insert_iterator< std::set< uint64_t > > ins(intersec, intersec.end());
      std::set_intersection(m_relsmap[i].first.get<2>().begin(), m_relsmap[i].first.get<2>().end(),
                            m_relsmap[j].first.get<2>().begin(), m_relsmap[j].first.get<2>().end(), ins);
      if (
        ((m_relsmap[i].first.get<0>() == m_relsmap[j].first.get<0>() && m_relsmap[i].first.get<1>() != m_relsmap[j].first.get<1>()) || intersec.empty())
        || ((m_relsmap[i].first.get<0>() == m_relsmap[j].second.get<0>() && m_relsmap[i].first.get<1>() != m_relsmap[j].second.get<1>()) || intersec.empty())
        || ((m_relsmap[i].second.get<0>() == m_relsmap[j].first.get<0>() && m_relsmap[i].second.get<1>() != m_relsmap[j].first.get<1>()) || intersec.empty())
        || ((m_relsmap[i].second.get<0>() == m_relsmap[j].second.get<0>() && m_relsmap[i].second.get<1>() != m_relsmap[j].second.get<1>()) || intersec.empty())
        || (m_relsmap[i].first.get<3>() == m_relsmap[j].first.get<3>()) )
      {
        exclusions[i].insert(j);
        exclusions[j].insert(i);
      }
    }
    for (uint64_t j = 0; j < m_nodesmap.size(); j++)
    {
      std::set< uint64_t > intersec;

      std::insert_iterator< std::set< uint64_t > > ins(intersec, intersec.end());
      std::set_intersection(m_relsmap[i].first.get<2>().begin(), m_relsmap[i].first.get<2>().end(),
                            m_nodesmap[j].get<2>().begin(), m_nodesmap[j].get<2>().end(), ins);
      if (
        ((m_relsmap[i].first.get<0>() == m_nodesmap[j].get<0>() && m_relsmap[i].first.get<1>() != m_nodesmap[j].get<1>()) || intersec.empty() )
        || ((m_relsmap[i].second.get<0>() == m_nodesmap[j].get<0>() && m_relsmap[i].second.get<1>() != m_nodesmap[j].get<1>()) || intersec.empty() ) )
      {
        exclusions[i].insert(i+j);
        exclusions[i+j].insert(i);
      }
    }
  }
  for (uint64_t i = 0; i < m_nodesmap.size(); i++)
  {
    for (uint64_t j = 0; j < m_nodesmap.size(); j++)
    {
      std::set< uint64_t > intersec;

      std::insert_iterator< std::set< uint64_t > > ins(intersec, intersec.end());
      std::set_intersection(m_nodesmap[i].get<2>().begin(), m_nodesmap[i].get<2>().end(),
                            m_nodesmap[j].get<2>().begin(), m_nodesmap[j].get<2>().end(), ins);
      if ( ((m_nodesmap[i].get<0>() == m_nodesmap[j].get<0>()) && (m_nodesmap[i].get<1>() != m_nodesmap[j].get<1>())) || (!intersec.empty()) )
      {
        exclusions[m_relsmap.size()+i].insert(m_relsmap.size()+i+j);
        exclusions[m_relsmap.size()+i+j].insert(m_relsmap.size()+i);
      }
    }
  }

  return exclusions;
  */
}

// remplir le tableau des tailles
std::map< uint64_t, bool > DepGraphCompoundsBuildVisitorPrivate::computeSizes()
{
/*
  Critical Function : comment logging messages
*/
//  MORPHOLOGINIT;
//  LDEBUG << "Computing sizes";
  std::map< uint64_t, bool > sizes;
  for (uint64_t i = 0; i < m_relsmap.size(); i++)
  {
    sizes.insert(std::make_pair(i, true));
  }
  for (uint64_t i = m_relsmap.size(); i < m_relsmap.size()+m_nodesmap.size(); i++)
  {
    sizes.insert(std::make_pair(i, false));
  }
  return sizes;
}

// calculer l'ordre
std::vector< uint64_t > DepGraphCompoundsBuildVisitorPrivate::computeOrder(
    const std::map< uint64_t,
    std::set< uint64_t > >& exclusions)
{
/*
  Critical Function : comment logging messages
*/
//  MORPHOLOGINIT;
//  LDEBUG << "Computing order";
  std::vector< uint64_t > order;
  std::multimap< uint64_t, uint64_t > tmp;
  std::map< uint64_t, std::set< uint64_t > >::const_iterator it, it_end;
  it = exclusions.begin(); it_end = exclusions.end();
  for (; it != it_end; it++)
  {
    tmp.insert(std::make_pair((*it).second.size(), (*it).first));
  }

  std::multimap< uint64_t, uint64_t >::const_reverse_iterator tmpIt, tmpIt_end;
  tmpIt = tmp.rbegin(); tmpIt_end = tmp.rend();
  for (; tmpIt != tmpIt_end; tmpIt++)
  {
    order.push_back((*tmpIt).second);
  }
  return order;
}

DependencyGraphVertex DepGraphCompoundsBuildVisitorPrivate::findFinalSourceOfCompoundInRelationsSet(
  std::set< uint64_t >& relations)
{
// #ifdef DEBUG_LP
//   MORPHOLOGINIT;
//   LDEBUG << "DepGraphCompoundsBuildVisitorPrivate::findFinalSourceOfCompoundInRelationsSet"
//          <<  relations.size();
//   LDEBUG << "Critical function : comment logging messages"
// #endif
  // le seul noeud cible d'une relation qui n'est pas aussi source

  // ne devrait pas arriver ; test de precaution
  if (relations.empty())
  {
    MORPHOLOGINIT;
    LERROR << "Trying to find a source of compound in an empty set !";
    return 0;
  }

  std::set< DependencyGraphVertex > sources, targets;
  std::set< uint64_t >::const_iterator it, it_end;
  it = relations.begin(); it_end = relations.end();
  for (; it != it_end; it++)
  {
    sources.insert(m_relsmap[*it].first.get<1>());
    targets.insert(m_relsmap[*it].second.get<1>());
  }
  std::set< DependencyGraphVertex > res;
  std::insert_iterator< std::set< DependencyGraphVertex > > ins(res, res.end());
  std::set_difference(targets.begin(), targets.end(),
                      sources.begin(), sources.end(), ins );

  // si il y a un cycle de relations qui n'etait pas detectable
  if (res.empty())
  {
// #ifdef DEBUG_LP
//     LDEBUG << "No final source of compound found in a proposed set !";
//     LDEBUG << "    There was probably a dependencies cycle.";
//     LDEBUG << "    Trying by removing a random relation.";
// #endif
    // retrait d'une relation au hasard et nouvel essai
    relations.erase(*relations.rbegin());
    return findFinalSourceOfCompoundInRelationsSet(relations);
  }
  //    possible d'avoir plusieurs relations vers la meme tete !!!
  DependencyGraphVertex result = *(res.begin());
// #ifdef DEBUG_LP
//   if (res.size() > 1)
//   {
//     LDEBUG << "Multiple sources of compound found in a proposed set ! Choosing one randomly..." << result;
//   }
// #endif
  return result;

}

/** @todo See what changes are possible on the underlying PoS graph */
AnnotationGraphVertex DepGraphCompoundsBuildVisitorPrivate::buildTermFor(
  DependencyGraphVertex vx,
  const std::set< uint64_t > relations,
  bool theHead)
{
  LinguisticGraphVertex vxTokVertex = m_syntacticData->tokenVertexForDepVertex(vx);
#ifdef DEBUG_LP
  MORPHOLOGINIT;
  LDEBUG << "DepGraphCompoundsBuildVisitorPrivate::buildTermFor" << vx << vxTokVertex << theHead;
#endif


  AnnotationGraphVertex cpdAnnotVertex = createAnnotationVertex(vxTokVertex);

  /** if the current vertex is the head of the token (true at first call), the
   * mark it as such */
  if (theHead)
  {
    m_annotationData->annotate(cpdAnnotVertex, Common::Misc::utf8stdstring2limastring("cpdHead"), 1);
    m_annotationData->addMatching("PosGraph", vxTokVertex, "cpdHead", cpdAnnotVertex);
  }


//   EdgeDepRelTypePropertyMap relTypeMap = get(edge_deprel_type, *m_depGraph);

    // Gets the source vertices of the non-compound relations (determiners,
    // adverbs, etc.)
  DependencyGraphInEdgeIt init, init_end;
  boost::tie(init, init_end) = in_edges(vx, *m_depGraph);
  for (; init != init_end; init++)
  {
    if ( !isDepEdgeACompoundNounRelation(*init) )
    {
#ifdef DEBUG_LP
      LDEBUG << "building annotation for non-compound relation " << source( *init, *m_depGraph ) << " -> " << target( *init, *m_depGraph );
#endif
      /** handle modifiers modifiers (like adverbs modifying the adverb
       * handled by the current relation : */
      DependencyGraphVertex inSrc = source( *init, *m_depGraph );
      LinguisticGraphVertex inSrcMorph = m_syntacticData->tokenVertexForDepVertex(inSrc);
      ConceptModifier modifier= createModifierFor(inSrcMorph);
//      if (modifier.getConceptType() == MediaticData::MediaticData::single().getConceptType("LatticeDown"))
      if (modifier.getConceptType() == m_context->getConceptType("LatticeDown"))
      {
#ifdef DEBUG_LP
        LDEBUG << "DepGraphCompoundsBuildVisitorPrivate::buildTermFor Not storable concept type" << modifier.getConceptType()
               << ". aborting this annotation.";
#endif
        break;
      }
      if (!m_annotationData->hasAnnotation(cpdAnnotVertex, Common::Misc::utf8stdstring2limastring("CompoundTokenAnnotation")))
      {
        CompoundTokenAnnotation annot;
        GenericAnnotation ga(annot);
        m_annotationData->annotate(cpdAnnotVertex, Common::Misc::utf8stdstring2limastring("CompoundTokenAnnotation"), ga);
      }
      CompoundTokenAnnotation* annot = m_annotationData->annotation(cpdAnnotVertex, Common::Misc::utf8stdstring2limastring("CompoundTokenAnnotation")).pointerValue<CompoundTokenAnnotation>();
      if (annot != 0)
        annot->push_back(modifier);
      else
      {
        MORPHOLOGINIT;
        LERROR << "DepGraphCompoundsBuildVisitor::buildTermFor annotation CompoundTokenAnnotation has not been created for" << cpdAnnotVertex;
      }
    }
  }

    // Gets the source vertices of this compound's relations
  std::set< DependencyGraphVertex > governors;


  for (auto it = relations.cbegin(), it_end = relations.cend();
       it != it_end;
       it++)
  {
    if (m_relsmap[*it].second.get<1>() == vx)
    {
      DependencyGraphVertex srcDepVertex = m_relsmap[*it].first.get<1>();
#ifdef DEBUG_LP
      LDEBUG << "building annotation for compound relation " << srcDepVertex << " -> " << vx;
#endif
      Common::MediaticData::SyntacticRelationId relType = m_relsmap[*it].first.get<3>();
      if (governors.find(srcDepVertex) == governors.end())
      {
        governors.insert(srcDepVertex);
#ifdef DEBUG_LP
        LDEBUG << "recursive call from" << vx << vxTokVertex;
#endif
        AnnotationGraphVertex srcAnnotVertex = buildTermFor(srcDepVertex, relations);
#ifdef DEBUG_LP
        LinguisticGraphVertex srcTokVertex = m_syntacticData->tokenVertexForDepVertex(srcDepVertex);
        LDEBUG << "returned from recursive call on" << srcDepVertex << srcTokVertex;
#endif
        /// annotation de l'arc

        if (!m_annotationData->hasAnnotation(srcAnnotVertex, "cpdExt"))
        {
          m_annotationData->annotate(srcAnnotVertex, "cpdExt", 1);
          m_annotationData->addMatching("PosGraph", m_syntacticData->tokenVertexForDepVertex(srcDepVertex), "cpdExt", srcAnnotVertex);
        }
        if (!m_annotationData->hasAnnotation(srcAnnotVertex, cpdAnnotVertex, "CompoundTokenAnnotation"))
        {
          CompoundTokenAnnotation edgeAnnot;
          GenericAnnotation ga(edgeAnnot);
          m_annotationData->annotate(srcAnnotVertex, cpdAnnotVertex, Common::Misc::utf8stdstring2limastring("CompoundTokenAnnotation"), ga);
        }
        if (!m_annotationData->hasAnnotation(srcAnnotVertex, cpdAnnotVertex, "CompoundTokenAnnotation"))
        {
        MORPHOLOGINIT;
          LERROR << "DepGraphCompoundsBuildVisitor::buildTermFor annotation CompoundTokenAnnotation has not been created for" << srcAnnotVertex << "->" << cpdAnnotVertex;
          continue;
        }
        CompoundTokenAnnotation* edgeAnnot = m_annotationData->annotation(srcAnnotVertex, cpdAnnotVertex, Common::Misc::utf8stdstring2limastring("CompoundTokenAnnotation")).pointerValue<CompoundTokenAnnotation>();
        if (relType == static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getSyntacticRelationId("ADJPRENSUB")
            || relType == static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getSyntacticRelationId("SUBADJPOST"))
        {
          /// creer juste une annotation de type mod
          //ConceptModifier mod(static_cast<StringsPoolIndex>(0),m_context->getConceptType("Complementer"));
          Lima::Common::MediaticData::ConceptType type = m_context->getConceptType("Complementer");
          ConceptModifier mod(static_cast<StringsPoolIndex>(0),type);
          //ConceptModifier mod(static_cast<StringsPoolIndex>(0));

          edgeAnnot->push_back(mod);
        }
        else if (relType == static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getSyntacticRelationId("COMPDUNOM"))
        {
//           LDEBUG << "Building annot for COMPDUNOM";
          /** creer une annotation de type comp et chercher si le noeud source
           * est cible d'une relation PREPSUB. Si oui, prendre le lemme de la
           * preposition comme type de comp */
          EdgeDepRelTypePropertyMap relTypeMap =
              get(edge_deprel_type, *m_depGraph);

          StringsPoolIndex modString = static_cast<StringsPoolIndex>(0);
          std::vector< uint64_t > modReferences;
          DependencyGraphInEdgeIt modInit, modInit_end;
          boost::tie(modInit, modInit_end) = in_edges(srcDepVertex, *m_depGraph);
          for (; modInit != modInit_end; modInit++)
          {
            uint64_t modRelType = relTypeMap[*modInit];
            if (modRelType == static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getSyntacticRelationId("PREPSUB"))
            {
              VertexDataPropertyMap dataMap = get(vertex_data, *(m_syntacticData->graph()));
              const MorphoSyntacticData* data = dataMap[source(*modInit, *m_depGraph)];
              if (data != 0 && !data->empty())
              {
                modString = *(data->allLemma().begin());
#ifdef DEBUG_LP
                LDEBUG << "Found modifier '" <<Common::MediaticData::MediaticData::changeable().stringsPool(m_language)[modString] << "'";
#endif
                modReferences.push_back(m_syntacticData->tokenVertexForDepVertex(source(*modInit, *m_depGraph)));
              }
              break;
            }
          }
          ConceptModifier mod(
              modString,
              m_context->getConceptType("Complementer"),
              modReferences);
          edgeAnnot->push_back(mod);
        }
      }
    }
  }
#ifdef DEBUG_LP
  LDEBUG << "DepGraphCompoundsBuildVisitor: == DONE buildTermFor " << vx;
#endif
  return cpdAnnotVertex;
}

AnnotationGraphVertex DepGraphCompoundsBuildVisitorPrivate::createAnnotationVertex(
    const LinguisticGraphVertex v)
{
#ifdef DEBUG_LP
  MORPHOLOGINIT;
  LDEBUG << "Creating annotation for morph vertex " << v;
#endif

  AnnotationGraphVertex agv = m_annotationData->createAnnotationVertex();
  m_annotationData->addMatching("PosGraph", v, "cpd", agv);
  m_annotationData->annotate(agv, Common::Misc::utf8stdstring2limastring("PosGraph"), v);

  /* Inutile, la presence de la CompoundTokenAnnotation sera suffisante pour cela

  /// On cree une annotation qui identifie le fait que c'est un noeud de mot compose
  m_annotationData->annotate(agv, Common::Misc::utf8stdstring2limastring("cpd"), 1);
  */

  /** @todo voir que faire pour les noeuds EN et EI */

#ifdef DEBUG_LP
  LDEBUG << "Built annotation vertex " << agv;
#endif
  return agv;
}


std::vector< DependencyGraphEdge >
    DepGraphCompoundsBuildVisitorPrivate::getCompoundsRelationsFromWithChain(
        DependencyGraphVertex from,
        const ChainIdStruct& chId)
{
 #ifdef DEBUG_LP
   MORPHOLOGINIT;
   LDEBUG << "DepGraphCompoundsBuildVisitorPrivate::getCompoundsRelationsFromWithChain"
          << from << " on chain " << chId;
   LDEBUG << "Critical function : comment logging messages";
 #endif
  std::set< DependencyGraphVertex > stackedVxs;
  std::list< DependencyGraphVertex > vxsToExplore;
  vxsToExplore.push_back(from);
  stackedVxs.insert(from);
  std::vector< DependencyGraphEdge > collectedEdges;
  std::set< std::pair<DependencyGraphVertex, DependencyGraphVertex> > collectedEdgesSet;

  VertexChainIdPropertyMap chainsMap = get(vertex_chain_id, *(m_syntacticData->graph()));
  while (!vxsToExplore.empty())
  {
    DependencyGraphVertex vx = vxsToExplore.front();
 #ifdef DEBUG_LP
     LDEBUG << "Exploring from" << vx;
 #endif
    m_visited.insert(vx);
    vxsToExplore.pop_front();
    DependencyGraphOutEdgeIt outit, outit_end;
    boost::tie(outit, outit_end) = out_edges(vx, *m_depGraph);
    for (; outit != outit_end; outit++)
    {
 #ifdef DEBUG_LP
       LDEBUG << "Looking at " << vx << " out relation ";
 #endif
      LinguisticGraphVertex tgtTokVertex = m_syntacticData->tokenVertexForDepVertex(target(*outit, *m_depGraph));
      if ( isDepEdgeACompoundNounRelation(*outit) &&
           chainsMap[tgtTokVertex].find(chId) != chainsMap[tgtTokVertex].end())
      {
 #ifdef DEBUG_LP
        LDEBUG << "Elected";
 #endif
        if (stackedVxs.find(target(*outit, *m_depGraph)) == stackedVxs.end())
        {
          vxsToExplore.push_back(target(*outit, *m_depGraph));
          stackedVxs.insert(target(*outit, *m_depGraph));
        }
        if (collectedEdgesSet.find(std::make_pair(source(*outit,*m_depGraph),target(*outit,*m_depGraph))) == collectedEdgesSet.end())
        {
 #ifdef DEBUG_LP
          LDEBUG << " and collected";
 #endif
          collectedEdges.push_back(*outit);
          collectedEdgesSet.insert(std::make_pair(source(*outit,*m_depGraph),target(*outit,*m_depGraph)));
          m_visitedWithChains.insert(std::make_pair(target(*outit, *m_depGraph),chId));
        }
        else
        {
 #ifdef DEBUG_LP
          LDEBUG << " but not collected";
 #endif
        }
      }
      else
      {
 #ifdef DEBUG_LP
        LDEBUG << "Not collected because not electable because ";
        //LDEBUG << "\t- " << (*outit) << " is not a compounds nouns relation ";
        LDEBUG << "\t- or " << chId << " is not a chain of " << tgtTokVertex;
 #endif
      }
    }
    DependencyGraphInEdgeIt init, init_end;
    boost::tie(init, init_end) = in_edges(vx, *m_depGraph);
    for (; init != init_end; init++)
    {
#ifdef DEBUG_LP
      LDEBUG << "Looking at " << vx << " in relation ";
//               << MediaticData::single().getEntityName(relTypeMap[*init])
//               << " on " << *init;
#endif
      LinguisticGraphVertex srcTokVertex = m_syntacticData->tokenVertexForDepVertex(source(*init, *m_depGraph));
      if ( isDepEdgeACompoundNounRelation(*init) &&
           chainsMap[srcTokVertex].find(chId) != chainsMap[srcTokVertex].end())
      {
//        LDEBUG << "Elected";
        if (stackedVxs.find(source(*init, *m_depGraph)) == stackedVxs.end())
        {
          vxsToExplore.push_back(source(*init, *m_depGraph));
          stackedVxs.insert(source(*init, *m_depGraph));
        }
        if (collectedEdgesSet.find(std::make_pair(source(*init,*m_depGraph),target(*init,*m_depGraph))) == collectedEdgesSet.end())
        {
//          LDEBUG << " and collected";
          collectedEdges.push_back(*init);
          collectedEdgesSet.insert(std::make_pair(source(*init,*m_depGraph),target(*init,*m_depGraph)));
          m_visitedWithChains.insert(std::make_pair(source(*init, *m_depGraph),chId));
        }
//        else
//        {
//          LDEBUG << " but not collected because " << chId << " is not a chain of " << srcTokVertex;
//        }
      }
//      else
//      {
//        LDEBUG << "Not collected because not electable because ";
//        LDEBUG << "\t- " << (*init) << " is not a compounds nouns relation ";
//        LDEBUG << "\t- or " << chId << " is not a chain of " << srcTokVertex;
//      }
    }
  }
  return collectedEdges;
}

std::vector< DependencyGraphEdge >
    DepGraphCompoundsBuildVisitorPrivate::getCompoundsRelationsFromNoChain(
        DependencyGraphVertex from)
{
 #ifdef DEBUG_LP
   MORPHOLOGINIT;
   LDEBUG << "DepGraphCompoundsBuildVisitorPrivate::getCompoundsRelationsFromNoChain"
          << from;
   LDEBUG << "Critical function : comment logging messages";
 #endif
  std::set< DependencyGraphVertex > stackedVxs;
  std::list< DependencyGraphVertex > vxsToExplore;
  vxsToExplore.push_back(from);
  stackedVxs.insert(from);
  std::vector< DependencyGraphEdge > collectedEdges;
  std::set< std::pair<DependencyGraphVertex, DependencyGraphVertex> > collectedEdgesSet;

  while (!vxsToExplore.empty())
  {
    DependencyGraphVertex vx = vxsToExplore.front();
 #ifdef DEBUG_LP
     LDEBUG << "Exploring from" << vx;
 #endif
    m_visited.insert(vx);
    vxsToExplore.pop_front();
    DependencyGraphOutEdgeIt outit, outit_end;
    boost::tie(outit, outit_end) = out_edges(vx, *m_depGraph);
    for (; outit != outit_end; outit++)
    {
 #ifdef DEBUG_LP
       LDEBUG << "Looking at " << vx << " out relation ";
 #endif
      if ( isDepEdgeACompoundNounRelation(*outit) )
      {
 #ifdef DEBUG_LP
        LDEBUG << "Elected";
 #endif
        if (stackedVxs.find(target(*outit, *m_depGraph)) == stackedVxs.end())
        {
          vxsToExplore.push_back(target(*outit, *m_depGraph));
          stackedVxs.insert(target(*outit, *m_depGraph));
        }
        if (collectedEdgesSet.find(std::make_pair(source(*outit,*m_depGraph),target(*outit,*m_depGraph))) == collectedEdgesSet.end())
        {
 #ifdef DEBUG_LP
          LDEBUG << " and collected";
 #endif
          collectedEdges.push_back(*outit);
          collectedEdgesSet.insert(std::make_pair(source(*outit,*m_depGraph),target(*outit,*m_depGraph)));
        }
        else
        {
 #ifdef DEBUG_LP
          LDEBUG << " but not collected";
 #endif
        }
      }
      else
      {
 #ifdef DEBUG_LP
        LDEBUG << "Not collected because not electable because ";
//         LDEBUG << "\t- " << (*outit) << " is not a compounds nouns relation ";
 #endif
      }
    }
    DependencyGraphInEdgeIt init, init_end;
    boost::tie(init, init_end) = in_edges(vx, *m_depGraph);
    for (; init != init_end; init++)
    {
#ifdef DEBUG_LP
      LDEBUG << "Looking at " << vx << " in relation ";
//               << MediaticData::single().getEntityName(relTypeMap[*init])
//               << " on " << *init;
#endif
      if ( isDepEdgeACompoundNounRelation(*init) )
      {
//        LDEBUG << "Elected";
        if (stackedVxs.find(source(*init, *m_depGraph)) == stackedVxs.end())
        {
          vxsToExplore.push_back(source(*init, *m_depGraph));
          stackedVxs.insert(source(*init, *m_depGraph));
        }
        if (collectedEdgesSet.find(std::make_pair(source(*init,*m_depGraph),target(*init,*m_depGraph))) == collectedEdgesSet.end())
        {
//          LDEBUG << " and collected";
          collectedEdges.push_back(*init);
          collectedEdgesSet.insert(std::make_pair(source(*init,*m_depGraph),target(*init,*m_depGraph)));
        }
      }
//      else
//      {
//        LDEBUG << "Not collected because not electable because ";
//        LDEBUG << "\t- " << (*init) << " is not a compounds nouns relation ";
//      }
    }
  }
  return collectedEdges;
}

// calculer les incompatibilites binaires
std::map< uint64_t, std::set< uint64_t > >
    DepGraphCompoundsBuildVisitorPrivate::computeExclusionsWithChain(
        const std::vector< DependencyGraphEdge >& relations,
        const ChainIdStruct& chId)
{
//  MORPHOLOGINIT;
//  LDEBUG << "DepGraphCompoundsBuildVisitorPrivate::computeExclusionsWithChain" << chId;
//   LDEBUG << "Critical Function : comment logging messages"
  std::map< uint64_t, std::set< uint64_t > > exclusions;
  EdgeDepRelTypePropertyMap relTypeMap = get(edge_deprel_type, *m_depGraph);
//   VertexChainIdPropertyMap chainsMap = get(vertex_chain_id, *(m_syntacticData->graph()));
  m_relsmap.clear();
  m_nodesmap.clear();
  std::vector< DependencyGraphEdge >::const_iterator relsit, relsit_end;
  relsit = relations.begin(); relsit_end = relations.end();
  //preparation des donnees
  for (uint64_t relNum=0; relsit != relsit_end; relsit++,relNum++)
  {
    DependencyGraphVertex src = source(*relsit, *m_depGraph);
    DependencyGraphVertex tgt = target(*relsit, *m_depGraph);
    LinguisticGraphVertex srcTokVertex = m_syntacticData->tokenVertexForDepVertex(src);
    LinguisticGraphVertex tgtTokVertex = m_syntacticData->tokenVertexForDepVertex(tgt);
    const Token* srcToken = get(vertex_token, *(m_iter->getGraph()), src);
    const Token* tgtToken = get(vertex_token, *(m_iter->getGraph()), tgt);
    uint64_t srcPos = srcToken->position();
    uint64_t tgtPos = tgtToken->position();
    std::set<uint64_t> srcChains;
    srcChains.insert(chId.chainId());
    std::set<uint64_t> tgtChains;
    tgtChains.insert(chId.chainId());
    Common::MediaticData::SyntacticRelationId relType = relTypeMap[boost::edge(src,tgt,*m_depGraph).first];
//    LDEBUG << srcTokVertex << " -> " << tgtTokVertex << " is number: " << relNum;
    m_relsmap.push_back(std::make_pair(boost::make_tuple(srcPos,srcTokVertex,srcChains,relType),boost::make_tuple(tgtPos,tgtTokVertex,tgtChains,relType)));
    m_nodesmap.push_back(boost::make_tuple(srcPos,srcTokVertex,srcChains));
    m_nodesmap.push_back(boost::make_tuple(tgtPos,tgtTokVertex,tgtChains));
  }
  for (uint64_t i = 0; i < m_relsmap.size(); i++)
  {
    exclusions.insert(std::make_pair(i, std::set< uint64_t >()));
  }
  for (uint64_t i = m_relsmap.size(); i < m_relsmap.size()+m_nodesmap.size(); i++)
  {
    exclusions.insert(std::make_pair(i, std::set< uint64_t >()));
  }
  //  return exclusions;
  // calcul des exclusions
  for (uint64_t i = 0; i < m_relsmap.size(); i++)
  {
    // incompatibilites entre relations
    for (uint64_t j = i+1; j < m_relsmap.size(); j++)
    {
      if (
           // Les positions des sources sont egales
        ( (m_relsmap[i].first.get<0>() == m_relsmap[j].first.get<0>())
          /*             && ( m_relsmap[i].first.get<1>() != m_relsmap[j].first.get<1>()
                            || m_relsmap[i].first.get<3>() != m_relsmap[j].first.get<3>())*/
        )
        ||
           // position de la source du premier egal position de la cible du second
           // mais les 2 vertex sont differents
        ( (m_relsmap[i].first.get<0>() == m_relsmap[j].second.get<0>())
          && ( m_relsmap[i].first.get<1>() != m_relsmap[j].second.get<1>()
               /*|| m_relsmap[i].first.get<3>() == m_relsmap[j].first.get<3>()*/))
        ||
           // position de la source du second egal position de la cible du premier
           // mais les 2 vertex sont differents
           ( (m_relsmap[i].second.get<0>() == m_relsmap[j].first.get<0>())
          && ( m_relsmap[i].second.get<1>() != m_relsmap[j].first.get<1>()
               /*|| m_relsmap[i].first.get<3>() == m_relsmap[j].first.get<3>()*/))
        ||
           // position des deux cibles egales mais vertex differents
        ( (m_relsmap[i].second.get<0>() == m_relsmap[j].second.get<0>())
          && ( m_relsmap[i].second.get<1>() != m_relsmap[j].second.get<1>()
               /*|| m_relsmap[i].first.get<3>() == m_relsmap[j].first.get<3>()*/))
      )
      {
//        LDEBUG << i << " and " << j << " are incompatible";
        exclusions[i].insert(j);
        exclusions[j].insert(i);
      }
//      else
//      {
        //std::cerr << m_relsmap[i].first.get<0>() << " / " << m_relsmap[j].first.get<0>() << std::endl;

//        LDEBUG << i << " and " << j << " are compatible: ";
//      }
    }
    // incompatibilites entre une relation et les noeuds
    for (uint64_t j = 0; j < m_nodesmap.size(); j++)
    {
      if (
           // source: meme position que le noeud mais vertex differents
        (m_relsmap[i].first.get<0>() == m_nodesmap[j].get<0>() && m_relsmap[i].first.get<1>() != m_nodesmap[j].get<1>())
           // cible: meme position que le noeud mais vertex differents
           || (m_relsmap[i].second.get<0>() == m_nodesmap[j].get<0>() && m_relsmap[i].second.get<1>() != m_nodesmap[j].get<1>()) )
      {
//        LDEBUG << i << " and " << i+j << " are incompatible";
        exclusions[i].insert(i+j);
        exclusions[i+j].insert(i);
      }
//      else
//      {
//        LDEBUG << i << " and " << i+j << " are compatible";
//      }
    }
  }
  // incompatibilites entre les noeuds
  for (uint64_t i = 0; i < m_nodesmap.size(); i++)
  {
    for (uint64_t j = 0; j < m_nodesmap.size(); j++)
    {
      // memes positions mais vertex differents
      if ( (m_nodesmap[i].get<0>() == m_nodesmap[j].get<0>()) && (m_nodesmap[i].get<1>() != m_nodesmap[j].get<1>()))
      {
//        LDEBUG << m_relsmap.size()+i << " and " << m_relsmap.size()+i+j << " are incompatible";
        exclusions[m_relsmap.size()+i].insert(m_relsmap.size()+i+j);
        exclusions[m_relsmap.size()+i+j].insert(m_relsmap.size()+i);
      }
//      else
//      {
//        LDEBUG << m_relsmap.size()+i << " and " << m_relsmap.size()+i+j << " are compatible";
//      }
    }
  }

  return exclusions;
}

// calculer les incompatibilites binaires
std::map< uint64_t, std::set< uint64_t > >
    DepGraphCompoundsBuildVisitorPrivate::computeExclusionsNoChain(
        const std::vector< DependencyGraphEdge >& relations)
{
//  MORPHOLOGINIT;
//  LDEBUG << "DepGraphCompoundsBuildVisitorPrivate::computeExclusionsNoChain" << chId;
//   LDEBUG << "Critical Function : comment logging messages"
  std::map< uint64_t, std::set< uint64_t > > exclusions;
  EdgeDepRelTypePropertyMap relTypeMap = get(edge_deprel_type, *m_depGraph);
  m_relsmap.clear();
  m_nodesmap.clear();
  std::vector< DependencyGraphEdge >::const_iterator relsit, relsit_end;
  relsit = relations.begin(); relsit_end = relations.end();
  //preparation des donnees
  for (uint64_t relNum=0; relsit != relsit_end; relsit++,relNum++)
  {
    DependencyGraphVertex src = source(*relsit, *m_depGraph);
    DependencyGraphVertex tgt = target(*relsit, *m_depGraph);
    LinguisticGraphVertex srcTokVertex = m_syntacticData->tokenVertexForDepVertex(src);
    LinguisticGraphVertex tgtTokVertex = m_syntacticData->tokenVertexForDepVertex(tgt);
    const Token* srcToken = get(vertex_token, *(m_iter->getGraph()), src);
    const Token* tgtToken = get(vertex_token, *(m_iter->getGraph()), tgt);
    uint64_t srcPos = srcToken->position();
    uint64_t tgtPos = tgtToken->position();
    std::set<uint64_t> srcChains;
    std::set<uint64_t> tgtChains;
    Common::MediaticData::SyntacticRelationId relType = relTypeMap[boost::edge(src,tgt,*m_depGraph).first];
//    LDEBUG << srcTokVertex << " -> " << tgtTokVertex << " is number: " << relNum;
    m_relsmap.push_back(std::make_pair(boost::make_tuple(srcPos,srcTokVertex,srcChains,relType),boost::make_tuple(tgtPos,tgtTokVertex,tgtChains,relType)));
    m_nodesmap.push_back(boost::make_tuple(srcPos,srcTokVertex,srcChains));
    m_nodesmap.push_back(boost::make_tuple(tgtPos,tgtTokVertex,tgtChains));
  }
  for (uint64_t i = 0; i < m_relsmap.size(); i++)
  {
    exclusions.insert(std::make_pair(i, std::set< uint64_t >()));
  }
  for (uint64_t i = m_relsmap.size(); i < m_relsmap.size()+m_nodesmap.size(); i++)
  {
    exclusions.insert(std::make_pair(i, std::set< uint64_t >()));
  }
  //  return exclusions;
  // calcul des exclusions
  for (uint64_t i = 0; i < m_relsmap.size(); i++)
  {
    // incompatibilites entre relations
    for (uint64_t j = i+1; j < m_relsmap.size(); j++)
    {
      if (
           // Les positions des sources sont egales
        ( (m_relsmap[i].first.get<0>() == m_relsmap[j].first.get<0>())
          /*             && ( m_relsmap[i].first.get<1>() != m_relsmap[j].first.get<1>()
                            || m_relsmap[i].first.get<3>() != m_relsmap[j].first.get<3>())*/
        )
        ||
           // position de la source du premier egal position de la cible du second
           // mais les 2 vertex sont differents
        ( (m_relsmap[i].first.get<0>() == m_relsmap[j].second.get<0>())
          && ( m_relsmap[i].first.get<1>() != m_relsmap[j].second.get<1>()
               /*|| m_relsmap[i].first.get<3>() == m_relsmap[j].first.get<3>()*/))
        ||
           // position de la source du second egal position de la cible du premier
           // mais les 2 vertex sont differents
           ( (m_relsmap[i].second.get<0>() == m_relsmap[j].first.get<0>())
          && ( m_relsmap[i].second.get<1>() != m_relsmap[j].first.get<1>()
               /*|| m_relsmap[i].first.get<3>() == m_relsmap[j].first.get<3>()*/))
        ||
           // position des deux cibles egales mais vertex differents
        ( (m_relsmap[i].second.get<0>() == m_relsmap[j].second.get<0>())
          && ( m_relsmap[i].second.get<1>() != m_relsmap[j].second.get<1>()
               /*|| m_relsmap[i].first.get<3>() == m_relsmap[j].first.get<3>()*/))
      )
      {
//        LDEBUG << i << " and " << j << " are incompatible";
        exclusions[i].insert(j);
        exclusions[j].insert(i);
      }
//      else
//      {
        //std::cerr << m_relsmap[i].first.get<0>() << " / " << m_relsmap[j].first.get<0>() << std::endl;

//        LDEBUG << i << " and " << j << " are compatible: ";
//      }
    }
    // incompatibilites entre une relation et les noeuds
    for (uint64_t j = 0; j < m_nodesmap.size(); j++)
    {
      if (
           // source: meme position que le noeud mais vertex differents
        (m_relsmap[i].first.get<0>() == m_nodesmap[j].get<0>() && m_relsmap[i].first.get<1>() != m_nodesmap[j].get<1>())
           // cible: meme position que le noeud mais vertex differents
           || (m_relsmap[i].second.get<0>() == m_nodesmap[j].get<0>() && m_relsmap[i].second.get<1>() != m_nodesmap[j].get<1>()) )
      {
//        LDEBUG << i << " and " << i+j << " are incompatible";
        exclusions[i].insert(i+j);
        exclusions[i+j].insert(i);
      }
//      else
//      {
//        LDEBUG << i << " and " << i+j << " are compatible";
//      }
    }
  }
  // incompatibilites entre les noeuds
  for (uint64_t i = 0; i < m_nodesmap.size(); i++)
  {
    for (uint64_t j = 0; j < m_nodesmap.size(); j++)
    {
      // memes positions mais vertex differents
      if ( (m_nodesmap[i].get<0>() == m_nodesmap[j].get<0>()) && (m_nodesmap[i].get<1>() != m_nodesmap[j].get<1>()))
      {
//        LDEBUG << m_relsmap.size()+i << " and " << m_relsmap.size()+i+j << " are incompatible";
        exclusions[m_relsmap.size()+i].insert(m_relsmap.size()+i+j);
        exclusions[m_relsmap.size()+i+j].insert(m_relsmap.size()+i);
      }
//      else
//      {
//        LDEBUG << m_relsmap.size()+i << " and " << m_relsmap.size()+i+j << " are compatible";
//      }
    }
  }

  return exclusions;
}

ConceptModifier DepGraphCompoundsBuildVisitorPrivate::createModifierFor(
    LinguisticGraphVertex inSrcMorph)
{
// #ifdef DEBUG_LP
//   MORPHOLOGINIT;
//   LDEBUG << "DepGraphCompoundsBuildVisitorPrivate::createModifierFor " << inSrcMorph;
// #endif
  VertexDataPropertyMap dataMap = get(vertex_data, *(m_syntacticData->graph()));
  const MorphoSyntacticData* data = dataMap[inSrcMorph];
  LinguisticCode macro;
  StringsPoolIndex realization = static_cast<StringsPoolIndex>(0);
  if (data != 0 && !data->empty())
  {
    macro = m_macroAccessor->readValue(data->begin()->properties);
    if (!data->allLemma().empty())
    {
      realization = *(data->allLemma().begin());
    }
  }
  // MediaticData::ConceptType conceptType = static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getConceptForMacro(macro);
  MediaticData::ConceptType conceptType = m_context->getConceptForMacro(/* m_language, */macro);

  std::vector< uint64_t > references;
// #ifdef DEBUG_LP
//   LDEBUG << "DepGraphCompoundsBuildVisitorPrivate::createModifierFor Got macro       " << macro;
//   LDEBUG << "DepGraphCompoundsBuildVisitorPrivate::createModifierFor Got conceptType " << conceptType;
//   LDEBUG << "DepGraphCompoundsBuildVisitorPrivate::createModifierFor Got realization "
//          << realization << Common::MediaticData::MediaticData::single().stringsPool(m_language)[realization];
// #endif
  if (conceptType == m_context->getConceptType("Determiner")
      || conceptType == m_context->getConceptType("SemanticModifier") )
  {
    references.push_back(inSrcMorph);
  }
  return ConceptModifier(realization, conceptType, references);
}

} // closing namespace Compounds
} // closing namespace LinguisticProcessing
} // closing namespace Lima

