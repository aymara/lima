/*
    Copyright 2002-2020 CEA LIST

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
 *   Copyright (C) 2004 by Benoit Mathieu                                  *
 *   mathieub@zoe.cea.fr                                                   *
 *                                                                         *
 ***************************************************************************/
#include "tools/tva/AnalysisTestCase.h"

#include "common/Data/strwstrtools.h"
#include "linguisticProcessing/common/BagOfWords/bowText.h"
#include "linguisticProcessing/common/BagOfWords/bowXMLWriter.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>

#include "linguisticProcessing/client/AnalysisHandlers/BowTextHandler.h"

using namespace std;
using namespace Lima::Common::TGV;
using namespace Lima::LinguisticProcessing;

namespace Lima
{

namespace AnalysisValidation
{

AnalysisTestCaseProcessor::AnalysisTestCaseProcessor(
  const std::string& workingDirectory,
  LinguisticProcessing::AbstractLinguisticProcessingClient* client,
  const std::map<std::string, AbstractAnalysisHandler*> &handlers) :
  TestCaseProcessor(workingDirectory),
  m_lpclient(client),
  m_handlers(handlers) {}

TestCaseError AnalysisTestCaseProcessor::processTestCase(
    const Lima::Common::TGV::TestCase& testCase)
{
  auto input_filename = testCase.getParam( "file" );
  auto text = testCase.getParam( "text" );
  if (input_filename.size() > 0)
  {
    input_filename = m_workingDirectory+"/"+input_filename;
    ifstream fin(input_filename.c_str(),
                 std::ifstream::in|std::ifstream::binary);
    if (!fin.is_open())
    {
      TestCaseError error(testCase,
                          TestCaseError::TestCaseFailed,
                          std::string("Unable to open file for reading: ") + input_filename,
                          "",
                          TestCase::TestUnit());
      return error;
    }
    getline(fin, text, (char) fin.eof());
    fin.close();
  }

  // write text in file
  std::string filename(m_workingDirectory+"/test"+testCase.id.toUtf8().constData()+".txt");
  {
    ofstream fout(filename.c_str(),
                  std::ofstream::out|std::ofstream::trunc|std::ofstream::binary);
    fout << text;
    fout.flush();
    fout.close();
  }

  QString contentText = QString::fromStdString(text);
  auto language = testCase.getParam("language");
  auto metaValuesStr = testCase.getParam("meta");
  std::map<std::string, std::string> userMetaData;
  if(!metaValuesStr.empty())
  {
    std::string::size_type k=0;
    do
    {
      k = metaValuesStr.find(",");
      //if (k==std::string::npos) continue;
      std::string str(metaValuesStr,0,k);
      std::string::size_type i=str.find(":");
      if (i == std::string::npos)
      {
        std::cerr << "in test Case " << testCase.id
                  << " meta argument '" << str
                  << "' is not of the form XXX:YYY: ignored"
                  << std::endl;
      }
      else
      {
        //std::cout << "add metadata " << std::string(str,0,i) << "=>" << std::string(str,i+1) << std::endl;
        userMetaData.insert(std::make_pair(std::string(str,0,i),
                                           std::string(str,i+1)));
      }
      if (k!=std::string::npos)
      {
        metaValuesStr = std::string(metaValuesStr,k+1);
      }
    } while (k!=std::string::npos);
  }

  // For each pipeline process test
  auto pos = testCase.multiValCallParams.find("pipelines");
  if( pos != testCase.multiValCallParams.end() )
  {
    const auto& pipeList = (*pos).second;
    for (const auto& pip: pipeList)
    {
      auto filenameWithPipeLine = filename+"." + pip;

      // Analyse text
      std::map<std::string,std::string> metaData;
      metaData["Lang"] = language;
      metaData["FileName"] = filenameWithPipeLine;
      metaData["DocumentName"] = testCase.id.toStdString();
      metaData.insert(userMetaData.begin(), userMetaData.end());
      m_lpclient->analyze(contentText, metaData, pip, m_handlers);
      auto bowHandler = static_cast<BowTextHandler*>(m_handlers["bowTextHandler"]);
      // dump results
      auto& text=bowHandler->getBowText();
      text.lang = language;
      std::string outputfile = filenameWithPipeLine + ".indexed.xml";
      {
        std::ofstream fout(outputfile,
                           std::ofstream::out|std::ofstream::trunc|std::ofstream::binary);
        if (fout.is_open())
        {
          fout << "<?xml version='1.0' encoding='UTF-8'?>" << std::endl;
          Common::BagOfWords::BoWXMLWriter writer(fout);
          writer.writeBoWText(&text, true, false);
          fout.flush();
          fout.close();
          std::cout << "Test results saved to \"" << outputfile << "\"" << std::endl;
        }
        else
        {
          TestCaseError error(testCase,
                              TestCaseError::TestCaseFailed,
                              std::string("Unable to open file for writing: ") + outputfile,
                              pip,
                              TestCase::TestUnit());
          return error;
        }
      }

      auto error = evalTestCase(testCase, pip, filename, filenameWithPipeLine);
      if (error())
        return error;
    }
  }
  return TestCaseError();
}


}

}
