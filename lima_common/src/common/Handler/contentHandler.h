// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                      *
 *                                                                         *
 ***************************************************************************/
#ifndef CONTENTHANDLER_H
#define CONTENTHANDLER_H

#include <map>
#include <vector>
#include "common/Data/DataTypes.h"
#include "common/LimaCommon.h" 

namespace Lima {

//! @brief manipulateur de contenu
template <typename Content> class ContentHandler
{
public:
    ContentHandler() {};
    virtual ~ContentHandler() {
        resetHandler();
    }

    //! @brief Ajouter un contenu au handler
    void addContent ( CONTENT_ID ContentId, Content datas);

    void setContents ( const std::map < CONTENT_ID , Content >  contents);

    //! @brief Récupérer un contenu du handler
    //! @param ContentId identifiant numerique du contenu à récupérer
    Content* getContent ( CONTENT_ID ContentId ) ;

    //! @brief Récupérer tous les contenus du handler
    const std::map < CONTENT_ID , Content >& getContents() const ;

    //! @brief Effacer les contenus du handler
    void resetHandler();

private:
    std::map < CONTENT_ID , Content > m_contents;

};
}
#include "common/Handler/contentHandler.tcc"


#endif
