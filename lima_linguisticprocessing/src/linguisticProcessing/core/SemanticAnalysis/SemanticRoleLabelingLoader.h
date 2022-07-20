// Copyright 2002-2014 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/**
 * @file       SemanticRoleLabelingLoader.h
 * @author     Clémence Filmont
 * @author     Gael de Chalendar <gael.de-chalendar@cea.fr> 
 * @date       2014-04-17
 * copyright   Copyright (C) 2014-2016 by CEA LIST
 */

#ifndef SEMANTICROLELABELINGLOADER_H
#define SEMANTICROLELABELINGLOADER_H

#include "linguisticProcessing/core/LinguisticProcessors/AnalysisLoader.h"

namespace Lima {
namespace LinguisticProcessing {
namespace SemanticAnalysis {

#define SEMANTICROLELABELINGLOADER_CLASSID "SemanticRoleLabelingLoader"

class SemanticRoleLabelingLoaderPrivate;

/**
 * @brief      A Semantic Role Labeling Loader class
 * @author     Clémence Filmont
 * @author     Gael de Chalendar <gael.de-chalendar@cea.fr> 
 */
class SemanticRoleLabelingLoader : public AnalysisLoader
{
 public:
  SemanticRoleLabelingLoader();
  virtual ~SemanticRoleLabelingLoader();

  void init(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
            Manager* manager) override;

  LimaStatusCode process(AnalysisContent& analysis) const override;

private:
  SemanticRoleLabelingLoaderPrivate* m_d;

};

} // end namespace
} // end namespace
} // end namespace

#endif
