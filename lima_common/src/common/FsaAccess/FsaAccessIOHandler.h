// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
                          FsaAccessIOHandler.h  -  description
                             -------------------
    begin                :  aout 31 2005
    copyright            : CEA 2003
    email                : olivier.mesnard@cea.fr
 ***************************************************************************/
#ifndef FSA_IO_HANDLER_HPP
#define FSA_IO_HANDLER_HPP

#include <boost/serialization/strong_typedef.hpp>
#include <common/FsaAccess/FsaAccessIOStreamWrapper.h>

namespace Lima {
namespace Common {
namespace FsaAccess {

BOOST_STRONG_TYPEDEF(uint64_t, VERT_ID);

template <typename graphType >
class FsaAccessIOHandler {
public:    
  virtual ~FsaAccessIOHandler() {}
  void readEdgeAsInt( AbstractFsaAccessIStreamWrapper& iw, VERT_ID& source, VERT_ID& target );
  void readVerticeAsInt( AbstractFsaAccessIStreamWrapper& iw, VERT_ID& current,
    VERTEX_PROPERTY_16& propVal, Lima::LimaString& text );
  void writeEdgeAsInt( AbstractFsaAccessOStreamWrapper& ow, VERT_ID source, VERT_ID target );
  void writeVerticeAsInt( AbstractFsaAccessOStreamWrapper& ow, VERT_ID current,
    VERTEX_PROPERTY_16 propVal, const Lima::LimaString& text );
    
  virtual bool isInMap( typename boost::graph_traits<graphType>::vertex_descriptor from );
  virtual void readEdge( AbstractFsaAccessIStreamWrapper& iw,
    typename boost::graph_traits<graphType>::vertex_descriptor& source,
    typename boost::graph_traits<graphType>::vertex_descriptor& target ) = 0;
  virtual void readVertice( AbstractFsaAccessIStreamWrapper& iw,
    typename boost::graph_traits<graphType>::vertex_descriptor& current,
    VERTEX_PROPERTY_16& propVal, Lima::LimaString& text ) = 0;
  virtual void writeEdge( AbstractFsaAccessOStreamWrapper& ow,
    typename boost::graph_traits<graphType>::vertex_descriptor source,
    typename boost::graph_traits<graphType>::vertex_descriptor target ) = 0;
  virtual void writeVertice( AbstractFsaAccessOStreamWrapper& ow,
    typename boost::graph_traits<graphType>::vertex_descriptor current,
    VERTEX_PROPERTY_16 propVal,
    const Lima::LimaString& text ) = 0;
protected:  
    // map where to store conversion of node identifier
    // (from pointer to int) (used in write() )
    std::map<typename boost::graph_traits<graphType>::vertex_descriptor,
                          VERT_ID> m_ptr2IndexMapPtr;
    // vector where to store conversion of node identifier
    // (from int to pointer) (used in read??() )
    std::vector<typename boost::graph_traits<graphType>::vertex_descriptor> m_index2PtrVectorPtr ;
};

template <typename graphType >
class FsaAccessIOHandlerWithMapping : public FsaAccessIOHandler<graphType> {
  public:    
    FsaAccessIOHandlerWithMapping() {}
    void readEdge( AbstractFsaAccessIStreamWrapper& iw,
      typename boost::graph_traits<graphType>::vertex_descriptor& source,
      typename boost::graph_traits<graphType>::vertex_descriptor& target ) override;
    void readVertice( AbstractFsaAccessIStreamWrapper& iw,
      typename boost::graph_traits<graphType>::vertex_descriptor& current,
      VERTEX_PROPERTY_16& propVal, Lima::LimaString& text ) override;
    void writeEdge( AbstractFsaAccessOStreamWrapper& ow,
      typename boost::graph_traits<graphType>::vertex_descriptor source,
      typename boost::graph_traits<graphType>::vertex_descriptor target ) override;
    void writeVertice( AbstractFsaAccessOStreamWrapper& ow,
      typename boost::graph_traits<graphType>::vertex_descriptor current,
      VERTEX_PROPERTY_16 propVal,
      const Lima::LimaString& text ) override;
  
  private:
};

template <typename graphType >
class FsaAccessIOHandlerWithoutMapping : public FsaAccessIOHandler<graphType> {
  public:    
    FsaAccessIOHandlerWithoutMapping() {}
    void readEdge( AbstractFsaAccessIStreamWrapper& iw,
      typename boost::graph_traits<graphType>::vertex_descriptor& source,
      typename boost::graph_traits<graphType>::vertex_descriptor& target ) override;
    void readVertice( AbstractFsaAccessIStreamWrapper& iw,
      typename boost::graph_traits<graphType>::vertex_descriptor& current,
      VERTEX_PROPERTY_16& propVal, Lima::LimaString& text ) override;
    void writeEdge( AbstractFsaAccessOStreamWrapper& ow,
      typename boost::graph_traits<graphType>::vertex_descriptor source,
      typename boost::graph_traits<graphType>::vertex_descriptor target ) override;
    void writeVertice( AbstractFsaAccessOStreamWrapper& ow,
      typename boost::graph_traits<graphType>::vertex_descriptor current,
      VERTEX_PROPERTY_16 propVal,
      const Lima::LimaString& text ) override;
  
  private:
};

} // namespace compactDict
} // namespace Common
} // namespace Lima

#include "common/FsaAccess/FsaAccessIOHandler.tcc"

#endif   //FSA_IO_HANDLER_HPP
