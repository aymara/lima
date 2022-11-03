// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
