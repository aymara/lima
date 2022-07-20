// Copyright 2002-2020 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_LINGUISTICPROCESSING_METADATAHANDLER_H
#define LIMA_LINGUISTICPROCESSING_METADATAHANDLER_H

#include "AnalysisHandlersExport.h"
#include "common/Handler/AbstractAnalysisHandler.h"
#include "linguisticProcessing/client/AnalysisHandlers/SimpleStreamHandler.h"
#include "linguisticProcessing/client/AnalysisHandlers/AbstractTextualAnalysisHandler.h"

#include <ostream>

namespace Lima
{
namespace LinguisticProcessing
{

class LIMA_ANALYSISHANDLERS_EXPORT MetaDataHandler : public AbstractTextualAnalysisHandler
{
  Q_OBJECT
public:
  MetaDataHandler(std::ostream* out);

  MetaDataHandler();
  virtual ~MetaDataHandler();


  virtual void endAnalysis() override;
  virtual void handle(const char* buf, int length) override ;
  virtual void startAnalysis() override;

  void startDocument(const Common::Misc::GenericDocumentProperties&) override;
  void endDocument() override;
  void startNode( const std::string& elementName, bool forIndexing ) override;
  void endNode(const Common::Misc::GenericDocumentProperties& props) override;
  
  virtual void setOut( std::ostream* out ) override;
  
  const std::map<std::string,std::string>& getMetaData();

private:
  std::ostringstream* m_stream;
  std::ostream* m_out;
  Lima::LinguisticProcessing::SimpleStreamHandler* m_writer;
  std::map<std::string,std::string> m_metadata;

};

} // closing namespace LinguisticProcessing
} // closing namespace Lima

#endif
