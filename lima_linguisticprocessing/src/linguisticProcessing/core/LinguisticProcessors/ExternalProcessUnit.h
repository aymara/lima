// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Mon Jan 17 2011
 * @brief this class contains a generic process unit that use a system call
 * to let an external process do the job. 
 ***********************************************************************/
#ifndef LIMA_LINGUISTICPROCESSING_EXTERNALPROCESSUNIT_H
#define LIMA_LINGUISTICPROCESSING_EXTERNALPROCESSUNIT_H

#include "LinguisticProcessorsExport.h"
#include "common/MediaProcessors/MediaProcessUnit.h"
#include "linguisticProcessing/client/AnalysisHandlers/AbstractTextualAnalysisHandler.h"

namespace Lima {
namespace LinguisticProcessing {

#define EXTERNALPROCESSUNIT_CLASSID "ExternalProcessUnit"

/*
 * @brief this class contains a generic process unit that use a system
 * call to let an external process do the job. The input for this
 * external program is produced by a dumper given as a parameter,
 * and the output is read by a Loader also given as a parameter
 */
class LIMA_LINGUISTICPROCESSORS_EXPORT ExternalProcessUnit : public MediaProcessUnit
{
public:
  ExternalProcessUnit();

  virtual ~ExternalProcessUnit();

  void init(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
            Manager* manager) override
    ;

  LimaStatusCode process(AnalysisContent& analysis) const override;
    
private:
  std::shared_ptr<MediaProcessUnit> m_dumper;
  std::shared_ptr<MediaProcessUnit> m_loader;
  QString m_commandLine;
  QString m_inputSuffix;
  QString m_outputSuffix;
};

} // end namespace
} // end namespace

#endif
