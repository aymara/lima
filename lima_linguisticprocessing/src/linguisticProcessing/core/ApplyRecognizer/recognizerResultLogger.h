// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       recognizerResultLogger.h
 * @author     besancon (romaric.besancon@cea.fr)
 * @date       Tue Oct 25 2005
 * @version    $Id$
 * copyright   Copyright (C) 2005-2012 by CEA LIST
 * Project     
 * 
 * @brief      logger for the results of the recognizer
 * 
 * 
 ***********************************************************************/

#ifndef RECOGNIZERRESULTLOGGER_H
#define RECOGNIZERRESULTLOGGER_H

#include "ApplyRecognizerExport.h"
#include "linguisticProcessing/common/misc/AbstractLinguisticLogger.h"

namespace Lima {
namespace LinguisticProcessing {
namespace ApplyRecognizer {

#define RECOGNIZERRESULTLOGGER_CLASSID "RecognizerResultLogger"

class LIMA_APPLYRECOGNIZER_EXPORT RecognizerResultLogger : public AbstractLinguisticLogger
{
 public:
  RecognizerResultLogger(); 
  ~RecognizerResultLogger();

  void init(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
            Manager* manager) override;

  LimaStatusCode process(AnalysisContent& analysis) const override;

 private:
  MediaId m_language;
  std::string m_data;
};

} // end namespace
} // end namespace
} // end namespace

#endif
