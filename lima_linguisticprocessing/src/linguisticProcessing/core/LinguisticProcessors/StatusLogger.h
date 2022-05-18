// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_LINGUISTICPROCESSINGSTATUSLOGGER_H
#define LIMA_LINGUISTICPROCESSINGSTATUSLOGGER_H

#include "LinguisticProcessorsExport.h"
#include "common/MediaProcessors/MediaProcessUnit.h"

#include <set>
#include <fstream>

#ifdef WIN32
#include <WinSock2.h>
#endif

namespace Lima
{

namespace LinguisticProcessing
{

#define STATUSLOGGER_CLASSID "StatusLogger"

/**
@author Benoit Mathieu
*/
class LIMA_LINGUISTICPROCESSORS_EXPORT StatusLogger : public MediaProcessUnit
{
public:

  StatusLogger();
  virtual ~StatusLogger();
  
  virtual void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;
  
  virtual LimaStatusCode process(AnalysisContent& analysis) const override;

private:

  std::unique_ptr< std::ofstream > m_out;
  std::set<std::string> m_toLog;
  std::string m_statusFile;
  uint64_t m_predTime;

};

}

}

#endif
