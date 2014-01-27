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
 *   Copyright (C) 2009-2012 by CEA LIST                      *
 *                                                                         *
 ***************************************************************************/
#ifndef STRUCTUREHANDLER_H
#define STRUCTUREHANDLER_H

#include "common/Data/DataTypes.h"
#include "common/LimaCommon.h" 

namespace Lima {

class StructureHandlerPrivate;
class LIMA_DATAHANDLER_EXPORT StructureHandler
{
public:
  StructureHandler();
  virtual ~StructureHandler();
    
  //! @brief Récupérer toutes les structures du handler
  Structure* get_Structures();

  void set_Structures(Structure* structures);

  //! @brief Récupérer un noeud du handler
  Node* get_Node(CONTENT_ID);
  void add_Node(const Node& node);

  //! @brief Accesseur de lastStructureId
  STRUCT_ID get_LastStructureId();
  void set_LastStructureId(STRUCT_ID);
  CONTENT_ID get_LastContentId();
  void set_LastContentId(CONTENT_ID);

  void resetStructure();

private:
  StructureHandler(const StructureHandler& sh);
  StructureHandler& operator=(const StructureHandler& sh);
  StructureHandlerPrivate* m_d;
};

}


#endif
