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
 * @file       SegmentationResultsLoader.h
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Mon Jan 17 2011
 * copyright   Copyright (C) 2011 by CEA LIST (LVIC)
 * Project     MM
 * 
 * @brief a class to load results from text segmentation
 * 
 * 
 ***********************************************************************/
#ifndef LIMA_LINGUISTICPROCESSING_SEGMENTATIONRESULTSLOADER_H
#define LIMA_LINGUISTICPROCESSING_SEGMENTATIONRESULTSLOADER_H

#include "TextSegmentationExport.h"
#include "SegmentationData.h"
#include "linguisticProcessing/core/LinguisticProcessors/AnalysisLoader.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"

#include <QtXml/QXmlDefaultHandler>
#include <iostream>
#include <fstream>

namespace Lima {
namespace LinguisticProcessing {

#define SEGMENTATIONRESULTSLOADER_CLASSID "SegmentationResultsLoader"

/*
 * @brief this is the abstract class for analysis loaders, that read
 * informations from external files to insert them in the analysis
 * data
 */
class LIMA_TEXTSEGMENTATION_EXPORT SegmentationResultsLoader : public AnalysisLoader
{
public:
  SegmentationResultsLoader();

  virtual ~SegmentationResultsLoader();

  void init(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
            Manager* manager)
    ;

  LimaStatusCode process(AnalysisContent& analysis) const;

private:
  std::string m_graph;
  std::string m_dataName;
  QXmlSimpleReader* m_parser;   /*< xerces XML parser for the loader*/

  // xerces XML handler
  class LIMA_TEXTSEGMENTATION_EXPORT XMLHandler : public QXmlDefaultHandler
  {
  public:
    XMLHandler(SegmentationData* s, LinguisticAnalysisStructure::AnalysisGraph* graph);
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
    LinguisticAnalysisStructure::AnalysisGraph* m_graph;
    SegmentationData* m_data;
    uint64_t m_position;
    uint64_t m_length;
    std::string m_type;
    std::string m_currentElement;

    // private member functions
    std::string toString(const LimaString& xercesString);
  };
};

} // end namespace
} // end namespace

#endif
