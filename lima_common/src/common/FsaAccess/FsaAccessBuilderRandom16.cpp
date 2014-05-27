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
 *   Copyright (C) 2003 by  CEA                                            *
 *   author Olivier MESNARD olivier.mesnard@cea.fr                         *
 *                                                                         *
 *  Compact dictionnary based on finite state automata implemented with    *
 *  Boost Graph library.                                                   *
 *  Algorithm is described in article from Daciuk, Mihov, Watson & Watson: *
 *  "Incremental Construction of Minimal Acyclic Finite State Automata"    *
 ***************************************************************************/

// From boost library
#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>

#include "common/LimaCommon.h"
#include "common/Data/strwstrtools.h"

#include "FsaAccessBuilderRandom16.h"
using namespace Lima;

namespace Lima {
namespace Common {
namespace FsaAccess {

FsaAccessBuilderRandom16::FsaAccessBuilderRandom16( bool trie_direction_fwd )
: FsaAccessReader16<selected_graph_types16::builderGraphType>(trie_direction_fwd ) ,
  m_packingStatus(BUILDER)
{
}

FsaAccessBuilderRandom16::~FsaAccessBuilderRandom16()
{
}


FsaAccessIOHandler<selected_graph_types16::builderGraphType>* FsaAccessBuilderRandom16::getFsaAccessIOHandler() const {
  return new FsaAccessIOHandlerWithMapping<selected_graph_types16::builderGraphType>();
//  return new FsaAccessIOHandlerWithoutMapping<selected_graph_types16::builderGraphType>();
}

// Same code as FsaAccessBuilder !!
// duplicated toi avoid multiple inheritance
void FsaAccessBuilderRandom16::write( const std::string & filename  ){

  std::ofstream os(filename.data(), std::ios::out | std::ios::binary | std::ios::app );
  if( os.bad() ) {
    std::string mess = "FsaAccessBuilderRandom16::write: Can't open file " + filename;
#ifdef DEBUG_CD
    FSAAIOLOGINIT;
    LERROR;
#endif
    throw( FsaNotSaved( mess ) );
  }
//  os.seekp(HEADER_SIZE ,std::ios_base::beg );
  FsaAccessBuilderRandom16::write( os );
}


// Same code as FsaAccessBuilderRandom !!
// duplicated toi avoid multiple inheritance
void FsaAccessBuilderRandom16::write ( std::ostream &os ){
#ifdef DEBUG_CD
  FSAAIOLOGINIT;
  LDEBUG <<  "FsaAccessBuilderRandom16::write(std::ostream)";
#endif
  
  FsaAccessOStreamWrapper ow(os);
  FsaAccessBuilderRandom16::write(ow);
}

// Same code as FsaAccessBuilderRandom !!
// duplicated toi avoid multiple inheritance
void FsaAccessBuilderRandom16::write ( FsaAccessDataOutputHandler& oh ){
#ifdef DEBUG_CD
  FSAAIOLOGINIT;
  LDEBUG <<  "FsaAccessBuilderRandom16::write(std::ostream)";
#endif
  
  FsaAccessOutputHandlerWrapper ow(oh);
  FsaAccessBuilderRandom16::write(ow);
}

// Same code as FsaAccessBuilder !!
// duplicated toi avoid multiple inheritance
void FsaAccessBuilderRandom16::write( AbstractFsaAccessOStreamWrapper &ow ){
#ifdef DEBUG_CD
  FSAAIOLOGINIT;
  LDEBUG <<  "FsaAccessBuilderRandom16::write()";
#endif

  FsaAccessHeader::setPackingStatus(m_packingStatus);

  boost::graph_traits<graphType>::vertices_size_type nbVerts =
    boost::num_vertices(m_graph);
  boost::graph_traits<graphType>::edges_size_type nbEdges =
    boost::num_edges(m_graph);

  FsaAccessHeader::setNbVertices(nbVerts);
  FsaAccessHeader::setNbEdges(nbEdges);

  FsaAccessHeader::write(ow);

  writeBody( ow );
}

void FsaAccessBuilderRandom16::addRandomWord( const LimaString & newWord ) {
  FSAALOGINIT;
#ifdef DEBUG_CD
  LDEBUG <<  "FsaAccessBuilderRandom16::addRandomWord("
            << newWord << ")";
  std::ostringstream stro1(std::ios::in | std::ios::out);
  stro1 << Lima::Common::Misc::limastring2utf8stdstring(LimaString(newWord.data()))
        << "), m_rootVertex=" << m_rootVertex;
  LWARN << "FsaAccessBuilderRandom16::addRandomWord("
        << stro1.str().c_str();

#endif

  PrefixIterator* prefixIt = getPrefixIterator(newWord);
  dicoVertex root = m_rootVertex;
//  checkIntegrity( m_rootVertex );
  getPrefix( root, prefixIt );
  dicoGraph_traits16<graphType>::nconst_vname_map_type vname_map =
    boost::get(boost::vertex_name,m_graph);

  dicoVertex prefix_leaf;

  if( !prefixIt->hasNextLetter() ) {
    delete prefixIt;
    std::pair<AccessSuperWordIterator, AccessSuperWordIterator> superwords = getSuperWords(newWord );
    if( superwords.first == superwords.second ) {
      LWARN <<  "FsaAccessBuilderRandom16::addRandomWord: "
            << newWord
            << " already in dictionary!!";
      return;
    }
#ifdef DEBUG_CD
    LDEBUG <<  "FsaAccessBuilderRandom16::addRandomWord: "
           << newWord
           << " as prefix of existing word";
#endif
    Lima::LimaString superword =
      *(superwords.first);
    (superwords.first)++;
    for( ; superwords.first != superwords.second ; (superwords.first)++ ) {
      Lima::LimaString nextSuperword =
        *(superwords.first);
      if( nextSuperword.length() <= superword.length() )
        break;
      else
        superword = nextSuperword;
    }
#ifdef DEBUG_CD
    LDEBUG <<  "FsaAccessBuilderRandom16::addRandomWord: superWord = "
          << superword;
#endif

    PrefixIterator* prefixItSuperWord = getPrefixIterator(superword);
    /*bool hasFirstState =*/ scanAndCloneConfluentStates( m_rootVertex, prefixItSuperWord, prefix_leaf );

    PrefixIterator* prefixItWord = getPrefixIterator(newWord);
    dicoVertex root = m_rootVertex;
    getPrefix( root, prefixItWord );
#ifdef DEBUG_CD
    LWARN <<  "FsaAccessBuilderRandom16::addRandomWord: put(vname_map,"
           << root << "," << std::hex << get(vname_map, root)
           << " -> ";
#endif
    put(vname_map, root, get(vname_map, root)|FINAL_16);
#ifdef DEBUG_CD
    LWARN << std::hex << get(vname_map, root) << ")" << std::dec;
#endif
#ifdef DEBUG_CD
    LWARN <<  "FsaAccessBuilderRandom16::addRandomWord: put(vname_map,"
           << prefix_leaf << "," << std::hex << get(vname_map, prefix_leaf)
           << " -> ";
#endif
    put(vname_map, prefix_leaf, get(vname_map, prefix_leaf)|FINAL_16);
#ifdef DEBUG_CD
    LWARN << std::hex << get(vname_map, prefix_leaf) << ")" << std::dec;
#endif

    prefixItSuperWord = getPrefixIterator(superword);
    replaceOrRegister( m_rootVertex, prefixItSuperWord );
    delete prefixItSuperWord;

    prefixItWord = getPrefixIterator(newWord);
    std::ostringstream stro (std::ios::in | std::ios::out);
    stro << m_rootVertex << "," << *prefixItWord;
#ifdef DEBUG_CD
    LDEBUG <<  "FsaAccessBuilderRandom16::addRandomWord: updateHash("
          << stro.str().c_str() << ")";
#endif
    updateHash( m_rootVertex, prefixItWord );
    delete prefixItWord;
  
//    checkIntegrity( m_rootVertex );
    return;
  }

  if( prefixIt->getWordPos() == 0 ) {
#ifdef DEBUG_CD
    LDEBUG <<  "FsaAccessBuilderRandom16::addRandomWord: add complete word to root ";
#endif
    prefix_leaf = m_rootVertex;
  }
  else {
    const LimaString prefix = prefixIt->getPastPrefix();
    PrefixIterator* prefixItClone = getPrefixIterator(prefix);
#ifdef DEBUG_CD
    LDEBUG <<  "FsaAccessBuilderRandom16::addRandomWord:  m_rootVertex=" << m_rootVertex;
#endif
    /*bool hasFirstState =*/ scanAndCloneConfluentStates( m_rootVertex, prefixItClone, prefix_leaf );
//    checkIntegrity( prefix_leaf );

#ifdef DEBUG_CD
//    LDEBUG <<  "FsaAccessBuilderRandom16::addRandomWord: addSuffix " << prefix->getCurrentPrefix() << " to " << prefix_leaf;
    LDEBUG <<  "FsaAccessBuilderRandom16::addRandomWord: addSuffix to " << prefix_leaf;
#endif
  }
  if( prefixIt->hasNextLetter() ) {
    // ad first transition of suffix
    dicoVertex newState = add_vertex(m_graph);
    put(vname_map, newState, 0);

    int32_t wordOffset;
    char32_t currentChar = prefixIt->getNextLetter(wordOffset);

#ifdef DEBUG_CD
    LDEBUG <<  "FsaAccessBuilderRandom16::addRandomWord: add first letter of suffix "
           << std::hex << currentChar << std::dec;
#endif
//    checkIntegrity( prefix_leaf );
    addEdge( prefix_leaf, newState, currentChar, prefixIt->getCurrentContent(), wordOffset );
//  checkIntegrity( prefix_leaf );
    prefixIt->next(wordOffset);

    const LimaString trail = prefixIt->getCurrentPrefix();
    PrefixIterator* prefixItAdd = getPrefixIterator(trail);
//    PrefixIterator* prefixItAdd = getPrefixIterator(prefixIt->getCurrentPrefix());
    if( prefixItAdd->hasNextLetter() ) {
#ifdef DEBUG_CD
//    LDEBUG <<  "FsaAccessBuilderRandom16::addRandomWord: add end of suffix " << trail
    LDEBUG <<  "FsaAccessBuilderRandom16::addRandomWord: add end of suffix "
         << " to " << newState;
#endif
      addSuffix( newState, prefixItAdd);
    }
    else {
      put(vname_map, newState, FINAL_16);
    }
    PrefixIterator* prefixItWord = getPrefixIterator(newWord);
    replaceOrRegister( m_rootVertex, prefixItWord );
    delete prefixItWord;

    prefixItWord = getPrefixIterator(newWord);
    updateHash( m_rootVertex, prefixItWord );
    delete prefixItWord;
  }
//  checkIntegrity( prefix_leaf );

//  checkIntegrity( m_rootVertex );

  return;

}

/**
 *  search for first confluent state among prefixpath (end of prefix included)
 *  if one is found, create (derivation) path for prefix from first confluent state 
 *  up to end of prefix by cloning states 
 *  start: root of trie
 *  prefix_length: length of prefix
 *  word_content: content of prefix
 *  lastState: last state of prefix path in automaton
 *  return false if 
**/
bool FsaAccessBuilderRandom16::scanAndCloneConfluentStates(
  boost::graph_traits<graphType>::vertex_descriptor from,
  PrefixIterator* prefixIt,
  boost::graph_traits<graphType>::vertex_descriptor& lastState ) {
#ifdef DEBUG_CD
  FSAALOGINIT;
  LDEBUG <<  "FsaAccessBuilderRandom16::scanAndCloneConfluentStates("
                 << from << "," << prefixIt->getCurrentPrefix() << ")";
#endif
  dicoGraph_traits16<graphType>::nconst_vtext_map_type vtext_map =
    boost::get(vertex_text,m_graph);
  dicoGraph_traits16<graphType>::nconst_vname_map_type vname_map =
    boost::get(boost::vertex_name,m_graph);
  dicoGraph_traits16<graphType>::nconst_vcount_map_type vcount_map =
    boost::get(vertex_count,m_graph);

  int32_t wordOffset;
  for( ; prefixIt->hasNextLetter() ; ) {
    // get first char (or last one if reverse) of word to search for in graph
    char32_t currentChar = prefixIt->getNextLetter(wordOffset);
#ifdef DEBUG_CD
    LDEBUG <<  "FsaAccessBuilderRandom16::scanAndCloneConfluentStates: currentChar="
                 << std::hex << currentChar << std::dec << ")";
#endif
/* TODO: replace FsaAccess16<graphType>::findEdge() by ForwardPrefixIterator::findEdge()

    Lima::LimaString& text = get(vtext_map,from);
    int32_t highCharTextPos = get(vname_map,from)&TEXT_POS_16;
    ForwardPrefixIterator vertexTextIt( text );
    int32_t edgeOffset = vertexTextIt.findEdge( currentChar, wordOffset, highCharTextPos);
*/
    // find the path in the atomaton defined by the prefix
    int32_t edgeOffset;
    int32_t textOffset;
    Lima::LimaString& text = get(vtext_map,from);
#ifdef DEBUG_CD
    std::string text8 = Lima::Common::Misc::limastring2utf8stdstring(LimaString(text.data()));
    LDEBUG <<  "FsaAccessBuilderRandom16::scanAndCloneConfluentStates: text = " << text8.c_str();
#endif
    int32_t highCharTextPos = get(vname_map,from)&TEXT_POS_16;
    if( wordOffset == 1 ) {
      textOffset = findEdge( currentChar, text, 0, highCharTextPos );
      edgeOffset = textOffset;
    }
    else {
      textOffset = findEdge( currentChar, text, highCharTextPos, text.length() );
      edgeOffset = highCharTextPos + (textOffset - highCharTextPos)/2;
    }

    if( edgeOffset >= 0 ) {
      boost::graph_traits<graphType>::out_edge_iterator ei, edge_end;
      boost::tie(ei,edge_end) = boost::out_edges(from,m_graph);

// TODO: optimize it!
//      edgeOffset = 0;
//      dicoEdgeType edge = *(ei+edgeOffset);
      for( int i = 0 ; i < edgeOffset ; i++ )
        ei++;
      dicoEdgeType edge = *ei;

      boost::graph_traits<graphType>::vertex_descriptor to = target(edge, m_graph);
#ifdef DEBUG_CD
      LDEBUG <<  "FsaAccessBuilderRandom16::scanAndCloneConfluentStates: match " << edgeOffset;
#endif
      graphType::degree_size_type ind = boost::in_degree(to, m_graph);
      if( ind > 1 ) {
        // c'est un etat confluent: suppressEdge( from, to, currentChar );
#ifdef DEBUG_CD
        std::ostringstream oss;
        oss << "FsaAccessBuilderRandom16::scanAndCloneConfluentStates: supressEdge("
               << edge << "," << std::hex << currentChar << std::dec << ")";
        LDEBUG <<  oss.str().c_str();
#endif
        // remove element from vector counts
        // tableau des coefficients
        graphType::degree_size_type outd0 = boost::out_degree(from, m_graph);
        std::vector<int>& counts = get(vcount_map,from);
        if( outd0 > 1 ) {
          assert( (counts.size() + 1) == outd0 );
          counts.pop_back();
          assert( (counts.size() + 2) == outd0 );
        }
        else {
          assert( counts.size() == 0 );
        }

#ifdef DEBUG_CD
        LDEBUG <<  "FsaAccessBuilderRandom16::scanAndCloneConfluentStates: before remove_edge("
               << from << "," << to << "), outd=" << outd0;
        std::ostringstream oss2;
        oss2 << "FsaAccessBuilderRandom16::scanAndCloneConfluentStates: remove_edge("
               << edge << "," << std::hex << currentChar << std::dec << ")";
        LDEBUG <<  oss2.str().c_str();
#endif
        remove_edge(edge, m_graph);
        graphType::degree_size_type outd = boost::out_degree(from, m_graph);
#ifdef DEBUG_CD
        LDEBUG <<  "FsaAccessBuilderRandom16::scanAndCloneConfluentStates: after remove_edge, outd="
               << outd;
#endif
        assert( (outd+1) == outd0);
        if( outd > 1 )
          assert( (counts.size() +1) == outd);
        else 
          assert( counts.size() == 0 );

#ifdef DEBUG_CD
        LDEBUG <<  "FsaAccessBuilderRandom16::scanAndCloneConfluentStates: before erase text[to] ="
        << LimaString(text.data());
#endif
        text.remove(textOffset, wordOffset);
        assert( static_cast<graphType::degree_size_type>(text.size()) == outd);
#ifdef DEBUG_CD
        LDEBUG <<  "FsaAccessBuilderRandom16::scanAndCloneConfluentStates: after erase text[to] ="
        << LimaString(text.data());
#endif
        VERTEX_PROPERTY_16 vval = get(vname_map, from);
#ifdef DEBUG_CD
        LDEBUG <<  "FsaAccessBuilderRandom16::scanAndCloneConfluentStates: read val[to] ="
               << std::hex << vval << std::dec;
#endif
        VERTEX_PROPERTY_16 qualif = vval & QUALITY_16;
        VERTEX_PROPERTY_16 hicharOff = vval & TEXT_POS_16;
        if( wordOffset == 1 )
          hicharOff--;
        assert( hicharOff == outd);
#ifdef DEBUG_CD
        LDEBUG <<  "FsaAccessBuilderRandom16::scanAndCloneConfluentStates: set val[to] :="
               << std::hex << ( (qualif&(~HEAD_OF_CLASS_16)) | hicharOff) << std::dec;
#endif
        put(vname_map, from,  (qualif&(~HEAD_OF_CLASS_16)) | hicharOff);

        boost::graph_traits<graphType>::vertex_descriptor newState;
//        bool LIMA_UNUSED(ret) = cloneConfluentStates(currentChar, to, word_content, wordPos, wordOffset, prefix_length, from, newState);
        /*bool ret =*/ cloneConfluentStates(currentChar, wordOffset, to, prefixIt, from, newState);

#ifdef DEBUG_CD
       LDEBUG <<  "FsaAccessBuilderRandom16::scanAndCloneConfluentStates: duplicate out edges of "
              << to << " into " << newState;
#endif
        lastState = newState;
        break;
      }
      else  {
        prefixIt->next(wordOffset);
        from = to;
        lastState = to;
      }
    }
    else {
#ifdef DEBUG_CD
      LWARN <<  "FsaAccessBuilderRandom16::scanAndCloneConfluentStates: no match for ";
#endif
      return false;
    }
  }
  return true;
}

/**
 *  create (derivation) path for automata for prefix = string(word_content, wordPos, prefix_length-wordPos)
 *  start of derivatin is state 'fromNewPath'
 *  derivation is obtained by cloning states. first state to be cloned is is 'toOldPath'
 *  cloning means: 
 *                < supressing the transition edge( fromOldPath, firstChar, toOldPath ) >
 *                creating a new state toNewPath
 *                adding a new transition edge(fromNewPath, currentChar, toNewPath)
 *                < adding a new transition edge(fromNewPath, lastChar, toOldPath) >
 *  fromOldPath: state before first state to be cloned
 *  currentChar: label of transition adge(from, to)
 *  toOldPath: currentState to be cloned
 *  word_content: content of prefix
 *  wordPos: current pos in prefix
 *  fromNewPath: current State of path already created
 *  toNewPath: new State to be created
 *  return false if no path in automata match prefix
**/

bool FsaAccessBuilderRandom16::cloneConfluentStates(
  char32_t currentChar,
  int32_t wordOffset,
  boost::graph_traits<graphType>::vertex_descriptor& toOldPath,
  PrefixIterator* prefixIt,
  boost::graph_traits<graphType>::vertex_descriptor fromNewPath,
  boost::graph_traits<graphType>::vertex_descriptor& toNewPath ) {

#ifdef DEBUG_CD
  FSAALOGINIT;
  LDEBUG <<  "FsaAccessBuilderRandom16::cloneConfluentStates("
         << toOldPath << "," << prefixIt->getCurrentPrefix()
         << "," << wordOffset
         << "," << fromNewPath << ")";
#endif
  dicoGraph_traits16<graphType>::nconst_vtext_map_type vtext_map =
    boost::get(vertex_text,m_graph);
  dicoGraph_traits16<graphType>::nconst_vname_map_type vname_map =
    boost::get(boost::vertex_name,m_graph);

#ifdef DEBUG_CD
  graphType::degree_size_type out_size = boost::out_degree(fromNewPath, m_graph);
  LDEBUG <<  "FsaAccessBuilderRandom16::cloneConfluentStates: degree_size_type("
                 << fromNewPath << ")=" << out_size << ")";
#endif
  // Create a new state
  toNewPath = add_vertex(m_graph);
  // put(vname_map, toNewPath, 0);
  // put(vtext_map, newState, Lima::LimaString());
  //put(vcount_map, newState, std::vector<int>());

  addEdge(fromNewPath, toNewPath, currentChar, prefixIt->getCurrentContent(), wordOffset );
  cloneVertex(toOldPath, toNewPath );

  prefixIt->next(wordOffset);

  // get first char (or last one if reverse) of word to search for in graph
  if( !prefixIt->hasNextLetter() ) {
    return true;
  }
  currentChar = prefixIt->getNextLetter(wordOffset);

  // find the path in the atomaton defined by the prefix
  int32_t edgeOffset;
  // iterator to select the right path among the out_edges
  boost::graph_traits<graphType>::out_edge_iterator ei, edge_end;
  boost::tie(ei,edge_end) = boost::out_edges(toOldPath, m_graph);
  const Lima::LimaString& text = get(vtext_map,toOldPath);
  int32_t highCharTextPos = get(vname_map,toOldPath)&TEXT_POS_16;
  if( wordOffset == 1 ) {
    edgeOffset = findEdge( currentChar, text, 0, highCharTextPos );
  }
  else {
    int32_t textOffset;
    textOffset = findEdge( currentChar, text, highCharTextPos, text.length() );
    edgeOffset = highCharTextPos + (textOffset - highCharTextPos)/2;
  }

  if( edgeOffset >= 0 ) {
    boost::graph_traits<graphType>::out_edge_iterator ei, edge_end;
    boost::tie(ei,edge_end) = boost::out_edges(toOldPath,m_graph);
    dicoEdgeType edge = *(ei+edgeOffset);

#ifdef DEBUG_CD
    LDEBUG <<  "FsaAccessBuilderRandom16::cloneConfluentStates: match " << edgeOffset;
#endif
    dicoVertex oldTarget = target(edge, m_graph);

    suppressEdge(toNewPath, oldTarget, currentChar, prefixIt->getCurrentContent(), wordOffset);
    toOldPath = oldTarget;

    bool ret = cloneConfluentStates( currentChar, wordOffset, toOldPath,
                                     prefixIt, toNewPath, toNewPath);
    return ret;
  }
  else {
#ifdef DEBUG_CD
    LERROR << "FsaAccessBuilderRandom16::cloneConfluentStates: no match for"
           << currentChar;
#endif
    return false;
  }
}

// creation du clone d'un noeud: duplique les transitions depuis ce noeud
void FsaAccessBuilderRandom16::cloneVertex(
  const boost::graph_traits<graphType>::vertex_descriptor oldTo,
  const boost::graph_traits<graphType>::vertex_descriptor newTo )
{
#ifdef DEBUG_CD
  FSAALOGINIT;
  LDEBUG <<  "FsaAccessBuilderRandom16::cloneVertex("
         << oldTo << ", " << newTo << ")";
#endif
  dicoGraph_traits16<graphType>::nconst_vtext_map_type vtext_map =
    boost::get(vertex_text,m_graph);
  dicoGraph_traits16<graphType>::nconst_vname_map_type vname_map =
    boost::get(boost::vertex_name,m_graph);
  dicoGraph_traits16<graphType>::nconst_vcount_map_type vcount_map =
    boost::get(vertex_count,m_graph);

  boost::graph_traits<graphType>::out_edge_iterator ei, edge_end;
  boost::tie(ei,edge_end) = boost::out_edges(oldTo, m_graph);
  graphType::degree_size_type outd0 = boost::out_degree(newTo, m_graph);
  graphType::degree_size_type outdRef = boost::out_degree(oldTo, m_graph);
  assert(outd0 == 0);
  for( ; ei != edge_end ; ei++ ) {
    dicoVertex currentTarget = target(*ei,m_graph);
#ifdef DEBUG_CD
    LDEBUG <<  "FsaAccessBuilderRandom16::cloneVertex: add_edge("
           << newTo << "," << currentTarget << ")";
#endif
    add_edge(newTo, currentTarget, m_graph );
  }
  graphType::degree_size_type outd = boost::out_degree(newTo, m_graph);
  assert( outd == outdRef );
  // std::vector<int>& counts = get(vcount_map,oldTo);
  std::vector<int>& counts = get(vcount_map,oldTo);
  put(vcount_map,newTo,counts);
  put(vtext_map,newTo,get(vtext_map,oldTo));
  
  std::vector<int>& newCounts = get(vcount_map,newTo);
  if( outd > 1  )
    assert( (newCounts.size()+1) == outd );
  else
    assert( newCounts.size() == 0 );

  Lima::LimaString& text = get(vtext_map,newTo);
  assert( static_cast<graphType::degree_size_type>(text.size()) == outd );
  
  // mise a jour du nombre de caracteres avec remise a zero des caracteres HEAD_OF_CLASS_16 et SET_16
  VERTEX_PROPERTY_16 vval = get(vname_map, oldTo);
  vval &= (~(HEAD_OF_CLASS_16 | SET_16));
  put(vname_map,newTo,vval);
  assert( (get(vname_map, newTo)&TEXT_POS_16) == outd );
}

// Insertion d'une transition supplementaire a partir du noeud from.
// Attention: pour respecter le meme ordre pour le tableau des caracteres qui constituent
// les etiquettes des transitions et pour le (tableau?) des transition, il faut
// Repliquer le tableau des transitions en sortie de from pour 
// controler leur ordre (le conteneur n'est pas trie)
void FsaAccessBuilderRandom16::addEdge(
  const boost::graph_traits<graphType>::vertex_descriptor from,
  const boost::graph_traits<graphType>::vertex_descriptor to,
  const char32_t currentChar,
  const Lima::LimaChar* const word_content,
  const int32_t wordOffset ) {

#ifdef DEBUG_CD
  FSAALOGINIT;
  LDEBUG <<  "FsaAccessBuilderRandom16::addEdge("
         << from << ", " << to << ", " << currentChar << ","
         << std::hex << LimaString(*word_content) << std::dec << ", " << wordOffset << ")";
#endif
  dicoGraph_traits16<graphType>::nconst_vtext_map_type vtext_map =
    boost::get(vertex_text,m_graph);
  dicoGraph_traits16<graphType>::nconst_vname_map_type vname_map =
    boost::get(boost::vertex_name,m_graph);
  dicoGraph_traits16<graphType>::nconst_vcount_map_type vcount_map =
    boost::get(vertex_count,m_graph);

  Lima::LimaString& text = get(vtext_map,from);
  VERTEX_PROPERTY_16 vval = get(vname_map, from);
  VERTEX_PROPERTY_16 qualif = vval & QUALITY_16;
  VERTEX_PROPERTY_16 highCharTextPos = vval & TEXT_POS_16;
#ifdef DEBUG_CD
  LDEBUG <<  "FsaAccessBuilderRandom16::addEdge: vval="
         << std::hex << vval
         << ", highCharTextPos=" << highCharTextPos << std::dec;
#endif

  // tableau des coefficients
  std::vector<int>& counts = get(vcount_map,from);
  graphType::degree_size_type outd0 = boost::out_degree(from, m_graph);

  int32_t textOffset0;
  if( wordOffset == 1 ) {
    if( highCharTextPos > 0 )
      textOffset0 = findOffsetToInsertBefore( currentChar, text, 0, highCharTextPos );
    else
      textOffset0 = 0;

    // Memorisation des transitions au dela de textOffset
    std::list<dicoVertex> newOrderedTargetList;

    // degree_size_type size = out_degree(from, m_graph);
    // iterator to duplicate the list of out_edges
    boost::graph_traits<graphType>::out_edge_iterator ei, edge_end;
    boost::tie(ei,edge_end) = boost::out_edges(from,m_graph);
    int32_t textOffset(0);
    // fill the list up to the new transition to be created
    for( ; ei != edge_end ; ei++ ) {
      if( textOffset == textOffset0 )
        break;
#ifdef DEBUG_CD
//       LDEBUG <<  "FsaAccessBuilderRandom16::addEdge: newOrderedTargetList.push_back("
//              << (*ei) << ") (1)";
#endif
      newOrderedTargetList.push_back(target(*ei,m_graph));
      textOffset++;
    }

    // add target of new transition
    newOrderedTargetList.push_back(to);
    // fill the end of the list
    for( ; ei != edge_end ; ei++ ) {
#ifdef DEBUG_CD
//       LDEBUG <<  "FsaAccessBuilderRandom16::addEdge: newOrderedTargetList.push_back("
//              << (*ei) << ") (2)";
#endif
      newOrderedTargetList.push_back(target(*ei,m_graph));
    }
    // remove all out_edges
#ifdef DEBUG_CD
  LDEBUG <<  "FsaAccessBuilderRandom16::addEdge: clear_edge("
         << from << ")" << "(" << newOrderedTargetList.size() << ")";
#endif
    clear_out_edges(from, m_graph);
    assert(boost::out_degree(from, m_graph) == 0);
    // add new list of out_edges
    for( std::list<dicoVertex>::const_iterator vIt = newOrderedTargetList.begin() ;
      vIt != newOrderedTargetList.end() ; vIt++ ) {
#ifdef DEBUG_CD
  LDEBUG <<  "FsaAccessBuilderRandom16::addEdge: add_edge("
         << from << "," << *vIt << "";
#endif
        add_edge(from, *vIt, m_graph );
    }
    graphType::degree_size_type outd = boost::out_degree(from, m_graph);
    assert(outd == newOrderedTargetList.size() );
    assert(outd == (outd0+1) );
    // insert an additional element with value 0 for vector counts
    // value will be set later within updateHash() function
    if (outd > 1 ) {
#ifdef DEBUG_CD
      LDEBUG <<  "FsaAccessBuilderRandom16::addEdge: counts.push_back(0)";
#endif
      counts.push_back(0);
    }
    // adjust highCharTextPos
    highCharTextPos++;
#ifdef DEBUG_CD
    LDEBUG <<  "FsaAccessBuilderRandom16::addEdge: outd=" << outd
           << ", highCharTextPos = " << highCharTextPos;
#endif
    assert(outd == highCharTextPos );
    // from has been modified
    // set vertex "from" as possibly no more "head of class"
    qualif = qualif & (~HEAD_OF_CLASS_16);
#ifdef DEBUG_CD
  LDEBUG <<  "FsaAccessBuilderRandom16::addEdge: put(vname_map,"
         << from << "," << std::hex << qualif
         << " | " << highCharTextPos << "" << std::dec;
#endif
    put(vname_map, from, qualif | highCharTextPos);
#ifdef DEBUG_CD
    LDEBUG <<  "FsaAccessBuilderRandom16::addEdge: text("
           << from << ")="
           << LimaString(get(vtext_map,from).data())
          ;
    LDEBUG <<  "FsaAccessBuilderRandom16::addEdge: text="
           << LimaString(text.data())
          ;
    Lima::LimaString textpart = LimaString(word_content).left(wordOffset);
    LDEBUG <<  "FsaAccessBuilderRandom16::addEdge: text.insert("
           << textOffset0 << ","
           << LimaString(textpart.data()) << ")"<< LENDL;
#endif
    text.insert(textOffset0, word_content, wordOffset);
#ifdef DEBUG_CD
    LDEBUG <<  "FsaAccessBuilderRandom16::addEdge: text("
           << from << ")="
           << LimaString(get(vtext_map,from).data())
          ;
    LDEBUG <<  "FsaAccessBuilderRandom16::addEdge: text="
           << LimaString(text.data());
#endif
  }
  else {
#ifdef DEBUG_CD
    LDEBUG <<  "FsaAccessBuilderRandom16::addEdge: findOffsetToInsertBefore("
           << currentChar << ","
           << LimaString(text.data())
           << ")";
#endif
    textOffset0 = findOffsetToInsertBefore( currentChar, text, highCharTextPos, text.length() );
    assert(false);
    // TODO:
  }

  graphType::degree_size_type outdCheck = boost::out_degree(from, m_graph);
  if( outdCheck > 1 )
    assert( (counts.size()+1) == outdCheck );
  else
    assert( counts.size() == 0 );
  assert( (get(vname_map, from)&TEXT_POS_16) == outdCheck );
  Lima::LimaString& textCheck = get(vtext_map,from);
  assert( static_cast<graphType::degree_size_type>(textCheck.size()) == outdCheck );
}

// Remplacement d'une transition a partir du noeud from. Utile pour la fonction merge
// Attention: pour respecter le meme ordre pour le tableau des caracteres qui constituent
// les etiquettes des transitions et pour le (tableau?) des transition, il faut
// Repliquer le tableau des transitions en sortie de from pour 
// controler leur ordre (le conteneur n'est pas trie)
// par contre, contrairement a addEdge on considere que les proprietes (et en particulier 
// le tableau de caractere) ne changent pas
void FsaAccessBuilderRandom16::replaceEdge(
  const boost::graph_traits<graphType>::vertex_descriptor from,
  const boost::graph_traits<graphType>::vertex_descriptor to,
  const char32_t currentChar,
  const int32_t wordOffset ) {

#ifdef DEBUG_CD
  FSAALOGINIT;
  LDEBUG <<  "FsaAccessBuilderRandom16::replaceEdge("
         << from << ", " << to << ", " << currentChar << ","
         << ", " << wordOffset << ")";
#endif
  dicoGraph_traits16<graphType>::nconst_vtext_map_type vtext_map =
    boost::get(vertex_text,m_graph);
  dicoGraph_traits16<graphType>::nconst_vcount_map_type vcount_map =
    boost::get(vertex_count,m_graph);
  dicoGraph_traits16<graphType>::nconst_vname_map_type vname_map =
    boost::get(boost::vertex_name,m_graph);

  Lima::LimaString& text = get(vtext_map,from);

  VERTEX_PROPERTY_16 vval = get(vname_map, from);
  VERTEX_PROPERTY_16 qualif = vval & QUALITY_16;
  VERTEX_PROPERTY_16 highCharTextPos = vval & TEXT_POS_16;

  graphType::degree_size_type outd0 = boost::out_degree(from, m_graph);
  uint32_t textOffset0;
  if( wordOffset == 1 ) {
    if( highCharTextPos > 0 )
      textOffset0 = findOffsetToInsertBefore( currentChar, text, 0, highCharTextPos );
    else
      textOffset0 = 0;

#ifdef DEBUG_CD
    LDEBUG <<  "FsaAccessBuilderRandom16::replaceEdge: textOffset0="
           << textOffset0;
#endif

    // Memorisation des transitions au dela de textOffset
    std::list<dicoVertex> newOrderedTargetList;

    // degree_size_type size = out_degree(from, m_graph);
    // iterator to duplicate the list of out_edges
    boost::graph_traits<graphType>::out_edge_iterator ei, edge_end;
    boost::tie(ei,edge_end) = boost::out_edges(from,m_graph);
    uint32_t textOffset(0);
    // fill the list up to the new transition to be created
    for( ; ei != edge_end ; ei++ ) {
      if( textOffset == textOffset0 ) {
        ei++;
        break;
      }
#ifdef DEBUG_CD
//       LDEBUG <<  "FsaAccessBuilderRandom16::replaceEdge: newOrderedTargetList.push_back("
//              << (*ei).c_str() << ") (1)";
#endif
      newOrderedTargetList.push_back(target(*ei,m_graph));
      textOffset++;
    }
    newOrderedTargetList.push_back(to);
    assert(newOrderedTargetList.size() == textOffset+1);

    // fill the end of the list
    for( ; ei != edge_end ; ei++ ) {
#ifdef DEBUG_CD
//       LDEBUG <<  "FsaAccessBuilderRandom16::replaceEdge: newOrderedTargetList.push_back("
//              << *ei << ") (2)";
#endif
      newOrderedTargetList.push_back(target(*ei,m_graph));
    }
    assert(newOrderedTargetList.size() == outd0);
    // remove all out_edges
#ifdef DEBUG_CD
  LDEBUG <<  "FsaAccessBuilderRandom16::replaceEdge: clear_edge("
         << from << ")";
#endif
    clear_out_edges(from, m_graph);
    assert(boost::out_degree(from, m_graph) == 0);
    // add new list of out_edges
    for( std::list<dicoVertex>::iterator vIt = newOrderedTargetList.begin() ;
      vIt != newOrderedTargetList.end() ; vIt++ ) {
#ifdef DEBUG_CD
  LDEBUG <<  "FsaAccessBuilderRandom16::replaceEdge: add_edge("
         << from << "," << *vIt << "";
#endif
        add_edge(from, *vIt, m_graph );
    }
    assert(boost::out_degree(from, m_graph) == outd0);
    // from has been modified
    // set vertex "from" as possibly no more "head of class"
    qualif = qualif & (~HEAD_OF_CLASS_16);
#ifdef DEBUG_CD
  LDEBUG <<  "FsaAccessBuilderRandom16::replaceEdge: put(vname_map,"
         << from << "," << std::hex << qualif << " | " << highCharTextPos << std::dec << "";
#endif
    put(vname_map, from, qualif | highCharTextPos);
  }
  else {
#ifdef DEBUG_CD
    LDEBUG <<  "FsaAccessBuilderRandom16::replaceEdge: findOffsetToInsertBefore("
//           << currentChar << "," << text << ")";
           << currentChar << "," << ")";
#endif
    textOffset0 = findOffsetToInsertBefore( currentChar, text, highCharTextPos, text.length() );
    // TODO:
    assert(false);
  }
  
  // check
  assert( (get(vname_map, from)&TEXT_POS_16) == outd0 );
  std::vector<int>& counts = get(vcount_map,from);
  if( outd0 > 1 )
    assert( (counts.size()+1) == outd0 );
  else
    assert( counts.size() == 0 );  
  Lima::LimaString& textCheck = get(vtext_map,from);
  assert( static_cast<graphType::degree_size_type>(textCheck.size()) == outd0 );

}

// suppression d'une transition avec une etiquette precise entre deux noueds
void FsaAccessBuilderRandom16::suppressEdge(
  const boost::graph_traits<graphType>::vertex_descriptor from,
  const boost::graph_traits<graphType>::vertex_descriptor to,
  const char32_t currentChar,
  const Lima::LimaChar* const word_content,
  const int32_t wordOffset ) {

#ifdef DEBUG_CD
  FSAALOGINIT;
  LDEBUG <<  "FsaAccessBuilderRandom16::suppressEdge("
         << from << ", " << to << ", " << currentChar << ","
         << std::hex << LimaString(*word_content) << std::dec << ", " << wordOffset << ")";
#endif
  dicoGraph_traits16<graphType>::nconst_vtext_map_type vtext_map =
    boost::get(vertex_text,m_graph);
  dicoGraph_traits16<graphType>::nconst_vname_map_type vname_map =
    boost::get(boost::vertex_name,m_graph);
  dicoGraph_traits16<graphType>::nconst_vcount_map_type vcount_map =
    boost::get(vertex_count,m_graph);

  Lima::LimaString& text = get(vtext_map,from);
  VERTEX_PROPERTY_16 vval = get(vname_map, from);
  VERTEX_PROPERTY_16 qualif = vval & QUALITY_16;
  VERTEX_PROPERTY_16 highCharTextPos = vval & TEXT_POS_16;

  // tableau des coefficients
  std::vector<int>& counts = get(vcount_map,from);
  graphType::degree_size_type outd0 = boost::out_degree(from, m_graph);
  assert( outd0 > 0 );
  if( outd0 > 1 )
    assert( (counts.size()+1) == outd0 );
  else
    assert( counts.size() == 0 );

  int32_t textOffset0;
  if( wordOffset == 1 ) {
    if( highCharTextPos > 0 ) {
      textOffset0 = findOffsetToInsertBefore( currentChar, text, 0, highCharTextPos );
    }
    else
      textOffset0 = 0;

    // Memorisation des transitions au dela de textOffset
    std::list<dicoVertex> newOrderedTargetList;

    // degree_size_type size = out_degree(from, m_graph);
    // iterator to duplicate the list of out_edges
    boost::graph_traits<graphType>::out_edge_iterator ei, edge_end;
    boost::tie(ei,edge_end) = boost::out_edges(from,m_graph);
    int32_t textOffset(0);
    // fill the list up to the new transition to be created
    std::vector<int>::iterator cIt = counts.begin();
    for( ; ei != edge_end ; ei++, cIt++ ) {
      if( textOffset == textOffset0 )
        break;
#ifdef DEBUG_CD
//       LDEBUG <<  "FsaAccessBuilderRandom16::suppressEdge: newOrderedTargetList.push_back("
//              << *ei << ") (1)";
#endif
      newOrderedTargetList.push_back(target(*ei,m_graph));
      textOffset++;
    }

    // skip edge
    if (outd0 > 1 )
      counts.pop_back();
    ei++;
    // adjust highCharTextPos
    highCharTextPos--;

    // fill the end of the list
    for( ; ei != edge_end ; ei++ ) {
#ifdef DEBUG_CD
//       LDEBUG <<  "FsaAccessBuilderRandom16::suppressEdge: newOrderedTargetList.push_back("
//              << *ei << ") (2)";
#endif
      newOrderedTargetList.push_back(target(*ei,m_graph));
    }
    // remove all out_edges
#ifdef DEBUG_CD
  LDEBUG <<  "FsaAccessBuilderRandom16::suppressEdge: clear_edge("
         << from << ")";
#endif
    clear_out_edges(from, m_graph);
    assert(boost::out_degree(from, m_graph) == 0);
    // add new list of out_edges
    for( std::list<dicoVertex>::iterator vIt = newOrderedTargetList.begin() ;
      vIt != newOrderedTargetList.end() ; vIt++ ) {
#ifdef DEBUG_CD
  LDEBUG <<  "FsaAccessBuilderRandom16::suppressEdge: add_edge("
         << from << "," << *vIt << "";
#endif
        add_edge(from, *vIt, m_graph );
    }
    // from has been modified
    // set vertex "from" as possibly no more "head of class"
    qualif = qualif & (~HEAD_OF_CLASS_16);
#ifdef DEBUG_CD
  LDEBUG <<  "FsaAccessBuilderRandom16::suppressEdge: put(vname_map,"
         << from << "," << std::hex << qualif << " | " << highCharTextPos << std::dec << "";
#endif
    put(vname_map, from, qualif | highCharTextPos);
#ifdef DEBUG_CD
    LDEBUG <<  "FsaAccessBuilderRandom16::suppressEdge: text("
//           << from << ")=" << get(vtext_map,from);
           << from << ")=";
    LDEBUG <<  "FsaAccessBuilderRandom16::suppressEdge: text="
//           << text;
          ;
    Lima::LimaString textpart = LimaString(word_content).left(wordOffset);
    LDEBUG <<  "FsaAccessBuilderRandom16::suppressEdge: text.erase("
//           << textOffset0 << "," << textpart << ")"<< LENDL;
           << textOffset0 << "," << ")"<< LENDL;
#endif
    text.remove(textOffset0, wordOffset);
#ifdef DEBUG_CD
    LDEBUG <<  "FsaAccessBuilderRandom16::suppressEdge: text("
//           << from << ")=" << get(vtext_map,from);
           << from << ")=";
    LDEBUG <<  "FsaAccessBuilderRandom16::suppressEdge: text="
//           << text;
          ;
#endif
  }
  else {
#ifdef DEBUG_CD
    LDEBUG <<  "FsaAccessBuilderRandom16::suppressEdge: findOffsetToInsertBefore("
//           << currentChar << "," << text << ")";
           << currentChar << "," << ")";
#endif
    textOffset0 = findOffsetToInsertBefore( currentChar, text, highCharTextPos, text.length() );
    // TODO:
    assert(false);
  }

  // check
  graphType::degree_size_type outd = boost::out_degree(from, m_graph);
  assert((outd+1) == outd0);
  assert( (get(vname_map, from)&TEXT_POS_16) == outd );
  if( outd > 0 )
    assert( (counts.size()+1) == outd );
  else
   assert( counts.size() == 0 );  
  Lima::LimaString& textCheck = get(vtext_map,from);
  assert( static_cast<graphType::degree_size_type>(textCheck.size()) == outd );
}


void FsaAccessBuilderRandom16::replaceOrRegister( dicoVertex candidateState,
  PrefixIterator* prefixIt ){
//  const LimaChar* word_content, int word_length, int32_t wordPos ) throw( AccessByStringNotInitialized ) {
#ifdef DEBUG_CD
  FSAALOGINIT;
  LDEBUG <<  "FsaAccessBuilderRandom16::replaceOrRegister: (" << candidateState << ")";
#endif

  // check if leaf
  // degree_size_type nbChild = out_degree(dicoVertex, m_graph);
  // if( nbChild == 0)
  dico_degree_size nbChild = boost::out_degree(candidateState, m_graph);
  if( nbChild == 0 ) {
#ifdef DEBUG_CD
    LDEBUG <<  "FsaAccessBuilderRandom16::replaceOrRegister: out_degree = 0";
#endif
    return;
  }

  // get first char (or last one if reverse) of word to search for in graph
  int32_t wordOffset;
  assert( prefixIt->hasNextLetter() );
  char32_t currentChar = prefixIt->getNextLetter(wordOffset);

#ifdef DEBUG_CD
  LDEBUG <<  "FsaAccessBuilderRandom16::replaceOrRegister: currentChar="
         << std::hex << currentChar << std::dec << ")";
#endif

  dicoGraph_traits16<graphType>::nconst_vtext_map_type vtext_map =
    boost::get(vertex_text,m_graph);
  dicoGraph_traits16<graphType>::nconst_vname_map_type vname_map =
    boost::get(boost::vertex_name,m_graph);
  // find the path in the atomaton defined by the prefix
  int32_t edgeOffset;
  int32_t textOffset;
  Lima::LimaString& text = get(vtext_map,candidateState);
#ifdef DEBUG_CD
  std::string text8 = Lima::Common::Misc::limastring2utf8stdstring(LimaString(text.data()));
  LDEBUG <<  "FsaAccessBuilderRandom16::replaceOrRegister: text = " << text8.c_str();
#endif
  int32_t highCharTextPos = get(vname_map,candidateState)&TEXT_POS_16;
  if( wordOffset == 1 ) {
    textOffset = findEdge( currentChar, text, 0, highCharTextPos );
    edgeOffset = textOffset;
  }
  else {
    textOffset = findEdge( currentChar, text, highCharTextPos, text.length() );
    edgeOffset = highCharTextPos + (textOffset - highCharTextPos)/2;
  }

  if( edgeOffset >= 0 ) {
    prefixIt->next(wordOffset);
    boost::graph_traits<graphType>::out_edge_iterator ei, edge_end;
    boost::tie(ei,edge_end) = boost::out_edges(candidateState,m_graph);
    dicoEdgeType edge = *(ei+edgeOffset);
    boost::graph_traits<graphType>::vertex_descriptor lastChild = target(edge, m_graph);

//    replaceOrRegister( lastChild, word_content, word_length, wordPos );
    replaceOrRegister( lastChild, prefixIt );

    std::pair<const dicoVertex,bool> equivalent = findEquivalentInRegister( lastChild );
    if( equivalent.second ) {
      ForwardPrefixIterator textIt( text, textOffset );
      merge( equivalent.first, lastChild, candidateState, textIt );
    }
    else {
#ifdef DEBUG_CD
      LDEBUG <<  "FsaAccessBuilderRandom16::replaceOrRegister: m_register.push_back("
             << lastChild << ")";
#endif
      dicoGraph_traits16<graphType>::nconst_vname_map_type vname_map =
        boost::get(boost::vertex_name,m_graph);
      put(vname_map,lastChild, get(vname_map,lastChild)|HEAD_OF_CLASS_16);
    }
  }
  else {
    std::string mess("FsaAccessBuilderRandom16::replaceOrRegister: no path to reach 0degreeVertex!!");
#ifdef DEBUG_CD
    LERROR <<  mess.c_str() ;
#endif
    throw( AccessByStringNotInitialized( mess ) );
  }

}

void FsaAccessBuilderRandom16::merge( dicoVertex inRegister,
       dicoVertex tempState, dicoVertex parentState,
       const ForwardPrefixIterator& textIt) {
#ifdef DEBUG_CD
  FSAALOGINIT;
  LDEBUG <<  "FsaAccessBuilderRandom16::merge( " << inRegister << ", "
                                          << tempState << ", "
                                          << parentState << ")";
#endif
  // find in transition parentState -> tempstate
  std::pair<dicoEdgeType, bool> trans = edge(parentState, tempState, m_graph);
  assert( trans.second );
  // forward this transition to inRegister
  if( trans.second ) {
#ifdef DEBUG_CD
    LDEBUG <<  "FsaAccessBuilderRandom16::merge: replaceEdge(" << parentState << ", "
                                         << inRegister << ", "
                                         << ")";
#endif

    int32_t wordOffset;
    char32_t currentChar = textIt.getNextLetter(wordOffset);
    replaceEdge( parentState, inRegister, currentChar, wordOffset );

//    std::pair<dicoEdgeType, bool> res = add_edge(parentState, inRegister, m_graph);
//    assert( res.second );
  }

  // delete tempstate
  clear_vertex(tempState, m_graph);
  remove_vertex(tempState, m_graph);
}


// mise �jour des coefficients 
int FsaAccessBuilderRandom16::updateHash( dicoVertex from,
  PrefixIterator* prefixIt ) {
//  const LimaChar* word_content, int word_length, int32_t wordPos ) throw( AccessByStringNotInitialized ) {
#ifdef DEBUG_CD
  FSAALOGINIT;
  std::ostringstream stro1(std::ios::in | std::ios::out);
  stro1 << from << "(nbChild=" << boost::out_degree(from, m_graph) << "), " << *prefixIt;
  LDEBUG << "FsaAccessBuilderRandom16::updateHash("
        << stro1.str().c_str() << ")";
#endif

  dicoGraph_traits16<graphType>::nconst_vname_map_type vname_map =
    boost::get(boost::vertex_name,m_graph);
  dicoGraph_traits16<graphType>::nconst_vcount_map_type vcount_map =
    boost::get(vertex_count,m_graph);
  dicoGraph_traits16<graphType>::nconst_vtext_map_type vtext_map =
    boost::get(vertex_text,m_graph);

  // vocabulaire du sous_graphe
  int total(0);
  // tableau des coefficients
  std::vector<int>& counts = get(vcount_map,from);
  VERTEX_PROPERTY_16 val = get(vname_map, from);

  // check if leaf
  // degree_size_type nbChild = out_degree(dicoVertex, m_graph);
  // if( nbChild == 0)
  dico_degree_size nbChild = boost::out_degree(from, m_graph);
  if( nbChild == 0 ) {
#ifdef DEBUG_CD
    LDEBUG <<  "FsaAccessBuilderRandom16::updateHash: out_degree = 0";
#endif
    // On ajoute le noeud courant s'il est final
    if( (val & FINAL_16) == FINAL_16 ) {
#ifdef DEBUG_CD
      LDEBUG << "FsaAccessBuilderRandom16::updateHash: FINAL node, increment " << total ;
#endif
      total++;
    }
    return total;
  }
#ifdef DEBUG_CD
  if( nbChild > 1 )
    assert(nbChild == (counts.size()+1) );
  else
    assert( counts.size() == 0 );
#endif

  // get first char (or last one if reverse) of word to search for in graph
  int32_t wordOffset;
  // find the path in the atomaton defined by the prefix
  int32_t edgeOffset;
  int32_t textOffset;
  if( prefixIt->hasNextLetter() ) {
    char32_t currentChar = prefixIt->getNextLetter((int32_t&)wordOffset);
#ifdef DEBUG_CD
    LDEBUG <<  "FsaAccessBuilderRandom16::updateHash: currentChar="
           << std::hex << currentChar << std::dec << ")";
#endif

    Lima::LimaString& text = get(vtext_map,from);
#ifdef DEBUG_CD
    std::string text8 = Lima::Common::Misc::limastring2utf8stdstring(LimaString(text.data()));
    LDEBUG <<  "FsaAccessBuilderRandom16::updateHash: text = " << text8.c_str();
#endif
    int32_t highCharTextPos = val&TEXT_POS_16;
    if( wordOffset == 1 ) {
      textOffset = findEdge( currentChar, text, 0, highCharTextPos );
      edgeOffset = textOffset;
    }
    else {
      textOffset = findEdge( currentChar, text, highCharTextPos, text.length() );
      edgeOffset = highCharTextPos + (textOffset - highCharTextPos)/2;
    }
    prefixIt->next(wordOffset);
  }
  else {
    edgeOffset = 0; 
    textOffset = 0;
  }

  if( edgeOffset >= 0 ) { /// TODO edgeOffset is unsigned: comparison always true!!!
    boost::graph_traits<graphType>::out_edge_iterator ei, edge_end;
    boost::tie(ei,edge_end) = boost::out_edges(from,m_graph);
    dicoEdgeType edge = *(ei+edgeOffset);
    boost::graph_traits<graphType>::vertex_descriptor lastChild = target(edge, m_graph);
    // On met �jour les coefficients sur le sous arbre modifi�//    int subtotal = updateHash( lastChild, word_content, word_length, wordPos );
    int subtotal = updateHash( lastChild, prefixIt );

    // nombre de sous automates
    graphType::degree_size_type outd = boost::out_degree(from, m_graph);
    // On saute les sous-arbres jusqu'au sous arbre modifié
    std::vector<int>::iterator cIt = counts.begin();
#ifdef DEBUG_CD
    LDEBUG <<  "FsaAccessBuilderRandom16::updateHash: counts.size()="
           << counts.size();
#endif
    int32_t i = 0;
    for( ; (i+1 < static_cast<int32_t>(outd)) && (i<edgeOffset) ; ei++ , i++, cIt++ ) {
      assert(ei != edge_end);
      total = total + *cIt;
    }
    // On calcul la mise a jour a faire sur le tableau de coefficients
    int delta = 0;
    if( i+1 < static_cast<int32_t>(outd) ){
#ifdef DEBUG_CD
      LDEBUG <<  "FsaAccessBuilderRandom16::updateHash: i="
             << i << ",total=" << total << ", subtotal=" << subtotal;
#endif
      delta = (total + subtotal) - *cIt;
      *cIt = total + subtotal;
      ei++;
      i++;
      cIt++;
    }
    // On modifie les coefficients sur la suite du tableau
    for( ; i+1 < static_cast<int32_t>(outd)  ; ei++ , i++, cIt++ ) {
#ifdef DEBUG_CD
      LDEBUG <<  "FsaAccessBuilderRandom16::updateHash: i="
             << i << ",*cIt=" << *cIt << ", delta=" << delta;
#endif
      assert(ei != edge_end);
      total = *cIt + delta;
      *cIt = total;
    }

    // On calcul le dernier puisqu'il n'a pas memorise
    if( ei != edge_end ) {
#ifdef DEBUG_CD
//       LDEBUG <<  "FsaAccessBuilderRandom16::updateHash: call computeHash("
//              << target(*ei,m_graph) << ")";
#endif
      int subtotal = computeHash( target(*ei,m_graph) );
      total = total + subtotal;
    }
    put(vname_map, from, get(vname_map, from) | SET_16);
    // On ajoute le noeud courant s'il est final
    if( (val & FINAL_16) == FINAL_16 ) {
#ifdef DEBUG_CD
      LDEBUG << "FsaAccessBuilderRandom16::updateHash: FINAL node, increment " << total ;
#endif
      total++;
    }
#ifdef DEBUG_CD
    LDEBUG << "FsaAccessBuilderRandom16::updateHash: return " << total ;
#endif
    return total;
  }
  else {
    std::string mess("FsaAccessBuilderRandom16::updateHash: no path to reach 0degreeVertex!!");
#ifdef DEBUG_CD
    LERROR <<  mess.c_str() ;
#endif
    throw( AccessByStringNotInitialized( mess ) );
  }
}

void FsaAccessBuilderRandom16::addSuffix( dicoVertex from, PrefixIterator* prefixIt ) {
#ifdef DEBUG_CD
  FSAALOGINIT;
  Lima::LimaString s = prefixIt->getCurrentPrefix();
  LDEBUG <<  "FsaAccessBuilderRandom16::addSuffix: (" << from
            << ", " << s << ")";
#endif

  dicoGraph_traits16<graphType>::nconst_vname_map_type vname_map =
    boost::get(boost::vertex_name,m_graph);

  int32_t prefixOffset;
  dicoVertex to = from;
  for( ; prefixIt->hasNextLetter() ; prefixIt->next(prefixOffset) ) {
  
    to = add_vertex(m_graph);
    put(vname_map, to, 0);

    char32_t letter = prefixIt->getNextLetter(prefixOffset);
#ifdef DEBUG_CD
    char buff[256];
    sprintf(buff, "letter = %04x, suffixPos=%d\n", letter, prefixIt->getExternalWordPos() );
    LDEBUG << buff;
#endif
    
#ifdef DEBUG_CD
    LDEBUG <<  "FsaAccessBuilderRandom16::addSuffix: add_edge(" << from
              << ", " << to << ")";
#endif

    addEdge( from, to, letter, prefixIt->getCurrentContent(), prefixOffset );
    from = to;
  }
#ifdef DEBUG_CD
  LDEBUG <<  "FsaAccessBuilderRandom16::addSuffix: put(vname_map, to="
         << to << ", " <<  FINAL_16 << ")";
#endif
  put(vname_map, to, FINAL_16);
}



} // namespace FsaAccess
} // namespace Commmon
} // namespace Lima
