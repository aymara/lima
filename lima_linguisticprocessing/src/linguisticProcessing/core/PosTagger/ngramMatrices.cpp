// Copyright 2002-2020 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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

#include <QRegularExpression>
#include <QString>
#include <iostream>

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

  QRegularExpression linere("^(.+)\t(.+)\t(.+)\t(\\d+(\\.\\d+)?)$");
  QRegularExpressionMatch what;
  QRegularExpression numre("^\\d+$");

  auto lineString = QString::fromStdString(
    Lima::Common::Misc::readLine(ifl)).trimmed();
  size_t linenum(0);
  while (ifl.good() && !ifl.eof())
  {
    linenum++;
    if ( (lineString.size() > 0) && (lineString[0] != '#') )
    {
      LinguisticCode trigram[3];
      // QString strigram[3];
      float proba;

      what = linere.match(lineString);
      if (what.hasMatch())
      {
          for (int count = 1; count <= 3; ++count)
          {
            QString elem = what.captured(count);
            // strigram[count-1] = elem;
            auto numreMatch = numre.match(elem);
            if (numreMatch.hasMatch())
            {
              trigram[count-1] = LinguisticCode::fromString(elem.toStdString());
            }
            else
            {
              trigram[count-1] = m_microManager->getPropertyValue(elem.toStdString());
            }
          }

          QString sproba = what.captured(4);
          bool ok;
          proba = sproba.toDouble(&ok);
          if (!ok)
          {
            PTLOGINIT;
            LERROR << "Invalid trigram line " << linenum << " in: " << fileName;
            throw(std::runtime_error(
              QString::fromUtf8(
                "invalid trigram line: %1").arg(linenum).toUtf8().constData()));
          }
      }
      else
      {
          PTLOGINIT;
          LERROR << "Invalid trigram line " << linenum << " in: " << fileName;
          throw(std::runtime_error(QString::fromUtf8("invalid trigram line: %1").arg(linenum).toUtf8().constData()));
      }

      //      LDEBUG << "Got trigram: ["<<strigram[0]<<";"<<strigram[1]<<";"<<strigram[2]<<"]/["<<trigram[0]<<";"<<trigram[1]<<";"<<trigram[2]<<"]";
      m_trigrams[trigram[0]][trigram[1]][trigram[2]] = proba;
    }
    lineString = QString::fromStdString(
      Lima::Common::Misc::readLine(ifl)).trimmed();
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

void BigramMatrix::readBigramMatrixFile(const std::string& fileName)
{
  PTLOGINIT;
  LINFO << "Loading bigrams matrix file: " << fileName;

  std::ifstream ifl;
  // Open the data file TriGramMatrix.dat in read mode
  ifl.open(fileName.c_str(), std::ifstream::binary);

  if (!ifl)
  {
    LERROR << "can't read trigrams from file " << fileName;
    throw  InvalidConfiguration();
  }

  QRegularExpression linere("^(.+)\t(.+)\t(\\d+(\\.\\d+)?)$");
  QRegularExpressionMatch what;
  QRegularExpression numre("^\\d+$");

  auto lineString = QString::fromStdString(
    Lima::Common::Misc::readLine(ifl)).trimmed();
  size_t linenum(0);
  while (ifl.good() && !ifl.eof())
  {
    linenum++;
    if ( (lineString.size() > 0) && (lineString[0] != '#') )
    {
      LinguisticCode bigram[2];
      // QString sbigram[3];
      float proba;

      what = linere.match(lineString);
      if (what.hasMatch())
      {
          for (int count = 1; count <= 2; ++count)
          {
            QString elem = what.captured(count);
            // sbigram[count-1] = elem;
            auto numreMatch = numre.match(elem);
            if (numreMatch.hasMatch())
            {
              bigram[count-1] = LinguisticCode::fromString(elem.toStdString());
            }
            else
            {
              bigram[count-1] = m_microManager->getPropertyValue(elem.toStdString());
            }
          }

          QString sproba = what.captured(3);
          bool ok;
          proba = sproba.toDouble(&ok);
          if (!ok)
          {
            PTLOGINIT;
            LERROR << "Invalid bigram line " << linenum << " in: " << fileName;
            throw(std::runtime_error(
              QString::fromUtf8(
                "invalid bigram line: %1").arg(linenum).toUtf8().constData()));
          }
      }
      else
      {
          PTLOGINIT;
          LERROR << "Invalid bigram line " << linenum << " in: " << fileName;
          throw(std::runtime_error(QString::fromUtf8("invalid bigram line: %1").arg(linenum).toUtf8().constData()));
      }

      //      LDEBUG << "Got bigram: ["<<sbigram[0]<<";"<<sbigram[1]<<"]/["<<bigram[0]<<";"<<bigram[1]<<"]";
      m_bigrams[bigram[0]][bigram[1]] = proba;
    }
    lineString = QString::fromStdString(
      Lima::Common::Misc::readLine(ifl)).trimmed();
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
