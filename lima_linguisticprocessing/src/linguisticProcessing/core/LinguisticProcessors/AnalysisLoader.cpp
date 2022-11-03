// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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

class AnalysisLoaderPrivate
{
  friend class AnalysisLoader;
public:
  AnalysisLoaderPrivate();

  ~AnalysisLoaderPrivate();

  QString m_inputFileName;
  QString m_inputFileExtension;
  QString m_temporaryFileMetadata;
};


AnalysisLoaderPrivate::AnalysisLoaderPrivate() :
m_inputFileName(),
m_inputFileExtension(),
m_temporaryFileMetadata()
{
}

AnalysisLoaderPrivate::~AnalysisLoaderPrivate()
{

}

//***********************************************************************
// constructors and destructors
AnalysisLoader::AnalysisLoader():
MediaProcessUnit(),
m_d(new AnalysisLoaderPrivate())
{
}

AnalysisLoader::~AnalysisLoader() 
{
}

//***********************************************************************
void AnalysisLoader::init(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
          Manager* /*manager*/)
  
{
  LOGINIT("LP::AnalysisLoader");
  LDEBUG << "Initialization";
  
  bool parameterFound(false);
  try 
  {
    m_d->m_temporaryFileMetadata = QString::fromUtf8(unitConfiguration.getParamsValueAtKey("temporaryFileMetadata").c_str());
    parameterFound=true;
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {} // keep default value (empty)

  try {
    m_d->m_inputFileName=QString::fromUtf8(unitConfiguration.getParamsValueAtKey("inputFile").c_str());
    parameterFound=true;
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {
  }

  try {
    m_d->m_inputFileExtension=QString::fromUtf8(unitConfiguration.getParamsValueAtKey("inputSuffix").c_str());
    parameterFound=true;
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {
  }

  if (! parameterFound) {
    LERROR << "No 'inputFile' or 'inputSuffix' or 'temporaryFileMetadata' parameter in AnalysisLoader";
    throw InvalidConfiguration();
  }

}

QString AnalysisLoader::getInputFile(AnalysisContent& analysis) const
{
  QString inputFile;
  if (! m_d->m_temporaryFileMetadata.isEmpty()) {
    // get temporary filename from metadata
    auto metadata = std::dynamic_pointer_cast<LinguisticMetaData>(analysis.getData("LinguisticMetaData"));
    if (metadata == 0)
    {
      LOGINIT("LP::AnalysisLoader");
      LERROR << "no LinguisticMetaData : cannot use 'temporaryFileMetadata' parameter for AnalysisLoader";
      return inputFile;
    }
    
    inputFile = QString::fromUtf8(metadata->getMetaData(m_d->m_temporaryFileMetadata.toUtf8().constData()).c_str());
    return inputFile;
  }
  else if (! m_d->m_inputFileName.isEmpty()) {
    return m_d->m_inputFileName;
  }
  else if (! m_d->m_inputFileExtension.isEmpty()) {
    // get filename from metadata
    auto metadata = std::dynamic_pointer_cast<LinguisticMetaData>(analysis.getData("LinguisticMetaData"));
    if (metadata == 0)
    {
      LOGINIT("LP::AnalysisLoader");
      LERROR << "no LinguisticMetaData : cannot use 'inputSuffix' parameter for AnalysisLoader";
      return inputFile;
    }
    
    QString textFileName = QString::fromUtf8(metadata->getMetaData("FileName").c_str());
    inputFile = textFileName + m_d->m_inputFileExtension;
    return inputFile;
  }
  LOGINIT("LP::AnalysisLoader");
  LERROR << "No 'inputFile' found in AnalysisLoader";
  return inputFile;
}


} // end namespace
} // end namespace
