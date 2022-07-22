// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
