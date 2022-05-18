// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2009-2012 by CEA LIST                      *
 * @brief                                                                         *
 ***************************************************************************/
/**
 * @file               AbstractDocumentHandler.h
 * @date               begin Mar, 2009
 * @author             <herve.le-borgne@cea.fr> 

 *                     Copyright (c) 2009-2012 by CEA LIST
 * @version            $Id: AbstractDocumentHandler.h
 * @todo
 */
#ifndef ABSTRACTDOCUMENTHANDLER_H
#define ABSTRACTDOCUMENTHANDLER_H

#include <string>
#include "common/Data/genericDocumentProperties.h"

namespace Lima{


//! @brief Callback to handle multimedia documents
class AbstractDocumentHandler
{
    public:
        //! @brief destructor
        virtual ~AbstractDocumentHandler(){};

  virtual void writeDocumentsHeader(){};
  virtual void writeDocumentsFooter(){};
  
  virtual void openSNode(const Lima::Common::Misc::GenericDocumentProperties* properties,
      const std::string& elementName) = 0;
  virtual void openSIndexingNode(const Lima::Common::Misc::GenericDocumentProperties* properties,
      const std::string& elementName) = 0;
  virtual void processProperties(const Lima::Common::Misc::GenericDocumentProperties* properties, bool useIterator, bool useIndexIterator) = 0;
  virtual void closeSNode() = 0;  
  virtual  void processSContent( const Lima::Common::Misc::GenericDocumentProperties* /*properties*/ ){};
  virtual  void closeSContent(){};
};

} // namespace Lima

#endif
