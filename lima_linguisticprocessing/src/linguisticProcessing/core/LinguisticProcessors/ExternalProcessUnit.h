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
 * @file       externalProcessUnit.h
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Mon Jan 17 2011
 * copyright   Copyright (C) 2011 by CEA LIST (LVIC)
 * Project     MM
 * 
 * @brief this class contains a generic process unit that use a system call
 * to let an external process do the job. 
 * 
 * 
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
  const MediaProcessUnit* m_dumper;
  const MediaProcessUnit* m_loader;
  QString m_commandLine;
  QString m_inputSuffix;
  QString m_outputSuffix;
};

} // end namespace
} // end namespace

#endif
