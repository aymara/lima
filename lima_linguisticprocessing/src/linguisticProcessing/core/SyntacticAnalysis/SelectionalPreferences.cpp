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
/**
  *
  * @file        SelectionalPreferences.cpp
  * @author      Gael de Chalendar (Gael.de-Chalendar@cea.fr)
  * @date        Created on  : Sat May 22 2010
  *              Copyright   : (c) 2010 by CEA
  * @version     $Id:  $
  *
  */

#include "SelectionalPreferences.h"
#include "common/Data/strwstrtools.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "common/MediaticData/mediaticData.h"
#include "linguisticProcessing/common/PropertyCode/PropertyCodeManager.h"
#include "linguisticProcessing/common/linguisticData/languageData.h"

#include <fstream>

using namespace Lima::Common::PropertyCode;

namespace Lima
{
namespace LinguisticProcessing
{
namespace SyntacticAnalysis
{

SimpleFactory<AbstractResource,SelectionalPreferences> SelectionalPreferencesFactory(SELECTIONALPREFERENCES_CLASSID);


SelectionalPreferences::SelectionalPreferences() :
    m_language()
{
}

SelectionalPreferences::~SelectionalPreferences() {}


void SelectionalPreferences::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  SALOGINIT;
  LDEBUG << "Creating a SelectionalPreferences (loads file)" << LENDL;
  m_language=manager->getInitializationParameters().language;
  
  try {
    std::string resourcePath=Common::MediaticData::MediaticData::single().getResourcesPath();
    std::string preferencesFileName=resourcePath + "/" + unitConfiguration.getParamsValueAtKey("file");
    loadFromFile(preferencesFileName);
  
  } catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LERROR << "no parameter 'file' in SelectionalPreferences group for language " << (int) m_language << " !" << LENDL;
    throw InvalidConfiguration();
  }

//  display();
}


void SelectionalPreferences::loadFromFile(const std::string& fileName)
{
  SALOGINIT;
  LDEBUG << "Loading selectional preferences from " << fileName << LENDL;
  std::ifstream ifl(fileName.c_str(), std::ifstream::binary);
  size_t linesCounter = 0;
  
  const PropertyManager& macroManager = static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyManager("MACRO");
  if (!ifl)
  {
    LWARN << "Selectional preferences definition file not found: " << fileName << LENDL;
    return;
  }
  
  std::string line;
  getline(ifl, line);
  Common::Misc::chomp(line);
  linesCounter++;
  while (ifl.good() && !ifl.eof())
  {
    if ( (line != "") && (line[0] != '#') )
    {
      LDEBUG << "read line : " << line << LENDL;
      
      size_t index = 0;
      
      size_t next = line.find_first_of(";", index);
      if (next == std::string::npos)
      {
        LERROR << "Cannot find first delimiter in line: " << line << LENDL;
        throw InvalidConfiguration();
      }
      std::string target = line.substr(0, next);
      
      index = next+1;
      next=line.find_first_of(";",index);
      if (next == std::string::npos)
      {
        LERROR << "Cannot find second delimiter in line: " << line << LENDL;
        throw InvalidConfiguration();
      }
      LinguisticCode targetMacro = macroManager.getPropertyValue(line.substr(index, next-index));
      
      index = next+1;
      next=line.find_first_of(";",index);
      if (next == std::string::npos)
      {
        LERROR << "Cannot find third delimiter in line: " << line << LENDL;
        throw InvalidConfiguration();
      }
      std::string dependency = line.substr(index, next-index);
      
      index = next+1;
      next=line.find_first_of(";",index);
      if (next == std::string::npos)
      {
        LERROR << "Cannot find fourth delimiter in line: " << line << LENDL;
        throw InvalidConfiguration();
      }
      std::string source = line.substr(index, next-index);
      
      index = next+1;
      next=line.find_first_of(";",index);
      if (next == std::string::npos)
      {
        LERROR << "Cannot find fifth delimiter in line: " << line << LENDL;
        throw InvalidConfiguration();
      }
      LinguisticCode soureceMacro = macroManager.getPropertyValue(line.substr(index, next-index));
      
      index = next+1;
      double probability = 0.0;
      std::string probabilityString = line.substr(index);
      std::istringstream probabilityStringIss(probabilityString);
      probabilityStringIss >> probability;

      boost::tuple< std::string, LinguisticCode, std::string, std::string, LinguisticCode > tuple(target,targetMacro,dependency,source,soureceMacro);
      m_preferences.insert(std::make_pair(tuple, probability));
    }
    getline(ifl, line);
    Common::Misc::chomp(line);
    linesCounter++;
  }
}

double SelectionalPreferences::dependencyProbability(std::string target, LinguisticCode targetCode, std::string dependency, std::string source, LinguisticCode sourceCode) const
{
  SALOGINIT;
  boost::tuple< std::string, LinguisticCode, std::string, std::string, LinguisticCode > tuple = boost::make_tuple(target, targetCode, dependency, source, sourceCode);
  std::map< boost::tuple< std::string, LinguisticCode, std::string, std::string, LinguisticCode >, double, compareTuple >::const_iterator preferencesIt = m_preferences.find(tuple);
  if (preferencesIt == m_preferences.end())
  {
    return 0.0;
  }
  LDEBUG << "Probability of (" << target << ", " << targetCode << ", " << dependency << ", " << source << ", " << sourceCode << ") is " << (*preferencesIt).second << LENDL;
  return (*preferencesIt).second;
}

bool SelectionalPreferences::compareTuple::operator()(
    const boost::tuple< std::string, LinguisticCode, std::string, std::string, LinguisticCode >& t1,
    const boost::tuple< std::string, LinguisticCode, std::string, std::string, LinguisticCode >& t2) const
{
  return (t1.get<0>() < t2.get<0>()
  || ( (t1.get<0>() == t2.get<0>()) && (t1.get<1>() < t2.get<1>()))
  || ( (t1.get<0>() == t2.get<0>()) && (t1.get<1>() == t2.get<1>()) && (t1.get<2>() < t2.get<2>()))
  || ( (t1.get<0>() == t2.get<0>()) && (t1.get<1>() == t2.get<1>()) && (t1.get<2>() == t2.get<2>()) && (t1.get<3>() < t2.get<3>()))
  || ( (t1.get<0>() == t2.get<0>()) && (t1.get<1>() == t2.get<1>()) && (t1.get<2>() == t2.get<2>()) && (t1.get<3>() == t2.get<3>()) && (t1.get<4>() < t2.get<4>()))
  );
}

} // namespace SyntacticAnalysis
} // namespace LinguisticProcessing
} // namespace Lima
