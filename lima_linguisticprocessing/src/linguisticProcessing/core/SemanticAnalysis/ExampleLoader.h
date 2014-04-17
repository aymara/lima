/*
    Copyright 2002-2014 CEA LIST

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
/************************************************************************
 *
 * @file       ExampleLoader.h
 * @author     Clémence Filmont <clemence.filmont@cea.fr>
 * @date       2014-04-17
 * copyright   Copyright (C) 2014 by CEA LIST
 * Project     mm_linguisticprocessing
 *
 * @brief      an example Loader class 
 *
 *
 ***********************************************************************/

#ifndef EXAMPLELOADER_H
#define EXAMPLELOADER_H

#include "linguisticProcessing/core/LinguisticProcessors/AnalysisLoader.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include <QtXml/QXmlSimpleReader>
#include <QtXml/QXmlDefaultHandler>
#include <iostream>
#include <fstream>

namespace Lima {
namespace LinguisticProcessing {
namespace Example {

#define EXAMPLELOADER_CLASSID "ExampleLoader"

class ExampleLoader : public AnalysisLoader
{
 public:
  ExampleLoader(); 
  virtual ~ExampleLoader();

  void init(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
            Manager* manager);

  LimaStatusCode process(AnalysisContent& analysis) const;
  
 private:
  MediaId m_language;
  std::string m_graph;
  QXmlSimpleReader* m_parser;   /*< XML parser for the loader*/
  
  // XML handler
  class XMLHandler : public QXmlDefaultHandler
  {
  public:
    QMap<uint64_t,LinguisticCode> m_tagIndex;

    XMLHandler(MediaId language, AnalysisContent& analysis, LinguisticAnalysisStructure::AnalysisGraph* graph);
    virtual ~XMLHandler();
    
    // -----------------------------------------------------------------------
    //  Implementations of the SAX DocumentHandler interface
    // -----------------------------------------------------------------------
    bool startElement(const QString & namespaceURI, const QString & name, const QString & qName, const QXmlAttributes & attributes);
    
    // -----------------------------------------------------------------------
    //  Implementations of the SAX ErrorHandler interface
    // -----------------------------------------------------------------------
    bool warning(const QXmlParseException & exception);
    bool error(const QXmlParseException & exception);
    bool fatalError(const QXmlParseException & exception);
    
    
  private:
    MediaId m_language;
    AnalysisContent& m_analysis;
    LinguisticAnalysisStructure::AnalysisGraph* m_graph;
    uint64_t m_position;
    uint64_t m_length;
    std::string m_type;
    std::string m_string;
    std::string m_currentElement;

  };
  
};

} // end namespace
} // end namespace
} // end namespace

#endif
