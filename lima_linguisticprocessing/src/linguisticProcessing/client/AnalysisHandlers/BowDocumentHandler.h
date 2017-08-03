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
#ifndef LIMA_LINGUISTICPROCESSINGBOWDOCUMENTHANDLER_H
#define LIMA_LINGUISTICPROCESSINGBOWDOCUMENTHANDLER_H

#include "AnalysisHandlersExport.h"
#include "SBowDocumentWriter.h"
#include "linguisticProcessing/common/BagOfWords/bowDocument.h"

#include <sstream>

namespace Lima
{

namespace LinguisticProcessing
{

/**
@author Benoit Mathieu
*/
class LIMA_ANALYSISHANDLERS_EXPORT BowDocumentHandler : public AbstractTextualAnalysisHandler
{
  Q_OBJECT
public:
  BowDocumentHandler();

  virtual ~BowDocumentHandler();

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

  std::string getSBoWContent() const;

  const std::vector<Common::BagOfWords::BoWDocument>& getBoWDocuments() const
    { return m_bowdocs; }
  
  /** notify the start of a new hierarchyNode */
  void startNode( const std::string& elementName, bool forIndexing );
  
  /** notify the end of a hierarchyNode */
  void endNode( const Common::Misc::GenericDocumentProperties& props );

  /** set the output stream. Implementations can be empty if the handler should not write its output to a stream */
  // TODO a implémenter (car hérite de AbstractTextualAnalysisHandler)
  virtual void setOut( std::ostream*  out ){LIMA_UNUSED(out);std::cerr << "BowDocumentHandler::setOut NOT IMPLEMENTED!!!"<<std::endl;};

  
private:


  std::vector<Common::BagOfWords::BoWDocument> m_bowdocs;
  std::ostringstream* m_bowstream;
  SBowDocumentWriter* m_writer;

};


}

}

#endif
