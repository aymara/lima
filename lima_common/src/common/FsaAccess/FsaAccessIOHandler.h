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
      typename boost::graph_traits<graphType>::vertex_descriptor& target );
    void readVertice( AbstractFsaAccessIStreamWrapper& iw,
      typename boost::graph_traits<graphType>::vertex_descriptor& current,
      VERTEX_PROPERTY_16& propVal, Lima::LimaString& text );
    void writeEdge( AbstractFsaAccessOStreamWrapper& ow,
      typename boost::graph_traits<graphType>::vertex_descriptor source,
      typename boost::graph_traits<graphType>::vertex_descriptor target );
    void writeVertice( AbstractFsaAccessOStreamWrapper& ow,
      typename boost::graph_traits<graphType>::vertex_descriptor current,
      VERTEX_PROPERTY_16 propVal,
      const Lima::LimaString& text );
  
  private:
};

template <typename graphType >
class FsaAccessIOHandlerWithoutMapping : public FsaAccessIOHandler<graphType> {
  public:    
    FsaAccessIOHandlerWithoutMapping() {}
    void readEdge( AbstractFsaAccessIStreamWrapper& iw,
      typename boost::graph_traits<graphType>::vertex_descriptor& source,
      typename boost::graph_traits<graphType>::vertex_descriptor& target );
    void readVertice( AbstractFsaAccessIStreamWrapper& iw,
      typename boost::graph_traits<graphType>::vertex_descriptor& current,
      VERTEX_PROPERTY_16& propVal, Lima::LimaString& text );
    void writeEdge( AbstractFsaAccessOStreamWrapper& ow,
      typename boost::graph_traits<graphType>::vertex_descriptor source,
      typename boost::graph_traits<graphType>::vertex_descriptor target );
    void writeVertice( AbstractFsaAccessOStreamWrapper& ow,
      typename boost::graph_traits<graphType>::vertex_descriptor current,
      VERTEX_PROPERTY_16 propVal,
      const Lima::LimaString& text );
  
  private:
};

} // namespace compactDict
} // namespace Common
} // namespace Lima

#include "common/FsaAccess/FsaAccessIOHandler.tcc"

#endif   //FSA_IO_HANDLER_HPP
