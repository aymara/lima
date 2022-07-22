// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
  void startDocument(const Common::Misc::GenericDocumentProperties& props) override;

  /** notify the end of the document */
  void endDocument() override;

    /** notify the start of an analysis content */
  void startAnalysis() override;

  /** notify the end of an analysis content */
  void endAnalysis() override;
  
  /** gives content. Content is a serialized form of the expected resultType */
  void handle(const char* buf,int length) override; 

  std::string getSBoWContent() const;

  const std::vector<Common::BagOfWords::BoWDocument>& getBoWDocuments() const
    { return m_bowdocs; }
  
  /** notify the start of a new hierarchyNode */
  void startNode( const std::string& elementName, bool forIndexing ) override;
  
  /** notify the end of a hierarchyNode */
  void endNode( const Common::Misc::GenericDocumentProperties& props ) override;

  /** set the output stream. Implementations can be empty if the handler should not write its output to a stream */
  // TODO a implémenter (car hérite de AbstractTextualAnalysisHandler)
  virtual void setOut( std::ostream*  out ) override
  {
    LIMA_UNUSED(out);
    std::cerr << "BowDocumentHandler::setOut NOT IMPLEMENTED!!!"<<std::endl;
  };

  
private:


  std::vector<Common::BagOfWords::BoWDocument> m_bowdocs;
  std::ostringstream* m_bowstream;
  SBowDocumentWriter* m_writer;

};


}

}

#endif
