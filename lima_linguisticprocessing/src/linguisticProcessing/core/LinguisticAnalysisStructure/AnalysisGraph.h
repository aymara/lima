// Copyright 2002-2020 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2020 by CEA LIST                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_LINGUISTICPROCESSING_LINGUISTICANALYSISSTRUCTUREANALYSISGRAPH_H
#define LIMA_LINGUISTICPROCESSING_LINGUISTICANALYSISSTRUCTUREANALYSISGRAPH_H


#include "LinguisticAnalysisStructureExport.h"
#include "LinguisticGraph.h"
#include "common/ProcessUnitFramework/AnalysisContent.h"
#include "linguisticProcessing/common/PropertyCode/PropertyAccessor.h"

namespace Lima
{
namespace Common
{
namespace AnnotationGraphs
{
  class AnnotationData;
}
}
namespace LinguisticProcessing
{
}
}

namespace Lima
{

namespace LinguisticProcessing
{

namespace LinguisticAnalysisStructure
{

/**
 * @brief An @ref AnalysisData containing a @ref LinguisticGraph with a language and an id
 * @author Benoit Mathieu
 */
class LIMA_LINGUISTICANALYSISSTRUCTURE_EXPORT AnalysisGraph : public AnalysisData
{
public:
  AnalysisGraph(const std::string& graphId,
  MediaId language,
  bool deleteTokenWhenDestroyed,
  bool deleteDataWhenDestroyed);
  AnalysisGraph(const std::string& graphId,
  MediaId language,
  bool deleteTokenWhenDestroyed,
  bool deleteDataWhenDestroyed,
  const AnalysisGraph& graph);

  virtual ~AnalysisGraph();

  /** @brief Returns the first vertex of the graph
   * @return LinguisticGraphVertex The first vertex of the graph */
  const LinguisticGraphVertex& firstVertex(void) const { return m_firstVertex; }

  /** @brief Returns the last vertex of the graph
   * @return LinguisticGraphVertex The last vertex of the graph */
  const LinguisticGraphVertex& lastVertex(void) const { return m_lastVertex; }

  /** @brief Returns the underlying graph structure
   * @return const LinguisticGraph* a const pointer to the boost graph
   *         containing the data
   */
  const LinguisticGraph* getGraph(void) const { return m_graph; }

  /** @brief Returns the underlying graph structure
   * @return LinguisticGraph* a mutable pointer to the boost graph
   *         containing the data
   */
  LinguisticGraph* getGraph(void) { return m_graph; }

  const std::string& getGraphId(void) const { return m_graphId; }

  /**
  * @brief Finds the next unambiguated vertex for which micro categories
  *        are all included in the microFilters list. Returns last vertex
  *        if has reached it.
  * @param start @b IN vertex from which to start
  * @param
  */
  LinguisticGraphVertex nextMainPathVertex(
    LinguisticGraphVertex start,
    const Common::PropertyCode::PropertyAccessor& microAccessor,
    const std::list<LinguisticCode> microFilters,
    LinguisticGraphVertex end);

  /**
    * @brief Finds the next vertex after the input vertex that:
    *        1. is on a subgraph with no paralel branch ; <br>
    *        2. is a punctuation (macro category is "ponctu forte")
    *        This method should be called <b>after</b> the disambiguation such
    *        that the VertexWordProperty is filled.
    * @param v @b IN <I>LinguisticGraphVertex\&</I>
    *        The vertex from which to start the search
    * @param g @b IN <I>LinguisticGraph\&</I>
    *        The graph containing @ref v
    * @return <I>LinguisticGraphVertex</I>
    *         The searched vertex or the last one if there is no such vertex
    */
  LinguisticGraphVertex nextChainsBreakFrom(
    const LinguisticGraphVertex& v,
    const Common::PropertyCode::PropertyAccessor& macroAccessor,
    const LinguisticCode& ponctu,
    const Common::PropertyCode::PropertyAccessor& microAccessor,
    LinguisticGraphVertex& nextSentenceBreak);

    //bool isFirstWordOfSentence(const LinguisticGraphVertex& v, MediaId language) const;

    /** @brief Creates the annotations in the agdata corresponding to this graphs
    * vertices
    * @note The given annotation data should generaly be a new fresh empty one
    * @param anagraph @b IN/OUT <I>AnnotationData\&</I>
    *        The annatation graph data to be populated
    */
    void populateAnnotationGraph(
                   Common::AnnotationGraphs::AnnotationData* annotData,
                   const std::string& src);

  inline bool ownsTokens() const {return m_deleteTokenWhenDestroyed;}

  inline bool ownsMorphData() const {return m_deleteDataWhenDestroyed;}

private:

  /** @{ Caution: copy constructor and assignment operators do
      not clone graph (just copy pointer)
      Prevent Copy*/
  AnalysisGraph(const AnalysisGraph&);
  AnalysisGraph& operator = (const AnalysisGraph&);
  /** @} */

  /**
   * create a new graph
   *
   */
  void createGraph();

  /**
   * delete the graph
   */
  void deleteGraph();

  /** @brief A pointer towards the graph structure */
  LinguisticGraph* m_graph;

  /** @brief The first vertex of the graph. Does not change after
   * initialization */
  LinguisticGraphVertex m_firstVertex;

  /** @brief The last vertex of the graph. Does not change after
   * initialization */
  LinguisticGraphVertex m_lastVertex;

  /** @brief if true, delete all tokenProperty pointer when destroyed
    */
  bool m_deleteTokenWhenDestroyed;

  /** @brief if true, delete all dataProperty pointer when destroyed
    */
  bool m_deleteDataWhenDestroyed;

  MediaId m_language;
  std::string m_graphId;
};


template<class T> inline
void getFollowingNodes(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                       const LinguisticGraphVertex v,
                       T& output_container)
{
  LinguisticGraphOutEdgeIt outEdge,outEdge_end;
  boost::tie (outEdge,outEdge_end)=out_edges(v,*(graph.getGraph()));
  for (; outEdge!=outEdge_end; outEdge++)
  {
    output_container.insert(target(*outEdge,*(graph.getGraph())));
  }
}

template<class T> inline
T getFollowingNodes(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                    const LinguisticGraphVertex v)
{
  T followingNodes;
  getFollowingNodes<T>(graph, v, followingNodes);
  return followingNodes;
}

inline
LinguisticGraphVertex getOneFollowingNode(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                                          const LinguisticGraphVertex v)
{
  LinguisticGraphOutEdgeIt outEdge,outEdge_end;
  boost::tie (outEdge,outEdge_end)=out_edges(v,*(graph.getGraph()));
  LinguisticGraphVertex result;
  while (outEdge!=outEdge_end)
  {
    result=target(*outEdge,*(graph.getGraph()));
    outEdge++;
    if (outEdge!=outEdge_end)
    {
      throw std::logic_error("getOneFollowingNode assumes that there is one and only one following node. Multiple nodes detected.");
    }
    return result;
  }

  throw std::logic_error("getOneFollowingNode assumes that there is one and only one following node. No preceding nodes found.");
}

template<class T> inline
void getPrecedingNodes(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                       const LinguisticGraphVertex& v,
                       T& output_container)
{
  LinguisticGraphInEdgeIt inEdge,inEdge_end;
  boost::tie (inEdge,inEdge_end)=in_edges(v,*(graph.getGraph()));
  for (; inEdge!=inEdge_end; inEdge++)
  {
    output_container.insert(source(*inEdge,*(graph.getGraph())));
  }
}

template<class T> inline
T getPrecedingNodes(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                    const LinguisticGraphVertex v)
{
  T precedingNodes;
  getPrecedingNodes<T>(graph, v, precedingNodes);
  return precedingNodes;
}

inline
LinguisticGraphVertex getOnePrecedingNode(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                                          const LinguisticGraphVertex v)
{
  LinguisticGraphInEdgeIt inEdge,inEdge_end;
  boost::tie (inEdge,inEdge_end)=in_edges(v,*(graph.getGraph()));
  LinguisticGraphVertex result;
  while (inEdge!=inEdge_end)
  {
    result=source(*inEdge,*(graph.getGraph()));
    inEdge++;
    if (inEdge!=inEdge_end)
    {
      throw std::logic_error("getOnePrecedingNode assumes that there is one and only one preceding node. Multiple nodes detected.");
    }
    return result;
  }

  throw std::logic_error("getOnePrecedingNode assumes that there is one and only one preceding node. No preceding nodes found.");
}

}

}

}

#endif
