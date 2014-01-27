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
 * @file       Modex.h
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Mon Jan 22 2007
 * copyright   Copyright (C) 2007 by CEA LIST
 * Project     InformationExtraction
 * 
 * @brief a modex is a module designed for information extraction and
 * uses entity and template definitions. It is defined as a process
 * unit to be integrated in a linguistic analysis pipeline.
 * 
 * 
 ***********************************************************************/

#ifndef MODEX_H
#define MODEX_H

#include "common/MediaProcessors/MediaProcessUnit.h"
#include "common/MediaProcessors/MediaProcessUnitPipeline.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileParser.h"
#include "common/XMLConfigurationFiles/groupConfigurationStructure.h"
#include "common/XMLConfigurationFiles/moduleConfigurationStructure.h"

namespace Lima {
namespace LinguisticProcessing {

#define MODEX_CLASSID "Modex"

class Modex : public LinguisticProcessing::LinguisticProcessUnit
{
 public:
  Modex(); 
  virtual ~Modex();
  
  void init(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
            Manager* manager)
    ;

  LimaStatusCode process(AnalysisContent& analysis) const;
    
 private:
   MediaId m_language;
  const MediaProcessUnitPipeline* m_pipeline;  

  // private member functions
  void initModex(const std::string& configFile,MediaId lang);
  void initEntities(const std::string& filename);
  void initEntities(Common::XMLConfigurationFiles::XMLConfigurationFileParser& configParser);
  void initModexConfig(Common::XMLConfigurationFiles::XMLConfigurationFileParser& configParser);
  void addConfiguration(Common::XMLConfigurationFiles::ModuleConfigurationStructure& modexConfig,
                        Common::XMLConfigurationFiles::ModuleConfigurationStructure& commonConfig);
  
};

} // end namespace
} // end namespace

#endif
