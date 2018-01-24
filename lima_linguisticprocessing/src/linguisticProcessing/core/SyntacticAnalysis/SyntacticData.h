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
/** @brief       Data used for the syntactic analyzis of texts
  *
  * @file        SyntacticData.h
  * @author      Gael de Chalendar (Gael.de-Chalendar@cea.fr) 

  *              Copyright (c) 2003 by CEA
  * @date        Created on Oct, 1 2003
  * @version     $Id$
  *
  */
// clazy:exclude=rule-of-two-soft


#ifndef LIMA_SYNTACTICANALYSIS_SYNTACTICDATA_H
#define LIMA_SYNTACTICANALYSIS_SYNTACTICDATA_H

#include "SyntacticAnalysisExport.h"
#include "DependencyGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/common/linguisticData/languageData.h" // for SyntacticRelationId
#include "common/ProcessUnitFramework/AnalysisContent.h"

#include <tuple>

namespace Lima {
namespace LinguisticProcessing {

namespace LinguisticAnalysisStructure
{
  class Token;
  class MorphoSyntacticData;
}

namespace SyntacticAnalysis {

class SyntagmDefStruct;

/**
  * @brief This class points to a graph, its dependency graph and the structure
  *        that holds the maping between the nodes of both graphs
  * @note  if some vertices are added or removed from the graph, the changes
  *        should be reported in the dependency graph and the mapping
  */
class LIMA_SYNTACTICANALYSIS_EXPORT SyntacticData : public AnalysisData
{
public:
typedef std::tuple< LinguisticGraphVertex, LinguisticGraphVertex, Common::MediaticData::SyntacticRelationId > Relation;

    /** @brief This constructor uses the given graph to construct a dependency
      *        graph with the same number of vertices and it fills the mapping
      *        between them
      * @param graph @b IN/OUT <I>LinguisticGraph*</I> the graph containing
      *        morphological data
      */
    SyntacticData(LinguisticAnalysisStructure::AnalysisGraph* pit, const SyntagmDefStruct* matrices);

    virtual ~SyntacticData();

    inline LinguisticGraph* graph() {return m_graph;}
    inline const LinguisticGraph* graph() const {return m_graph;}

    inline DependencyGraph* dependencyGraph() {return m_depGraph;}
    inline const DependencyGraph* dependencyGraph() const {return m_depGraph;}

    inline LinguisticAnalysisStructure::AnalysisGraph* iterator() {return m_anagraph;}
    inline const LinguisticAnalysisStructure::AnalysisGraph* iterator() const {return m_anagraph;}

    inline const SyntagmDefStruct* matrices() const {return m_matrices;}
    inline void matrices(const SyntagmDefStruct* matrices) {m_matrices=matrices;}

  inline std::vector<LinguisticAnalysisStructure::Token*>& ownedTokens() {return m_ownedTokens;}
  inline std::vector<LinguisticAnalysisStructure::MorphoSyntacticData*>& ownedMorphosyntacticData() {return m_ownedMorphData;}
  
    DependencyGraphVertex depVertexForTokenVertex(const LinguisticGraphVertex& v) const;
    LinguisticGraphVertex tokenVertexForDepVertex(const DependencyGraphVertex& v) const;

    /** 
      * @brief Computes the number of dependency relations in the graph between
      * the given source and taget morphosyntactic vertices
      */
    uint64_t depsBetween(
        const LinguisticGraphVertex& srcVertex, 
        const LinguisticGraphVertex& tgtVertex);
    /**
      * @brief This method is used to setup the search of the dependency relations
      *        when the PoS tagging is done and thus the number of vertices will not
      *        change anymore in the morphosyntactic graph. It creates a dependency
      *        graph with the same number of nodes than in the morphosyntactic graph
      *        and a mapping expressing the bijection between the nodes of both
      *        graphs.
      */
    void setupDependencyGraph();

    /** 
     * find vertices related to a given vertex by a given relation
     * (related vertices can be either source or target of the relation) 
     * 
     * @param LinguisticGraphVertex the given vertex
     * @param relation the type of relation to look for
     * 
     * @return the list of related vertices
     */
    std::vector<LinguisticGraphVertex> 
      findRelated(const LinguisticGraphVertex,
                  const Common::MediaticData::SyntacticRelationId& relation);

    /** 
     * find vertices related to a given vertex a a relation
     * that belongs to a given set of relations
     * (related vertices can be either source or target of the relation) 
     * 
     * @param LinguisticGraphVertex the given vertex
     * @param relation the set of relations to look for
     * 
     */
    std::vector<LinguisticGraphVertex> 
      findRelated(const LinguisticGraphVertex,
                  const std::set<Common::MediaticData::SyntacticRelationId>& relations);


    inline void clearStoredRelations() {m_relations.clear();}
    
    inline Relation relation()
    {
      if (m_relations.empty()) {
          return std::make_tuple(m_anagraph->firstVertex(), m_anagraph->lastVertex(), 0);
      }
        else
        {
          Relation r = m_relations.front(); // clazy:exclude=rule-of-two-soft
          m_relations.pop_front();
          return r;
        }
    }

    inline bool relation(const LinguisticGraphVertex& v1, 
                         const LinguisticGraphVertex& v2,
                         const Common::MediaticData::SyntacticRelationId relationType=0)
    {
//        SALOGINIT;
        Relation r = std::make_tuple(v1,v2,relationType);
      if (m_relations.empty() || !(m_relations.front() == r))
        {
          m_relations.push_back(r);
          return true;
        }
        else
        {
          return false;
        }  
    };

    std::pair<LinguisticGraphVertex, DependencyGraphVertex> addVertex();

    void addRelationNoChain(const Common::MediaticData::SyntacticRelationId& relType, 
                            const LinguisticGraphVertex& src, 
                            const LinguisticGraphVertex& dest);

    void modifyRelationNoChain(const Common::MediaticData::SyntacticRelationId& relType, 
                            const LinguisticGraphVertex& src, 
                            const LinguisticGraphVertex& dest,
                            const Common::MediaticData::SyntacticRelationId& newType);
                                
    inline bool storeRelationForSelectionalConstraint(const LinguisticGraphVertex& v1,
                          const LinguisticGraphVertex& v2,
                          const Common::MediaticData::SyntacticRelationId relationType=0)
    {
      m_relationStoredForSelectionalConstraint = std::make_tuple(v1,v2,relationType);
      return true;
    };

    inline Relation relationStoredForSelectionalConstraint()
    {
      return m_relationStoredForSelectionalConstraint; // clazy:exclude=rule-of-two-soft
    };

    bool removeDependency(const LinguisticGraphVertex& v1,
                          const LinguisticGraphVertex& v2,
                          const Common::MediaticData::SyntacticRelationId relationType=0);

  private:
    /** @brief  */
    LinguisticAnalysisStructure::AnalysisGraph* m_anagraph;

    /** @brief The graph containing morphological data */
    LinguisticGraph* m_graph;

    /** @brief The graph containing syntactic dependency relations */
    DependencyGraph* m_depGraph;

    const SyntagmDefStruct* m_matrices;

    /** @brief The mapping between both graphs */
    std::pair<
        std::map< LinguisticGraphVertex, DependencyGraphVertex >,
        std::map< DependencyGraphVertex, LinguisticGraphVertex > > m_mapping;

    std::list< Relation > m_relations;
  
  std::vector<LinguisticAnalysisStructure::Token*> m_ownedTokens;
  std::vector<LinguisticAnalysisStructure::MorphoSyntacticData*> m_ownedMorphData;

  Relation m_relationStoredForSelectionalConstraint;
};

} // closing namespace SyntacticAnalysis
} // closing namespace LinguisticProcessing
} // closing namespace Lima

#endif
