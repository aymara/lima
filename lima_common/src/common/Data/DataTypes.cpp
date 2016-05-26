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

#include "DataTypes.h"
#include "common/LimaCommon.h"
#include <iostream>
#include <limits>

using namespace std;
using namespace Lima;

namespace Lima {

class LIMA_DATA_EXPORT NodePrivate
{
  friend class Node;
public:
  NodePrivate();
  virtual ~NodePrivate();
  NodePrivate(const NodePrivate& n);
  NodePrivate& operator=(const NodePrivate& n);

  NodePrivate ( const std::string& docName, STRUCT_ID structId, NODE_ID nodeId, CONTENT_ID contentId, int32_t indexid , std::string uri, std::string nodeMedia, std::string nodeType, std::string descrId , bool isRoot, bool isLeaf, bool isFile, bool isMultimedia, TOPO_POS nodeStart, TOPO_POS nodeEnd, TOPO_POS nodeLength, NODE_ID nodeParent );

  //! @brief identifiant de structure
  STRUCT_ID m_structId;
  // OME Pour test
  std::string m_docName;
  //! @brief identifiant de noeud au sein de la structure.
  NODE_ID m_nodeId;
  //! @brief identifiant de contenu au sein de la structure.
  CONTENT_ID m_contentId;
  std::string m_uri;
  std::string m_nodeMedia;
  std::string m_nodeType;
  //! @brief identifiant de type de contenu
  std::string m_descrId;
  bool m_isRoot;
  bool m_isLeaf;
  bool m_isFile;
  bool m_isMultimedia;
  TOPO_POS m_nodeStart;
  TOPO_POS m_nodeEnd;
  TOPO_POS m_nodeLength;
  NODE_ID m_nodeParent;
  int32_t m_indexId;
};

////////////////////: NodePrivate methods
NodePrivate::NodePrivate() {}
NodePrivate::~NodePrivate() {}
NodePrivate::NodePrivate(const NodePrivate& n)
{
  m_docName = n.m_docName;
  m_structId = n.m_structId;
  m_contentId = n.m_contentId;
  m_nodeId = n.m_nodeId;
  m_uri = n.m_uri;
  m_nodeMedia = n.m_nodeMedia;
  m_nodeType = n.m_nodeType;
  m_isRoot = n.m_isRoot;
  m_isLeaf = n.m_isLeaf;
  m_isFile = n.m_isFile;
  m_isMultimedia = n.m_isMultimedia;
  m_nodeStart = n.m_nodeStart;
  m_nodeEnd = n.m_nodeEnd;
  m_nodeLength = n.m_nodeLength;
  m_nodeParent = n.m_nodeParent;
  m_descrId = n.m_descrId;
  m_indexId = n.m_indexId;
}

NodePrivate& NodePrivate::operator=(const NodePrivate& n)
{
  m_docName = n.m_docName;
  m_structId = n.m_structId;
  m_contentId = n.m_contentId;
  m_nodeId = n.m_nodeId;
  m_uri = n.m_uri;
  m_nodeMedia = n.m_nodeMedia;
  m_nodeType = n.m_nodeType;
  m_isRoot = n.m_isRoot;
  m_isLeaf = n.m_isLeaf;
  m_isFile = n.m_isFile;
  m_isMultimedia = n.m_isMultimedia;
  m_nodeStart = n.m_nodeStart;
  m_nodeEnd = n.m_nodeEnd;
  m_nodeLength = n.m_nodeLength;
  m_nodeParent = n.m_nodeParent;
  m_descrId = n.m_descrId;
  m_indexId = n.m_indexId;

  return *this;
}

NodePrivate::NodePrivate ( const string& docName, STRUCT_ID structId, NODE_ID nodeId, CONTENT_ID contentId, int32_t indexid, string uri, string nodeMedia, string nodeType, string descrId, bool isRoot, bool isLeaf, bool isFile, bool isMultimedia, TOPO_POS nodeStart, TOPO_POS nodeEnd, TOPO_POS nodeLength, NODE_ID nodeParent )
{
  m_docName = docName;
  m_structId = structId;
  m_contentId = contentId;
  m_nodeId = nodeId;
  m_uri = uri;
  m_nodeMedia = nodeMedia;
  m_nodeType = nodeType;
  m_isRoot = isRoot;
  m_isLeaf = isLeaf;
  m_isFile = isFile;
  m_isMultimedia = isMultimedia;
  m_nodeStart = nodeStart;
  m_nodeEnd = nodeEnd;
  m_nodeLength = nodeLength;
  m_nodeParent = nodeParent;
  m_descrId = descrId;
  m_indexId = indexid;
};

////////////////////: Node methods
Node::Node() : m_d(new NodePrivate()) {}
Node::~Node() {delete m_d;}
Node::Node(const Node& n) : m_d(new NodePrivate(*n.m_d)) {}
Node &Node::operator=(const Node& n)
{
  if (m_d != 0) {
    delete m_d;
  }
  m_d = new NodePrivate(*n.m_d);
  return *this;
}


Node::Node ( const std::string& docName, STRUCT_ID structId, NODE_ID nodeId, CONTENT_ID contentId, int indexid , std::string uri, std::string nodeMedia, std::string nodeType, std::string descrId , bool isRoot, bool isLeaf, bool isFile, bool isMultimedia, TOPO_POS nodeStart, TOPO_POS nodeEnd, TOPO_POS nodeLength, NODE_ID nodeParent ) : m_d(new NodePrivate(docName,structId, nodeId, contentId, indexid , uri, nodeMedia, nodeType, descrId , isRoot, isLeaf, isFile, isMultimedia, nodeStart, nodeEnd, nodeLength, nodeParent))
{
}

std::string Node::get_DocName() const {
  return m_d->m_docName;
};
void  Node::set_DocName ( const std::string& docName ) {
  m_d->m_docName=docName;
}
STRUCT_ID Node::get_StructId() const {
  return m_d->m_structId;
};
void  Node::set_StructId ( STRUCT_ID structId ) {
  m_d->m_structId=structId;
}
NODE_ID Node::get_NodeId() const {
  return m_d->m_nodeId;
};
void  Node::set_NodeId ( NODE_ID NodeId ) {
  m_d->m_nodeId=NodeId;
}
CONTENT_ID Node::get_ContentId() const {
  return m_d->m_contentId;
};
void  Node::set_ContentId ( CONTENT_ID ContentId ) {
  m_d->m_contentId=ContentId;
}

int32_t Node::indexId() const
{
  return m_d->m_indexId;
}

void Node::indexId(int32_t id)
{
  m_d->m_indexId = id;
}

const std::string& Node::descrId() const
{
  return m_d->m_descrId;
}

const std::string& Node::nodeType() const
{
  return m_d->m_nodeType;
}

bool Node::isRoot() const
{
  return m_d->m_isRoot;
}

bool Node::isLeaf() const
{
  return m_d->m_isLeaf;
}

bool Node::isFile() const
{
  return m_d->m_isFile;
}

bool Node::isMultimedia() const
{
  return m_d->m_isMultimedia;
}

TOPO_POS Node::nodeStart() const
{
  return m_d->m_nodeStart;
}

TOPO_POS Node::nodeEnd() const

{
  return m_d->m_nodeEnd;
}

TOPO_POS Node::nodeLength() const
{
  return m_d->m_nodeLength;
}


NODE_ID Node::nodeParent() const
{
  return m_d->m_isRoot;
}

const std::string& Node::uri() const
{
  return m_d->m_uri;
}

const std::string& Node::nodeMedia() const
{
  return m_d->m_nodeMedia;
}

void Node::nodeStart(TOPO_POS ns)
{
  m_d->m_nodeStart = ns;
}

void Node::nodeEnd(TOPO_POS ns)
{
  m_d->m_nodeEnd = ns;
}

void Node::nodeLength(TOPO_POS ns)
{
  m_d->m_nodeLength = ns;
}

void Node::nodeParent(NODE_ID np)
{
  m_d->m_nodeParent = np;
}


////////////// StructurePrivate class

class LIMA_DATA_EXPORT StructurePrivate
{
    friend class Structure;
public:
  StructurePrivate();
  StructurePrivate(const StructurePrivate& sp);
  StructurePrivate ( STRUCT_ID structID );
  StructurePrivate& operator= ( const StructurePrivate& sp );
  
    //! @brief destructeur: delete des 'Nodes' de contenu
    ~StructurePrivate();

    //! @brief the set of
    std::map<CONTENT_ID,Node> m_nodes;

    //! @brief the numeric identifier of the current structure
    STRUCT_ID m_structId;
};

StructurePrivate::StructurePrivate() : m_structId(std::numeric_limits< STRUCT_ID >::max()) {}

StructurePrivate::StructurePrivate(const StructurePrivate& sp) :
m_nodes(sp.m_nodes), m_structId(sp.m_structId)
{
}

StructurePrivate& StructurePrivate::operator=(const StructurePrivate& sp)
{
  m_nodes = sp.m_nodes;
  m_structId = sp.m_structId;
  return *this;
}

StructurePrivate::StructurePrivate ( STRUCT_ID structID ) : m_structId(structID)
{
}

//! @brief destructeur: delete des 'Nodes' de contenu
StructurePrivate::~StructurePrivate()
{
}

///////////////// Structure methods
Structure::Structure() : m_d(new StructurePrivate()) {}
  
Structure::Structure ( STRUCT_ID structID )  : m_d(new StructurePrivate(structID))
{
}

Structure::Structure ( const Structure& s )  : m_d(new StructurePrivate(*s.m_d))
{
}

Structure& Structure::operator=( const Structure& s )
{
  if (m_d!=0) {
    delete m_d;
  }
  m_d = new StructurePrivate(*s.m_d);
  return *this;
}

//! @brief destructeur: delete des 'Nodes' de contenu
Structure::~Structure()
{
    delete m_d;
}

void Structure::addNode (const Node& node )
{
  m_d->m_nodes[node.get_ContentId()]=node;
}

void Structure::addNode ( CONTENT_ID ContentId, const Node& node )
{
    m_d->m_nodes[ContentId]=node;
}

Node* Structure::getNode ( CONTENT_ID ContentId )
{
  if (m_d->m_nodes.find(ContentId) == m_d->m_nodes.end())
    return 0;
  return &m_d->m_nodes[ContentId];
}

const Node* Structure::getNode ( CONTENT_ID ContentId ) const
{
  if (m_d->m_nodes.find(ContentId) == m_d->m_nodes.end())
    return 0;
  return &m_d->m_nodes[ContentId];
}

const map<CONTENT_ID,Node>* Structure::getNodes() const
{
  return &m_d->m_nodes;
}

map<CONTENT_ID,Node>* Structure::getNodes()
{
  return &m_d->m_nodes;
}

STRUCT_ID Structure::getStructId() const {
    return m_d->m_structId;
}

Node* Structure::getFirstNode ( STRUCT_ID StructureId )
{
  LIMA_UNUSED(StructureId);
  map<CONTENT_ID,Node>::iterator ItrNodes;
  for (ItrNodes=m_d->m_nodes.begin();ItrNodes!=m_d->m_nodes.end();ItrNodes++)
  {
    if ((ItrNodes->second.nodeType()=="topic"))
    {
      return getNode(ItrNodes->first);
    }
  }
  return 0;
}

const Node* Structure::getFirstNode ( STRUCT_ID StructureId ) const
{
  LIMA_UNUSED(StructureId);
  for (auto nodesIt=m_d->m_nodes.begin();nodesIt!=m_d->m_nodes.end();nodesIt++)
  {
    if ((nodesIt->second.nodeType()=="topic"))
    {
      return getNode(nodesIt->first);
    }
  }
  return 0;
}

std::ostream& operator<<(ostream& os, const Node& node)
{
  os<<"Node "<<node.get_StructId()<<" "<<node.get_NodeId()<<" "<<node.get_ContentId()<<" "<<node.indexId()<<" "<<node.descrId()<<" "<<node.get_DocName()<<std::endl;
  return os;
}

QDebug& operator<<(QDebug& os, const Node& node)
{
  os<<"Node(structId:"<<node.get_StructId()<<", nodeId:"<<node.get_NodeId()<<", contentId:"<<node.get_ContentId()<<", indexId:"<<node.indexId()<<", descrId:"<<node.descrId()<<", docName:"<<node.get_DocName() << ")";
  return os;
}

std::ostream& operator<<(ostream& os, const Structure& structure)
{
  const map<CONTENT_ID,Node>* nodes=structure.getNodes();
  map<CONTENT_ID,Node>::const_iterator ItrNodes;
  for (ItrNodes=nodes->begin();ItrNodes!=nodes->end();ItrNodes++)
  {
    os<<ItrNodes->second;
  }
  return os;
}
QDebug& operator<<(QDebug& os, const Structure& structure)
{
  const map<CONTENT_ID,Node>* nodes=structure.getNodes();
  os << "Structure( structId:" << structure.getStructId() << ", nodes ("<<nodes->size()<<"): ";
  for (map<CONTENT_ID,Node>::const_iterator ItrNodes = nodes->begin(); ItrNodes != nodes->end() ; ItrNodes++)
  {
    os<<"node ("<<ItrNodes->first<<":" << ItrNodes->second<<")";
  }
  return os;
}
/*
NODE_ID Structure::getNodeId(string featId,CONTENT_ID contentId,int indexId)
{
      map<CONTENT_ID,Node>* nodes=getNodes();
      map<CONTENT_ID,Node>::iterator ItrNodes;
for (ItrNodes=nodes->begin();ItrNodes!=nodes->end();ItrNodes++)
{
  Node node=ItrNodes->second;
  if ((node.m_descrId==featId)&&(node.m_contentId==contentId)&&(node.m_indexId==indexId))
    return node.m_nodeId;
}
return 0;
}*/

void Structure::setStructId(STRUCT_ID structId)
{
  m_d->m_structId=structId;
}

  
} // Lima
