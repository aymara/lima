// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
#include "common/tools/FileUtils.h"
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
  LDEBUG << "Creating a SelectionalPreferences (loads file)";
  m_language=manager->getInitializationParameters().language;
  
  try {
    std::string resourcePath=Common::MediaticData::MediaticData::single().getResourcesPath();
    std::string preferencesFileName = Common::Misc::findFileInPaths(resourcePath.c_str(), unitConfiguration.getParamsValueAtKey("file").c_str()).toUtf8().constData();
    loadFromFile(preferencesFileName);
  
  } catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LERROR << "no parameter 'file' in SelectionalPreferences group for language " << (int) m_language << " !";
    throw InvalidConfiguration();
  }

//  display();
}


void SelectionalPreferences::loadFromFile(const std::string& fileName)
{
  SALOGINIT;
  LDEBUG << "Loading selectional preferences from " << fileName;
  std::ifstream ifl(fileName.c_str(), std::ifstream::binary);
  size_t linesCounter = 0;
  
  const PropertyManager& macroManager = static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyManager("MACRO");
  if (!ifl)
  {
    LWARN << "Selectional preferences definition file not found: " << fileName;
    return;
  }
  
  std::string line = Lima::Common::Misc::readLine(ifl);
  Common::Misc::chomp(line);
  linesCounter++;
  while (ifl.good() && !ifl.eof())
  {
    if ( (line != "") && (line[0] != '#') )
    {
      LDEBUG << "read line : " << line;
      
      size_t index = 0;
      
      size_t next = line.find_first_of(";", index);
      if (next == std::string::npos)
      {
        LERROR << "Cannot find first delimiter in line: " << line;
        throw InvalidConfiguration();
      }
      std::string target = line.substr(0, next);
      
      index = next+1;
      next=line.find_first_of(";",index);
      if (next == std::string::npos)
      {
        LERROR << "Cannot find second delimiter in line: " << line;
        throw InvalidConfiguration();
      }
      LinguisticCode targetMacro = macroManager.getPropertyValue(line.substr(index, next-index));
      
      index = next+1;
      next=line.find_first_of(";",index);
      if (next == std::string::npos)
      {
        LERROR << "Cannot find third delimiter in line: " << line;
        throw InvalidConfiguration();
      }
      std::string dependency = line.substr(index, next-index);
      
      index = next+1;
      next=line.find_first_of(";",index);
      if (next == std::string::npos)
      {
        LERROR << "Cannot find fourth delimiter in line: " << line;
        throw InvalidConfiguration();
      }
      std::string source = line.substr(index, next-index);
      
      index = next+1;
      next=line.find_first_of(";",index);
      if (next == std::string::npos)
      {
        LERROR << "Cannot find fifth delimiter in line: " << line;
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
    line = Lima::Common::Misc::readLine(ifl);
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
  LDEBUG << "Probability of (" << target << ", " << targetCode << ", " << dependency << ", " << source << ", " << sourceCode << ") is " << (*preferencesIt).second;
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
