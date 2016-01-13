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

/** @brief      Implementation of desagglutination resources
  *
  * @file       DesagglutinationResources.h
  * @author     Benoit Mathieu <mathieub@zoe.cea.fr>
  * @date       Created on January 2004
  * @version    $Id$
  *
  */

#include "DesagglutinationResources.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "common/Data/FileUtils.h"
#include "common/MediaticData/mediaticData.h"
#include "linguisticProcessing/common/PropertyCode/PropertyCodeManager.h"
#include "linguisticProcessing/common/PropertyCode/PropertyManager.h"
#include "linguisticProcessing/common/linguisticData/languageData.h"

#include <iostream>
#include <fstream>

using namespace std;
using namespace Lima::Common::MediaticData;


namespace Lima
{
namespace LinguisticProcessing
{
namespace MorphologicAnalysis
{
namespace Desagglutination
{

SimpleFactory<AbstractResource,DesagglutinationResources> desagglutinationResourcesFactory(DESAGGLUTINATIONRESOURCES_CLASSID);

void DesagglutinationResources::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  DESAGGLOGINIT
  m_language = manager->getInitializationParameters().language;
  string resourcesPath=MediaticData::single().getResourcesPath();
  try
  {
    QString file = Common::Misc::findFileInPaths(resourcesPath.c_str(), unitConfiguration.getParamsValueAtKey("categoriesMappingFile").c_str());
    loadMicroCategoriesMappingFromFile(file.toUtf8().constData());
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LERROR << "no param 'categoriesMappingFile' in DesagglutinationResources group for language " << (int)m_language;
    throw InvalidConfiguration();
  }
  try
  {
    QString file=Common::Misc::findFileInPaths(resourcesPath.c_str(), unitConfiguration.getParamsValueAtKey("delimiterFile").c_str());
    loadDelimitersFromFile(file.toUtf8().constData());
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LERROR << "no param 'delimiterFile' in DesagglutinationResources group for language " << (int)m_language;
    throw InvalidConfiguration();
  }
}


void DesagglutinationResources::loadMicroCategoriesMappingFromFile(const std::string& microcategoriesMappingFile)
{
  DESAGGLOGINIT;

  LINFO << "load micro categories mapping from file " << microcategoriesMappingFile;
  const Common::PropertyCode::PropertyManager& microManager=
    static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyManager("MICRO");

  std::ifstream fin(microcategoriesMappingFile.data(), std::ifstream::binary);
  if (!fin)
  {
    LERROR << "can't open file : " << microcategoriesMappingFile;
    throw std::runtime_error("can't open file for desagglutinator microcategories");
  }
  while (!fin.eof())
  {
    std::string line;
    std::getline(fin,line);
    if (line.size()>0)
    {
      size_t currentIndex,nextStop;
      nextStop=line.find(" ");
      if (nextStop==std::string::npos)
      {
        LERROR << "invalid desagglutinator microcategories file";
        throw std::runtime_error("invalid desagglutinator microcategories file");
      }
      uint64_t cat= microManager.getPropertyValue(line.substr(0,nextStop));
      CompoundCategories compoundCats;
      currentIndex=nextStop+1;
      nextStop=line.find(" ",currentIndex);
      if (nextStop==std::string::npos)
      {
        LERROR << "invalid desagglutinator microcategories file";
        throw std::runtime_error("invalid desagglutinator microcategories file");
      }
      compoundCats.begin=microManager.getPropertyValue(line.substr(currentIndex,nextStop-currentIndex));
      currentIndex=nextStop+1;
      nextStop=line.find(" ",currentIndex);
      if (nextStop==std::string::npos)
      {
        LERROR << "invalid desagglutinator microcategories file";
        throw std::runtime_error("invalid desagglutinator microcategories file");
      }
      compoundCats.part=microManager.getPropertyValue(line.substr(currentIndex,nextStop-currentIndex));
      currentIndex=nextStop+1;
      compoundCats.end=microManager.getPropertyValue(line.substr(currentIndex,line.size()-currentIndex).data());

      LDEBUG << "read CategorieMapping : " << cat << " => " << compoundCats.begin << " - " << compoundCats.part << " - " << compoundCats.end;
      m_categoriesMapping.insert(std::make_pair(LinguisticCode(cat),compoundCats));
    }
  }

  fin.close();

  LINFO << "loaded " << m_categoriesMapping.size() << " microcategory mappings";
}

void DesagglutinationResources::loadDelimitersFromFile(const std::string& delimitersFile)
{

  DESAGGLOGINIT;
  LINFO << "load delimiters from file : " << delimitersFile;

  std::ifstream ifs (delimitersFile.c_str(), std::ifstream::binary);

  while(ifs.good()&&!ifs.eof())
  {
    Lima::LimaString del = Common::Misc::utf8stdstring2limastring(Lima::Common::Misc::readLine(ifs));
    m_delimiters.insert(del);
    LINFO << "read delimiter >" << Common::Misc::limastring2utf8stdstring(del) << "<";
  }

  ifs.close();
}



} // Desagglutination
} // MorphologicAnalysis
} // LinguisticProcessing
} // Lima
