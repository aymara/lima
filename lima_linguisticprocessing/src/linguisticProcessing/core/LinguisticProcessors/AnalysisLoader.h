// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Mon Jan 17 2011
 * @brief abstract class for analysis loaders
 ***********************************************************************/
#ifndef LIMA_LINGUISTICPROCESSING_ANALYSISLOADER_H
#define LIMA_LINGUISTICPROCESSING_ANALYSISLOADER_H

#include "LinguisticProcessorsExport.h"
#include "common/MediaProcessors/MediaProcessUnit.h"
#include <QSharedPointer>
#include <iostream>
#include <fstream>

namespace Lima {
namespace LinguisticProcessing {

#define ANALYSISLOADER_CLASSID "AnalysisLoader"

class AnalysisLoaderPrivate;
/*
 * @brief this is the abstract class for analysis loaders, that read
 * informations from external files to insert them in the analysis
 * data
 */
class LIMA_LINGUISTICPROCESSORS_EXPORT AnalysisLoader : public MediaProcessUnit
{
  friend class AnalysisLoaderPrivate;
public:
  AnalysisLoader();

  virtual ~AnalysisLoader();

  void init(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
            Manager* manager) override;

  LimaStatusCode process(AnalysisContent& /*analysis*/) const override { return SUCCESS_ID; }

  QString getInputFile(AnalysisContent& analysis) const;

protected:
  QSharedPointer<AnalysisLoaderPrivate> m_d;
};

} // end namespace
} // end namespace

#endif
