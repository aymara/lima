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
/**              Implementation of Bigrams and Trigrams matrices  *
  * @file        ngramMatrices.cpp
  * @author      Benoit Mathieu <mathieub@zoe.cea.fr>

  *              Copyright (c) 2003 by CEA
  * @date        Created on  Oct, 27 2003
  *
  */

#include "ngramMatrices.h"
#include "common/tools/FileUtils.h"
#include "common/Data/strwstrtools.h"
#include "common/MediaticData/mediaticData.h"
#include "linguisticProcessing/common/linguisticData/languageData.h"
#include "linguisticProcessing/common/PropertyCode/PropertyCodeManager.h"
#include "linguisticProcessing/common/PropertyCode/PropertyManager.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"

#include <boost/regex.hpp>

#include <fstream>
#include <sstream>
#include <stdexcept>

using namespace std;
using namespace Lima::Common::MediaticData;

namespace Lima
{
namespace LinguisticProcessing
{
namespace PosTagger
{

SimpleFactory<AbstractResource,TrigramMatrix> trigramMatrixFactory(TRIGRAMMATRIX_CLASSID);

void TrigramMatrix::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  /** @addtogroup ResourceConfiguration
   * - <b>&lt;group name="..." class="TrigramMatrix"&gt;</b>
   *    -  trigramFile : file containing trigram matrix
   */
    
  PTLOGINIT;
  m_language = manager->getInitializationParameters().language;
  m_microManager=&( static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyManager("MICRO"));
  string resourcesPath=MediaticData::single().getResourcesPath();
  try
  {
    string trigramFile = Common::Misc::findFileInPaths(resourcesPath.c_str(), unitConfiguration.getParamsValueAtKey("trigramFile").c_str()).toUtf8().constData();
    readTrigramMatrixFile(trigramFile);
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LERROR << "no param 'trigramFile' in TrigramMatrix group for language " << (int)m_language;
    throw InvalidConfiguration();
  }
}

void TrigramMatrix::readTrigramMatrixFile(const std::string& fileName)
{
  PTLOGINIT;
  LINFO << "Loading trigrams matrix file: " << fileName;

  std::ifstream ifl;
  // Open the data file TriGramMatrix.dat in read mode
  ifl.open(fileName.c_str(), std::ifstream::binary);

  if (!ifl)
  {
    LERROR << "can't read trigrams from file " << fileName;
    throw  InvalidConfiguration();
  }

  boost::regex linere("^(.+)\t(.+)\t(.+)\t(\\d+(\\.\\d+)?)$");
  boost::regex numre("^\\d+$");

  std::string lineString = Lima::Common::Misc::readLine(ifl);
  size_t linenum(0);
  while (ifl.good() && !ifl.eof())
  {
    Common::Misc::chomp(lineString);
    linenum++;
    if ( (lineString.size() > 0) && (lineString[0] != '#') )
    {
      LinguisticCode trigram[3];
      std::string strigram[3];
      float proba;


      std::string::const_iterator start, end;
      start = lineString.begin();
      end = lineString.end();
      boost::match_results<std::string::const_iterator> what;
      //    boost::match_flag_type flags = boost::match_default;
      if (regex_search(start, end, what, linere))
      {
        for (unsigned count = 1; count <= 3; count++)
        {
          std::string elem(what[count].first,what[count].second);
          strigram[count-1] = elem;
          if (boost::regex_match(elem,numre))
          { // numerical element
            std::istringstream matchStream(elem);
            matchStream >> trigram[count];
          }
          else
          {
            trigram[count-1] = m_microManager->getPropertyValue(elem);
          }
        }
        std::string sproba(what[4].first,what[4].second);
        std::istringstream sprobaStream(sproba);
        sprobaStream >> proba;
      }
      else throw(std::runtime_error("invalid trigram line: " + linenum));

      //      LDEBUG << "Got trigram: ["<<strigram[0]<<";"<<strigram[1]<<";"<<strigram[2]<<"]/["<<trigram[0]<<";"<<trigram[1]<<";"<<trigram[2]<<"]";
      m_trigrams[trigram[0]][trigram[1]][trigram[2]] = proba;
    }
    lineString = Lima::Common::Misc::readLine(ifl);
  }
}

bool TrigramMatrix::exists(LinguisticCode cat1,LinguisticCode cat2,LinguisticCode cat3) const
{
  map< LinguisticCode, map<LinguisticCode, map<LinguisticCode,float> > >::const_iterator it1=m_trigrams.find(cat1);
  if (it1==m_trigrams.end()) return false;
  map<LinguisticCode, map<LinguisticCode,float> >::const_iterator it2=it1->second.find(cat2);
  if (it2==it1->second.end()) return false;
  return (it2->second.find(cat3)!=it2->second.end());
}

float TrigramMatrix::freq(LinguisticCode cat1,LinguisticCode cat2,LinguisticCode cat3) const
{
  map< LinguisticCode, map<LinguisticCode, map<LinguisticCode,float> > >::const_iterator it1=m_trigrams.find(cat1);
  if (it1==m_trigrams.end()) return 0;
  map<LinguisticCode, map<LinguisticCode,float> >::const_iterator it2=it1->second.find(cat2);
  if (it2==it1->second.end()) return 0;
  map<LinguisticCode, float>::const_iterator it3=it2->second.find(cat3);
  if (it3==it2->second.end()) return 0;
  return it3->second;
}

SimpleFactory<AbstractResource,BigramMatrix> bigramMatrixFactory(BIGRAMMATRIX_CLASSID);

void BigramMatrix::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  /** @addtogroup ResourceConfiguration
   * - <b>&lt;group name="..." class="BigramMatrix"&gt;</b>
   *    -  bigramFile : file containing trigram matrix
   */
  PTLOGINIT;
  m_language = manager->getInitializationParameters().language;
  m_microManager=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyManager("MICRO"));
  string resourcesPath=MediaticData::single().getResourcesPath();
  try
  {
    string trigramFile = Common::Misc::findFileInPaths(resourcesPath.c_str(), unitConfiguration.getParamsValueAtKey("bigramFile").c_str()).toUtf8().constData();
    readBigramMatrixFile(trigramFile);
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LERROR << "no param 'bigramFile' in BigramMatrix group for language " << (int)m_language;
    throw InvalidConfiguration();
  }
}

void  BigramMatrix::readBigramMatrixFile(const std::string& fileName)
{
  PTLOGINIT;
  std::ifstream ifl;

  // Open the data file BiGramMatrix.dat in read mode
  ifl.open(fileName.c_str(), std::ifstream::binary);

  if (!ifl)
  {
    // Standard error output
    LERROR << "can't read bigrams from file " << fileName;
    throw  InvalidConfiguration();
  }

  boost::regex linere("^(.+)\t(.+)\t(\\d+(\\.\\d+)?)$");
  boost::regex numre("^\\d+$");

  std::string lineString;
  size_t linenum(0);
  lineString = Lima::Common::Misc::readLine(ifl);
  while (ifl.good() && !ifl.eof())
  {
    Common::Misc::chomp(lineString);
    linenum++;

    LinguisticCode bigram[2];
    std::string sbigram[2];
    float proba;

    std::string::const_iterator start, end;
    start = lineString.begin();
    end = lineString.end();
    boost::match_results<std::string::const_iterator> what;
    //    boost::match_flag_type flags = boost::match_default;
    if (regex_search(start, end, what, linere))
    {
      for (unsigned count = 1; count <= 2; count++)
      {
        std::string elem(what[count].first,what[count].second);
        sbigram[count-1] = elem;
        if (boost::regex_match(elem,numre))
        { // numerical element
          std::istringstream matchStream(elem);
          matchStream >> bigram[count-1];
        }
        else
        {
          bigram[count-1] = m_microManager->getPropertyValue(elem);
        }
      }
      std::string sproba(what[3].first,what[3].second);
      std::istringstream sprobaStream(sproba);
      sprobaStream >> proba;
    }
    else throw(std::runtime_error("invalid bigram line: " + linenum));

    //    LDEBUG << "Got bigram: ["<<sbigram[0]<<";"<<sbigram[1]<<"]/["<<bigram[0]<<";"<<bigram[1]<<"]";
    m_bigrams[bigram[0]][bigram[1]] = proba;

    lineString = Lima::Common::Misc::readLine(ifl);
  }
}

bool BigramMatrix::exists(LinguisticCode cat1,LinguisticCode cat2) const
{
  map<LinguisticCode, map<LinguisticCode,float> >::const_iterator it1=m_bigrams.find(cat1);
  if (it1==m_bigrams.end()) return false;
  return (it1->second.find(cat2)!=it1->second.end());
}

float BigramMatrix::freq(LinguisticCode cat1,LinguisticCode cat2) const
{
  map<LinguisticCode, map<LinguisticCode,float> >::const_iterator it1=m_bigrams.find(cat1);
  if (it1==m_bigrams.end()) return 0;
  map<LinguisticCode,float>::const_iterator it2=it1->second.find(cat2);
  if (it2==it1->second.end()) return 0;
  return it2->second;
}

} // closing namespace PosTagger
} // closing namespace LinguisticProcessing
} // closing namespace Lima
