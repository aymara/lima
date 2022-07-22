// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_LINGUISTICPROCESSING_SIMPLESTREAMHANDLER_H
#define LIMA_LINGUISTICPROCESSING_SIMPLESTREAMHANDLER_H

#include "AnalysisHandlersExport.h"
#include "linguisticProcessing/client/AnalysisHandlers/AbstractTextualAnalysisHandler.h"

#include <ostream>

namespace Lima
{
namespace Common {
  namespace Misc {
    class GenericDocumentProperties;
  }
}
namespace LinguisticProcessing
{
#define SIMPLESTREAMHANDLER_CLASSID "SimpleStreamHandler"

class SimpleStreamHandlerPrivate;
/**
@author Gael de Chalendar
*/
class LIMA_ANALYSISHANDLERS_EXPORT SimpleStreamHandler : public AbstractTextualAnalysisHandler
{
  Q_OBJECT
public:
  SimpleStreamHandler();

  virtual ~SimpleStreamHandler();

  /** notify the start of a new document */
  void startDocument(const Common::Misc::GenericDocumentProperties& props) override;

  /** notify the end of the document */
  void endDocument() override;
  
  void endAnalysis() override;
  
  void startAnalysis() override;

  /** gives content. Content is a serialized form of the expected resultType */
  void handle(const char* buf,int length) override;
  
  /** notify the start of a new hierarchyNode */
  void startNode( const std::string& elementName, bool forIndexing ) override;
  
  /** notify the end of a hierarchyNode */
  void endNode( const Common::Misc::GenericDocumentProperties& props ) override;

  void setOut(std::ostream* out) override;
  
private:
  SimpleStreamHandler(const SimpleStreamHandler& ssh);
  SimpleStreamHandler& operator=(const SimpleStreamHandler& ssh);
  SimpleStreamHandlerPrivate* m_d;
};

} // closing namespace LinguisticProcessing 

} // closing namespace Lima

#endif // LIMA_LINGUISTICPROCESSING_SIMPLESTREAMHANDLER_H
