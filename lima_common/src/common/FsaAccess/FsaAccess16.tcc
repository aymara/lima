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
 *                          FsaAccess16.cpp  -  description
 *                             -------------------
 *    begin                : mer mai 28 2003
 *    copyright            : (C) 2003 by Olivier Mesnard
 *    email                : olivier.mesnard@cea.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *  compact dictionnary based on finite state automata                     *
 *  implemented with Boost Graph library                                   *
 *                                                                         *
 ***************************************************************************/
#include <common/Data/LimaString.h>


namespace Lima {
namespace Common {
namespace FsaAccess {

template <typename vname_map_type, typename vtext_map_type, typename vcount_map_type, typename dicoVertex>
dicoVertexWriter16<vname_map_type, vtext_map_type, vcount_map_type,dicoVertex>::
dicoVertexWriter16(vname_map_type vname_map,  vtext_map_type vtext_map, vcount_map_type vcount_map)
: m_vname_map(vname_map), m_vtext_map(vtext_map), m_vcount_map(vcount_map) {}

template <typename vname_map_type, typename vtext_map_type, typename vcount_map_type, typename dicoVertex>
inline void dicoVertexWriter16<vname_map_type, vtext_map_type, vcount_map_type,dicoVertex>::operator()(std::ostream& out, const dicoVertex& v) const
{
  VERTEX_PROPERTY_16 prop = get(m_vname_map,v);
  out << "["
  << ( ((prop & Lima::Common::FsaAccess::FINAL_16) ==
  Lima::Common::FsaAccess::FINAL_16) ?
  "FINAL" : "" );
  out << "-"
  << ( ((prop & Lima::Common::FsaAccess::HEAD_OF_CLASS_16) ==
  Lima::Common::FsaAccess::HEAD_OF_CLASS_16) ?
  "HEAD_OF_CLASS" : "" );
  out << "-"
  << ( ((prop & Lima::Common::FsaAccess::SET_16) ==
  Lima::Common::FsaAccess::SET_16) ?
  "count SET]" : " count NOT SET]" );
  out << " ("
  << (prop & Lima::Common::FsaAccess::TEXT_POS_16) << ") ";
  const std::vector<int>& counts = get(m_vcount_map,v);
  copy(counts.begin(), counts.end(), std::ostream_iterator<int>(out, ", "));
  const Lima::LimaString& text = get(m_vtext_map,v);

  std::string text8 = Lima::Common::Misc::limastring2utf8stdstring(text);
  out << "'" << text8 << "'";

  VERTEX_PROPERTY_16 max = get(m_vname_map,v)&TEXT_POS_16;
  out << " hicharOff=" << max;
}



template <typename graphType>
FsaAccess16<graphType>::FsaAccess16(bool trie_direction_fwd) :
    FsaAccessHeader(trie_direction_fwd)
{
  m_rootVertex = add_vertex( m_graph );
  m_0degreeOutVertex = m_rootVertex;

  #ifdef DEBUG_CD
  FSAALOGINIT;
  LDEBUG <<  "FsaAccess16::FsaAccess16: m_rootVertex= "
  << m_rootVertex;
  #endif

  typename dicoGraph_traits16<graphType>::nconst_vname_map_type vname_map =
  boost::get(boost::vertex_name,m_graph);
  typename dicoGraph_traits16<graphType>::nconst_vcount_map_type vcount_map =
  boost::get(vertex_count,m_graph);
  put(vname_map, m_rootVertex, 0);
  std::vector<int> count;
  put(vcount_map, m_rootVertex, count);

}

template <typename graphType >
dicoVertexType FsaAccess16<graphType>::find0degreeVertex(
  dicoVertexType from ) const
{

  // On descend la premiere branche
  for( ; ; ) {
    typename boost::graph_traits<graphType>::out_edge_iterator ei, edge_end;
    boost::tie(ei,edge_end) = boost::out_edges(from,m_graph);
    if( ei == edge_end ) {
      return from;
    }
    else {
      from = target(*ei,m_graph);
    }
  }
}

template <typename graphType>
void FsaAccess16<graphType>::readBody(
  AbstractFsaAccessIStreamWrapper& iw)
{
  #ifdef DEBUG_CD
  FSAAIOLOGINIT;
  LDEBUG <<  "FsaAccess16::readBody";
  #endif

  FsaAccessIOHandler<graphType>* iOHandler = getFsaAccessIOHandler();

  typename dicoGraph_traits16<graphType>::nconst_vname_map_type vname_map =
  boost::get(boost::vertex_name,m_graph);
  typename dicoGraph_traits16<graphType>::nconst_vtext_map_type vtext_map =
  boost::get(vertex_text,m_graph);
  typename boost::graph_traits<graphType>::vertices_size_type nbVert =
  FsaAccessHeader::getNbVertices();

  if( nbVert > 0 ) {
    VERTEX_PROPERTY_16 propVal;
    Lima::LimaString text;
    iOHandler->readVertice( iw, m_rootVertex, propVal, text );
    put(vname_map, m_rootVertex, propVal&(~SET_16) );
    put(vtext_map, m_rootVertex, text );
  }

  #ifdef DEBUG_CD
  LDEBUG <<  "FsaAccess16::readBody read" << nbVert << "vertices";
  #endif
  for( uint32_t nv = 1 ; nv < nbVert ; nv++ ) {
    #ifdef DEBUG_CD
    if (nv % 1000 == 0)
      LDEBUG <<  "FsaAccess16::readBody vertex num = " << nv;
    #endif
    dicoVertexType newV = add_vertex(m_graph);
    VERTEX_PROPERTY_16 propVal;
    Lima::LimaString text;
    iOHandler->readVertice( iw, newV, propVal, text );
    put(vname_map, newV, propVal&(~SET_16) );
    put(vtext_map, newV, text );
  }

  typename boost::graph_traits<graphType>::edges_size_type nbEdges =
      FsaAccessHeader::getNbEdges();
  #ifdef DEBUG_CD
  LDEBUG <<  "FsaAccess16::readBody read" << nbEdges << "edges";
  #endif
  for( uint32_t ne = 0 ; ne < nbEdges ; ne++ ) {
    #ifdef DEBUG_CD
    if (ne % 1000 == 0)
      LDEBUG <<  "FsaAccess16::readBody edge num = " << ne;
    #endif
    dicoVertexType source, target;
    Lima::LimaString epropVal;
    iOHandler->readEdge( iw, source, target );
    std::pair<dicoEdgeType, bool> res = add_edge( source, target, m_graph );
    assert( res.second );
    if( epropVal.size() > 0 ) {
      Lima::LimaString text =  get(vtext_map, source);
      text.append(epropVal);
      put(vtext_map, source, text);
    }
  }
  m_0degreeOutVertex = find0degreeVertex( m_rootVertex );

  delete iOHandler;
  #ifdef DEBUG_CD
  LDEBUG <<  "FsaAccess16::readBody done";
  #endif
}

template <typename graphType>
//void FsaAccess16<graphType>::writeBody( const std::string & filename  )
void FsaAccess16<graphType>::writeBody( AbstractFsaAccessOStreamWrapper& ow  )
{
  #ifdef DEBUG_CD
  FSAAIOLOGINIT;
  LDEBUG <<  "FsaAccess16::writeBody()";
  #endif

  #ifdef DEBUG_CD
  LDEBUG <<  "FsaAccess16::writeBody: start writeVertices()";
  #endif

  FsaAccessIOHandler<graphType>* iOHandler = getFsaAccessIOHandler();
  writeVertices( ow, iOHandler, m_rootVertex );

  #ifdef DEBUG_CD
  LDEBUG <<  "FsaAccess16::writeBody: end writeVertices()";
  #endif

  #ifdef DEBUG_CD
  LDEBUG <<  "FsaAccess16::writeBody: start writeEdges()";
  #endif
  typename boost::graph_traits<graphType>::vertex_iterator vi, v_end;
  for( boost::tie(vi,v_end) = boost::vertices(m_graph) ; vi != v_end; vi++ ) {
    typename boost::graph_traits<graphType>::out_edge_iterator oei, oedge_end;
    boost::tie(oei,oedge_end) = boost::out_edges(*vi,m_graph);
    for( ; oei != oedge_end ; oei++ ) {
      iOHandler->writeEdge( ow, *vi, boost::target(*oei,m_graph) );
    }
  }
  #ifdef DEBUG_CD
  LDEBUG <<  "FsaAccess16::writeBody: end writeEdges()";
  #endif
  delete iOHandler;
  #ifdef DEBUG_CD
  LDEBUG <<  "FsaAccess16::writeBody: end writeBody()";
  #endif
}

template <typename graphType >
void FsaAccess16<graphType>::writeVertices( AbstractFsaAccessOStreamWrapper &ow,
                                            FsaAccessIOHandler<graphType>* iOHandler,
                                            dicoVertexType from ) {

  typename dicoGraph_traits16<graphType>::nconst_vname_map_type vname_map =
  boost::get(boost::vertex_name,m_graph);
  typename dicoGraph_traits16<graphType>::nconst_vtext_map_type vtext_map =
  boost::get(vertex_text,m_graph);

  // Le noeud from a-t-il d???? un Id qui lui correspond dans la map?
  typename std::map<dicoVertexType,
  uint64_t>::const_iterator pos;

  if( !(iOHandler->isInMap(from)) ) {
    // On recupere les proprietes
    VERTEX_PROPERTY_16 propVal = get(vname_map,from);
    const Lima::LimaString& text = get(vtext_map, from);

    iOHandler->writeVertice( ow, from, propVal, text );
    // On parcours les sous-arbre
    typename boost::graph_traits<graphType>::out_edge_iterator ei, edge_end;
    boost::tie(ei,edge_end) = boost::out_edges(from,m_graph);

    for( ; ei != edge_end ; ei++ ) {
      writeVertices( ow, iOHandler, target(*ei,m_graph) );
    }
  }
}

template <typename graphType >
void FsaAccess16<graphType>::printGraph( std::ostream &os ) const {
  #ifdef DEBUG_CD
  FSAAIOLOGINIT;
  LDEBUG <<  "FsaAccess16::printGraph()";
  #endif

  dicoVertexWriter16<typename dicoGraph_traits16<graphType>::vname_map_type,
  typename dicoGraph_traits16<graphType>::vtext_map_type,
  typename dicoGraph_traits16<graphType>::vcount_map_type,
  dicoVertexType> dvw(
    boost::get(boost::vertex_name,m_graph),
                      boost::get(vertex_text,m_graph),
                      boost::get(vertex_count,m_graph));
  //  dicoEdgeWriter16<dicoEdgeType> dew;
  //  boost::default_writer dw;
  // TODO: pourquoi dans la fonction
  //  inline void write_graphviz(std::ostream& out, const Graph& g,
  //                             VertexPropertiesWriter vpw,
  //                             EdgePropertiesWriter epw,
  //                             GraphPropertiesWriter gpw)
  //  du source graphviz.hpp de la librairie boost::graph
  //  out << get(vertex_index, *i); n'est pas reconnu correctementet ne trouve as d'impl�entation

  //  boost::write_graphviz(os, m_graph, dvw, dw, dw );

//  typedef typename boost::property_map<graphType, boost::vertex_index_t>::const_type vimap_t;
//   vimap_t vertex_index = boost::get(boost::vertex_index_t(), m_graph);
  typedef typename boost::graph_traits<graphType>::directed_category cat_type;
  typedef boost::graphviz_io_traits<cat_type> Traits;
  std::string name = "G";
  os << Traits::name() << " " << name << " {" << std::endl;

  // gpw(out); //print graph properties

  typename boost::graph_traits<graphType>::vertex_iterator i, end;

  for(boost::tie(i,end) = vertices(m_graph); i != end; ++i) {
    //      os << get(vertex_index, *i);
    os << *i;
    //      vpw(out, *i); //print vertex attributes
    dvw(os, *i); //print vertex attributes
    os << ";" << std::endl;
  }
  typename boost::graph_traits<graphType>::edge_iterator ei, edge_end;
  for(boost::tie(ei, edge_end) = boost::edges(m_graph); ei != edge_end; ++ei) {
    //      os << get(vertex_index, source(*ei, m_graph)) << Traits::delimiter() << get(vertex_index, target(*ei, m_graph)) << " ";
    os << boost::source(*ei, m_graph) << Traits::delimiter() << boost::target(*ei, m_graph) << " ";
    //      epw(os, *ei); //print edge attributes
    os << ";" << std::endl;
  }
  os << "}" << std::endl;
}

template <typename graphType>
std::ostream& operator << (std::ostream& os, const FsaAccess16<graphType>& dico) {
  dico.print( os );
  return os;
}

template <typename graphType >
void FsaAccess16<graphType>::print( std::ostream &os,
dicoVertexType from,
Lima::LimaString &prefix ) const{
  #ifdef DEBUG_CD
  FSAAIOLOGINIT;
  LTRACE <<  "FsaAccess16::print(" << from
  << ", prefix='" << prefix << "')"
  ;
  #endif

  typename boost::graph_traits<graphType>::out_edge_iterator ei, edge_end;
  boost::tie(ei,edge_end) = boost::out_edges(from,m_graph);

  typename dicoGraph_traits16<graphType>::vname_map_type vname_map =
  boost::get(boost::vertex_name,m_graph);
  typename dicoGraph_traits16<graphType>::vtext_map_type vtext_map =
  boost::get(vertex_text,m_graph);

  const Lima::LimaString& text = get(vtext_map,from);
  const QChar* const content = text.data();
  char32_t label;
  int textLength = text.length();
  int32_t textPos;

  if( (get(vname_map,from)&Lima::Common::FsaAccess::FINAL_16) == Lima::Common::FsaAccess::FINAL_16 ) {
    std::string text8 = Lima::Common::Misc::limastring2utf8stdstring(prefix);
    os << text8 << std::endl;
  }
  if(ei == edge_end) {
    return;
  }

  for( textPos = 0 ; textPos < textLength ; ei++ ) {
    int32_t textPos0 = textPos;
    U16_NEXT(content, textPos, textLength, label);
    int32_t delta = textPos - textPos0;
    if( FsaAccessHeader::getTrieDirectionForward() )
      prefix.append( LimaString(content).mid(textPos0).left(delta) );
    else
      prefix.insert( 0, content+textPos0, delta );
    print( os, target(*ei,m_graph), prefix);
    if( FsaAccessHeader::getTrieDirectionForward() )
      prefix.remove( prefix.length() - delta, delta );
    else
      prefix.remove( 0, delta );
  }
}

template <typename graphType >
void FsaAccess16<graphType>::print( std::ostream &os ) const{
  #ifdef DEBUG_CD
  FSAAIOLOGINIT;
  LDEBUG <<  "FsaAccess16::print()";
  #endif
  Lima::LimaString prefix;

  print( os, m_rootVertex, prefix);

}

template <typename graphType >
void FsaAccess16<graphType>::pack() {
  #ifdef DEBUG_CD
  FSAAIOLOGINIT;
  LDEBUG <<  "FsaAccess16::pack()";
  #endif
}



template <typename graphType >
void FsaAccess16<graphType>::checkIntegrity(
  dicoVertexType from ) const
{
  #ifdef DEBUG_CD
  FSAALOGINIT;
  LTRACE <<  "FsaAccess16::checkIntegrity(" << from << ")";
  #endif

  typename boost::graph_traits<graphType>::out_edge_iterator ei, edge_end;
  boost::tie(ei,edge_end) = boost::out_edges(from,m_graph);

  typename dicoGraph_traits16<graphType>::vname_map_type vname_map =
  boost::get(boost::vertex_name,m_graph);
  typename dicoGraph_traits16<graphType>::vtext_map_type vtext_map =
  boost::get(vertex_text,m_graph);
  typename dicoGraph_traits16<graphType>::vcount_map_type vcount_map =
  boost::get(vertex_count,m_graph);

  const Lima::LimaString& text = get(vtext_map,from);
  const std::vector<int>& counts = get(vcount_map,from);
  typename graphType::degree_size_type outd = boost::out_degree(from, m_graph);
  if( outd == 0 )  {
    assert( text.length() == 0 );
    assert( counts.size() == 0 );
  }
  else {
    assert( outd == text.length() );
    assert( outd == counts.size()+1 );
  }

  for( ; ei != edge_end ; ei++ ) {
    checkIntegrity( target(*ei,m_graph));
  }
}

template <typename graphType>
void FsaAccess16<graphType>::getPrefix(
    dicoVertexType &from,
    PrefixIterator* prefixIt ) const
{

  #ifdef DEBUG_CD
  FSAALOGINIT;
  LTRACE <<  "FsaAccess16::getPrefix(" << from << ", " << prefixIt->getCurrentPrefix() << ")";
  #endif

  if( !prefixIt->hasNextLetter() ) {
    return;
  }

  int32_t prefixOffset = 0;
  char32_t currentChar = prefixIt->getNextLetter(prefixOffset);

  #ifdef DEBUG_CD
  LTRACE << "FsaAccess16::getPrefix: currentChar = "<<LimaChar(currentChar)<<", prefixOffset=" << prefixOffset;
  #endif

  // get the property map for 'edge_name' property
  typename dicoGraph_traits16<graphType>::vtext_map_type vtext_map =
  boost::get(vertex_text,m_graph);
  // get the property map for 'vertex_name' property
  typename dicoGraph_traits16<graphType>::vname_map_type vname_map =
  boost::get(boost::vertex_name,m_graph);

  // Iterator for dictionary: out_edge iterator starting at root (vertex 0)
  typename boost::graph_traits<graphType>::out_edge_iterator ei, edge_end;
  boost::tie(ei,edge_end) = boost::out_edges(from,m_graph);

  const Lima::LimaString& text = get(vtext_map,from);
  #ifdef DEBUG_CD
  LTRACE << "FsaAccess16::getPrefix: transitions text = "<<text<<", from out degree=" << boost::out_degree(from,m_graph);
  if (text.size() != static_cast<int>(boost::out_degree(from,m_graph)))
  {
    assert(text.size() == static_cast<int>(boost::out_degree(from,m_graph)));
  }
  #endif
  

  int32_t highCharTextPos = get(vname_map,from)&TEXT_POS_16;

  int32_t edgeOffset;
  if( prefixOffset == 1 ) {
    edgeOffset = findEdge( currentChar, text, 0, highCharTextPos );
  }
  else {
    int32_t textOffset;
    textOffset = findEdge( currentChar, text, highCharTextPos, text.length() );
    edgeOffset = highCharTextPos + (textOffset - highCharTextPos)/2;
  }
  if( edgeOffset >= 0 ) {
    prefixIt->next(prefixOffset);
    typename boost::graph_traits<graphType>::out_edge_iterator ei, edge_end;
    boost::tie(ei,edge_end) = boost::out_edges(from,m_graph);
    #ifdef DEBUG_CD
    LTRACE <<  "FsaAccess16::getPrefix: edgeOffset=" << edgeOffset << "; out_degree=" << boost::out_degree(from,m_graph);
    assert(edgeOffset < static_cast<int>(boost::out_degree(from,m_graph)));
    #endif
    dicoEdgeType edge = *(ei+edgeOffset);
    from = target(edge, m_graph);
    #ifdef DEBUG_CD
    LTRACE <<  "FsaAccess16::getPrefix: recursive call";
    #endif
    getPrefix( from, prefixIt );
  }
}

template <typename graphType> int32_t FsaAccess16<graphType>::findEdge(
const char32_t searchChar,
const Lima::LimaString& textString,
int32_t min, int range ) const {

  #ifdef DEBUG_CD
  FSAALOGINIT;
  LTRACE <<  "FsaAccess16::findEdge searchChar=" << LimaChar(searchChar) << " ("<<searchChar<<"), textString=" << textString << ", min=" << min << ", range=" << range;
  #endif
  if (textString.isEmpty())
  {
    return -1;
  }
  const QChar* text = textString.constData();
  int textLength = textString.length();
  char32_t edgeLabel = 0;
  int32_t offset = min;
  //  int midrange = range/2;

  while( range > 0 ) {
    offset = min + range/2;
    int32_t offset0 = offset;
    #ifdef DEBUG_CD
    LTRACE <<  "FsaAccess16::findEdge: min = " << min << " range = " << range << " try " << offset;
    #endif
    U16_NEXT(text, offset, textLength, edgeLabel);
    #ifdef DEBUG_CD
    LTRACE <<  "FsaAccess16::findEdge: offset = " << offset << " textLength = " << textLength  << " edgeLabel = " << LimaChar(edgeLabel) << "("<<edgeLabel<<")";
    LTRACE <<  "FsaAccess16::findEdge: min = " << min << "; range = " << range << "; offset0 = " << offset0;
    #endif
    if( edgeLabel == searchChar ) {
      #ifdef DEBUG_CD
      LTRACE <<  "FsaAccess16::findEdge: match " << (LimaChar)edgeLabel << " return " << offset0;
      #endif
      return offset0;
    }
    else if( searchChar > edgeLabel ){
      range = min + range - offset0 - 1;
      min = offset0 + 1;
    }
    else {
      range = offset0 - min;
    }
  }
  #ifdef DEBUG_CD
  LTRACE <<  "FsaAccess16::findEdge: min = " << min << " range = " << range << " try " << min;
  #endif
  if (min >= textString.length())
  {
    #ifdef DEBUG_CD
    LTRACE <<  "FsaAccess16::findEdge: unmatch " << (LimaChar)edgeLabel << " and " << (LimaChar)searchChar;
    #endif
    return -1;
  }
  int32_t offset0 = min;
  U16_NEXT(text, min, textLength, edgeLabel);
  #ifdef DEBUG_CD
  LTRACE <<  "FsaAccess16::findEdge: min = " << min << " textLength = " << textLength  << " edgeLabel = " << LimaChar(edgeLabel) ;
  #endif
  if( edgeLabel == searchChar ) {
    #ifdef DEBUG_CD
    LTRACE <<  "FsaAccess16::findEdge: match " << (LimaChar)edgeLabel << " return " << offset0;
    #endif
    return offset0;
  }
  else {
    #ifdef DEBUG_CD
    LTRACE <<  "FsaAccess16::findEdge: unmatch " << (LimaChar)edgeLabel << " and " << (LimaChar)searchChar;
    #endif
    return -1;
  }
}

template <typename graphType> int32_t FsaAccess16<graphType>::findOffsetToInsertBefore(
const char32_t searchChar,
const Lima::LimaString& textString,
int32_t min, int range ) const {

  #ifdef DEBUG_CD
  FSAALOGINIT;
  LTRACE <<  "FsaAccess16::findOffsetToInsertBefore searchChar="<<LimaChar(searchChar)<<"; textString="<<textString<<"; min="<<min<<"; range=" << range;
  #endif
  if (textString.isEmpty())
  {
    #ifdef DEBUG_CD
    LTRACE <<  "FsaAccess16::findOffsetToInsertBefore empty string. return 0";
    #endif
    return 0;
  }
  
  const QChar* text = textString.constData();
  int textLength = textString.length();
  char32_t edgeLabel;
  int32_t offset = min;

  while( range > 1 ) {
    offset = min + range/2;
    int32_t offset0 = offset;
    #ifdef DEBUG_CD
    LTRACE <<  "FsaAccess16::findOffsetToInsertBefore: min = " << min
    << " range = " << range << " try " << offset;
    #endif
    U16_NEXT(text, offset, textLength, edgeLabel);
    if( searchChar > edgeLabel ){
      range = min + range - offset0;
      min = offset0;
    }
    else {
      range = offset0 - min;
    }
  }
  #ifdef DEBUG_CD
  LTRACE <<  "FsaAccess16::findOffsetToInsertBefore: min = " << min
  << " range = " << range << " try " << min;
  #endif
  int32_t offset0 = min;
  U16_NEXT(text, min, textLength, edgeLabel);
  if( searchChar > edgeLabel ) {
    #ifdef DEBUG_CD
    LTRACE <<  "FsaAccess16::findOffsetToInsertBefore: at "<<offset0<<", "<<LimaChar(searchChar)<<" > "<<LimaChar(edgeLabel)<<": return "
    << offset0+1 ;
    #endif
    return offset0+1;
  }
  else {
    #ifdef DEBUG_CD
    LTRACE <<  "FsaAccess16::findOffsetToInsertBefore: at "<<offset0<<", "<<LimaChar(searchChar)<<" <= "<<LimaChar(edgeLabel)<<": return "
    << offset0 ;
    #endif
    return offset0;
  }

}

template <typename graphType>
void FsaAccess16<graphType>::addSuffix(
  dicoVertexType from,
  PrefixIterator* prefixIt ) {
  #ifdef DEBUG_CD
  FSAALOGINIT;
  Lima::LimaString s = prefixIt->getCurrentPrefix();
  LTRACE <<  "FsaAccess16::addSuffix: (" << from << ", " << s << ")";
  #endif

  typename dicoGraph_traits16<graphType>::nconst_vtext_map_type vtext_map =
  boost::get(vertex_text,m_graph);
  typename dicoGraph_traits16<graphType>::nconst_vname_map_type vname_map =
  boost::get(boost::vertex_name,m_graph);
//   typename dicoGraph_traits16<graphType>::nconst_vcount_map_type vcount_map =
//   boost::get(vertex_count,m_graph);

  dicoVertexType to=from;

  int32_t prefixOffset(0);
  for( ; prefixIt->hasNextLetter() ; prefixIt->next(prefixOffset) ) {

    to = add_vertex(m_graph);
    char32_t letter = prefixIt->getNextLetter(prefixOffset);
    #ifdef DEBUG_CD
    LTRACE << "FsaAccess16::addSuffix added vertex="<<to<<", letter=" << (LimaChar)letter << ", suffixPos="<<prefixIt->getExternalWordPos();
    #endif

    std::pair<typename boost::graph_traits<graphType>::edge_descriptor , bool> res = add_edge(from, to, m_graph);
    #ifdef DEBUG_CD
    LTRACE <<  "FsaAccess16::addSuffix: add_edge(" << from << ", " << to << ") : " << res.second;
    assert(res.second);
    #endif

    Lima::LimaString& text = get(vtext_map,from);
    VERTEX_PROPERTY_16 vval = get(vname_map, from);
    VERTEX_PROPERTY_16 qualif = vval & QUALITY_16;
    VERTEX_PROPERTY_16 hicharOff = vval & TEXT_POS_16;

  
    int32_t posToInsertBefore = findOffsetToInsertBefore( LimaString(prefixIt->getCurrentContent()).left(prefixOffset)[0].unicode(),
                              text, 0, text.size());
    #ifdef DEBUG_CD
    LTRACE <<  "FsaAccess16::addSuffix: insert the left "<<prefixOffset<<" chars from " << LimaString(prefixIt->getCurrentContent()) << "(i.e. "<<LimaString(prefixIt->getCurrentContent()).left(prefixOffset)<<") to " << text << " at position" << posToInsertBefore;
    #endif
    text.insert(posToInsertBefore,LimaString(prefixIt->getCurrentContent()).left(prefixOffset));

    if( prefixOffset == 1) hicharOff++;

    #ifdef DEBUG_CD
    LTRACE <<  "FsaAccess16::addSuffix: put(vname_map, from="
    << from << ", " <<  qualif << " | " << hicharOff << ")";
    #endif
    put(vname_map, from, qualif | hicharOff);
    put(vname_map, to, 0);

    from = to;
  }
  #ifdef DEBUG_CD
  LTRACE <<  "FsaAccess16::addSuffix: put(vname_map, to="
  << to << ", " <<  FINAL_16 << ")";
  #endif
  put(vname_map, to, FINAL_16);
}


template <typename graphType >
PrefixIterator* FsaAccess16<graphType>::getPrefixIterator(const LimaString & word,
    const uint64_t offset) const {
  if( FsaAccessHeader::getTrieDirectionForward() )
    return( new ForwardPrefixIterator(word, offset) );
  else
    return( new ReversePrefixIterator(word, offset) );
}

template <typename graphType >
std::pair<
const dicoVertexType,
bool > FsaAccess16<graphType>::findEquivalentInRegister( dicoVertexType tempState ){
  #ifdef DEBUG_CD
  FSAALOGINIT;
  LTRACE <<  "FsaAccess16::findEquivalentInRegister("
  << tempState << ")";
  #endif
  if (tempState == m_0degreeOutVertex) {
    return(std::pair<const dicoVertexType,bool>(tempState,false));
  }
  dicoVertexType result = tempState;
  typename boost::graph_traits<graphType>::degree_size_type nbOut = boost::out_degree(tempState, m_graph);
  if( nbOut == 0) {
    typename boost::graph_traits<graphType>::degree_size_type nbOut0 =
    boost::out_degree(m_0degreeOutVertex, m_graph);
    // tempstate is the new vertex in the register without out_edges
    result = m_0degreeOutVertex;
    if( nbOut0 != 0 ) {
      #ifdef DEBUG_CD
      LTRACE <<  "FsaAccess16::findEquivalentInRegister: " << tempState
      << " replace m_0degreeOutVertex " << m_0degreeOutVertex
      ;
      #endif
      m_0degreeOutVertex = tempState;
      return(std::pair<const dicoVertexType,bool>(tempState,false));
    }
    else {
      #ifdef DEBUG_CD
      LTRACE <<  "FsaAccess16::findEquivalentInRegister: found "
      << "m_0degreeOutVertex " << m_0degreeOutVertex;
      #endif
      return(std::pair<const dicoVertexType,bool>(result,true));
    }
  }

  // Equivalent vertices share the same out_edges. So every candidate must
  // share the first one.
  // We select the first out_edge from tempState, we folllow it to reach a
  // vertex called commonTarget, and we note the label of the edge.
  // Then we search for candidate vertices reachable from commonTarget
  // through in_edges with the same label.
  typename dicoGraph_traits16<graphType>::nconst_vtext_map_type vtext_map =
  boost::get(vertex_text,m_graph);
  typename dicoGraph_traits16<graphType>::nconst_vname_map_type vname_map =
  boost::get(boost::vertex_name,m_graph);

  typename boost::graph_traits<graphType>::out_edge_iterator oei, oe_end;
  boost::tie(oei,oe_end) = boost::out_edges(tempState,m_graph);
  dicoVertexType commonTarget = target(*oei,m_graph);
  //LimaString label =  get(ename_map,*oei);

  // TODO: s'assurer que les in_edges sont ordonnes selon les caracteres
  // et optimiser la recherche
  typename boost::graph_traits<graphType>::in_edge_iterator iei, ie_end;
  boost::tie(iei,ie_end) = boost::in_edges(commonTarget,m_graph);
  for( ; iei != ie_end ; iei++ ) {
    dicoVertexType attracteur = source(*iei, m_graph);
    #ifdef DEBUG_CD
    LTRACE <<  "FsaAccess16::findEquivalentInRegister: compare "
    << attracteur << " and " << tempState;
    #endif
    #ifdef DEBUG_CD
    VERTEX_PROPERTY_16 sA = vname_map[attracteur];
    VERTEX_PROPERTY_16 sT = vname_map[tempState];
    LTRACE <<  "FsaAccess16::findEquivalentInRegister: attracteur = (" << sA << "/" << sT << ")";
    #endif
    if( (attracteur != tempState)
      && ( (vname_map[attracteur]&FINAL_16) == (vname_map[tempState]&FINAL_16) )
      && ( vtext_map[attracteur] == vtext_map[tempState] )
      && ( equivalent(tempState, attracteur) ) ) {
        #ifdef DEBUG_CD
        LTRACE <<  "FsaAccess16::findEquivalentInRegister: return " << attracteur << " : " << true;
        #endif
        return( std::pair<const dicoVertexType,bool>(attracteur,true) );
      }
  }
  #ifdef DEBUG_CD
  LTRACE <<  "FsaAccess16::findEquivalentInRegister: return " << tempState << " : " << false;
  #endif
  return(std::pair<const dicoVertexType,bool>(tempState,false));
}

template <typename graphType >
bool FsaAccess16<graphType>::equivalent( dicoVertexType referenceState, dicoVertexType candidateState ) const {
  #ifdef DEBUG_CD
  FSAALOGINIT;
  LTRACE <<  "FsaAccess16::equivalent(" << referenceState << ", " << candidateState << ")";
  #endif

//   typename dicoGraph_traits16<graphType>::vname_map_type vname_map =
//   boost::get(boost::vertex_name,m_graph);

  typename boost::graph_traits<graphType>::out_edge_iterator aei, a_end;
  boost::tie(aei,a_end) = boost::out_edges(candidateState,m_graph);
  typename boost::graph_traits<graphType>::out_edge_iterator rei, r_end;
  boost::tie(rei,r_end) = boost::out_edges(referenceState,m_graph);

  for( ; rei != r_end ; rei++, aei++ ) {
    if( ( aei == a_end )
      ||( target(*rei, m_graph) != target(*aei, m_graph) ) ) {
    #ifdef DEBUG_CD
      if( aei == a_end ) {
        LTRACE <<  "FsaAccess16::findEquivalentInRegister: failure  aei = a_end";
      }
      else {
        LTRACE <<  "FsaAccess16::findEquivalentInRegister: failure  "
        << target(*rei, m_graph) << "!=" << target(*aei, m_graph);
      }
      #endif
      return false;
      }
  }
  if( aei != a_end ) {
    #ifdef DEBUG_CD
    LTRACE <<  "FsaAccess16::equivalent: failure nboutC aei != a_end";
    #endif
    return false;
  }
  #ifdef DEBUG_CD
  LTRACE <<  "FsaAccess16::equivalent: true";
  #endif
  return true;
}

} // namespace FsaAccess
} // namespace Common
} // namespace Lima
