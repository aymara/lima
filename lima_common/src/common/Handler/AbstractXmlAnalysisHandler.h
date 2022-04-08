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
#ifndef ABSTRACTXMLANALYSISHANDLER_H
#define ABSTRACTXMLANALYSISHANDLER_H

#include <sstream>
#include "common/Handler/shiftFrom.h"
#include "common/Handler/AbstractAnalysisHandler.h"
#include "common/Data/genericDocumentProperties.h"

namespace Lima {

/**
 * The AbstractAnalysisHandler class defines the callback interface:
 * the functions that are to be defined to handle the events generated
 * by the LIMA linguistic analyzer.
 *
 * The interface includes functions that are used only in the analysis
 * of structured input
 */
class AbstractXmlAnalysisHandler : public AbstractAnalysisHandler
{
public:
  AbstractXmlAnalysisHandler(std::shared_ptr<const ShiftFrom> shiftFrom) :
      m_shiftFrom(shiftFrom)
  {}

  virtual ~AbstractXmlAnalysisHandler() {}

  /** notify the start of a new document, and give its properties */
  virtual void startDocument(const Common::Misc::GenericDocumentProperties& props) = 0;

  /** notify the end of the document */
  virtual void endDocument() = 0;

  /** gives content. Content is a serialized form of the expected resultType */
  virtual void handle(const char* buf,int length) override = 0;

  /** notify the start of a new hierarchyNode */
  virtual void startNode( const std::string& elementName, bool forIndexing ) = 0;

  /** notify the end of a hierarchyNode */
  virtual void endNode( const Common::Misc::GenericDocumentProperties& props ) = 0;

  /** set the output stream. Implementations can be empty if the handler should not write its output to a stream */
  virtual void setOut( std::ostream* out ) = 0;

  std::shared_ptr<const ShiftFrom> shiftFrom() const {return m_shiftFrom;}

private:
  std::shared_ptr<const ShiftFrom> m_shiftFrom;
};

} // Lima

#endif
