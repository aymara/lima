/*
    Copyright 2002-2014 CEA LIST

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
            Manager* manager);

  LimaStatusCode process(AnalysisContent& analysis) const;

private:
  SemanticRoleLabelingLoaderPrivate* m_d;

};

} // end namespace
} // end namespace
} // end namespace

#endif
