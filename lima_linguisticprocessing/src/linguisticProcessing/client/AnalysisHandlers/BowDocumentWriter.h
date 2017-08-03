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
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_LINGUISTICPROCESSINGBOWDOCUMENTWRITER_H
#define LIMA_LINGUISTICPROCESSINGBOWDOCUMENTWRITER_H

#include "AnalysisHandlersExport.h"
#include "linguisticProcessing/client/AnalysisHandlers/AbstractTextualAnalysisHandler.h"
#include <common/Data/genericDocumentProperties.h>

#include <iostream>

namespace Lima
{

namespace LinguisticProcessing
{


/**
@author Benoit Mathieu
*/
class LIMA_ANALYSISHANDLERS_EXPORT BowDocumentWriter : public AbstractTextualAnalysisHandler
{
  Q_OBJECT
public:
  BowDocumentWriter(std::ostream* out);

  virtual ~BowDocumentWriter();

  /** notify the start of a new document */
  void startDocument(const Common::Misc::GenericDocumentProperties& props);

  /** notify the end of the document */
  void endDocument();

    /** notify the start of an analysis content */
  void startAnalysis();

  /** notify the end of an analysis content */
  void endAnalysis();
  
  /** gives content. Content is a serialized form of the expected resultType */
  void handle(const char* buf,int length); 
  
  /** notify the start of a new hierarchyNode */
  void startNode( const std::string& elementName, bool forIndexing );
  
  /** notify the end of a hierarchyNode */
  void endNode( const Common::Misc::GenericDocumentProperties& props );

private:
  std::ostream* m_out;

};

}

}

#endif
