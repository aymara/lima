// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
                          compactDict.h  -  description
                             -------------------
    begin                : mer mai 28 2003
    copyright            : (C) 2003 by Olivier Mesnard
    email                : olivier.mesnard@cea.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *  Compact dictionnary based on finite state automata implemented with    *
 *  Boost Graph library.                                                   *
 *  Algorithm is described in article from Daciuk, Mihov, Watson & Watson: *
 *  "Incremental Construction of Minimal Acyclic Finite State Automata"    *
 *  How to use it to compute hash code is explained in  'perfect hashing' *
 *  of document http://odur.let.rug.nl/alfa/fsa_stuff/#PerfHash            *
 *                                                                         *
 ***************************************************************************/

#ifndef FSA_ACCESS_16_H
#define FSA_ACCESS_16_H

#include "common/misc/adjacency_listnowarn.hpp"
#include "common/misc/graphviznowarn.hpp"
#include "common/Data/strwstrtools.h"
#include "FsaExceptions.h"
#include "FsaAccessHeader.h"
#include "FsaAccessIOHandler.h"
#include "PrefixIterator.h"

// From standard library
#include <fstream>
#include <map>
#include <algorithm>
#include <iostream>
#include <vector>
#include <string>

// // From boost library
#include <boost/config.hpp>

// // From ICU
#define U16_IS_LEAD(c) (((c)&0xfffffc00)==0xd800)
#define U16_IS_TRAIL(c) (((c)&0xfffffc00)==0xdc00)
#define U16_SURROGATE_OFFSET ((0xd800<<10UL)+0xdc00-0x10000)
#define U16_GET_SUPPLEMENTARY(lead, trail) \
(((char32_t)(lead)<<10UL)+(char32_t)(trail)-U16_SURROGATE_OFFSET)

#define U16_LEAD(supplementary) (char32_t)(((supplementary)>>10)+0xd7c0)
#define U16_TRAIL(supplementary) (char32_t)(((supplementary)&0x3ff)|0xdc00)

#define U_IS_SURROGATE(c) (((c)&0xfffff800)==0xd800)
#define U16_IS_SINGLE(c) !U_IS_SURROGATE(c)
#define U16_IS_SURROGATE(c) U_IS_SURROGATE(c)
#define U16_IS_SURROGATE_LEAD(c) (((c)&0x400)==0)

#define U16_NEXT(s, i, length, c) { \
(c)=(s)[(i)++].unicode(); \
if(U16_IS_LEAD(c)) { \
  uint16_t __c2; \
  if((i)<(length) && U16_IS_TRAIL(__c2=(s)[(i)].unicode())) { \
    ++(i); \
    (c)=U16_GET_SUPPLEMENTARY((c), __c2); \
    } \
    } \
    }
enum vertex_text_t { vertex_text = 4000 };
namespace boost {BOOST_INSTALL_PROPERTY(vertex, text);}
enum vertex_count_t { vertex_count = 4001 };
namespace boost {BOOST_INSTALL_PROPERTY(vertex, count);}

namespace Lima {
namespace Common {
namespace FsaAccess {

// TODO: use boost::tuple to implement state property (FINAL,HEAD_OF_CLASS,CONFLUENT,HASH_NUM)

enum vertex_qualif {
 FINAL_16=        0x20000000,
 HEAD_OF_CLASS_16=0x40000000,
 SET_16=    0x80000000,
 QUALITY_16=      0xE0000000,
 TEXT_POS_16=     0x1FFFFFFF } ;

template <typename graphType>
struct dicoGraph_traits16 {

  // type of the 'vertex_name' property (with const_type modifier!!)
  // used to store number of characters of text whose codepoint can be
  // stored on UTF-16 word
  // The 3 most significatnt bits are reserved to store status of vertex:
  // final, confluent or head of class
  typedef typename boost::property_map<graphType,boost::vertex_name_t>::const_type
   vname_map_type;
  typedef typename boost::property_map<graphType,boost::vertex_name_t>::type
   nconst_vname_map_type;

  // type of the 'vertex_text' property used to store 
  // character (ordered according to codepoint) in relation with transition
  typedef typename boost::property_map<graphType,vertex_text_t>::const_type
   vtext_map_type;
  typedef typename boost::property_map<graphType,vertex_text_t>::type
   nconst_vtext_map_type;

  // type of the 'vertex_count' property used to store
  // number of elements in subautomata to compute hash function
  typedef typename boost::property_map<graphType,vertex_count_t>::const_type
   vcount_map_type;
  typedef typename boost::property_map<graphType,vertex_count_t>::type
   nconst_vcount_map_type;
};

struct selected_graph_types16
{
  /** Declare a property (vertex_text) of type LimaString
    * to store more efficiently edge label */
  typedef boost::property< vertex_text_t, LimaString >
   dicoVertexTextProperty;
  /** Declare a property (vertex_name) of type uint8_t to store
   vertex quality :
   (qualifer & 1) == 1: final, (qualifier & 2) == 2: head of class,
   (qualifer & 4) == 4: confluent */
  typedef boost::property< boost::vertex_name_t, VERTEX_PROPERTY_16, dicoVertexTextProperty>
   dicoVertexStatusProperty;
  /** Declare a property (vertex_count) of type std::vector<int> to store
   count of sub automata (to compute hash) */
  typedef boost::property< vertex_count_t, std::vector<int>, dicoVertexStatusProperty>
   dicoVertexCountProperty;
  /**  Graph used for FsaDictSpare
   container types are chosen for their minimal size */
  typedef boost::adjacency_list<boost::vecS,
                                boost::vecS,
                                boost::bidirectionalS,
                                dicoVertexCountProperty
  > spareGraphType;
  
  /** Graph used for FsaDictBuilder
   container types are chosen for their efficiency in insertion
   bidirectional capabilities are used in FsaDictBuilder::replaceOrRegister operation*/
  typedef boost::adjacency_list<boost::vecS,
                                boost::listS,
//                                         boost::vecS,
                                boost::bidirectionalS,
                                dicoVertexCountProperty
  > builderGraphType;
};

// Helpers to print graph (for trace and debug) used by Graphviz functions
template <typename vname_map_type, typename vtext_map_type, typename vcount_map_type,
          typename dicoVertex> class dicoVertexWriter16 {
  public:
    dicoVertexWriter16(vname_map_type vname_map,
      vtext_map_type vtext_map, vcount_map_type vcount_map);
    void operator()(std::ostream& out, const dicoVertex& v) const;

  private:
   vname_map_type m_vname_map;
   vtext_map_type m_vtext_map;
   vcount_map_type m_vcount_map;
  };

template <typename dicoEdge> class dicoEdgeWriter16 {
  public:
    dicoEdgeWriter16() {}
    void operator()(std::ostream& out, const dicoEdge& edge ) const {
      LIMA_UNUSED(out)
      LIMA_UNUSED(edge)
    }
  private:
 };

#define dicoVertexType typename boost::graph_traits<graphType>::vertex_descriptor
 
template <typename graphType>
class FsaAccess16 : public FsaAccessHeader {

public:

  /// type of vertex descriptor
  /// type of edge descriptor
  typedef typename boost::graph_traits<graphType>::edge_descriptor dicoEdgeType;

  typedef typename boost::graph_traits<graphType>::degree_size_type dico_degree_size_type;

  typedef selected_graph_types16::dicoVertexStatusProperty dicoVertexProperty;

  FsaAccess16(bool trie_direction_fwd);
  virtual ~FsaAccess16() {}
//    friend std::ostream& operator << <graphType>(std::ostream& os, const FsaAccess16& dico);
  virtual void print( std::ostream &os ) const;
  virtual void printGraph( std::ostream &os ) const;
  void pack();
    
  /** check integrity of subgraph */
  void checkIntegrity( dicoVertexType from ) const;
  
protected:
    // For Builder and BuilderRandom
  /**
    * Search for equivalent state in register. v1 and v2 are Equivalent means:
    *    - v1 and v2 are both final or not final
    *    - v1 and V2 have same number of out edges
    *    - label on out edges are the same and lead to same target state
    * We use in this function the capacity of bidirectionnalS adjacency_list to
    * go from tempstate one step through out-edges and then one step through in-edges to find
    * vertex with same target vertex.
    * The final state without out edges is compared first because it is a special case
    * and is often the right state to merge with tempstate (for every word which are not
    * prefixes of another word.
    *
    **/
  std::pair<const dicoVertexType,bool> findEquivalentInRegister( dicoVertexType tempState );
  
  /** are both state equivalent?
   * We assume that edges are ordered */
  bool equivalent( dicoVertexType referenceState, dicoVertexType candidateState ) const;

  /** For IO
    * Factory of IO Handler: Handler depends on graphType: with mapping or not */
  virtual FsaAccessIOHandler<graphType>* getFsaAccessIOHandler() const = 0;

  /** Parcours recursif du graphe avec creation d'un tableau de conversion ptr -> Id
    * On renomme les noeuds avec des numeros consecutifs, pour eviter
    * d'ecrire et de lire l'indice des noeuds (dans le cas ou le parametre
    * vertexList est vecS) et pour se ramener au cas o le parametre vertexList
    * du template adjacencyList est vecS. */
  void writeVertices( AbstractFsaAccessOStreamWrapper &ow, FsaAccessIOHandler<graphType>* iOHandler,
    dicoVertexType from );
  void readBody( AbstractFsaAccessIStreamWrapper& iw );
  void writeBody( AbstractFsaAccessOStreamWrapper &ow );


  /** For all navigation
    * Factory of prefixIterator (prefixIt depends on direction: forward/reverse) */
  PrefixIterator* getPrefixIterator(const LimaString & word,
                const uint64_t offset = 0) const;

  /** Recursively goes through the graph from @ref from, following edges labelled by the prefix
   * iterator chars. When it returns, @ref from is the last vertex reachable using the initial
   * prefix content and @ref prefixIt gives access to the remaining of the word.
   */
  void getPrefix( dicoVertexType &from,
    PrefixIterator* prefixIt ) const;

  virtual void addSuffix( dicoVertexType from,
    PrefixIterator* prefixIt );

  graphType m_graph;
  dicoVertexType m_0degreeOutVertex;
  dicoVertexType m_rootVertex;

  uint64_t m_size;

private:
  void print( std::ostream &os, dicoVertexType from,
    LimaString &prefix ) const;
    
  /** Finds the uniq vertex with 0 out edges. Called once after graph loading */
  dicoVertexType find0degreeVertex(dicoVertexType from ) const;
protected:
  /**
    * find the right offset in the vector of out_edge:
    * search for the character currentChar in the string text
    * which contains label of all out_edges using dichotomy search
    * (assume characters are ordered in textString)
    */

  int32_t findEdge( const char32_t searchChar,
    const LimaString& textString,
    int32_t min, int range, int nb_unit_for_char ) const;

  /**
    * find where to insert currentChar in the string text
    * using dichotomy search
    * (assume characters are ordered in textString)
    */
  int32_t findOffsetToInsertBefore( const char32_t searchChar,
    const LimaString& textString,
    int32_t min, int range, int nb_unit_per_char ) const;
};

} // namespace FsaAccess
} // namespace Common
} // namespace Lima

#include "common/FsaAccess/FsaAccess16.tcc"

#endif   //FSA_ACCESS_16_H
