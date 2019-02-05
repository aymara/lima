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
#ifndef LIMA_LINGUISTICPROCESSING_EVENTHANDLER_H
#define LIMA_LINGUISTICPROCESSING_EVENTHANDLER_H

#include "EventAnalysisExport.h"
#include "common/Handler/AbstractAnalysisHandler.h"
#include "Events.h"
#include "linguisticProcessing/client/AnalysisHandlers/SimpleStreamHandler.h"
#include "linguisticProcessing/client/AnalysisHandlers/AbstractTextualAnalysisHandler.h"

#include <ostream>

namespace Lima
{

namespace LinguisticProcessing
{

namespace EventAnalysis
{

/**
@author Faiza GARA
*/
class LIMA_EVENTANALISYS_EXPORT EventHandler : public Lima::AbstractTextualAnalysisHandler
{
  Q_OBJECT
public:
  EventHandler(std::ostream* out);

  EventHandler();
  
  virtual ~EventHandler();

  /** notify the start of a new document */
  void startDocument(const Common::Misc::GenericDocumentProperties& props) override;

  /** notify the end of the document */
  void endDocument() override;
  
  void endAnalysis() override;
  
  void startAnalysis() override;

  Events& getEvents();


  /** gives content. Content is a serialized form of the expected resultType */
  void handle(const char* buf,int length) override ;
  
  /** notify the start of a new hierarchyNode */
  void startNode( const std::string& elementName, bool forIndexing ) override;
  
  /** notify the end of a hierarchyNode */
  void endNode( const Common::Misc::GenericDocumentProperties& props ) override;
  
  /** set the output stream. */
  virtual void setOut( std::ostream* out ) override;
  
private:
  std::ostringstream* m_stream;
  std::ostream* m_out;
  Lima::LinguisticProcessing::SimpleStreamHandler* m_writer;
  Events m_events;

};

}  // closing namespace EventAnalysis

} // closing namespace LinguisticProcessing

} // closing namespace Lima

#endif //LIMA_LINGUISTICPROCESSING_EVENTHANDLER_H
