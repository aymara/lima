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

/**
  *
  * @file        SyntacticAnalyzer-chains.h
  * @author      Gael de Chalendar (Gael.de-Chalendar@cea.fr) \n
  *              Copyright (c) 2003 by CEA
  * @date        Created on Aug, 31 2004
  * @version     $Id$
  *
  */

#ifndef SYNTACTICANALYZERNOCHAINS_H
#define SYNTACTICANALYZERNOCHAINS_H

#include "SyntacticAnalysisExport.h"
#include "SyntacticData.h"
#include "common/MediaProcessors/MediaProcessUnit.h"

#include <string>
#include <stack>

namespace Lima
{
namespace LinguisticProcessing
{
namespace SyntacticAnalysis
{

#define SACLOGINIT LOGINIT("LP::SyntacticAnalysis::Chains")
#ifdef WIN32
#undef min
#undef max
#endif

#define SYNTACTICANALYZERCHAINS_FIRSTID 1
#define SYNTACTICANALYZERNOCHAINS_CLASSID "SyntacticAnalyzerNoChains"

/** @brief This class is a processing unit that can be inserted in an analysis 
 * pipeline. It builds the nominal and verbal chains.
 * 
 * This is the first step of the syntactic analysis. It works on a pointer to a 
 * graph holding all syntactic data. Nominal and verbal chains are maximal 
 * syntagms linking together words related to, repectively, a noun and a verb.
 * They have no real linguistic validity but are useful to find compounds
 * 
 * It is a @ref MediaProcessUnit. It accepts the following parameters:
 *   - param:chainMatrix : 
 *   - param:maxChainsNbByVertex :
 *   - param:maxChainLength :
 *   - param:ponctuCategory :
 */
class LIMA_SYNTACTICANALYSIS_EXPORT SyntacticAnalyzerNoChains : public MediaProcessUnit
{
public:
  
  /**
   * @brief Default constructor. Nothing to do.
   */
  SyntacticAnalyzerNoChains();

  //  SyntacticAnalyzer(SyntacticAnalyzer&);

  /**
   * @brief Default destructor. Nothing to do.
   */
  virtual ~SyntacticAnalyzerNoChains() {}

  /**
   * @brief Process unit initialization function 
   * @param unitConfiguration @b IN <I>GroupConfigurationStructure&</I> 
   * Reference to this process unit configuration group in the configuration.
   * @param manager @b IN <I>Manager*</I> Process unit manager.
   * @throws InvalidConfiguration,std::exception
   */
  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;

  /**
   * @brief Process unit main processing function
   * @param analysis @b IN/OUT <I>AnalysisContent&</I> Reference to the 
   * structure holding the analysis data
   * @return <b>LimaStatusCode</b> The status of the process unit processing 
   * (success or failing).
   */
  LimaStatusCode process(AnalysisContent& analysis) const override;

  /** @brief First step of the syntactic analysis. Identify nominal and verbal
    *        chains in the subgraph defined by the given two vertices.
    * @param s @b IN <I>const LinguisticGraphVertex\&</I>
    *        The source vertex from which chains will be searched for
    * @param t @b IN <I>const LinguisticGraphVertex\&</I>
    *        The target vertex after which chains will not be searched for
    * @param startChainId @b IN/OUT <I>uint64_t&</I> The first chain id to 
    * use for new chains. Given by reference to be able to update the value in
    * the caller.
    */
  void identifyChains(SyntacticData* data,
                      const LinguisticGraphVertex& s,
                      const LinguisticGraphVertex& t,
                      uint64_t& startChainId) const;


  /**
   * @brief Read accessor to the language of the analyzed text
   * @return 
   */
  MediaId language() const;

private:

  /** @brief The first chain id to use for new chains at start of the process.*/
  static const uint64_t m_firstChainId = SYNTACTICANALYZERCHAINS_FIRSTID;
  
  /** @brief Exception thrown when an end of chain is expected in the search 
   * data structure but no one is found. */
  class NoChainEndInStackException : public std::exception {};
  
  /** @brief This is the element of the collection used during search.
   * 
   * The tuple elements are a vertex (the next vertex that will be examined), a 
   * boolean true if the vertex is a valid chain end and a stack of vertices 
   * which are the next vertex sons (used in the variable pileSons in the 
   * functions) 
   */
  typedef boost::tuple< LinguisticGraphVertex, bool, std::vector< LinguisticGraphVertex > > ChainStackTuple;

  /**
   * @brief When a chain is validated, this function create it in the 
   * underlying syntactic graph.
   * 
   * @param data @b IN/OUT <I>SyntacticData*</I> 
   * A pointer to the syntactic graph and other analysis data.
   * @param pile @b IN <I>const std::vector\< ChainStackTuple \>&</I>
   * The chain to report data.
   * @param type  @b IN <I>Common::MediaticData::ChainsType</I> 
   * Indicates the type of the chain (nominal or verbal).
   * @param alreadyFinished  @b IN/OUT <I>std::set< LinguisticGraphVertex >&</I> 
   * A vertex is put into this set as soon as all possible chains has been 
   * found for him. So, if found later, it will allow to abort this branch of 
   * the search.
   * @param chainId  @b IN/OUT <I>uint64_t&</I> 
   * The chain id to use for the new chain. Will be updated in the function.
   * @param stop  @b IN <I>const LinguisticGraphVertex&</I>
   * The vertex where to stop. If encountered in the chain stack, chain 
   * reporting is stopped there.
   */
  void reportChainInGraph(
                          SyntacticData* data,
                          const std::vector< ChainStackTuple >& pile,
                          Common::MediaticData::ChainsType type,
                          std::set< LinguisticGraphVertex >& alreadyFinished,
                          uint64_t& chainId,
                          const LinguisticGraphVertex& stop) const;

  /**
   * @brief Check wether all the parent vertices of the given vertex are 
   * already finished.
   * @param data @b IN <I>const SyntacticData*</I> 
   * A pointer to the syntactic graph and other analysis data.
   * @param v @b IN <I>const LinguisticGraphVertex&</I> The vertex to test
   * @param alreadyFinished  @b IN <I>const std::set< LinguisticGraphVertex >&</I> 
   * A vertex is put into this set as soon as all possible chains has been 
   * found for him. So, if found later, it will allow to abort this branch of 
   * the search.
   * @return true if all the parents (in-edges source vertices) are already 
   * finished.
   */
  bool parentsFinished(
                        const SyntacticData* data,
                        const LinguisticGraphVertex& v,
                        const std::set< LinguisticGraphVertex >& alreadyFinished) const;
  
  /**
   * @brief Computes a string representation of a chain.
   * 
   * The string representation of the chain allows to easily test if a chain is 
   * a new one or has already been produced, complete or as part of a longer 
   * chain.
   * @param data @b IN <I>SyntacticData*</I> 
   * A pointer to the syntactic graph and other analysis data.
   * @param pile @b IN <I>const std::vector\< ChainStackTuple \>&</I>
   * The chain to convert into string.
   * @param type  @b IN <I>Common::MediaticData::ChainsType</I> 
   * Indicates the type of the chain (nominal or verbal).
   * @param alreadyFinished  @b IN/OUT <I>std::set< LinguisticGraphVertex >&</I> 
   * A vertex is put into this set as soon as all possible chains has been 
   * found for him. So, if found later, it will allow to abort this branch of 
   * the search.
   * @param chainId  @b IN <I>uint64_t</I> 
   * The chain id to use for the new chain. Will be updated in the function.
   * @param stop  @b IN <I>const LinguisticGraphVertex&</I>
   * The vertex where to stop. If encountered in the chain stack, chain 
   * reporting is stopped there. If the default value is used, the search stops 
   * at the end of the graph.
   * @return The string representation of the given chain.
   */
  std::string stringChain(
                          const SyntacticData* data,
                          const std::vector< ChainStackTuple >& pile,
                          Common::MediaticData::ChainsType type,
                          std::set< LinguisticGraphVertex >& alreadyFinished,
                          uint64_t chainId,
                          const LinguisticGraphVertex& stop = std::numeric_limits<LinguisticGraphVertex>::max()) const;

  /**
   * @brief Unstacks the given stack until its last element is a chain end.
   * @param data @b IN <I>SyntacticData*</I> 
   * A pointer to the syntactic graph and other analysis data.
   * @param pile @b IN/OUT <I>std::vector\< ChainStackTuple \>&</I>
   * The chain to work on.
   * @param type  @b IN <I>Common::MediaticData::ChainsType</I> 
   * Indicates the type of the chain (nominal or verbal).
   * @return The vertex of the the last element of the stack (which is a chain 
   * end)
   * @retval FirstVertex If the given chain does not contain any
   * chain end
   */
  LinguisticGraphVertex unstackUptoChainEnd(
                                          const SyntacticData* data,
                                          std::vector< ChainStackTuple >& pile,
                                          Common::MediaticData::ChainsType type
                                        ) const;

protected:

  /** @brief The language of the analyzed text*/
  MediaId m_language;

  /** @brief A pointer onto the chains definition matrices.
   *
   * Only used to initialize the SyntacticData in the process method.
   */
  SyntagmDefStruct* m_chainMatrix;

  /** @brief Sets the max number of chains on one vertex.
   * 
   * This is a combinatorial explosion limiter parameter. A soon as there is 
   * this number of different chains on one vertex, this one will be added in
   * the list of finished vertices. It is initialized by the syntactic analysis
   * maxChainsNbByVertex configuration parameter or has a default value.
   */
  uint64_t m_maxChainsNbByVertex;
  
  /** @brief Sets the max length of a chain.
   * 
   * This is a combinatorial explosion limiter parameter. A soon as a chain 
   * length reach the value of this member, it will no be authorized to be 
   * longer. It is then validated or reduced up to the last chain end vertex.
   * It is initialized by the syntactic analysis maxChainLength configuration 
   * parameter or has a default value.
   */
  uint64_t m_maxChainLength;

  const Common::PropertyCode::PropertyAccessor* m_macroAccessor;
  const Common::PropertyCode::PropertyAccessor* m_microAccessor;
  LinguisticCode m_ponctuCategory;
};

inline MediaId SyntacticAnalyzerNoChains::language() const
{
  return m_language;
}

} // closing namespace SyntacticAnalysis
} // closing namespace LinguisticProcessing
} // closing namespace Lima

#endif
