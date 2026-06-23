// Copyright 2002-2020 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "viterbiUtils.h"
#include <QRegularExpression>
#include <QString>
#include <iostream>

namespace Lima {

namespace LinguisticProcessing {

namespace PosTagger {


template<typename Cost, typename CostFactory>
ViterbiCostFunction<Cost,CostFactory>::ViterbiCostFunction(
    MediaId language,
    const Common::PropertyCode::PropertyManager* microManager,
    const CostFactory& costFactory,
    const std::string& trigramsFile,
    const std::string& bigramsFile)
{
  m_language=language;
  m_microManager=microManager;
  m_minimumCost=costFactory.getMinimumCost();
  m_maximumCost=costFactory.getMaximumCost();
  m_unigramCost=costFactory.getCost(false,false,true,0);
  readBigramMatrixFile(bigramsFile,costFactory);
  readTrigramMatrixFile(trigramsFile,costFactory);
  sortData();
}


template<typename Cost, typename CostFactory> template<typename PredData>
void ViterbiCostFunction<Cost,CostFactory>::apply(
  LinguisticCode micro,
  PredData& predData,
  const std::vector<PredData>& predPredData) const
{
//  PTLOGINIT;
//  std::ostringstream os;
//  for (typename std::vector<PredData>::const_iterator it=predPredData.begin();
//       it!=predPredData.end();
//       it++)
//       {
//         os << it->m_predMicro << ",";
//       }
//  LDEBUG << "apply cost on " << micro << " " << predData.m_predMicro << " | " << os.str();
  predData.m_cost=m_maximumCost;

  typename std::map<LinguisticCode, std::map<LinguisticCode, GramsData> >::const_iterator microItr=m_data.find(micro);
  if (microItr!=m_data.end())
  {
    typename std::map<LinguisticCode, GramsData>::const_iterator predMicroItr=microItr->second.find(predData.m_predMicro);
    if (predMicroItr!=microItr->second.end())
    {
      // this could be either a bigram or a trigram. how can I know?
      // 1/ look at every pred-pred-microdata
      // 2/ match this with trigrams, and see if there's a match

      typename std::vector<PredData>::const_iterator predPredDataItr=predPredData.begin();
      typename std::vector<CategCostPair>::const_iterator gramItr=predMicroItr->second.m_grams.begin();

      /* We want to check the matches between two sorted collections. Let's use an exemple:
       *  - predPredDataItr: 5 7 10
       *  - gramItr: 1 3 4 5 8 10 134
       * (note that LinguisticCodes are only integers)
       * We want to know that 5 10 match and that 7 doesn't match in linear time.
       */
      while ((gramItr!=predMicroItr->second.m_grams.end()) && (predPredDataItr!=predPredData.end()))
      {
        if (gramItr->m_cat < predPredDataItr->m_predMicro ) {
          /* not interesting, let's move on */
          gramItr++;
        } else if (gramItr->m_cat > predPredDataItr->m_predMicro) {
          /* this is a non-match: backoff to a bigram */
//          LDEBUG << "trigram : " <<
//            m_microManager->getPropertySymbolicValue(predPredDataItr->m_predMicro) << " " <<
//            m_microManager->getPropertySymbolicValue(predData.m_predMicro) << " " <<
//            m_microManager->getPropertySymbolicValue(micro) << " doesn't exists ";
          Cost tmp=predPredDataItr->m_cost + predMicroItr->second.m_default;
//          LDEBUG << "cost for bigram is + " << predMicroItr->second.m_default << " = " << tmp;
          if (tmp<predData.m_cost) {
//            LDEBUG << "cost is better";
            predData.m_predPredMicros.clear();
            predData.m_predPredMicros.push_back(predPredDataItr->m_predMicro);
            predData.m_cost=tmp;
          } else if (tmp==predData.m_cost) {
//            LDEBUG << "cost is equal";
            predData.m_predPredMicros.push_back(predPredDataItr->m_predMicro);
          }
          predPredDataItr++;
        } else {
          /* this is a match */
//          LDEBUG << "found trigram : " <<
//            m_microManager->getPropertySymbolicValue(predPredDataItr->m_predMicro) << " " <<
//            m_microManager->getPropertySymbolicValue(predData.m_predMicro) << " " <<
//            m_microManager->getPropertySymbolicValue(micro) << " ! ";
          Cost tmp=predPredDataItr->m_cost + gramItr->m_cost;
//          LDEBUG << "cost for this trigram is " << gramItr->m_cost << " => " << tmp;
          if (tmp<predData.m_cost) {
//            LDEBUG << "cost is better";
            predData.m_predPredMicros.clear();
            predData.m_predPredMicros.push_back(predPredDataItr->m_predMicro);
            predData.m_cost=tmp;
          } else if (tmp==predData.m_cost) {
//            LDEBUG << "cost is equal";
            predData.m_predPredMicros.push_back(predPredDataItr->m_predMicro);
          }
          predPredDataItr++;
          gramItr++;
        }
      }
      // on check les derniers preds pour le cout du bigram
      while (predPredDataItr!=predPredData.end())
      {
//          LDEBUG << "trigram : " <<
//            m_microManager->getPropertySymbolicValue(predPredDataItr->m_predMicro) << " " <<
//            m_microManager->getPropertySymbolicValue(predData.m_predMicro) << " " <<
//            m_microManager->getPropertySymbolicValue(micro) << " doesn't exists ";
          Cost tmp=predPredDataItr->m_cost + predMicroItr->second.m_default;
//          LDEBUG << "cost for bigram is " << predMicroItr->second.m_default << " => " << tmp;
          if (tmp<predData.m_cost) {
//            LDEBUG << "cost is better";
            predData.m_predPredMicros.clear();
            predData.m_predPredMicros.push_back(predPredDataItr->m_predMicro);
            predData.m_cost=tmp;
          } else if (tmp==predData.m_cost) {
//            LDEBUG << "cost is equal";
            predData.m_predPredMicros.push_back(predPredDataItr->m_predMicro);
          }
          predPredDataItr++;
      }
    }
  }

  /* if we didn't find anything (cost didn't change): backoff to unigrams */
  if (predData.m_cost==m_maximumCost)
  {
    // on utilise le cout de l'unigram + le cout le plus faible
    typename std::vector<PredData>::const_iterator predPredDataItr=predPredData.begin();
    for (;predPredDataItr!=predPredData.end();predPredDataItr++)
    {
      Cost tmp=predPredDataItr->m_cost + m_unigramCost;
      if (tmp<predData.m_cost)
      {
//        LDEBUG << "better cost " << tmp;
        predData.m_cost=tmp;
        predData.m_predPredMicros.clear();
        predData.m_predPredMicros.push_back(predPredDataItr->m_predMicro);
      } else if (tmp==predData.m_cost) {
        predData.m_predPredMicros.push_back(predPredDataItr->m_predMicro);
      }
    }
  }
}


template<typename Cost, typename CostFactory>
void ViterbiCostFunction<Cost,CostFactory>::readTrigramMatrixFile(
  const std::string& fileName,
  const CostFactory& costFactory)
{
  PTLOGINIT;
  LINFO << "Loading trigrams matrix file: " << fileName;

  std::ifstream ifl;
  // Open the data file TriGramMatrix.dat in read mode
  ifl.open(fileName.c_str());

  if (!ifl)
  {
    LERROR << "can't read trigrams from file " << fileName;
    throw  InvalidConfiguration();
  }

  QRegularExpression linere("^(.+)\t(.+)\t(.+)\t(\\d+(\\.\\d+)?)$");
  QRegularExpression numre("^\\d+$");

  std::string lineString;
  size_t linenum = 0;

  while (std::getline(ifl, lineString))
  {
      Common::Misc::chomp(lineString);
      linenum++;
      if (!lineString.empty() && lineString[0] != '#')
      {
          LinguisticCode trigram[3];
          float proba;
          QRegularExpressionMatch match = linere.match(QString::fromStdString(lineString));
          if (match.hasMatch())
          {
              QStringList strigram;

              for (int count = 1; count <= 3; count++)
              {
                  QString elem = match.captured(count);
                  strigram << elem;
                  if (numre.match(elem).hasMatch())
                  {
                      // Numerical element
                      trigram[count - 1] = LinguisticCode::fromString(elem.toStdString());
                  }
                  else
                  {
                      // Assuming m_microManager is an instance of some class
                      trigram[count - 1] = m_microManager->getPropertyValue(elem.toStdString());
                  }
              }

              QString sproba = match.captured(4);
              proba = sproba.toFloat();
          }
          else throw(std::runtime_error(QString::fromUtf8("invalid trigram line: %1").arg(linenum).toUtf8().constData()));


      GramsData& gd=m_data[trigram[2]][trigram[1]];
      CategCostPair cpp;
      cpp.m_cat=trigram[0];
      // retrieve bigram freq
      float biFreq=0;
      {
        std::map<LinguisticCode, std::map<LinguisticCode, float> >::const_iterator bi1Itr=m_biFreq.find(trigram[0]);
        if (bi1Itr!=m_biFreq.end()) {
          std::map<LinguisticCode, float>::const_iterator bi2Itr=bi1Itr->second.find(trigram[1]);
          if (bi2Itr!=bi1Itr->second.end())
          {
            biFreq=bi2Itr->second;
          }
        }
      }

      if (biFreq>0) {
        cpp.m_cost=costFactory.getCost(true,false,false,proba/biFreq);
//        LDEBUG << "Got trigram: ["<<strigram[0]<<";"<<strigram[1]<<";"<<strigram[2]<<"]/["<<trigram[0]<<";"<<trigram[1]<<";"<<trigram[2]<<"] proba=" << proba/biFreq;
      } else {
//        LDEBUG << "Got trigram: ["<<strigram[0]<<";"<<strigram[1]<<";"<<strigram[2]<<"]/["<<trigram[0]<<";"<<trigram[1]<<";"<<trigram[2]<<"] proba=" << proba;
        cpp.m_cost=costFactory.getCost(true,false,false,proba);
      }
      gd.m_grams.push_back(cpp);

      // test si trigram 2 1 existe, si non il faut mettre default � unigram
      {
        bool found=false;
        std::map<LinguisticCode, std::map<LinguisticCode, float> >::const_iterator bi1Itr=m_biFreq.find(trigram[1]);
        if (bi1Itr!=m_biFreq.end()) {
          std::map<LinguisticCode, float>::const_iterator bi2Itr=bi1Itr->second.find(trigram[2]);
          if (bi2Itr!=bi1Itr->second.end())
          {
            found=true;
          }
        }
        if (!found) {
          gd.m_default=m_unigramCost;
        }
      }
    }
    lineString = Lima::Common::Misc::readLine(ifl);
  }
}

template<typename Cost, typename CostFactory>
void ViterbiCostFunction<Cost,CostFactory>::readBigramMatrixFile(
  const std::string& fileName,
  const CostFactory& costFactory)
{
  PTLOGINIT;
  std::ifstream ifl;

  // Open the data file BiGramMatrix.dat in read mode
  ifl.open(fileName.c_str());

  if (!ifl)
  {
    // Standard error output
    LERROR << "can't read bigrams from file " << fileName;
    throw  InvalidConfiguration();
  }

QRegularExpression linere("^(.+)\t(.+)\t(\\d+(\\.\\d+)?)$");
QRegularExpression numre("^\\d+$");

std::string lineString;
size_t linenum = 0;

while (std::getline(ifl, lineString))
{
    LinguisticCode bigram[2];
    float proba;
    Common::Misc::chomp(lineString);
    linenum++;
    if (!lineString.empty())
    {
        QRegularExpressionMatch match = linere.match(QString::fromStdString(lineString));
        if (match.hasMatch())
        {
            QStringList sbigram;

            for (int count = 1; count <= 2; count++)
            {
                QString elem = match.captured(count);
                sbigram << elem;
                if (numre.match(elem).hasMatch())
                {
                    // Numerical element
                    bigram[count - 1] = LinguisticCode::fromString(elem.toStdString());
                }
                else
                {
                    // Assuming m_microManager is an instance of some class
                    bigram[count - 1] = m_microManager->getPropertyValue(elem.toStdString());
                }
            }

            QString sproba = match.captured(3);
            proba = sproba.toFloat();
        }
        else
        {
            std::ostringstream oss;
            oss << "Invalid bigram line: '" << lineString << "' at line " << linenum;
            throw std::runtime_error(oss.str());
        }
    }

//    LDEBUG << "Got bigram: ["<<sbigram[0]<<";"<<sbigram[1]<<"]/["<<bigram[0]<<";"<<bigram[1]<<"] proba=" << proba;

    GramsData& gd=m_data[bigram[1]][bigram[0]];
    gd.m_default=costFactory.getCost(false,true,false,proba);
    m_biFreq[bigram[0]][bigram[1]]=proba;
    lineString = Lima::Common::Misc::readLine(ifl);
  }
}

/* This function sorts the CategCostPair by LinguisticCode.
 * The fact that it is sorted is used later in ViterbiCostFunction::apply()
 */
template<typename Cost, typename CostFactory>
void ViterbiCostFunction<Cost,CostFactory>::sortData()
{
  for (typename std::map<LinguisticCode, std::map<LinguisticCode, GramsData> >::iterator cat1Itr=m_data.begin();
       cat1Itr!=m_data.end();
       cat1Itr++)
       {
        for (typename std::map<LinguisticCode, GramsData>::iterator cat2Itr=cat1Itr->second.begin();
             cat2Itr!=cat1Itr->second.end();
             cat2Itr++)
             {
                sort(cat2Itr->second.m_grams.begin(),cat2Itr->second.m_grams.end());
             }
       }
}


} // PosTagger
} // LinguisticProcessing
} // Lima
