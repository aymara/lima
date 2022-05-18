// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
            Manager* manager) override;

  LimaStatusCode process(AnalysisContent& analysis) const override;
  
 private:
  MediaId m_language;
  std::string m_graph;
  QXmlSimpleReader* m_parser;   /*< xerces XML parser for the loader*/

  // xerces XML handler
  class XMLHandler : public QXmlDefaultHandler
  {
  public:
    XMLHandler(MediaId language, AnalysisContent& analysis, 
               LinguisticAnalysisStructure::AnalysisGraph* graph);
    virtual ~XMLHandler();
    
    // -----------------------------------------------------------------------
    //  Implementations of the SAX DocumentHandler interface
    // -----------------------------------------------------------------------
    bool endElement(const QString & namespaceURI, 
                    const QString & name, 
                    const QString & qName) override;
    bool characters(const QString& chars) override;
    bool startElement(const QString & namespaceURI, 
                      const QString & name, 
                      const QString & qName, 
                      const QXmlAttributes & attributes) override;
    
    // -----------------------------------------------------------------------
    //  Implementations of the SAX ErrorHandler interface
    // -----------------------------------------------------------------------
    bool warning(const QXmlParseException & exception) override;
    bool error(const QXmlParseException & exception) override;
    bool fatalError(const QXmlParseException & exception) override;
    
    
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
