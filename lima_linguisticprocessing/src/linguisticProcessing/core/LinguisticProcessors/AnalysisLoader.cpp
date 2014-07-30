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
 * @file       AnalysisLoader.cpp
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Tue Jan 18 2011
 * copyright   Copyright (C) 2011 by CEA LIST
 * 
 ***********************************************************************/

#include "AnalysisLoader.h"

#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"

namespace Lima {
namespace LinguisticProcessing {

SimpleFactory<MediaProcessUnit,AnalysisLoader> AnalysisLoaderFactory(ANALYSISLOADER_CLASSID);

//***********************************************************************
// constructors and destructors
AnalysisLoader::AnalysisLoader():
MediaProcessUnit(),
m_inputFileName(),
m_inputFileExtension()
{
}

AnalysisLoader::~AnalysisLoader() {
}

//***********************************************************************
void AnalysisLoader::init(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
          Manager* /*manager*/)
  
{
  LOGINIT("LP::AnalysisLoader");
  LDEBUG << "Initialization";
  
  bool parameterFound(false);
  try {
    m_inputFileName=unitConfiguration.getParamsValueAtKey("inputFile");
    parameterFound=true;
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {
  }

  try {
    m_inputFileExtension=unitConfiguration.getParamsValueAtKey("inputSuffix");
    parameterFound=true;
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {
  }

  if (! parameterFound) {
    LERROR << "No 'inputFile' or 'inputSuffix' parameter in AnalysisLoader";
    throw InvalidConfiguration();
  }

}

const std::string& AnalysisLoader::getInputFile(AnalysisContent& analysis) const
{
  static std::string inputFile("");
  if (! m_inputFileName.empty()) {
    return m_inputFileName;
  }
  else if (! m_inputFileExtension.empty()) {
    // get filename from metadata
    LinguisticMetaData* metadata=static_cast<LinguisticMetaData*>(analysis.getData("LinguisticMetaData"));
    if (metadata == 0)
    {
      LOGINIT("LP::AnalysisLoader");
      LERROR << "no LinguisticMetaData : cannot use 'inputSuffix' parameter for AnalysisLoader";
      return inputFile;
    }
    
    std::string textFileName = metadata->getMetaData("FileName");
    inputFile = textFileName + m_inputFileExtension;
    return inputFile;
  }
  LOGINIT("LP::AnalysisLoader");
  LERROR << "No 'inputFile' found in AnalysisLoader";
  return inputFile;
}


} // end namespace
} // end namespace
