// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/** **********************************************************************
 *
 * @file       DataTypes.h
 * @author     Bertrand Delezoide <bertrand.delezoide@cea.fr>

 *             Hervé Le Borgne <herve.le-borgne@cea.fr>
 * @date       fev 2008
 * copyright   Copyright (C) 2003-2012 by CEA LIST
 * Project     mm_common
 *
 * @brief      Common Types
 *
 ***********************************************************************/
#ifndef DATATYPES_H
#define DATATYPES_H

#include "common/LimaCommon.h"
// #include <../amose_imageprocessing/src/imageProcessing/core/PIRIA/tiff.h>

#include <sstream>
#include <iostream>
#include <map>
#include <vector>

namespace Lima {

    /**
     * Identifier of a structure in the structure index
     */
    typedef uint64_t STRUCT_ID;

     /**
     * Identifier of a node in a document
     */
    typedef uint64_t NODE_ID;

     /**
     * Identifier of an feature content
     */
    typedef uint64_t CONTENT_ID;

    /**
    * Topology information
    */
    typedef uint64_t TOPO_POS;


class LIMA_DATA_EXPORT NodePrivate;
/**
 * @brief noeud de contenu au sein d'une structure.
 * Un noeud de contenu est caractérisé par trois identifiants au sein de la
 * structure (structId,nodeId,contentId) ainsi que par le type de contenu (descrId).
 * Il existe aussi un identifiant au sein de la base de contenu correspondante (contentId
 * dans la base SQL) qui n'est pas dans la classe Node
 */
class LIMA_DATA_EXPORT Node
{
public:
    //! @brief default constructor
    Node();

    //! @brief default destructor
    virtual ~Node();

    //! @brief copy constructor
    Node(const Node &n);
    Node &operator=(const Node &n);

    //! @brief main constructor
    //! @param docName
    //! @param structId
    //! @param nodeId
    //! @param contentId
    //! @param indexid
    //! @param uri
    //! @param nodeMedia
    //! @param nodeType
    //! @param descrId
    //! @param nodeStart
    //! @param nodeEnd
    //! @param nodeLength
    //! @param nodeParent
    Node(const std::string &docName,
         STRUCT_ID structId,
         NODE_ID nodeId,
         CONTENT_ID contentId,
         int indexid ,
         std::string uri,
         std::string nodeMedia,
         std::string nodeType,
         std::string descrId,
         TOPO_POS nodeStart,
         TOPO_POS nodeEnd,
         TOPO_POS nodeLength,
         NODE_ID nodeParent);
    std::string get_DocName() const;
    void  set_DocName ( const std::string& docName );
    STRUCT_ID get_StructId() const;
    void  set_StructId ( STRUCT_ID structId );
    NODE_ID get_NodeId() const;
    void  set_NodeId ( NODE_ID NodeId );
    CONTENT_ID get_ContentId() const;
    void  set_ContentId ( CONTENT_ID ContentId );
    const std::string& nodeType() const;
    int32_t indexId() const;
    void indexId(int32_t id);
    const std::string& descrId() const;

    TOPO_POS nodeStart() const;
    void nodeStart(TOPO_POS ns);
    TOPO_POS nodeEnd() const;
    void nodeEnd(TOPO_POS ns);
    TOPO_POS nodeLength() const;
    void nodeLength(TOPO_POS ns);
    NODE_ID nodeParent() const;
    void nodeParent(NODE_ID np);

    const std::string& uri() const;
    const std::string& nodeMedia() const;

private:
  NodePrivate* m_d;
};

//! @brief structure d'un document multimedia ou d'une collection.
//! Un document multimedia complexe (e.g une vidéo, un doc XML) ou une
//! collection de documents mono/multimedia (qui est alors représenté
//! dans un fichier XML) a sa structure topologique représentéee par
//! cette classe. Il s'agit d'un arbre dont chaque noeud représente
//! soit un document (décrit lui-même par un ou plusieurs contenus)
//! soit un ensemble de documents (eux-même structures).
class LIMA_DATA_EXPORT StructurePrivate;
class LIMA_DATA_EXPORT Structure
{
public:
    Structure();
    Structure(const Structure& s);
    Structure ( STRUCT_ID structID );
    Structure& operator=( const Structure& s );

    //! @brief destructeur: delete des 'Nodes' de contenu
    ~Structure();

    void addNode (const Node& node );

    void addNode ( CONTENT_ID ContentId, const Node& node );

    //! @brief get a content node, given its identifiers
    //! @param ContentId content identifier (different for each type)
    //! @return a content node
    Node* getNode ( CONTENT_ID ContentId );
    const Node* getNode ( CONTENT_ID ContentId ) const;

    //! @brief get a content node, given its identifiers
    //! @return a content node
    Node* getFirstNode ( STRUCT_ID /*unused*/ );
    const Node* getFirstNode ( STRUCT_ID /*unused*/ ) const;

    //! @brief get all nodes of the current structure
    //! @return the set of all nodes of the current structure
    const std::map<CONTENT_ID, Node>& getNodes() const;
    std::map<CONTENT_ID, Node>& getNodes();

    STRUCT_ID getStructId() const;

    void setStructId(STRUCT_ID structId);

private:
    StructurePrivate* m_d;
};

LIMA_DATA_EXPORT  std::ostream& operator<<(std::ostream &os, const Node& node);
LIMA_DATA_EXPORT  QDebug& operator<<(QDebug& os, const Node& node);
LIMA_DATA_EXPORT  std::ostream& operator<<(std::ostream &os, const Structure& structure);
LIMA_DATA_EXPORT  QDebug& operator<<(QDebug& os, const Structure& structure);

} // namespace Lima

#endif
