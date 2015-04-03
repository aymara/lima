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

 ************************************************************************
 *
 * @file       SpecificEntitiesCRFTag.h
 * @author     Grandjean Alex (alex.grandjean@cea.fr)
 * @date       Wed Mar 12 2014
 * copyright   Copyright (C) 2006-2014 by CEA LIST
 * Project     Lima
 * 
 * @brief      Execute the labelling with CRF
 * 
 * 
 ***********************************************************************/

#ifndef LIMA_LINGUISTICPROCESSING_SPECIFICENTITIESCRFTAG_H
#define LIMA_LINGUISTICPROCESSING_SPECIFICENTITIESCRFTAG_H

#include "SpecificEntitiesCRFExport.h"
#include "common/MediaProcessors/MediaProcessUnit.h"
#include "linguisticProcessing/core/Automaton/recognizer.h"
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"
//#include "ToolsCRF.h"

#include "model.h"
#include "decoder.h"

namespace Lima
{
class AnalysisContent;
namespace LinguisticProcessing
{
namespace LinguisticAnalysisStructure
{
  class AnalysisGraph;
}
namespace Automaton
{
  class RecognizerMatch;
}
namespace SpecificEntitiesCRF
{

#define SPECIFICENTITIESCRFTAG_CLASSID "SpecificEntitiesCRFTag"

class LIMA_SPECIFICENTITIESCRF_EXPORT SpecificEntitiesCRFTag : public MediaProcessUnit
{

 public:
  SpecificEntitiesCRFTag();

  virtual ~SpecificEntitiesCRFTag();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager)
  ;


   LimaStatusCode process(
    AnalysisContent& analysis) const;

private:

  std::string m_crflib;

  std::deque<std::string> m_listmodel;

  MediaId m_lg;

  std::vector<mdl_t*> m_vecmod;


};
}
}
}

#endif
