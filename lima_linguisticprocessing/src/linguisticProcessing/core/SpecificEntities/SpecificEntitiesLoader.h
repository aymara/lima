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
/************************************************************************
 *
 * @file       SpecificEntitiesLoader.h
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Thu Jun 16 2011
 * copyright   Copyright (C) 2011 by CEA LIST
 * Project     mm_linguisticprocessing
 * 
 * @brief      a Loader class to load external entity annotations
 * 
 * 
 ***********************************************************************/

#ifndef SPECIFICENTITIESLOADER_H
#define SPECIFICENTITIESLOADER_H

#include "SpecificEntitiesExport.h"
#include "linguisticProcessing/core/LinguisticProcessors/AnalysisLoader.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include <QtXml/QXmlSimpleReader>
#include <QtXml/QXmlDefaultHandler>
#include <iostream>
#include <fstream>

namespace Lima {
namespace LinguisticProcessing {
namespace SpecificEntities {

#define SPECIFICENTITIESLOADER_CLASSID "SpecificEntitiesLoader"

class LIMA_SPECIFICENTITIES_EXPORT SpecificEntitiesLoader : public AnalysisLoader
{
 public:
  SpecificEntitiesLoader(); 
  virtual ~SpecificEntitiesLoader();

  void init(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
            Manager* manager)
    ;

  LimaStatusCode process(AnalysisContent& analysis) const;
  
 private:
  MediaId m_language;
  std::string m_graph;
  QXmlSimpleReader* m_parser;   /*< xerces XML parser for the loader*/

  // xerces XML handler
  class XMLHandler : public QXmlDefaultHandler
  {
  public:
    XMLHandler(MediaId language, AnalysisContent& analysis, LinguisticAnalysisStructure::AnalysisGraph* graph);
    virtual ~XMLHandler();
    
    // -----------------------------------------------------------------------
    //  Implementations of the SAX DocumentHandler interface
    // -----------------------------------------------------------------------
    bool endElement(const QString & namespaceURI, const QString & name, const QString & qName);
    bool characters(const QString& chars);
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

    // private member functions
    std::string toString(const QString& xercesString);

    void addSpecificEntity(AnalysisContent& analysis,
                           LinguisticAnalysisStructure::AnalysisGraph* graph,
                           const std::string& str,
                           const std::string& type, 
                           uint64_t position, 
                           uint64_t length);
  };
  
};

} // end namespace
} // end namespace
} // end namespace

#endif
