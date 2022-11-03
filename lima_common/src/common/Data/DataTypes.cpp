// Copyright 2002-2019 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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

  NodePrivate(const std::string& docName,
              STRUCT_ID structId,
              NODE_ID nodeId,
              CONTENT_ID contentId,
              int32_t indexid ,
              std::string uri,
              std::string nodeMedia,
              std::string nodeType,
              std::string descrId ,
              TOPO_POS nodeStart,
              TOPO_POS nodeEnd,
              TOPO_POS nodeLength,
              NODE_ID nodeParent);

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
  m_nodeStart = n.m_nodeStart;
  m_nodeEnd = n.m_nodeEnd;
  m_nodeLength = n.m_nodeLength;
  m_nodeParent = n.m_nodeParent;
  m_descrId = n.m_descrId;
  m_indexId = n.m_indexId;

  return *this;
}

NodePrivate::NodePrivate(const string& docName,
                         STRUCT_ID structId,
                         NODE_ID nodeId,
                         CONTENT_ID contentId,
                         int32_t indexid,
                         string uri,
                         string nodeMedia,
                         string nodeType,
                         string descrId,
                         TOPO_POS nodeStart,
                         TOPO_POS nodeEnd,
                         TOPO_POS nodeLength,
                         NODE_ID nodeParent )
{
  m_docName = docName;
  m_structId = structId;
  m_contentId = contentId;
  m_nodeId = nodeId;
  m_uri = uri;
  m_nodeMedia = nodeMedia;
  m_nodeType = nodeType;
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
  if (this == &n)
    return *this;

  if (m_d != 0) {
    delete m_d;
  }
  m_d = new NodePrivate(*n.m_d);
  return *this;
}


Node::Node (const std::string& docName,
            STRUCT_ID structId,
            NODE_ID nodeId,
            CONTENT_ID contentId,
            int indexid ,
            std::string uri,
            std::string nodeMedia,
            std::string nodeType,
            std::string descrId ,
            TOPO_POS nodeStart,
            TOPO_POS nodeEnd,
            TOPO_POS nodeLength,
            NODE_ID nodeParent ) :
    m_d(new NodePrivate(docName,
                        structId,
                        nodeId,
                        contentId,
                        indexid ,
                        uri,
                        nodeMedia,
                        nodeType,
                        descrId ,
                        nodeStart,
                        nodeEnd,
                        nodeLength,
                        nodeParent))
{
}

std::string Node::get_DocName() const
{
  return m_d->m_docName;
};
void  Node::set_DocName ( const std::string& docName )
{
  m_d->m_docName=docName;
}
STRUCT_ID Node::get_StructId() const
{
  return m_d->m_structId;
};
void  Node::set_StructId ( STRUCT_ID structId )
{
  m_d->m_structId=structId;
}
NODE_ID Node::get_NodeId() const
{
  return m_d->m_nodeId;
};
void  Node::set_NodeId ( NODE_ID NodeId )
{
  m_d->m_nodeId=NodeId;
}
CONTENT_ID Node::get_ContentId() const
{
  return m_d->m_contentId;
};
void  Node::set_ContentId ( CONTENT_ID ContentId )
{
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
  return m_d->m_nodeParent;
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

StructurePrivate::StructurePrivate() :
    m_nodes(),
    m_structId(std::numeric_limits< uint64_t >::max())
{
}

StructurePrivate::StructurePrivate(const StructurePrivate& sp) :
    m_nodes(sp.m_nodes),
    m_structId(sp.m_structId)
{
}

StructurePrivate& StructurePrivate::operator=(const StructurePrivate& sp)
{
  m_nodes = sp.m_nodes;
  m_structId = sp.m_structId;
  return *this;
}

StructurePrivate::StructurePrivate ( STRUCT_ID structID ) :
    m_nodes(),
    m_structId(structID)
{
}

//! @brief destructeur: delete des 'Nodes' de contenu
StructurePrivate::~StructurePrivate()
{
}

///////////////// Structure methods
Structure::Structure() : m_d(new StructurePrivate())
{
}

Structure::Structure ( STRUCT_ID structID ) :
    m_d(new StructurePrivate(structID))
{
}

Structure::Structure ( const Structure& s ) :
    m_d(new StructurePrivate(*s.m_d))
{
}

Structure& Structure::operator=( const Structure& s )
{
  if (this == &s)
    return *this;

  if (m_d!=0)
  {
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
  m_d->m_nodes[node.get_ContentId()] = node;
}

void Structure::addNode ( CONTENT_ID ContentId, const Node& node )
{
    m_d->m_nodes[ContentId] = node;
}

Node* Structure::getNode ( CONTENT_ID ContentId )
{
  if (m_d->m_nodes.find(ContentId) == m_d->m_nodes.end())
    return nullptr;
  return &m_d->m_nodes[ContentId];
}

const Node* Structure::getNode ( CONTENT_ID ContentId ) const
{
  if (m_d->m_nodes.find(ContentId) == m_d->m_nodes.end())
    return nullptr;
  return &m_d->m_nodes[ContentId];
}

const map<CONTENT_ID,Node>& Structure::getNodes() const
{
  return m_d->m_nodes;
}

map<CONTENT_ID,Node>& Structure::getNodes()
{
  return m_d->m_nodes;
}

STRUCT_ID Structure::getStructId() const
{
    return m_d->m_structId;
}

Node*Structure::getFirstNode ( STRUCT_ID StructureId )
{
  LIMA_UNUSED(StructureId);
  for (auto ItrNodes=m_d->m_nodes.begin();
       ItrNodes!=m_d->m_nodes.end(); ItrNodes++)
  {
    if ((ItrNodes->second.nodeType() == "topic"))
    {
      return getNode(ItrNodes->first);
    }
  }
  return 0;
}

const Node* Structure::getFirstNode ( STRUCT_ID StructureId ) const
{
  LIMA_UNUSED(StructureId);
  for (auto nodesIt=m_d->m_nodes.begin(); nodesIt!=m_d->m_nodes.end();nodesIt++)
  {
    if ((nodesIt->second.nodeType() == "topic"))
    {
      return getNode(nodesIt->first);
    }
  }
  return 0;
}

std::ostream& operator<<(ostream& os, const Node& node)
{
  os << "Node " << node.get_StructId() << " " << node.get_NodeId() << " "
      << node.get_ContentId() << " " << node.indexId() << " " << node.descrId()
      << " " << node.get_DocName() << std::endl;
  return os;
}

QDebug& operator<<(QDebug& os, const Node& node)
{
  os << "Node(structId:" << node.get_StructId()
     << ", nodeId:" << node.get_NodeId()
     << ", contentId:" << node.get_ContentId()
     << ", indexId:" << node.indexId()
     << ", descrId:" << node.descrId()
     << ", docName:" << node.get_DocName()
     << ", nodeStart:" << node.nodeStart()
     << ", nodeEnd:" << node.nodeEnd()
     << ", nodeLength:" << node.nodeLength()
     << ", nodeParent:" << node.nodeParent() << ")";
  return os;
}

std::ostream& operator<<(ostream& os, const Structure& structure)
{
  const auto& nodes = structure.getNodes();
  for (auto ItrNodes = nodes.cbegin(); ItrNodes != nodes.cend(); ItrNodes++)
  {
    os << ItrNodes->second;
  }
  return os;
}

QDebug& operator<<(QDebug& os, const Structure& structure)
{
  const auto& nodes = structure.getNodes();
  os << "Structure( structId:" << structure.getStructId()
     << ", nodes ("<<nodes.size()<<"): ";
  for (auto ItrNodes = nodes.cbegin(); ItrNodes != nodes.cend() ; ItrNodes++)
  {
    os << "node (" << ItrNodes->first << ":" << ItrNodes->second<<")";
  }
  os << " )";
  return os;
}

void Structure::setStructId(STRUCT_ID structId)
{
  m_d->m_structId = structId;
}

} // Lima
