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
                          FsaAccessIOHandler.tcc  -  description
                             -------------------
    begin                :  aout 31 2005
    copyright            : CEA 2003
    email                : olivier.mesnard@cea.fr
 ***************************************************************************/

#include <iostream>

namespace Lima {
namespace Common {
namespace FsaAccess {


template <typename graphType >
bool FsaAccessIOHandler<graphType>::isInMap( 
  typename boost::graph_traits<graphType>::vertex_descriptor from ) {
  
  typename std::map<typename boost::graph_traits<graphType>::vertex_descriptor,
              VERT_ID>::const_iterator pos;

  pos = m_ptr2IndexMapPtr.find(from);
  return( pos != m_ptr2IndexMapPtr.end() );
}


template <typename graphType >
void FsaAccessIOHandlerWithMapping<graphType>::readEdge( AbstractFsaAccessIStreamWrapper& iw,
  typename boost::graph_traits<graphType>::vertex_descriptor& source,
  typename boost::graph_traits<graphType>::vertex_descriptor& target ) {
  
  VERT_ID sourceIndex, targetIndex;
  FsaAccessIOHandler<graphType>::readEdgeAsInt( iw, sourceIndex, targetIndex );
  source = FsaAccessIOHandler<graphType>::m_index2PtrVectorPtr[sourceIndex];
  target = FsaAccessIOHandler<graphType>::m_index2PtrVectorPtr[targetIndex];
}

template <typename graphType >
void FsaAccessIOHandlerWithoutMapping<graphType>::readEdge( AbstractFsaAccessIStreamWrapper& iw,
  typename boost::graph_traits<graphType>::vertex_descriptor& source,
  typename boost::graph_traits<graphType>::vertex_descriptor& target ) {
  
  VERT_ID sourceIndex, targetIndex;
  FsaAccessIOHandler<graphType>::readEdgeAsInt( iw, sourceIndex, targetIndex );
  source = static_cast<typename boost::graph_traits<graphType>::vertex_descriptor>(sourceIndex);
  target = static_cast<typename boost::graph_traits<graphType>::vertex_descriptor>(targetIndex);
}

template <typename graphType >
void FsaAccessIOHandler<graphType>::readEdgeAsInt( AbstractFsaAccessIStreamWrapper& iw,
  VERT_ID& source, VERT_ID& target ) {
#ifdef DEBUG_CD
  FSAAIOLOGINIT;
  LTRACE <<  "FsaAccessIOHandler::readEdgeAsInt()";
#endif
  iw.readData((char *)(&source), sizeof(VERT_ID) );
  iw.readData((char *)(&target), sizeof(VERT_ID) );
#ifdef DEBUG_CD
  LTRACE <<  "FsaAccessIOHandler::readEdgeAsInt: ("
            << source << ", " << target << ")"
           ;
#endif
}

template <typename graphType >
void FsaAccessIOHandlerWithMapping<graphType>::readVertice( AbstractFsaAccessIStreamWrapper& iw,
  typename boost::graph_traits<graphType>::vertex_descriptor& current,
  VERTEX_PROPERTY_16& propVal, Lima::LimaString& text ) {
  FsaAccessIOHandler<graphType>::m_index2PtrVectorPtr.push_back( current );
  VERT_ID currentId = static_cast<VERT_ID>(FsaAccessIOHandler<graphType>::m_index2PtrVectorPtr.size());
  FsaAccessIOHandler<graphType>::readVerticeAsInt( iw, currentId, propVal, text );
}

template <typename graphType >
void FsaAccessIOHandlerWithoutMapping<graphType>::readVertice( AbstractFsaAccessIStreamWrapper& iw,
  typename boost::graph_traits<graphType>::vertex_descriptor& current,
  VERTEX_PROPERTY_16& propVal, Lima::LimaString& text ) {
  VERT_ID currentId = static_cast<VERT_ID>(current);
  FsaAccessIOHandler<graphType>::readVerticeAsInt( iw, currentId, propVal, text );
#ifdef DEBUG_CD
  FSAAIOLOGINIT;
  LTRACE <<  "FsaAccessIOHandlerWithoutMapping::readVertice(" << currentId << "):  "
            << " propVal = " << propVal
            << " text = " << text;
#endif
}

template <typename graphType >
void FsaAccessIOHandler<graphType>::readVerticeAsInt( AbstractFsaAccessIStreamWrapper& iw,
  VERT_ID& current,
  VERTEX_PROPERTY_16& propVal, Lima::LimaString& text ) {
#ifdef DEBUG_CD
  FSAAIOLOGINIT;
  LTRACE <<  "FsaAccessIOHandler::readVerticeAsInt( " << current
            << ") ";
#else
  LIMA_UNUSED(current)
#endif
  iw.readData((char *)(&propVal), sizeof(VERTEX_PROPERTY_16) );
  int16_t strlenShort;
  iw.readData((char *)(&strlenShort), sizeof(int16_t) );
  int strlen = strlenShort;
  ushort* strbuf = new ushort[strlen+1] ;
  iw.readData((char *)(strbuf), sizeof(ushort)*strlen );
  strbuf[strlen] = 0;
  text = LimaString::fromUtf16(strbuf);
  delete [] strbuf;

#ifdef DEBUG_CD
  LTRACE <<  "FsaAccessIOHandler::readVerticeAsInt:  "
            << " propVal = " << propVal
            << " text = " << text;
#endif
}


template <typename graphType >
void FsaAccessIOHandlerWithMapping<graphType>::writeVertice(
 AbstractFsaAccessOStreamWrapper& ow, typename boost::graph_traits<graphType>::vertex_descriptor current,
 VERTEX_PROPERTY_16 propVal, const Lima::LimaString& text ) {
  
  // On ajoute le noeud dans la table de conversion 
  typename std::map<typename boost::graph_traits<graphType>::vertex_descriptor,
              VERT_ID>::const_iterator pos;
  pos = FsaAccessIOHandler<graphType>::m_ptr2IndexMapPtr.find(current);
  assert( pos == FsaAccessIOHandler<graphType>::m_ptr2IndexMapPtr.end() );

  VERT_ID currentId = static_cast<VERT_ID>(FsaAccessIOHandler<graphType>::m_ptr2IndexMapPtr.size());
  FsaAccessIOHandler<graphType>::m_ptr2IndexMapPtr.insert(
    std::pair<typename boost::graph_traits<graphType>::vertex_descriptor,
            VERT_ID>(current, currentId) );
    
#ifdef DEBUG_CD
  FSAAIOLOGINIT;
  std::ostringstream stro (std::ios::in | std::ios::out);
  std::string text8 = Lima::Common::Misc::limastring2utf8stdstring(text);
  stro << "FsaAccessIOHandlerWithMapping::writeVertice(" << current << ") = "
       << std::hex << propVal << std::dec
       << "," << text8 << " to " << currentId;
  LTRACE << stro.str().c_str();
#endif

  FsaAccessIOHandler<graphType>::writeVerticeAsInt( ow, currentId, propVal, text );
}

template <typename graphType >
void FsaAccessIOHandlerWithoutMapping<graphType>::writeVertice(
 AbstractFsaAccessOStreamWrapper& ow, typename boost::graph_traits<graphType>::vertex_descriptor current,
 VERTEX_PROPERTY_16 propVal, const Lima::LimaString& text ) {
  
  // On ajoute le noeud dans la table de conversion
  // C'est seulement utile pour ne parcourir qu'une fois les sous arbre
  // dans la fonction FsaAccess::writeVertices
  typename std::map<typename boost::graph_traits<graphType>::vertex_descriptor,
              VERT_ID>::const_iterator pos;
  pos = FsaAccessIOHandler<graphType>::m_ptr2IndexMapPtr.find(current);
  assert( pos == FsaAccessIOHandler<graphType>::m_ptr2IndexMapPtr.end() );

  VERT_ID index = static_cast<VERT_ID>(FsaAccessIOHandler<graphType>::m_ptr2IndexMapPtr.size());
  FsaAccessIOHandler<graphType>::m_ptr2IndexMapPtr.insert(
    std::pair<typename boost::graph_traits<graphType>::vertex_descriptor,
            VERT_ID>(current, index) );
    
#ifdef DEBUG_CD
  FSAAIOLOGINIT;
  std::ostringstream stro (std::ios::in | std::ios::out);
  std::string text8 = Lima::Common::Misc::limastring2utf8stdstring(text);
  stro << "FsaAccessIOHandlerWithoutMapping::writeVertice(" << current << ") = "
       << std::hex << propVal << std::dec
       << "," << text8 << " to " << (*pos).second;
  LTRACE << stro.str().c_str();
#endif

  FsaAccessIOHandler<graphType>::writeVerticeAsInt( ow, static_cast<VERT_ID>(current), propVal, text );
}

template <typename graphType >
void FsaAccessIOHandler<graphType>::writeVerticeAsInt( AbstractFsaAccessOStreamWrapper& ow, VERT_ID current,
 VERTEX_PROPERTY_16 propVal, const Lima::LimaString& text ) {
#ifdef DEBUG_CD
  FSAAIOLOGINIT;
  LTRACE <<  "FsaAccessIOHandler::writeVertice: " << current
                << "] = " << propVal;
#else
  LIMA_UNUSED(current)
#endif

  ow.writeData( (const char *)(&propVal), sizeof(VERTEX_PROPERTY_16) );
  int16_t strlenShort = text.size();
  ow.writeData( (const char *)(&strlenShort), sizeof(int16_t) );
  ow.writeData( (const char *)(text.utf16()), sizeof(ushort)*text.size() );
}


template <typename graphType >
void FsaAccessIOHandlerWithMapping<graphType>::writeEdge(
 AbstractFsaAccessOStreamWrapper& ow,
 typename boost::graph_traits<graphType>::vertex_descriptor source,
 typename boost::graph_traits<graphType>::vertex_descriptor target ) {

  typename std::map<typename boost::graph_traits<graphType>::vertex_descriptor,
              VERT_ID>::const_iterator posS;
  posS = FsaAccessIOHandler<graphType>::m_ptr2IndexMapPtr.find(source);
  assert( posS !=FsaAccessIOHandler<graphType>:: m_ptr2IndexMapPtr.end() );
  VERT_ID sourceVId = (*posS).second;
  
  typename std::map<typename boost::graph_traits<graphType>::vertex_descriptor,
              VERT_ID>::const_iterator posT;
  posT = FsaAccessIOHandler<graphType>::m_ptr2IndexMapPtr.find(target);
  assert( posT != FsaAccessIOHandler<graphType>::m_ptr2IndexMapPtr.end() );
  VERT_ID targetVId = (*posT).second;
  
  FsaAccessIOHandler<graphType>::writeEdgeAsInt( ow, sourceVId, targetVId );
}

template <typename graphType >
void FsaAccessIOHandlerWithoutMapping<graphType>::writeEdge(
 AbstractFsaAccessOStreamWrapper& ow,
 typename boost::graph_traits<graphType>::vertex_descriptor source,
 typename boost::graph_traits<graphType>::vertex_descriptor target ) {

  VERT_ID sourceVId = static_cast<VERT_ID>(source);
  VERT_ID targetVId = static_cast<VERT_ID>(target);
  
  FsaAccessIOHandler<graphType>::writeEdgeAsInt( ow, sourceVId, targetVId );
}

template <typename graphType >
void FsaAccessIOHandler<graphType>::writeEdgeAsInt(
 AbstractFsaAccessOStreamWrapper& ow,
 VERT_ID source,
 VERT_ID target ) {
#ifdef DEBUG_CD
  FSAAIOLOGINIT;
  LTRACE <<  "FsaAccessIOHandler::writeEdge(" << source
            << "," << target << ")";
#endif

  ow.writeData( (const char *)(&source), sizeof(VERT_ID) );

  ow.writeData( (const char *)(&target), sizeof(VERT_ID) );
}


/*template <typename graphType >
void FsaAccessIOHandler<graphType>::readData(
  AbstractFsaAccessIStreamWrapper& iw, const void *ptr, size_t n ) const {
  is.read( (char *)ptr, n );
}

template <typename graphType >
void FsaAccessIOHandler<graphType>::writeData(
  std::ostream &os, const void *ptr, size_t n ) const {
  os.write( (const char *)ptr, n );
}
*/

} // namespace FsaAccess
} // namespace Common
} // namespace Lima
