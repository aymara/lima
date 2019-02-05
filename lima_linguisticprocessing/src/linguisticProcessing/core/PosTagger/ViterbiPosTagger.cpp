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
/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                              *
 *                                                                         *
 ***************************************************************************/

#include "ViterbiPosTagger.h"

#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "common/tools/FileUtils.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/MediaticData/mediaticData.h"
#include "integerCost.h"

using namespace std;
using namespace Lima::Common::MediaticData;

namespace Lima
{
namespace LinguisticProcessing
{
namespace PosTagger
{

std::unique_ptr< ViterbiPosTaggerFactory > ViterbiPosTaggerFactory::s_instance = 
  std::unique_ptr< ViterbiPosTaggerFactory >(
    new ViterbiPosTaggerFactory(VITERBIPOSTAGGER_CLASSID));


ViterbiPosTaggerFactory::ViterbiPosTaggerFactory(const std::string& id) :
    InitializableObjectFactory<MediaProcessUnit>(id)
{}

MediaProcessUnit* ViterbiPosTaggerFactory::create(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  MediaProcessUnit::Manager* manager) const 
{

  PTLOGINIT;
  MediaId language = manager->getInitializationParameters().media;
  const auto& ldata = static_cast<const Common::MediaticData::LanguageData&>(
    MediaticData::single().mediaData(language));
  const auto& microManager = ldata.getPropertyCodeManager().getPropertyManager("MICRO");

  // setting default category
  LinguisticCode defaultCategory;
  try
  {
    std::string id = unitConfiguration.getParamsValueAtKey("defaultCategory");
    defaultCategory = microManager.getPropertyValue(id);
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LWARN << "No default microcateg category ! use category PONCTU_FORTE";
    defaultCategory = microManager.getPropertyValue("PONCTU_FORTE");
  }

  std::string trigramsFile;
  std::string bigramsFile;
  string resourcesPath = MediaticData::single().getResourcesPath();
  try
  {
    trigramsFile = Common::Misc::findFileInPaths(resourcesPath.c_str(), unitConfiguration.getParamsValueAtKey("trigramFile").c_str()).toUtf8().constData();
    bigramsFile = Common::Misc::findFileInPaths(resourcesPath.c_str(), unitConfiguration.getParamsValueAtKey("bigramFile").c_str()).toUtf8().constData();
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LERROR << "No param 'trigramFile' or 'bigramFile' in ViterbiPosTagger group for language " 
            << (int)language;
    throw InvalidConfiguration();
  }

  string costFunction;
  try
  {
    costFunction = unitConfiguration.getParamsValueAtKey("costFunction");
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LERROR << "No param 'costFuntion' in ViterbiPosTagger group for language " 
            << (int)language;
    throw InvalidConfiguration();
  }

  MediaProcessUnit* posTagger(0);
  
  LINFO << "ViterbiPosTagger cost function is" << costFunction;
  if (costFunction == "IntegerCost")
  {

    uint64_t trigramCost=0;
    uint64_t bigramCost=1;
    uint64_t unigramCost=3;
    try
    {
      trigramCost = atoi(unitConfiguration.getParamsValueAtKey("trigramCost").data());
      bigramCost = atoi(unitConfiguration.getParamsValueAtKey("bigramCost").data());
      unigramCost = atoi(unitConfiguration.getParamsValueAtKey("unigramCost").data());
    }
    catch (Common::XMLConfigurationFiles::NoSuchParam& )
    {
      LWARN << "can't read all parameters for IntegerCost function ! use defaults";
    }
    
    LINFO << "ViterbiPosTagger use ViterbiIntegerCost with trigramCost = " 
          << trigramCost << "; bigramCost = " << bigramCost 
          << "; unigramCost = " << unigramCost;
    
    ViterbiCostFunction<uint64_t,SimpleIntegerCostFactory<uint64_t> > costFunction(
      language, 
      &microManager, 
      SimpleIntegerCostFactory<uint64_t>(trigramCost, bigramCost, unigramCost), 
      trigramsFile, 
      bigramsFile);
    
    posTagger = new ViterbiPosTagger< uint64_t, ViterbiCostFunction< uint64_t, SimpleIntegerCostFactory<uint64_t> > >(costFunction);
    
  }
  else if (costFunction == "FrequencyCost")
  {
    ViterbiCostFunction<FrequencyCost,FrequencyCostFactory> costFunction(
      language,
      &microManager,
      FrequencyCostFactory(),
      trigramsFile,
      bigramsFile);
    posTagger = new ViterbiPosTagger<FrequencyCost,ViterbiCostFunction<FrequencyCost,FrequencyCostFactory> >(costFunction);
  }
  else
  {
    LERROR << "unknown costFuntion : " << costFunction << " ! ";
    throw InvalidConfiguration();
  }

  posTagger->init(unitConfiguration,manager);

  return posTagger;
}

} // PosTagger

} // LinguisticProcessing

} // Lima
