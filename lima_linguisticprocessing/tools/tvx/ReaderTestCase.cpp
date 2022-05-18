// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2006 by Oliier Mesnard                                  *
 *   olivier.mesnard@cea.fr                                                *
 *                                                                         *
 ***************************************************************************/
#include "tools/tvx/ReaderTestCase.h"
#include "linguisticProcessing/client/AnalysisHandlers/SBowDocumentWriter.h"

#include "common/Data/strwstrtools.h"
#include "common/Handler/shiftFrom.h"
#include "linguisticProcessing/common/BagOfWords/bowDocument.h"
//#include "linguisticProcessing/client/AnalysisHandlers/BowDocumentHandler.h"
#include "linguisticProcessing/common/BagOfWords/bowXMLWriter.h"
#include "linguisticProcessing/client/AnalysisHandlers/MultimediaDocumentHandler.h"
#include "linguisticProcessing/client/AnalysisHandlers/MultimediaDocumentReaderWriter.h"

#include <QProcess>

#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
// for call to system()
#include <stdlib.h>
#include <memory>

using namespace std;
using namespace Lima::Common::BagOfWords;
using namespace Lima::Common::TGV;
// Pour readStream()
using namespace Lima::Common::Misc;
using namespace Lima::Handler;
using namespace Lima::XmlReader;

namespace Lima
{

namespace ReaderValidation
{

ReaderTestCaseProcessor::ReaderTestCaseProcessor(
    const string& workingDirectory, std::shared_ptr<AbstractXmlReaderClient> client) :
  TestCaseProcessor(workingDirectory),
  m_lpclient(client) {}

Lima::Common::TGV::TestCaseError ReaderTestCaseProcessor::processTestCase(const Lima::Common::TGV::TestCase& testCase)
{
  TGVLOGINIT;
  LDEBUG << "ReaderTestCaseProcessor::processTestCase(" << testCase.id << ")";
  Lima::Common::TGV::TestCaseError result = Lima::Common::TGV::TestCaseError();
  const std::string& textFilename = testCase.getParam( "textFilename" );

  std::string filename(textFilename+".xml");
  LDEBUG << "ReaderTestCaseProcessor::processTestCase filename =" << filename;
/*
  // write text in file
  std::string filename(m_workingDirectory+"/test"+testCase.id+".txt");
  {
    ofstream fout(filename.c_str(), std::ofstream::binary);
    fout << text;
    fout.flush();
    fout.close();
  }
*/
//  LimaString contentText = Common::Misc::utf8stdstring2limastring(text);
  std::ifstream file(filename.c_str(), std::ifstream::binary);
  std::string contentText;
  readStream(file, contentText);
  LDEBUG << "ReaderTestCaseProcessor::processTestCase contentText =" << contentText;

  const std::string& language = testCase.getParam("language");
  LDEBUG << "ReaderTestCaseProcessor::processTestCase language =" << language;

  QString qtext_s = QString::fromUtf8(contentText.c_str());

  QRegExp rx("(&[^;]*;)");
//   int shift = 0;
//   int indexofent = 0;

  auto shiftFrom = std::make_shared<const ShiftFrom>(qtext_s);
  LDEBUG << "shiftFrom is:" << *shiftFrom;

  if(qtext_s.trimmed().isEmpty()) {
      std::cerr << "file " << filename << " has empty input ! " << std::endl;
  }
  contentText = qtext_s.toUtf8().constData();


  // For each pipeline process test
  MultiValCallParams::const_iterator pos = testCase.multiValCallParams.find("pipelines");
  if( pos != testCase.multiValCallParams.end() ) {
    const list<string>& pipeList = (*pos).second;
    for (list<string>::const_iterator pipItr=pipeList.begin();
         pipItr!=pipeList.end();  pipItr++)
    {

      string filenameWithPipeLine=filename+"."+*pipItr;
      string tempFilename=filenameWithPipeLine+".mult";

      Lima::Handler::MultimediaDocumentHandler* handler = new Lima::Handler::MultimediaDocumentHandler(shiftFrom);
      QString handlerName = "xmlDocumentHandler";
      std::stringstream mult(std::stringstream::in|std::stringstream::out|std::stringstream::binary);
      handler->setOut(&mult);
      LDEBUG << "run call handler setAnalysisHandler " << handlerName << handler;
      m_lpclient->setAnalysisHandler(handlerName.toUtf8().constData(), handler);

      // Analyse text
      map<string,string> metaData;
      metaData["Lang"]=language;
      metaData["FileName"]=filenameWithPipeLine;
      metaData["DocumentName"]=testCase.id;
      LDEBUG << "ReaderTestCaseProcessor::processTestCase call to analyze text("
             << *pipItr << ")";
      std::map<std::string, AbstractAnalysisHandler*> handlers;
      m_lpclient->analyze(contentText,metaData,*pipItr,handlers);
      m_lpclient->releaseAnalysisHandler(handlerName.toUtf8().constData());

      string outputfile=filenameWithPipeLine+".bowdocument.xml";
      std::ofstream fout(outputfile.c_str(),std::ofstream::binary);
      MultimediaBinaryReader reader;
      reader.readHeader(mult);
      BoWDocument document;
      MultimediaXMLWriter writer(fout);
      writer.writeMultimediaDocumentsHeader();
      while (! mult.eof())
      {
        reader.readMultimediaDocumentBlock(mult, document, writer, false, false);
      }
      writer.writeMultimediaDocumentsFooter();


      result = evalTestCase( testCase, *pipItr, filename, filenameWithPipeLine );
      if (result() != TestCaseError::NoError) return result;
    }
  }
  return result;
}


}

}
