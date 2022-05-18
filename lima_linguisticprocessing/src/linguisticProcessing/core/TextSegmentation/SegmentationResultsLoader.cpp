// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       SegmentationResultsLoader.cpp
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Tue Jan 18 2011
 * copyright   Copyright (C) 2011 by CEA LIST
 *
 ***********************************************************************/

#include "SegmentationResultsLoader.h"

#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "common/Data/strwstrtools.h"

using namespace std;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

namespace Lima {
namespace LinguisticProcessing {

  SimpleFactory<MediaProcessUnit,SegmentationResultsLoader> SegmentationResultsLoaderFactory(SEGMENTATIONRESULTSLOADER_CLASSID);

//***********************************************************************
// constructors and destructors
SegmentationResultsLoader::SegmentationResultsLoader():
AnalysisLoader(),
m_graph("AnalysisGraph"),
m_dataName("segmentationData"),
m_parser(0)
{
}

SegmentationResultsLoader::~SegmentationResultsLoader() {
  if (m_parser!=0) {
    delete m_parser;
    m_parser=0;
  }
}

//***********************************************************************
void SegmentationResultsLoader::init(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
          Manager* manager)

{
  AnalysisLoader::init(unitConfiguration,manager);
  try {
    m_graph=unitConfiguration.getParamsValueAtKey("graph");
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {} // keep default value

  try {
    m_dataName=unitConfiguration.getParamsValueAtKey("data");
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {} // keep default value

  //  Create a SAX parser object.
  m_parser = new QXmlSimpleReader();

}

LimaStatusCode SegmentationResultsLoader::process(AnalysisContent& analysis) const
{
  // get analysis graph
  AnalysisGraph* graph=static_cast<AnalysisGraph*>(analysis.getData(m_graph));
  if (graph==0)
  {
    LOGINIT("LP::AnalysisLoader");
    LERROR << "no graph '" << m_graph << "' available !";
    return MISSING_DATA;
  }

  // get segmentation data or create new
  AnalysisData* data=analysis.getData(m_dataName);
  SegmentationData* segmData=0;
  if (data==0) {
    segmData=new SegmentationData;
    analysis.setData(m_dataName,segmData);
  }
  else {
    segmData = static_cast<SegmentationData*>(data);
    if (segmData==0) {
      LOGINIT("LP::AnalysisLoader");
      LERROR << "data "<< data <<" is not an object of class SegmentationData";
      return MISSING_DATA;
    }
  }


  try
  {
    SegmentationResultsLoader::XMLHandler handler(segmData,graph);
    m_parser->setContentHandler(&handler);
    m_parser->setErrorHandler(&handler);
    auto filename = getInputFile(analysis);
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
      LIMA_EXCEPTION_SELECT_LOGINIT(LOGINIT("LP::AnalysisLoader"),
                                    "Cannot open" << filename,
                                    XMLException);
    if (!m_parser->parse( QXmlInputSource(&file)))
    {
      LIMA_EXCEPTION_SELECT_LOGINIT(
        LOGINIT("LP::AnalysisLoader"),
        "Cannot parse" << filename << m_parser->errorHandler()->errorString(),
        XMLException);
    }
  }
  catch (const XMLException& )
  {
    return UNKNOWN_ERROR;
  }

  return SUCCESS_ID;
}

//***********************************************************************
// xerces XML handler
SegmentationResultsLoader::XMLHandler::XMLHandler(SegmentationData* s, AnalysisGraph* graph):
m_graph(graph),
m_data(s),
m_position(0),
m_length(0),
m_type()
{
  cout << "SegmentationResultsLoader::XMLHandler constructor" << endl;
}

SegmentationResultsLoader::XMLHandler::~XMLHandler()
{
}

bool SegmentationResultsLoader::XMLHandler::endElement(const QString & namespaceURI, const QString & eltName, const QString & qName)
{
  LIMA_UNUSED(namespaceURI);
  LIMA_UNUSED(qName);
  string name=toString(eltName);
  if (name=="segment") {
    LOGINIT("LP::AnalysisLoader");
    LDEBUG << "SegmentationResultsLoader::XMLHandler add data "  << m_type << "," << m_position << "," << m_length << "," << m_graph;
    Segment s(m_type);
    s.setVerticesFromPositions(m_position,m_length,m_graph);
    m_data->add(s);
  }
  return true;
}

bool SegmentationResultsLoader::XMLHandler::characters(const QString& chars)
{
  LOGINIT("LP::AnalysisLoader");
  LDEBUG << "SegmentationResultsLoader::XMLHandler characters in "  << m_currentElement;
  if (m_currentElement=="position") {
    std::string pos=toString(chars);
    m_position=atoi(pos.c_str());
  }
  else if (m_currentElement=="length") {
    std::string len=toString(chars);
    m_length=atoi(len.c_str());
  }
  else if (m_currentElement=="type") {
    m_type=toString(chars);
  }
  return true;
}

bool SegmentationResultsLoader::XMLHandler::
startElement(const QString & namespaceURI, const QString & eltName, const QString & qName, const QXmlAttributes & attributes)
{
  LIMA_UNUSED(namespaceURI);
  LIMA_UNUSED(qName);
  LIMA_UNUSED(attributes);
  LOGINIT("LP::AnalysisLoader");
  LDEBUG << "SegmentationResultsLoader::XMLHandler start element "  << toString(eltName);
  m_currentElement=toString(eltName);
  return true;
}

bool SegmentationResultsLoader::XMLHandler::warning(const QXmlParseException & e)
{
  LOGINIT("LP::AnalysisLoader");
  LERROR << "Error at file " << toString(e.systemId())
         << ", line " << e.lineNumber()
         << ", char " << e.columnNumber()
         << "  Message: " << toString(e.message());
         return true;
}
bool SegmentationResultsLoader::XMLHandler::error(const QXmlParseException & e)
{
  LOGINIT("LP::AnalysisLoader");
  LERROR << "Fatal error at file " << toString(e.systemId())
         << ", line " << e.lineNumber()
         << ", char " << e.columnNumber()
         << "  Message: " << toString(e.message());
         return false;
}
bool SegmentationResultsLoader::XMLHandler::fatalError(const QXmlParseException & e)
{
  LOGINIT("LP::AnalysisLoader");
  LWARN << "Warning at file " << toString(e.systemId())
        << ", line " << e.lineNumber()
        << ", char " << e.columnNumber()
        << "  Message: " << toString(e.message());
        return false;
}

std::string SegmentationResultsLoader::XMLHandler::toString(const LimaString& xercesString)
{
  return Common::Misc::limastring2utf8stdstring(xercesString);
}

} // end namespace
} // end namespace
