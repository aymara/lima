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
#include "linguisticProcessing/core/LinguisticProcessors/AbstractLinguisticLogger.h"

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
            Manager* manager)
    ;

  LimaStatusCode process(AnalysisContent& analysis) const;

 private:
  MediaId m_language;
  std::string m_data;
};

} // end namespace
} // end namespace
} // end namespace

#endif
