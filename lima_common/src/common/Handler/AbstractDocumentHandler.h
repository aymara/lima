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
