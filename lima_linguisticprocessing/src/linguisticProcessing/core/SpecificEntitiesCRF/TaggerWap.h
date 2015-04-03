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
 * @file       TaggerWap.h
 * @author     Grandjean Alex (alex.grandjean@cea.fr)
 * @date       Fri Mar 28 2014
 * copyright   Copyright (C) 20062014 by CEA LIST
 * Project     Lima
 * 
 * @brief      Label the data using the wapiti crf library
 * 
 * 
 ***********************************************************************/


#ifndef TAGGERWAP_H
#define TAGGERWAP_H


//#include "linguisticProcessing/common/PropertyCode/PropertyAccessor.h"
//#include "common/AbstractFactoryPattern/RegistrableFactory.h"
//#include "SpecificEntitiesCRFExport.h"
#include "AbstractTagger.h"
#include "model.h"
#include "decoder.h"

//#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "SpecificEntitiesCRFExport.h"
//#include "common/MediaProcessors/MediaProcessUnit.h"
//#include "linguisticProcessing/core/Automaton/recognizer.h"
//#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"

#include "common/MediaticData/EntityType.h"
#include "common/misc/fsaStringsPool.h"

namespace Lima
{
class AnalysisContent; 
namespace LinguisticProcessing
{
  /*
namespace LinguisticAnalysisStructure
{
  class AnalysisGraph;
}
  */
 


#define TaggerWap_ID "tagWap"
class TaggerWap : public AbstractTagger
{
public:
  TaggerWap();

  ~TaggerWap();

 /**
   * @brief Initialize the labelling options
   */
  void initOptions(const std::map<std::string, std::string>& options);
  
   /**
   * @brief Apply CRF to label the datas
   */
  void tag(AnalysisContent& analysis, MediaId lg);
 /**
   * @brief Load the model to use in CRF
   */
  void setMod(mdl_t *m);


  /**
   * @brief Create a list and add Position and Length according to the graph
   */
  listDat_t* firstList(AnalysisContent& analysis, MediaId lg);

 private:
  mdl_t *m_mod;
  opt_t opt;

 
  
};
}
}
#endif
