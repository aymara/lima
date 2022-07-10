// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_LINGUISTICPROCESSINGSBOWDOCUMENTWRITER_H
#define LIMA_LINGUISTICPROCESSINGSBOWDOCUMENTWRITER_H

#include "AnalysisHandlersExport.h"
#include "linguisticProcessing/client/AnalysisHandlers/AbstractTextualAnalysisHandler.h"
#include <common/Data/genericDocumentProperties.h>

#include <iostream>

namespace Lima
{

namespace LinguisticProcessing
{
#define LPCLIENTSBOWHANDLERLOGINIT LOGINIT("LP::SBoWHandler");

/**
@author Benoit Mathieu
*/
class LIMA_ANALYSISHANDLERS_EXPORT SBowDocumentWriter : public AbstractTextualAnalysisHandler
{
  Q_OBJECT
public:
  SBowDocumentWriter();

  virtual ~SBowDocumentWriter();

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
  
  /** notify the start of a new hierarchyNode */
  void startNode( const std::string& elementName, bool forIndexing ) override;
  
  /** notify the end of a hierarchyNode */
  void endNode( const Common::Misc::GenericDocumentProperties& props ) override;

  /** set the output stream. Implementations can be empty if the handler should not write its output to a stream */
  virtual void setOut( std::ostream* out ) override;
  
private:
  std::ostream* m_out;

};

}

}

#endif
