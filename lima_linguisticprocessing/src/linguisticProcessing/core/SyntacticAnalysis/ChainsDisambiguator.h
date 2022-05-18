// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/** @brief      Definition of classes for disambiguation of syntagmatic chains paths.
  *
  * @file       ChainsDisambiguator.h
  * @author     Gael de Chalendar <Gael.de-Chalendar@cea.fr> 

  *             Copyright (c) 2004 by CEA
  * @version    $Id$
  */

#ifndef LIMA_LINGUISTICPROCESSING_SYNTACTICANALYSIS_CHAINSDISAMBIGUATOR_H
#define LIMA_LINGUISTICPROCESSING_SYNTACTICANALYSIS_CHAINSDISAMBIGUATOR_H

#include "SyntacticAnalysisExport.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/ChainIdStruct.h"
#include "linguisticProcessing/core/SyntacticAnalysis/SyntacticData.h"
#include "common/MediaticData/mediaticData.h"
#include "linguisticProcessing/common/PropertyCode/PropertyAccessor.h"

#include <vector>
#include <list>
#include <deque>
#include <set>
#include <stdexcept>

#ifdef WIN32
#undef min
#undef max
#endif

namespace Lima {
namespace LinguisticProcessing {
namespace SyntacticAnalysis {

#define SADLOGINIT  LOGINIT("LP::SyntacticAnalysis::Disamb")

/** @brief An element of a Path (out of chain vertex or candidate chain) 
 *
 * Can be either a vertex or a subchain defined by its vertices
 */
class LIMA_SYNTACTICANALYSIS_EXPORT Elem
{
public:
  Elem(uint64_t id, unsigned char type=0) : m_id(id), m_type(type) {}
  Elem(const Elem& elem) : m_id(elem.m_id), m_type(elem.m_type), m_elems(elem.m_elems) {}
  virtual ~Elem() {}
  Elem& operator=(const Elem& elem);
  
  /** @brief Accessor to the vertex or chain id 
   * @return the vertex or chain id
   */
  inline uint64_t id() const {return m_id;}
  
  /** @brief Accessor to the type of this elem: vertex or chain
   * @return 0 if vetex ; 1 if chain
   */
  inline unsigned char type() const {return m_type;}
  
/** @brief Const accessor to the subelements collection 
   * @return the elements of the subchain ; empty for a vertex element
   */
  inline const std::vector< uint64_t >& elems() const {return m_elems;}
  
  /** @brief Non const accessor to the subelements collection 
   * @return the elements of the subchain ; empty for a vertex element
   */
  inline std::vector< uint64_t >& elems() {return m_elems;}
  
private:
  /** @brief vertex or chain id */
  uint64_t m_id;
  /** @brief type of the elem: vertex if 0, chain if 1 */
  unsigned char m_type;
  /** @brief the vertices ids in the chain if type is 1; empty otherwise */
  std::vector< uint64_t > m_elems;
};

/** @brief Stores a path end vertex id and the chains ids that have been found to ignore.
 *
 * Chains to ignore are dynamically updated during the search
 */
class LIMA_SYNTACTICANALYSIS_EXPORT Key
{
public:
  Key(uint64_t id, const std::set< uint64_t>& chainsToIgnore = std::set< uint64_t>()) : 
      m_id(id), m_chainsToIgnore(chainsToIgnore) {}
      
  Key(const Key& key) : m_id(key.m_id), m_chainsToIgnore(key.m_chainsToIgnore) {}
  
  virtual ~Key() {}
  
  Key& operator=(const Key& key);
  
  /** @name Accessors 
   * 
   * Accessors to private members
   */
  /// @{ 
  inline uint64_t id() const {return m_id;}
  inline const std::set< uint64_t >& chainsToIgnore() const {return m_chainsToIgnore;}
  inline std::set< uint64_t >& chainsToIgnore() {return m_chainsToIgnore;}
  /// @}
   
   
private:
  /** @brief vertex id at the end of the path */
  uint64_t m_id;
  /** @brief chains to ignore ofter the vertex designed by the id */ 
  std::set< uint64_t > m_chainsToIgnore;
};

/** @brief Candidate path during chains search. Its elements describe chains and out of chains vertices */
class LIMA_SYNTACTICANALYSIS_EXPORT Path 
{
public:
  Path(
    uint64_t id, 
    const std::set< uint64_t>& chainsToIgnore, 
    unsigned char type=0,
    uint64_t chainId=std::numeric_limits<uint64_t>::max());
    
  Path(const Path& path);
  
  virtual ~Path() {}
  
  Path& operator=(const Path& path);
  
  bool operator<(const Path& path) const;
  
  /** @name Accessors 
   * 
   * Accessors to private members
   */
  /// @{ 
  inline const Key& key() const {return m_key;}
  inline const std::list< Elem >& elems() const {return m_elems;}
  inline uint64_t idealConjVerbNb() const {return m_idealConjVerbNb;}
  inline uint64_t conjVerbNb() const {return m_conjVerbNb;}
  inline uint64_t outChainsWordsNb() const {return m_outChainsWordsNb;}
  inline uint64_t chainsNb() const {return m_chainsNb;}
  inline Key& key() {return m_key;}
  inline std::list< Elem >& elems() {return m_elems;}
  inline uint64_t& idealConjVerbNb() {return m_idealConjVerbNb;}
  inline uint64_t& conjVerbNb() {return m_conjVerbNb;}
  inline uint64_t& outChainsWordsNb() {return m_outChainsWordsNb;}
  inline uint64_t& chainsNb() {return m_chainsNb;}
  inline void key(const Key& key) {m_key = key;}
  inline void elems(const std::list< Elem >& elems) {m_elems = elems;}
  inline void idealConjVerbNb(uint64_t idealConjVerbNb) {m_idealConjVerbNb = idealConjVerbNb;}
  inline void conjVerbNb(uint64_t conjVerbNb) {m_conjVerbNb = conjVerbNb;}
  inline void outChainsWordsNb(uint64_t outChainsWordsNb) {m_outChainsWordsNb = outChainsWordsNb;}
  inline void chainsNb(uint64_t chainsNb) {m_chainsNb = chainsNb;}
  inline void depsNb(uint64_t depsNb) { m_depsNb = depsNb; }
  inline uint64_t depsNb() const { return m_depsNb; }
  /// @}
  
  /** @brief Updates the parameters used to compute the weight of this path 
   * @param dw @b IN <I>const Data::DicoWord& </I> to update ideal conjugated verbs parameters
   * @param incrChainsNb @b IN <I>bool</I> true if a new chain is added, false otherwise
   * @param incrOutChainsWordsNb @b IN <I>bool</I> true if a new out of chains vertex is added, false otherwise
*/
  void updateParams(
      const LinguisticCode& microCateg, 
      bool incrChainsNb, 
      bool incrOutChainsWordsNb,
      MediaId language);
      
private:
  /** @brief Defines the end of the path and chains to ignore on this path */
  Key m_key;
  /** @brief Defines the content of the path (out of chains vertices and chains) */
  std::list< Elem > m_elems;
  
  /** @name Path weight computing parameters */
  /// @{
  /** @brief Ideal number of conjugated verbs = 1 + number of proposition introductors (subordination conjunctions, etc.) */
  uint64_t m_idealConjVerbNb;
  /** @brief Number of conjugated verbs */
  uint64_t m_conjVerbNb;
  /** @brief Number of out of chains words (tokens) */
  uint64_t m_outChainsWordsNb;
  /** @brief Number of chains on the path */
  uint64_t m_chainsNb;
  /** @brief Number of dependency relations with both ends on a vertex of the path */
  uint64_t m_depsNb;
  /// @}
};

/** @brief Builds possible chains paths between two vertices 
 *
 * This class builds possible chains paths between two vertices, trying to avoid
 * to build useless paths where chains are strictly included in the chains of 
 * another path. It disambiguates paths to keep only those that are the best 
 * with respect to some criteria (see Path class for these criteria).
 */
class LIMA_SYNTACTICANALYSIS_EXPORT ChainsDisambiguator
{
public:
  /** @brief Main constructor
   * 
   * Search bounds have to be the top and bottom of a diamond graph.
   * 
   * @param data @b IN/OUT <I>SyntacticData*</I> the dependency graph and 
   * related data
   * @param s @b IN <I>const LinguisticGraphVertex&</I> the top bound of searched 
   * part of the graph
   * @param t @b IN <I>const LinguisticGraphVertex&</I> the bottom bound of 
   * searched part of the graph
   * @param language @b IN <I>unsigned char</I> the language id of the analyzed 
   * text
   * @param depGraphMaxBranchingFactor @b IN <I>uint64_t</I> Max number of 
   * out edges looked at for each node. This is combinatorial explosion 
   * limitation parameter. 
*/
  ChainsDisambiguator(SyntacticData* data,
                      const LinguisticGraphVertex& s,
                      const LinguisticGraphVertex& t,
                      MediaId language,
                      uint64_t depGraphMaxBranchingFactor);

  /** @brief Copy constructor */
  ChainsDisambiguator(const ChainsDisambiguator& cd);
  
  /** @brief Destructor. Nothing to do */
  virtual ~ChainsDisambiguator() {}
  
  /** @brief Affectation operator */
  ChainsDisambiguator& operator=(const ChainsDisambiguator& cd);
  
  /** @brief create the initial paths */
  void initPaths();
  
  /** @brief compute the paths */
  void computePaths();
  
  /** @brief Modify the graph to keep only the selected path 
   * @todo Modify to be able to keep all equivalent paths with respect to the 
   * quality criteria
   */
  void applyDisambiguisation();
  
private:

  
  /**
   * @brief Replaces the last element of hyp by out of chain elements made from 
   * the nodes of elem 
   * @param elem @b IN <I>Elem</I> The element (have to be a chain element) 
   * that will be changed in vertex elements made from its nodes 
   * @param hyp @b IN/OUT <I>Path&</I> The working path
   */
  void cancelCurrentChain(Elem elem, Path& hyp);
  
  
  /**
   * @brief Do what is necessary when no chain is continued on the next vertex
   * and there is several chains ont it
   * @param nextVertex @b IN <I>LinguisticGraphVertex</I> the next vertex in 
   * making the current hypothesis
   * @param nextVertexChains @b IN <I>const std::set\< LinguisticAnalysisStructure::ChainIdStruct \>&</I> the chains on the next vertex
   * @param currentHyp @b IN <I>const Path&</I> The working path
   */
  void startWithSeveralChainsOnNewVertex(
      LinguisticGraphVertex nextVertex,
      const std::set< LinguisticAnalysisStructure::ChainIdStruct >& nextVertexChains, 
      const Path& currentHyp);
  
  /**
   * @brief Computes the number of syntactic dependencies with both bounds on the given path
   * @param path @b IN <I>const Path&</I> The working path
   * @return <B>uint64_t</B> the computed number of dependencies
   */
  uint64_t computeDepsNb(const Path& path);
  
  /** @brief The main data sructure used to hold partial paths during paths search */
  std::deque< Path > m_hypsStack;
  
  /** @brief A collection of completed paths (those that go from m_srcVertex to
   * m_tgtVertex 
   * 
   * The multiset allows to automaticaly sort the Paths according to their 
   * interest criteria with the help of the lesser-than operator of Path
   */
  std::multiset< Path > m_completePaths;
  
  /** @brief Pointer towards the linguistic analysis data */
  SyntacticData* m_data;
  
  /** @brief The vertex where to start the search */
  LinguisticGraphVertex m_srcVertex;
  
  /** @brief The vertex where to end the search */
  LinguisticGraphVertex m_tgtVertex;
  
  /** @brief The language of the analyzed text */
  MediaId m_language;
  
  /** @brief Max number of out edges looked at for each node. 
   * 
   * This is a combinatorial explosion limitation parameter given to the 
   * constructor */
  uint64_t m_depGraphMaxBranchingFactor;
  
  const Common::PropertyCode::PropertyAccessor* m_microAccessor;
};

/** Operators to dump to stream the classes defined here */ 
/// @{
LIMA_SYNTACTICANALYSIS_EXPORT std::ostream& operator<<(std::ostream &os, const Key& k);
LIMA_SYNTACTICANALYSIS_EXPORT std::ostream& operator<<(std::ostream &os, const Elem& e);
LIMA_SYNTACTICANALYSIS_EXPORT std::ostream& operator<<(std::ostream &os, const Path& p);
/// @}

} // closing namespace SyntacticAnalysis
} // closing namespace LinguisticProcessing
} // closing namespace Lima

#endif // LIMA_LINGUISTICPROCESSING_SYNTACTICANALYSIS_CHAINSDISAMBIGUATOR_H
