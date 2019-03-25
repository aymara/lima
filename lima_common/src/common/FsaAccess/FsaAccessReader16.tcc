/*
    Copyright 2002-2020 CEA LIST

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
                          FsaAccessReader16.tcc  -  description
                             -------------------
    begin                : mer mai 28 2003
    copyright            : (C) 2003 by Olivier Mesnard
    email                : olivier.mesnard@cea.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *  compact dictionnary based on finite state automata                     *
 *  implemented with Boost Graph library                                   *
 *                                                                         *
 ***************************************************************************/

// From standard library
#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <algorithm>
// From boost library
#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>

#include "common/LimaCommon.h"

#include "common/Data/strwstrtools.h"
#include "FsaAccess16.h"

namespace Lima {
namespace Common {
namespace FsaAccess {

template <typename graphType >
FsaAccessReader16<graphType>::FsaAccessReader16(bool trie_direction_fwd)
: FsaAccess16<graphType>(trie_direction_fwd), m_size(0) {
}

template <typename graphType>
void FsaAccessReader16<graphType>::read ( const std::string & filename )
{
  FSAAIOLOGINIT;
#ifdef DEBUG_CD
  LDEBUG <<  "FsaAccessReader16::read(" << filename.c_str() << ")";
#endif
  std::ifstream is(filename.data(), std::ios::binary );
  if( is.fail() ) {
    std::string mess = "FsaAccess16::read: Can't open file " + filename;
    LERROR << mess.c_str();
    throw( AccessByStringNotInitialized( mess ) );
  }

  FsaAccessReader16<graphType>::read(is);
  is.close();
#ifdef DEBUG_CD
  LDEBUG <<  "FsaAccessReader16::read(" << filename.c_str() << "): end";
#endif
}

template <typename graphType>
void FsaAccessReader16<graphType>::read ( std::istream& is )
{
#ifdef DEBUG_CD
  FSAAIOLOGINIT;
  LDEBUG <<  "FsaAccessReader16::read(std::istream)";
#endif
  FsaAccessIStreamWrapper iw(is);
  FsaAccessReader16<graphType>::read(iw);
}

template <typename graphType>
void FsaAccessReader16<graphType>::read ( FsaAccessDataInputHandler& ih )
{
#ifdef DEBUG_CD
  FSAAIOLOGINIT;
  LDEBUG <<  "FsaAccessReader16::read(FsaAccessDataInputHandler)";
#endif
  FsaAccessInputHandlerWrapper iw(ih);
  FsaAccessReader16<graphType>::read(iw);
}

template <typename graphType>
void FsaAccessReader16<graphType>::read ( AbstractFsaAccessIStreamWrapper& iw )
{
#ifdef DEBUG_CD
  FSAAIOLOGINIT;
  LDEBUG <<  "FsaAccessReader16::read(AbstractFsaAccessIStreamWrapper)";
#endif
  FsaAccessHeader::read(iw);

  FsaAccess16<graphType>::readBody( iw );
  m_size = buildHash();
#ifdef DEBUG_CD
  LDEBUG <<  "FsaAccessReader16::read computed size" << m_size;
#endif
}

template <typename graphType >
uint64_t FsaAccessReader16<graphType>::getSize() const {
 return m_size+1;
}

template <typename graphType >
uint64_t  FsaAccessReader16<graphType>::buildHash() {
#ifdef DEBUG_CD
  FSAALOGINIT;
  LDEBUG <<  "FsaAccessReader16::buildHash()";
#endif
  uint64_t result = computeHash( FsaAccess16<graphType>::m_rootVertex );
#ifdef DEBUG_CD
  LDEBUG <<  "FsaAccessReader16::buildHash result = " << result;
#endif
  return result;
}

// Parcours recursif du graphe pour le calcul des coefficients
// pour la fonction de hash
// se calcule a partir des tailles des sous automates (= nombre
// de mots differents reconnus par le sous-automate)
template <typename graphType >
int FsaAccessReader16<graphType>::computeHash( typename boost::graph_traits<graphType>::vertex_descriptor from )
{

#ifdef DEBUG_CD
  FSAAHASHLOGINIT;
  LTRACE << "FsaAccessReader16::computeHash(" << from << ")" ;
#endif

  auto vname_map = boost::get(boost::vertex_name,FsaAccess16<graphType>::m_graph);
  auto vcount_map = boost::get(vertex_count,FsaAccess16<graphType>::m_graph);

  // vocabulaire du sous_graphe
  int total(0);
  // tableau des coefficients
  auto& counts = get(vcount_map,from);
  // nombre de sous automates
  auto outd = boost::out_degree(from, FsaAccess16<graphType>::m_graph);

  VERTEX_PROPERTY_16 val = get(vname_map, from);
  if( (val & SET_16) == 0 ) {
  //if( counts.size() == 0 ) {
    // On parcours les sous-arbre
    typename boost::graph_traits<graphType>::out_edge_iterator ei, edge_end;
    boost::tie(ei,edge_end) = boost::out_edges(from,FsaAccess16<graphType>::m_graph);
    // d'abord les outd-1 premier
    for( unsigned  int i = 0 ; ei != edge_end  ; ei++ , i++ ) {
      int subtotal = computeHash( target(*ei,FsaAccess16<graphType>::m_graph) );
      total = total + subtotal;
      // memorizing outd-1 first to compute the hash
      if( i+1 < outd ) {
#ifdef DEBUG_CD
        LTRACE << "FsaAccessReader16::computeHash(" << from
                << "): counts.push_back(" << total << ")" ;
#endif
        counts.push_back(total);
      }
    }
    put(vname_map, from, get(vname_map, from) | SET_16);
  }
  else {
    // On totalise les tailles des sous-arbres
    typename boost::graph_traits<graphType>::out_edge_iterator ei, edge_end;
    boost::tie(ei,edge_end) = boost::out_edges(from,FsaAccess16<graphType>::m_graph);
    if( outd > 1 ) {
      assert( counts.size() == outd-1 );
      total = counts[outd-2];
#ifdef DEBUG_CD
      LTRACE << "FsaAccessReader16::computeHash(" << from << "): outd="
             << outd << ", total=" << total << ")" ;
      std::ostringstream oss;
      copy(counts.begin(), counts.end(), std::ostream_iterator<int>(oss, ", "));
      LTRACE << "FsaAccessReader16::computeHash(" << from << "): counts=["
      <<  oss.str().c_str() << "]" ;
#endif
      ei += (outd - 1);
    }
    if( ei != edge_end ) {
      total = total + computeHash( target(*ei,FsaAccess16<graphType>::m_graph) );
    }
/*
    for( uint32_t i = 0 ; i+1 < outd ; ei++ , i++, cIt++ ) {
#ifdef DEBUG_CD
      LTRACE << "FsaAccessReader16::computeHash: before assert 1004, outd="
             << outd << ", i=" << i << ")" ;
#endif
      assert(ei != edge_end);
      total = total + *cIt;
    }
    // On calcul le dernier puisqu'il n'a pas ete memorise
    if( ei != edge_end ) {
      int subtotal = computeHash( target(*ei,FsaAccess16<graphType>::m_graph) );
      total = total + subtotal;
    }
  */
  }
  if( (val & FINAL_16) == FINAL_16 ) {
#ifdef DEBUG_CD
    LTRACE << "FsaAccessReader16::computeHash(" << from << "): FINAL node, increment " << total ;
#endif
    total++;
  }
  return total;
}

/*
uint64_t FsaAccessReader16<graphType>::getSize() const {
  return FsaAccessReader16<selected_graph_types16::spareGraphType>::getSize();
}
*/
template <typename graphType >
uint64_t FsaAccessReader16<graphType>::getIndex(const LimaString & word ) const
 {
#ifdef DEBUG_CD
  FSAALOGINIT;
  LDEBUG <<  "FsaAccessReader::getIndex(" << word << ")";
#endif
 uint64_t result = 0;

  // TODO replace -1 by 0 or ACCESS_BY_STRING_NO_STRING
  if( word.size() == 0 )
    return 0;

  // start at initial State of Fsa
  typename boost::graph_traits<graphType>::vertex_descriptor currentVertex = FsaAccess16<graphType>::m_rootVertex;

  // get the property map for 'vertex_text' property
  typename dicoGraph_traits16<graphType>::vname_map_type vname_map =
    boost::get(boost::vertex_name,FsaAccess16<graphType>::m_graph);
  typename dicoGraph_traits16<graphType>::vtext_map_type vtext_map =
    boost::get(vertex_text,FsaAccess16<graphType>::m_graph);
  typename dicoGraph_traits16<graphType>::vcount_map_type vcount_map =
    boost::get(vertex_count,FsaAccess16<graphType>::m_graph);

  // Create an iterator to get letters one at a time
  PrefixIterator* prefixIt = FsaAccess16<graphType>::getPrefixIterator(word);
  int32_t wordOffset;
  for( ; prefixIt->hasNextLetter() ; prefixIt->next(wordOffset) ) {
    char32_t letter = prefixIt->getNextLetter(wordOffset);

#ifdef DEBUG_CD
    LTRACE <<  "FsaAccessReader::getIndex: search letter in text of vertex " << currentVertex
             ;
#endif
    VERTEX_PROPERTY_16 vprop = get(vname_map,currentVertex);

    if( (vprop&FINAL_16) == FINAL_16) {
      result++;
#ifdef DEBUG_CD
      LTRACE <<  "FsaAccessReader::getIndex: result++ " << result;
#endif
    }

    // find the character letter in text property
    const Lima::LimaString& text = get(vtext_map,currentVertex);
    int32_t highCharTextPos = vprop&TEXT_POS_16;
    int32_t edgeOffset = 0;
#ifdef DEBUG_CD
    LTRACE <<  "FsaAccessReader::getIndex: highCharTextPos= "<< highCharTextPos;
#endif
    if( wordOffset == 1 ) {
      edgeOffset = FsaAccess16<graphType>::findEdge( letter, text, 0, highCharTextPos, wordOffset );
    }
    else {
      int32_t textOffset = 0;
      textOffset = FsaAccess16<graphType>::findEdge( letter, text, highCharTextPos, text.length(), wordOffset );
#ifdef DEBUG_CD
    LTRACE <<  "FsaAccessReader::getIndex: textOffset= "<< textOffset;
#endif
      edgeOffset = highCharTextPos + (textOffset - highCharTextPos)/2;
    }
#ifdef DEBUG_CD
    LTRACE <<  "FsaAccessReader::getIndex: edgeOffset= "<< edgeOffset;
#endif

    if( edgeOffset >= 0 ) {
      const std::vector<int>& counts = get(vcount_map,currentVertex);
      typename boost::graph_traits<graphType>::out_edge_iterator ei, edge_end;
      boost::tie(ei,edge_end) = boost::out_edges(currentVertex,FsaAccess16<graphType>::m_graph);
      if (ei==edge_end) {
#ifdef DEBUG_CD
        LTRACE <<  "FsaAccessReader::getIndex: no output edges, edgeOffset="<< edgeOffset;
#endif
        return 0;
      }
      typename FsaAccess16<graphType>::dicoEdgeType edge = *(ei+edgeOffset);
      currentVertex = target(edge, FsaAccess16<graphType>::m_graph);
      if( edgeOffset > 0 ) {
        result += counts[edgeOffset-1];
      }
#ifdef DEBUG_CD
      LTRACE <<  "FsaAccessReader::getIndex: match " << edgeOffset;
      if( edgeOffset > 0 ) {
        LTRACE << " add " << counts[edgeOffset-1];
      }
#endif
    }
    else {
#ifdef DEBUG_CD
     LTRACE <<  "FsaAccessReader::getIndex: mismatch " << letter;
#endif
      delete prefixIt;
      return 0;
    }
  }
#ifdef DEBUG_CD
  LTRACE <<  "FsaAccessReader::getIndex: !prefixIt.hasNextLetter()";
#endif
  delete prefixIt;
#ifdef DEBUG_CD
  LTRACE <<  "FsaAccessReader::getIndex: get(vname_map,currentVertex) = "
         << get(vname_map,currentVertex) ;
#endif
  if( (get(vname_map,currentVertex)&FINAL_16) == FINAL_16)
    return result+1;
  else
    return 0;
}

template <typename graphType >
Lima::LimaString FsaAccessReader16<graphType>::getExtent(
  const Lima::LimaString & prefix ) const
{
#ifdef DEBUG_CD
  FSAALOGINIT;
  LDEBUG <<  "FsaDictionary::getExtent(" << prefix << ")";
#endif

  auto prefixPos = getStartNode(prefix);

  auto vtext_map = boost::get(vertex_text, FsaAccess16<graphType>::m_graph);
    // find the character letter in text property
  const Lima::LimaString& text = get(vtext_map,prefixPos);
  return text;
}

//template <typename graphType >
//uint64_t FsaAccessReader16<graphType>::getIndex(const LimaString & word ) const

template <typename graphType >
 std::pair<AccessSubWordIterator,AccessSubWordIterator >
 FsaAccessReader16<graphType>::getSubWords(
   const uint64_t offset,
   const LimaString & word ) const {
#ifdef DEBUG_CD
  FSAALOGINIT;
  LDEBUG <<  "FsaAccessReader16<graphType>::getSubWords("
          << offset << ", "
          << word << ")";
#endif

  // return prefix_iterator starting at this node
  return std::make_pair (
    AccessSubWordIterator(new fsaReader_subword_iterator16<graphType>(*this, word, FsaAccess16<graphType>::m_rootVertex, offset)),
    AccessSubWordIterator(new fsaReader_subword_iterator16<graphType>(*this, word)));
}

/**
 * constructor to build an iterator "past the last position"
**/
template <typename graphType >
fsaReader_subword_iterator16<graphType>::fsaReader_subword_iterator16(
   const FsaAccessReader16<graphType> & dico,
   const LimaString &word)
  : m_dico(dico), m_graph(m_dico.m_graph),
    m_prefixIt(m_dico.getPrefixIterator(word,word.length() )),
    m_curr( typename boost::graph_traits<graphType>::vertex_descriptor()),
    m_index(0) {
}

/**
 * constructor to build an iterator at beginning of entries
**/
template <typename graphType >
fsaReader_subword_iterator16<graphType>::fsaReader_subword_iterator16(
  const FsaAccessReader16<graphType> & dico,
  const LimaString &word,
  typename boost::graph_traits<graphType>::vertex_descriptor node,
  const uint64_t offset )
  : m_dico(dico), m_graph(m_dico.m_graph),
    m_prefixIt(m_dico.getPrefixIterator(word,offset)),
    m_curr(node),
    m_index(0) {
#ifdef DEBUG_CD
  FSAALOGINIT;
  LDEBUG <<  "fsaSpare_subword_iterator::fsaSpare_subword_iterator("
            << word
            << offset << ")";
#endif
  operator ++(0);
}

template <typename graphType >
fsaReader_subword_iterator16<graphType>::fsaReader_subword_iterator16(const fsaReader_subword_iterator16<graphType> &orig)
  : ClonableSubWordIterator(),
    m_dico(orig.m_dico), m_graph(m_dico.m_graph),
    m_prefixIt(orig.m_prefixIt->clone(orig.m_prefixIt)),
    m_curr(orig.m_curr),
    m_index(orig.m_index) {
#ifdef DEBUG_CD
  FSAALOGINIT;
  LDEBUG <<  "fsaSpare_subword_iterator::fsaSpare_subword_iterator(orig)"
        ;
#endif
}

template <typename graphType >
fsaReader_subword_iterator16<graphType>::~fsaReader_subword_iterator16() {
  delete m_prefixIt;
}

template <typename graphType >
const std::pair< uint64_t,uint64_t>
  fsaReader_subword_iterator16<graphType>::operator*() const {
#ifdef DEBUG_CD
  FSAALOGINIT;
  LDEBUG << "fsaSpare_subword_iterator::operator*(): externalWordPos = "
              << m_prefixIt->getExternalWordPos() << " curr = " << m_curr;
#endif
   /**
   * 06/10/2005 Benoit Mathieu :
   * J'ai ajout�- 1 pour corriger le d�alage. C'est pas b� �nettoyer par la suite
   * 10/11/2005 Benoit Mathieu :
   * Comme on d�ale de 1 afin d'avoir des ID de 1 �n, avec 0 pour vide, j'ai supprim�-1
   */
  return std::make_pair(m_prefixIt->getExternalWordPos(), m_index);
}

template <typename graphType >
bool fsaReader_subword_iterator16<graphType>::operator==(const AbstractSubWordIterator& abit) const {
  const fsaReader_subword_iterator16<graphType>& it=static_cast<const fsaReader_subword_iterator16<graphType>&>(abit);
  return( (m_prefixIt->operator==( *(it.m_prefixIt)) )
       && (&m_graph == &(it.m_graph))
       && (m_index == it.m_index) );
}

template <typename graphType >
bool fsaReader_subword_iterator16<graphType>::operator!=(const AbstractSubWordIterator& it) const {
#ifdef DEBUG_CD
  FSAALOGINIT;
  LDEBUG <<  "fsaSpare_subword_iterator::operator!=()";
#endif
  const fsaReader_subword_iterator16<graphType>& fsaAsi=dynamic_cast<const fsaReader_subword_iterator16<graphType>&>(it);
  return( (m_prefixIt->operator!=(*(fsaAsi.m_prefixIt)))
       || (m_index != fsaAsi.m_index)
       || (&m_graph != &(fsaAsi.m_graph)) );
}

template <typename graphType >
ClonableSubWordIterator* fsaReader_subword_iterator16<graphType>::clone() const {
#ifdef DEBUG_CD
  FSAALOGINIT;
  LDEBUG <<  "fsaSpare_subword_iterator::clone()";
#endif
  return new fsaReader_subword_iterator16<graphType>(*this);
}

template <typename graphType >
fsaReader_subword_iterator16<graphType> & fsaReader_subword_iterator16<graphType>::operator++( int dummy ) {
  LIMA_UNUSED(dummy);
#ifdef DEBUG_CD
  FSAALOGINIT;
  LDEBUG <<  "fsaSpare_subword_iterator::operator++(): begin";
#endif
  typename dicoGraph_traits16<graphType>::vname_map_type vname_map =
    boost::get(boost::vertex_name,m_graph);
  typename dicoGraph_traits16<graphType>::vtext_map_type vtext_map =
    boost::get(vertex_text,m_graph);
  typename dicoGraph_traits16<graphType>::vcount_map_type vcount_map =
    boost::get(vertex_count,m_graph);

  // iterate through the word (considered as a set of characters )
  for( ; m_prefixIt->hasNextLetter() ; ) {
    int32_t wordOffset;
#ifdef DEBUG_CD
    LDEBUG <<  "fsaSpare_subword_iterator::operator++(): m_curr = "
              << m_curr << ", m_wordPos = " << m_prefixIt->getWordPos();
#endif
    char32_t letter = m_prefixIt->getNextLetter(wordOffset);
#ifdef DEBUG_CD
    LDEBUG <<  "fsaSpare_subword_iterator::operator++(): search for letter "
              << LimaChar(letter) << "wordOffset=" << wordOffset;
#endif
    // find the character letter in text property
    const Lima::LimaString& text = get(vtext_map,m_curr);
    VERTEX_PROPERTY_16 vprop = get(vname_map,m_curr);
    int32_t highCharTextPos = vprop&TEXT_POS_16;
    int32_t edgeOffset = 0;
    if( wordOffset == 1 ) {
      edgeOffset = m_dico.findEdge( letter, text, 0, highCharTextPos, wordOffset);
    }
    else {
      int32_t textOffset;
      textOffset = m_dico.findEdge( letter, text, highCharTextPos, text.length(), wordOffset );
      edgeOffset = highCharTextPos + (textOffset - highCharTextPos)/2;
    }

    if( edgeOffset >= 0 ) {
      m_prefixIt->next(wordOffset);
      const std::vector<int>& counts = get(vcount_map,m_curr);
      typename boost::graph_traits<graphType>::out_edge_iterator ei, edge_end;
      boost::tie(ei,edge_end) = boost::out_edges(m_curr,m_graph);
      if (ei==edge_end) {
#ifdef DEBUG_CD
        LDEBUG <<  "FsaAccessReader::fsaReader_subword_iterator16<graphType>::operator++: no output edges, edgeOffset="<< edgeOffset;
#endif
        return *this;
      }
      typename FsaAccessReader16<graphType>::dicoEdgeType edge = *(ei+edgeOffset);
      m_curr = target(edge, m_graph);
      if( edgeOffset > 0 ) {
        m_index += counts[edgeOffset-1];
      }
#ifdef DEBUG_CD
      LDEBUG <<  "fsaSpare_subword_iterator::operator++(): match" << edgeOffset;
      if( edgeOffset > 0 ) {
        LDEBUG << " add " << counts[edgeOffset-1];
      }
#endif
      // check if current label is final
      if( (get(vname_map,m_curr)&FINAL_16) == FINAL_16) {
        m_index++;
        return *this;
      }
    }
    else {
#ifdef DEBUG_CD
      LDEBUG <<  "fsaSpare_subword_iterator::operator++(): unmatch letter " << LimaChar(letter);
#endif
      break;
    }
  }
  // return iterator "past the last position"
  m_prefixIt->terminate();
  m_index = 0;
  return *this;
}

template <typename graphType >
std::pair< AccessSuperWordIterator, AccessSuperWordIterator >
 FsaAccessReader16<graphType>::getSuperWords(const LimaString & word ) const {
#ifdef DEBUG_CD
  FSAALOGINIT;
  LDEBUG <<  "FsaDictionary::getSuperWords("
                 << word << ")";
#endif

  typename boost::graph_traits<graphType>::vertex_descriptor prefixPos = getStartNode(word);

  // There is no more exception thrown
  // when no key in the dico has word as substring, return start=end iterator
  typename boost::graph_traits<graphType>::vertex_descriptor first = FsaAccess16<graphType>::m_rootVertex;
  if( (prefixPos == first) && (word.length() != 0) ){

    return std::pair<AccessSuperWordIterator,AccessSuperWordIterator > (
    AccessSuperWordIterator(new fsaReader_superword_iterator16<graphType>(*this,word)),
    AccessSuperWordIterator(new fsaReader_superword_iterator16<graphType>(*this,word)));
  }
#ifdef DEBUG_CD
  LDEBUG <<  "FsaDictionary::getSuperWords: startVertex = " << prefixPos;
#endif

  // return prefix_iterator starting at this node
  return std::pair<AccessSuperWordIterator,AccessSuperWordIterator > (
    AccessSuperWordIterator(new fsaReader_superword_iterator16<graphType>(*this,prefixPos,word)),
    AccessSuperWordIterator(new fsaReader_superword_iterator16<graphType>(*this,word)));
}

/**
 * constructor to build an iterator "past the last position"
**/
template <typename graphType >
fsaReader_superword_iterator16<graphType>::fsaReader_superword_iterator16(
    const FsaAccessReader16<graphType> & dico,
    const LimaString &prefix)
  : m_dico(dico), m_graph(m_dico.m_graph), m_prefix(prefix) {
  m_curr = typename boost::graph_traits<graphType>::vertex_descriptor();
}

/**
 * constructor to build an iterator at beginning of entries
**/
template <typename graphType >
fsaReader_superword_iterator16<graphType>::fsaReader_superword_iterator16(const FsaAccessReader16<graphType> & dico,
    typename boost::graph_traits<graphType>::vertex_descriptor node,
    const LimaString &prefix)
  : m_curr(node), m_dico(dico), m_graph(m_dico.m_graph), m_prefix(prefix) {

  /* initialize context to be pushed in the context_stack */
  iterator_context<graphType> start_context;

  boost::tie(start_context.out_edge,start_context.out_edge_end) = boost::out_edges(m_curr,m_graph);
  typename dicoGraph_traits16<graphType>::vtext_map_type vtext_map =
    boost::get(vertex_text,m_graph);
  const Lima::LimaString& text = get(vtext_map,m_curr);
  start_context.word_content = text.data();
  start_context.word_length = text.length();
  start_context.wordPos = 0;
  start_context.wordOffset = 0;
  m_context_stack.push_back(start_context);


  if(start_context.out_edge != start_context.out_edge_end)
  {
    operator ++(0);
  }
}

template <typename graphType >
bool fsaReader_superword_iterator16<graphType>::operator==(const AbstractSuperWordIterator& abit) const {
  const fsaReader_superword_iterator16<graphType>& it=static_cast<const fsaReader_superword_iterator16<graphType>&>(abit);
  return( (&m_graph == &(it.m_graph))
       &&  (m_prefix == it.m_prefix)
       &&  (m_curr == it.m_curr) );
}

template <typename graphType >
bool fsaReader_superword_iterator16<graphType>::operator!=(
 const AbstractSuperWordIterator& it) const {
  return( !(*this == it));
}

template <typename graphType >
ClonableSuperWordIterator* fsaReader_superword_iterator16<graphType>::clone() const {
  return new fsaReader_superword_iterator16<graphType>(*this);
}

template <typename graphType >
LimaString FsaAccessReader16<graphType>::getSpelling(const uint64_t givenIndex )
  const  {
  uint64_t index = givenIndex-1;
  uint64_t accu = 0;
#ifdef DEBUG_CD
  FSAALOGINIT;
  LDEBUG <<  "FsaAccessSpare::getSpelling(" << index << ")";
#endif
  LimaString result;

  // start at initial State of Fsa
  typename boost::graph_traits<graphType>::vertex_descriptor currentVertex = FsaAccess16<graphType>::m_rootVertex;
#ifdef DEBUG_CD
  LDEBUG <<  "FsaAccessSpare::getSpelling: (0) currentV = " << currentVertex;
#endif

  typename dicoGraph_traits16<graphType>::vname_map_type vname_map =
    boost::get(boost::vertex_name,FsaAccess16<graphType>::m_graph);
  typename dicoGraph_traits16<graphType>::vtext_map_type vtext_map =
    boost::get(vertex_text,FsaAccess16<graphType>::m_graph);
  typename dicoGraph_traits16<graphType>::vcount_map_type vcount_map =
    boost::get(vertex_count,FsaAccess16<graphType>::m_graph);

  // test if index exceeds maxIndex
  if( index >= m_size ) {
#ifdef DEBUG_CD
    LERROR << "FsaAccessSpare::getSpelling: no word for id " << index;
#endif
    return result;
  }

  // walk through the tree and accumulate size of vocabulary of non matching branches
  for( ; accu < index ; ) {
#ifdef DEBUG_CD
    LDEBUG << "FsaAccessSpare::getSpelling: (1) currentV = " << currentVertex
              << ", accu = " << accu;
#endif
    if(( (get(vname_map, currentVertex)&FINAL_16) == FINAL_16) ) {
#ifdef DEBUG_CD
        LDEBUG << "FsaAccessSpare::getSpelling: (2) currentV = "
	          << currentVertex << " accu++ ";
#endif
      accu++;
    }
    typename boost::graph_traits<graphType>::out_edge_iterator ei, edge_end;
    boost::tie(ei,edge_end) = boost::out_edges(currentVertex,FsaAccess16<graphType>::m_graph);
    if (ei==edge_end) {
#ifdef DEBUG_CD
      LDEBUG <<  "FsaAccessReader::getSpelling: no output edges";
#endif
      return result;
    }
    const std::vector<int>& counts = get(vcount_map,currentVertex);
    int delta = 0;
    int32_t edgeOffset = 0;
    for( typename std::vector<int>::const_iterator it = counts.begin() ; it != counts.end() ; it++ ) {
      if( index >= (accu + *it)  ) {
        delta = *it;
        edgeOffset++;
      }
      else {
        break;
      }
    }
#ifdef DEBUG_CD
      LDEBUG << "FsaAccessSpare::getSpelling: edgeOffset = "
         << edgeOffset << ", delta = " << delta;
#endif
    accu += delta;
    const Lima::LimaString& text = get(vtext_map,currentVertex);
    const QChar* text_content = text.data();
    int32_t highCharTextPos = get(vname_map,currentVertex)&TEXT_POS_16;
    if( edgeOffset < highCharTextPos ) {
      Lima::LimaString letter = text_content[edgeOffset];
#ifdef DEBUG_CD
      LDEBUG << "FsaAccessSpare::getSpelling: edgeOffset < " << highCharTextPos
         << ", letter = " << letter.unicode() << ", text_length = " << text.length();
#endif
      if( FsaAccessHeader::getTrieDirectionForward() )
        result.push_back(letter);
      else
        result.push_front(letter);
    }
    else {
//      Lima::LimaString letter = text_content[edgeOffset];
#ifdef DEBUG_CD
      LDEBUG << "FsaAccessSpare::getSpelling: edgeOffset >= " << highCharTextPos
                << ", text_content += " << highCharTextPos;
#endif
      text_content += highCharTextPos;
#ifdef DEBUG_CD
      LDEBUG << "text_content += " << (edgeOffset - highCharTextPos)*2;
#endif
      text_content = text_content + (edgeOffset - highCharTextPos)*2;
      if( FsaAccessHeader::getTrieDirectionForward() ) {
        result.push_back(*text_content++);
        result.push_back(*text_content++);
      }
      else {
        result.push_front(*text_content++);
        result.push_front(*text_content++);
      }
    }
    currentVertex = target(*(ei+edgeOffset),FsaAccess16<graphType>::m_graph);

  }

  while(( (get(vname_map, currentVertex)&FINAL_16) != FINAL_16 ) ) {
#ifdef DEBUG_CD
    LDEBUG <<  "FsaAccessSpare::getSpelling phase2: accu = "
              << accu << ", currentV = " << currentVertex;
#endif
    typename boost::graph_traits<graphType>::out_edge_iterator ei, edge_end;
    boost::tie(ei,edge_end) = boost::out_edges(currentVertex,FsaAccess16<graphType>::m_graph);
    const Lima::LimaString& text = get(vtext_map,currentVertex);
    typename Lima::LimaString::const_iterator eprop = text.begin();
    if(ei != edge_end) {
#ifdef DEBUG_CD
      LDEBUG << "FsaAccessSpare::getSpelling: push '"
                << (*eprop).unicode() << "'";
#endif
      if( FsaAccessHeader::getTrieDirectionForward() )
        result.push_back(*eprop);
      else
        result.push_front(*eprop);
      currentVertex = target(*ei,FsaAccess16<graphType>::m_graph);
    }
    else {
#ifdef DEBUG_CD
      LERROR << "FsaAccessSpare::getSpelling: no word for id " << index
             << " because no final state at end of trie !";
#endif
      result.clear();
      return result;
    }
  }
  return result;
}

//#define DEBUG_CD

template <typename graphType >
const LimaString fsaReader_superword_iterator16<graphType>::operator*() const {
  if( m_dico.FsaAccessHeader::getTrieDirectionForward() ) {
    return LimaString(m_prefix + m_suffix);
  }
  else {
    return LimaString(m_suffix + m_prefix);
  }
}

template <typename graphType>
void iterator_context<graphType>::print(std::ostream& os) const {
  os << "CtxIterator = { "
     << Lima::Common::Misc::limastring2utf8stdstring(
         Lima::LimaString(word_content))
     << ", len=" << word_length
     << ", pos=" << wordPos
     << ", off=" << wordOffset << "}, out_edge=";
     if( out_edge!=out_edge_end )
        os << *out_edge;
     else
        os << "out_edge_end";
}

template <typename graphType >
fsaReader_superword_iterator16<graphType> &
  fsaReader_superword_iterator16<graphType>::operator++(int dummy) {
    LIMA_UNUSED(dummy);
#ifdef DEBUG_CD
  FSAALOGINIT;
  LDEBUG <<  "superword_iterator::operator++(): begin";
#endif

  typename dicoGraph_traits16<graphType>::vname_map_type vname_map =
    boost::get(boost::vertex_name,m_graph);
  typename dicoGraph_traits16<graphType>::vtext_map_type vtext_map =
    boost::get(vertex_text,m_graph);

  for( ; m_context_stack.size() != 0 ; ) {
    iterator_context<graphType>& context = m_context_stack.back();
#ifdef DEBUG_CD
    std::ostringstream stro(std::ios::in | std::ios::out);
    stro << "superword_iterator::operator++(): context=";
    context.print(stro);
    stro << "m_curr=" << m_curr;
    LDEBUG << stro.str().c_str();
#endif
    if( context.out_edge != context.out_edge_end) {
      int32_t wordPos0 = context.wordPos;
      char32_t letter;
      U16_NEXT(context.word_content, context.wordPos, context.word_length, letter);
      context.wordOffset = context.wordPos - wordPos0;
      if( m_dico.FsaAccessHeader::getTrieDirectionForward() ) {
        for( int i = wordPos0 ; i < context.wordPos ; i++) {
#ifdef DEBUG_CD
          LDEBUG << "superword_iterator::operator++(): m_suffix.append( 1,"
                  << (*(context.word_content+i)).unicode()  << ")"
                 << " = context.word_content + " << i;
#endif
          m_suffix.append( *(context.word_content+i) );
        }
      }
      else {
        for( int i = context.wordOffset ; i > 0 ; i--) {
          m_suffix.push_front( *(context.word_content-i) );
        }
      }
      iterator_context<graphType> new_context;
      new_context.wordOffset = 0;
      m_curr = target(*(context.out_edge),m_graph);
      boost::tie(new_context.out_edge,new_context.out_edge_end) =
         boost::out_edges(m_curr,m_graph);
      const Lima::LimaString& text = get(vtext_map,m_curr);
      new_context.word_content = text.data();
      new_context.word_length = text.length();
      new_context.wordPos = 0;
#ifdef DEBUG_CD
      std::ostringstream stro1(std::ios::in | std::ios::out);
      stro1 << "superword_iterator::operator++(): push(";
      new_context.print(stro1);
      stro1 << "), m_curr=" << m_curr;
      LDEBUG << stro1.str().c_str();
#endif
      m_context_stack.push_back(new_context);
#ifdef DEBUG_CD
      LDEBUG <<  "superword_iterator::operator++(): append ->"
            << m_suffix;
#endif
      if( (get(vname_map,m_curr)&FINAL_16) == FINAL_16 ) {
        return *this;
      }
    }
    else {
//      iterator_context<graphType>& previous_context = m_context_stack.back();
#ifdef DEBUG_CD
      LDEBUG <<  "superword_iterator::operator++(): ei= edge_end";
      std::ostringstream stro2(std::ios::in | std::ios::out);
      stro2 << "superword_iterator::operator++(): pop(";
      context.print(stro2);
      stro2 << "), m_curr=" << m_curr;
      LDEBUG << stro2.str().c_str();
#endif
      m_context_stack.pop_back();
      if(m_context_stack.size() == 0)
        break;
      iterator_context<graphType>& current_context = m_context_stack.back();
#ifdef DEBUG_CD
      std::ostringstream stro3(std::ios::in | std::ios::out);
      stro3 << "superword_iterator::operator++(): current_context=";
      current_context.print(stro3);
      LDEBUG << stro3.str().c_str();
#endif
      if( m_dico.FsaAccessHeader::getTrieDirectionForward() ) {
        for( int i = current_context.wordOffset ; i > 0 ; i--) {
          m_suffix.remove( m_suffix.length()-1, 1 );
        }
      }
      else {
        for( int i = current_context.wordOffset ; i > 0 ; i--) {
          m_suffix.remove( 0, 1 );
        }
      }
#ifdef DEBUG_CD
      LDEBUG <<  "superword_iterator::operator++(): erase -> "
          << m_suffix;
#endif
      current_context.out_edge++;
//      m_context_stack.push_back(current_context);
    }
  }
  m_curr = typename boost::graph_traits<graphType>::vertex_descriptor();
  return *this;
}

template <typename graphType >
typename boost::graph_traits<graphType>::vertex_descriptor
FsaAccessReader16<graphType>::getStartNode(const LimaString& word ) const {
#ifdef DEBUG_CD
  FSAALOGINIT;
  LDEBUG <<  "FsaDictionary::getStartNode("
                 << word << ")";
#endif
  typename dicoGraph_traits16<graphType>::vtext_map_type vtext_map =
    boost::get(vertex_text,FsaAccess16<graphType>::m_graph);
  typename dicoGraph_traits16<graphType>::vname_map_type vname_map =
    boost::get(boost::vertex_name,FsaAccess16<graphType>::m_graph);

  // Start at root (vertex 0)
  typename boost::graph_traits<graphType>::vertex_descriptor from = FsaAccess16<graphType>::m_rootVertex;

  // Create an iterator to get letters one at a time
  PrefixIterator* prefixIt = FsaAccess16<graphType>::getPrefixIterator(word);

  // find the path in the automaton defined by the prefix
  int32_t wordOffset;
  for( ; prefixIt->hasNextLetter() ; prefixIt->next(wordOffset) ) {
    char32_t letter = prefixIt->getNextLetter(wordOffset);

    // iterator to select the right path among the out_edges
    int32_t edgeOffset = 0;
    //typename boost::graph_traits<graphType>::out_edge_iterator ei, edge_end;
    //boost::tie(ei,edge_end) = boost::out_edges(from,FsaAccess16<graphType>::m_graph);
    const Lima::LimaString& text = get(vtext_map,from);
    int32_t highCharTextPos = get(vname_map,from)&TEXT_POS_16;
    if( wordOffset == 1 ) {
      edgeOffset = FsaAccess16<graphType>::findEdge( letter, text, 0, highCharTextPos, wordOffset );
    }
    else {
      int32_t textOffset;
      textOffset = FsaAccess16<graphType>::findEdge( letter, text, highCharTextPos, text.length(), wordOffset );
      edgeOffset = highCharTextPos + (textOffset - highCharTextPos)/2;
   }

    if( edgeOffset >= 0 ) {
      typename boost::graph_traits<graphType>::out_edge_iterator ei, edge_end;
      boost::tie(ei,edge_end) = boost::out_edges(from,FsaAccess16<graphType>::m_graph);
      if (ei==edge_end) {
#ifdef DEBUG_CD
        LDEBUG <<  "FsaAccessReader::getStartNode: no output edges, edgeOffset="<< edgeOffset;
#endif
        return( FsaAccess16<graphType>::m_rootVertex );
      }
      typename FsaAccessReader16<graphType>::dicoEdgeType edge = *(ei+edgeOffset);
      from = target(edge, FsaAccess16<graphType>::m_graph);
#ifdef DEBUG_CD
      LDEBUG <<  "FsaDictionary::getStartNode: match" << edgeOffset;
#endif
    }
    else {
      std::string mess = "FsaAccessSpare::getStartNode: no word with prefix '";
      LimaString temp = word;
      mess.append( Lima::Common::Misc::limastring2utf8stdstring(temp));
      mess.append("'. return start node ");
#ifdef DEBUG_CD
      LWARN << mess.c_str();
#endif
      return( FsaAccess16<graphType>::m_rootVertex );
    }
  }
  return from;
}

//#undef DEBUG_CD


} // namespace FsaAccess
} // namespace Common
} // namespace Lima

/*
template <typename graphType>
std::ostream& operator<<(std::ostream& os, const Lima::Common::FsaAccess::iterator_context<graphType>& t) {
  t.print(os);
  return os;
}
*/
