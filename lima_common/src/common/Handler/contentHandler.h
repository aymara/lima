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
    std::map < CONTENT_ID , Content > getContents() const ;

    //! @brief Effacer les contenus du handler
    void resetHandler();

private:
    std::map < CONTENT_ID , Content > m_contents;

};
}
#include "common/Handler/contentHandler.tcc"


#endif
