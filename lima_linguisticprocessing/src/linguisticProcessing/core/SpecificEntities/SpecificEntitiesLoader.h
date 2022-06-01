/*
    Copyright 2002-2022 CEA LIST

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
