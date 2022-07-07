// Copyright 2002-2022 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       SpecificEntitiesLoader.h
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Thu Jun 16 2011
 * copyright   Copyright (C) 2011 by CEA LIST
 * Project     mm_linguisticprocessing
 *
 * @brief      a Loader class to load external entity annotations
 *
 *
 ***********************************************************************/

#ifndef SPECIFICENTITIESLOADER_H
#define SPECIFICENTITIESLOADER_H

#include "SpecificEntitiesExport.h"
#include "linguisticProcessing/core/LinguisticProcessors/AnalysisLoader.h"

namespace Lima {
namespace LinguisticProcessing {
namespace SpecificEntities {

#define SPECIFICENTITIESLOADER_CLASSID "SpecificEntitiesLoader"

class SpecificEntitiesLoaderPrivate;
/**
 * @brief      A Loader class to load external entity annotations
 * @author     Romaric Besancon <romaric.besancon@cea.fr>
 * @date       Thu Jun 16 2011
 */
class LIMA_SPECIFICENTITIES_EXPORT SpecificEntitiesLoader : public AnalysisLoader
{
 public:
  SpecificEntitiesLoader();
  virtual ~SpecificEntitiesLoader();

  void init(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
            Manager* manager) override;

  LimaStatusCode process(AnalysisContent& analysis) const override;

 private:
   SpecificEntitiesLoaderPrivate* m_d;
};

} // end namespace
} // end namespace
} // end namespace

#endif
