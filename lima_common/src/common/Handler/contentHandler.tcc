// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                       *
 *                                                                         *
 ***************************************************************************/
#ifndef CONTENTHANDLER_TCC
#define CONTENTHANDLER_TCC

#include "contentHandler.h"
#include <map>

namespace Lima {

//! @brief Ajouter un contenu au handler
template <typename Content> void ContentHandler<Content>::addContent (CONTENT_ID ContentId, Content datas)
{
    if (m_contents.find(ContentId)!=m_contents.end())
        m_contents[ContentId]=datas;
    else
        m_contents.insert(std::make_pair(ContentId,datas));
}

template <typename Content> void ContentHandler<Content>::setContents ( const std::map < CONTENT_ID , Content >  contents)
{
    m_contents = contents;
}

//! @brief Récupérer un contenu du handler
template <typename Content> Content* ContentHandler<Content>::getContent ( CONTENT_ID ContentId )
{
    if (m_contents.find(ContentId)!=m_contents.end())
        return &m_contents[ContentId];
    else
        return NULL;
}

//! @brief Récupérer tous les contenus du handler
template <typename Content> 
const std::map < CONTENT_ID , Content >& ContentHandler<Content>::getContents() const
{
    return m_contents;
}


//! @brief Effacer les contenus du handler
template <typename Content> void ContentHandler<Content>::resetHandler()
{
    m_contents.erase (m_contents.begin(),m_contents.end());
}

}

#endif
