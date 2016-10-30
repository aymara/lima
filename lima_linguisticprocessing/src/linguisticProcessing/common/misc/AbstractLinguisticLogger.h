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

#include "MiscExport.h"
#include "common/MediaProcessors/MediaProcessUnit.h"

namespace Lima {
namespace LinguisticProcessing {

/**
 * @brief A generic process unit to log information in files:
 * contains some common informations such as : output suffix, 
 * files in append mode etc...
 */
class LIMA_MISC_EXPORT AbstractLinguisticLogger : public MediaProcessUnit
{
public:
  AbstractLinguisticLogger(const std::string& defaultSuffix=".log");
  virtual ~AbstractLinguisticLogger();

  virtual void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager)
    ;

  bool openLogFile(std::ofstream& output,const std::string& sourceFile) const;

  virtual LimaStatusCode process(AnalysisContent& analysis) const=0;

private:
  std::string m_outputSuffix;
  bool m_append;
};


} // end namespace
} // end namespace

#endif
