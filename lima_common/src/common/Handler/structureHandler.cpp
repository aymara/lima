// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                       *
 *                                                                         *
 ***************************************************************************/
#include "structureHandler.h"

using namespace Lima;

namespace Lima {

class StructureHandlerPrivate
{
  friend class StructureHandler;

  StructureHandlerPrivate();
  StructureHandlerPrivate(const StructureHandlerPrivate& shp);
  virtual ~StructureHandlerPrivate();

  Structure m_structures;
  STRUCT_ID m_lastStructureId;
  CONTENT_ID m_lastContentId;
};

StructureHandlerPrivate::StructureHandlerPrivate(): m_structures(), m_lastStructureId(0), m_lastContentId(0)
{
}

StructureHandlerPrivate::StructureHandlerPrivate(const StructureHandlerPrivate& shp):
    m_structures(shp.m_structures),
    m_lastStructureId(shp.m_lastStructureId),
    m_lastContentId(shp.m_lastContentId)
{
}

StructureHandlerPrivate::~StructureHandlerPrivate()
{
}

StructureHandler::StructureHandler(): m_d(new StructureHandlerPrivate())
{
};

StructureHandler::StructureHandler(const StructureHandler& sh): m_d(new StructureHandlerPrivate(*sh.m_d))
{
};

StructureHandler::~StructureHandler()
{
  delete m_d;
}

//! @brief Insérer une structure dans le handler
void StructureHandler::set_Structures(Structure* structures)
{
    m_d->m_structures=*structures;
}

//! @brief Récupérer toutes les structures du handler
Structure* StructureHandler::get_Structures()
{
    return &m_d->m_structures;
}

//! @brief Récupérer une structure du handler
Node* StructureHandler::get_Node(CONTENT_ID CID)
{
    return m_d->m_structures.getNode (CID);
}

void StructureHandler::add_Node(const Node& node)
{
    m_d->m_structures.addNode (node);
}

//! @brief Accesseur de lastStructureId
STRUCT_ID StructureHandler::get_LastStructureId()
{
    return m_d->m_lastStructureId;
}
void StructureHandler::set_LastStructureId(STRUCT_ID SID)
{
    m_d->m_lastStructureId=SID;
}
CONTENT_ID StructureHandler::get_LastContentId()
{
    return m_d->m_lastContentId;
}
void StructureHandler::set_LastContentId(CONTENT_ID CID)
{
    m_d->m_lastContentId = CID;
}

void StructureHandler::resetStructure()
{
    m_d->m_lastStructureId = 0;
    m_d->m_lastContentId = 0;
    m_d->m_structures.getNodes().erase(m_d->m_structures.getNodes().begin(),
                                       m_d->m_structures.getNodes().end());
}

}
