// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       AbstractLinguisticLogger.h
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Thu Aug  3 2006
 * copyright   Copyright (C) 2006-2012 by CEA LIST
 * Project     LinguisticProcessing
 * 
 * @brief      generic process unit logger
 * 
 ***********************************************************************/

#ifndef LIMA_LINGUISTICPROCESSING_LINGUISTICPROCESSORS_ABSTRACTLINGUISTICLOGGER_H
#define LIMA_LINGUISTICPROCESSING_LINGUISTICPROCESSORS_ABSTRACTLINGUISTICLOGGER_H

#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "common/MediaProcessors/MediaProcessUnit.h"

namespace Lima {
namespace LinguisticProcessing {

/**
 * @brief A generic process unit to log information in files:
 * contains some common informations such as : output suffix, 
 * files in append mode etc...
 */
class LIMA_LPMISC_EXPORT AbstractLinguisticLogger : public MediaProcessUnit
{
public:
  AbstractLinguisticLogger(const std::string& defaultSuffix=".log");
  virtual ~AbstractLinguisticLogger();

  virtual void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;

  bool openLogFile(std::ofstream& output,const std::string& sourceFile) const;

  virtual LimaStatusCode process(AnalysisContent& analysis) const override = 0;

private:
  std::string m_outputSuffix;
  bool m_append;
};


} // end namespace
} // end namespace

#endif
