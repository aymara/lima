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

#ifndef FSA_ACCESS_READER16_HPP
#define FSA_ACCESS_READER16_HPP

#include <iostream>
#include <vector>
#include <string>
#include <deque>
#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/config.hpp>
//#include <boost/graph/depth_first_search.hpp>
//#include <boost/range/irange.hpp>
#include <boost/pending/indirect_cmp.hpp>

#include "common/Data/LimaString.h"
#include "common/Data/strwstrtools.h"
#include "common/misc/AbstractAccessByString.h"
#include "FsaExceptions.h"
#include "PrefixIterator.h"
#include "FsaAccess16.h"

namespace Lima {
namespace Common {
namespace FsaAccess {

// forward declaration of FsaAccessReader16 for iterator
template <typename graphType>
class FsaAccessReader16;

template <typename graphType>
class iterator_context {
//  friend std::ostream& operator<<(std::ostream& os, const iterator_context<graphType>& t);
public:
  void print(std::ostream& os) const;
  typename boost::graph_traits<graphType>::out_edge_iterator out_edge;
  typename boost::graph_traits<graphType>::out_edge_iterator out_edge_end;
  const LimaChar* word_content;
  int32_t wordPos;
  int32_t word_length;
  int32_t wordOffset;
};

template <typename graphType>
class approx_iterator_context {
//  friend std::ostream& operator<<(std::ostream& os, const iterator_context<graphType>& t);
public:
  void print(std::ostream& os) const;
  typename boost::graph_traits<graphType>::out_edge_iterator out_edge;
  typename boost::graph_traits<graphType>::out_edge_iterator out_edge_end;
  const LimaChar* word_content;
  int32_t wordPos;
  int32_t word_length;
  int32_t wordOffset;
  unsigned int nbError;
  unsigned int nbSuccess;
  Lima::LimaChar* cinputPos;
};

// class iterator for FsaAccessReader::superWords() output
template <typename graphType>
class fsaReader_superword_iterator16 : public ClonableSuperWordIterator {
//  typedef selected_graph_types16::spareGraphType graphType;
  public:
   fsaReader_superword_iterator16(const FsaAccessReader16<graphType> & dico,
      typename boost::graph_traits<graphType>::vertex_descriptor node,
      const LimaString &prefix );
   fsaReader_superword_iterator16(const FsaAccessReader16<graphType> & dico,
      const LimaString &prefix);
   fsaReader_superword_iterator16(const fsaReader_superword_iterator16&) = default;
   fsaReader_superword_iterator16& operator=(const fsaReader_superword_iterator16&) = delete;
   const LimaString operator*() const override;
   fsaReader_superword_iterator16 &operator++(int) override;
   bool operator==(const AbstractSuperWordIterator& it) const override;
   bool operator!=(const AbstractSuperWordIterator& it) const override;
   virtual ClonableSuperWordIterator* clone() const override;
  private:
    typename boost::graph_traits<graphType>::vertex_descriptor m_curr;
    std::deque<iterator_context<graphType> > m_context_stack;
    const FsaAccessReader16<graphType> & m_dico;
    const graphType & m_graph;
    const LimaString m_prefix;
    LimaString m_suffix;
};

// class iterator for FsaAccessReader::subWords() output
template <typename graphType>
class fsaReader_subword_iterator16 : public ClonableSubWordIterator {
//  typedef selected_graph_types16::spareGraphType graphType;
  public:
    fsaReader_subword_iterator16(const fsaReader_subword_iterator16 &iter);
    fsaReader_subword_iterator16(const FsaAccessReader16<graphType> & dico,
      const LimaString &word);
    fsaReader_subword_iterator16(const FsaAccessReader16<graphType> & dico,
      const LimaString &word,
      typename boost::graph_traits<graphType>::vertex_descriptor node,
      const uint64_t offset );
    ~fsaReader_subword_iterator16();
    const std::pair<uint64_t, uint64_t> operator*() const override;
    fsaReader_subword_iterator16 &operator++(int) override;
    bool operator==(const AbstractSubWordIterator& it) const override;
    bool operator!=(const AbstractSubWordIterator& it) const override;
    virtual ClonableSubWordIterator* clone() const override;
  private:
	fsaReader_subword_iterator16<graphType>& operator=(const fsaReader_subword_iterator16<graphType>&) {return *this;}
    // TODO: supress m_dico (when getVprop() )will be supressed
    const FsaAccessReader16<graphType> & m_dico;
    const graphType & m_graph;
    PrefixIterator* m_prefixIt;
    typename boost::graph_traits<graphType>::vertex_descriptor m_curr;
    uint64_t m_index;
};

// class iterator for FsaAccessReader::superWords() output
template <typename graphType>
class fsaReader_approx_iterator16 : public ClonableApproxWordIterator {
//  typedef selected_graph_types16::spareGraphType graphType;
  public:
   fsaReader_approx_iterator16(const FsaAccessReader16<graphType> & dico,
      typename boost::graph_traits<graphType>::vertex_descriptor nbMaxError,
      const LimaString &word );
   fsaReader_approx_iterator16(const FsaAccessReader16<graphType> & dico);
   const ApproxSuggestion operator*() const override;
   fsaReader_approx_iterator16 &operator++(int) override;
   bool operator==(const AbstractApproxWordIterator& it) const override;
   bool operator!=(const AbstractApproxWordIterator& it) const override;
   virtual ClonableApproxWordIterator* clone() const override;
  private:
  fsaReader_approx_iterator16<graphType>& operator=(const fsaReader_approx_iterator16<graphType>&) {return *this;}
    typename boost::graph_traits<graphType>::vertex_descriptor m_curr;
    std::deque<iterator_context<graphType> > m_context_stack;
    const FsaAccessReader16<graphType> & m_dico;
    const graphType & m_graph;
    const LimaString m_prefix;
    LimaString m_suffix;
    std::multimap<unsigned int,ApproxSuggestion> suggestions;
    std::multimap<std::pair<unsigned int,unsigned int>,approx_iterator_context<graphType> > contextes;
};



//template <typename graphType, typename LimaChar>
template <typename graphType>
class FsaAccessReader16 : public AbstractAccessByString,
                          public FsaAccess16<graphType> {
  friend class fsaReader_superword_iterator16<graphType>;
  friend class fsaReader_subword_iterator16<graphType>;

  public:
    FsaAccessReader16(bool trie_direction_fwd);
    virtual ~FsaAccessReader16(){}
    // implementation of public operation defined in interface AbstractAccessByString
    virtual uint64_t getSize() const override;
    virtual uint64_t getIndex(const LimaString & word ) const override;
    virtual std::pair< AccessSuperWordIterator, AccessSuperWordIterator >
     getSuperWords(const LimaString & word ) const override;
    virtual std::pair<AccessSubWordIterator,AccessSubWordIterator >
      getSubWords(const uint64_t offset,
              const LimaString & word ) const override;

    // public optional operation in interface AbstractEnhancedAccess
    LimaString getSpelling(const uint64_t index ) const override;

    LimaString getExtent(const LimaString & prefix ) const;

    // for IO
    void read ( const std::string & filename );
    void read ( std::istream& is );
    void read ( FsaAccessDataInputHandler& is );
  protected:
    void read ( AbstractFsaAccessIStreamWrapper& iw);
    uint64_t  buildHash();
    int computeHash( typename boost::graph_traits<graphType>::vertex_descriptor from );
    uint64_t m_size;
  private:
    typename boost::graph_traits<graphType>::vertex_descriptor
     getStartNode(const LimaString& word ) const;
};

} // namespace FsaAccess
} // namespace Common
} // namespace Lima

/*
template <typename graphType>
std::ostream& operator<<(std::ostream& os, const Common::FsaAccess::iterator_context<graphType>& t);
*/

#include "common/FsaAccess/FsaAccessReader16.tcc"

#endif   //FSA_ACCESS_READER16_HPP
