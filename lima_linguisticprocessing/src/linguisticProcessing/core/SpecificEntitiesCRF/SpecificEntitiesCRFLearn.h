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
 * @file       SpecificEntitiesCRFLearn.h
 * @author     Grandjean Alex (alex.grandjean@cea.fr)
 * @date       Tue Feb 18 2014
 * copyright   Copyright (C) 2006-2014 by CEA LIST
 * Project     Lima
 * 
 * @brief      Execute the training operation to generate a model crf
 * 
 * 
 ***********************************************************************/

#ifndef LIMA_LINGUISTICPROCESSING_SPECIFICENTITIESCRFLEARN_H
#define LIMA_LINGUISTICPROCESSING_SPECIFICENTITIESCRFLEARN_H

#include "SpecificEntitiesCRFExport.h"
#include "common/MediaProcessors/MediaProcessUnit.h"
#include "linguisticProcessing/core/Automaton/recognizer.h"
//#include "TrainerSE.h"
//#include "ToolsCRF.h"

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

#define SPECIFICENTITIESCRFLEARN_CLASSID "SpecificEntitiesCRFLearn"

class LIMA_SPECIFICENTITIESCRF_EXPORT SpecificEntitiesCRFLearn : public MediaProcessUnit
{

 public:
  SpecificEntitiesCRFLearn();

  virtual ~SpecificEntitiesCRFLearn();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager)
  ;


   LimaStatusCode process(AnalysisContent& analysis) const;
   

private:

  bool m_useSentenceBounds;
  bool m_useDicoWords;
  std::string m_graph;
  std::string m_crflib;
  bool m_useAnnot;

  std::deque<std::string> m_listpattern;

  MediaId m_lg;

};
}
}
}

#endif
