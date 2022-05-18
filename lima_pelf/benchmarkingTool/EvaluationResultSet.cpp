// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "EvaluationResultSet.h"
#include <boost/concept_check.hpp>
using namespace Lima::Pelf;

void EvaluationResultSet::replaceResult (EvaluationResult::DIMENSION_ID dimensionId, EvaluationResult* result)
{
  if (result != 0)
  {
//     qDebug() << "EvaluationResultSet::replaceResult  Set new result for" << dimensionId;
    if (contains(dimensionId)) delete (*this)[dimensionId];
    (*this)[dimensionId] = result;
  }
}

void EvaluationResultSet::findEvaluationResults(QString cmdOutput)
{
//   qDebug() << "EvaluationResultSet::findEvaluationResults" << cmdOutput;
    QMap<EvaluationResult::DIMENSION_ID, EvaluationResultDimension*>& dimensions = EvaluationResult::getDimensions();
    Q_FOREACH(EvaluationResult::DIMENSION_ID dimensionId, dimensions.keys())
    {
        EvaluationResultDimension* dimension = dimensions[dimensionId];
        if(dimension != 0)
            replaceResult(dimensionId, findResultsType(cmdOutput, dimension->key, dimension->name));
    }
}

EvaluationResult* EvaluationResultSet::findResultsType(QString cmdOutput, QString typeKey, QString typeName)
{
//     qDebug() << "EvaluationResultSet::findResultsType  typeKey=" << typeKey << "typeName=" << typeName;
    QString resultsRegexpOld = "\\s+p=((0|1)?\\.?\\d+)\\s+r=((0|1)?\\.?\\d+)\\s+f=((0|1)?\\.?\\d+)";
    QString resultsRegexp = "\\s+fc=(\\d+)\\s+fp=(\\d+)\\s+cr=(\\d+)";
    QRegExp allRelationsRegexp(typeKey+resultsRegexpOld+resultsRegexp);
//     qDebug() << "EvaluationResultSet::findResultsType regexp=" << allRelationsRegexp.pattern();
    int allRelationsMatchIndex = allRelationsRegexp.indexIn(cmdOutput);
    EvaluationResult* result = 0;
//     qDebug() << "EvaluationResultSet::findResultsType allRelationsMatchIndex=" << allRelationsMatchIndex;
    if (allRelationsMatchIndex != -1)
    {
        QStringList allRelationsResults = allRelationsRegexp.capturedTexts();
//         qDebug() << "EvaluationResultSet::findResultsType allRelationsResults size=" << allRelationsResults.size();
        double fc = -1, fp = -1, cr = -1;
        if (allRelationsResults.size()>=10) cr = allRelationsResults[9].toDouble();
        if (allRelationsResults.size()>=9) fp = allRelationsResults[8].toDouble();
        if (allRelationsResults.size()>=8) fc = allRelationsResults[7].toDouble();
//         qDebug() << "EvaluationResultSet::findResultsType" << typeKey << "fc=" << fc << "fp=" << fp << "cr=" << cr;
        if(fc != -1 && fp != -1 && cr!= -1)
        {
///@TODO Possible memory leak:  ensure that this object will be deleted
          result = new EvaluationResult();
          result->fc = fc;
          result->fp = fp;
          result->cr = cr;

          // Go through the results again in order to store places of individual errors
          QRegExp resultsGroupsRegexp("(A|F)\\|"+typeName+"\\|([^|]*)\\|([^|]*)\\n");
          int resultsGroupsRegexpMatchIndex = 0;
          while((resultsGroupsRegexpMatchIndex = resultsGroupsRegexp.indexIn(cmdOutput, resultsGroupsRegexpMatchIndex)) != -1)
          {
            QStringList resultsGroupsResults = resultsGroupsRegexp.capturedTexts();
            //qDebug() << "resultsGroupsRegexpMatchIndex = " << resultsGroupsRegexpMatchIndex << resultsGroupsResults[1] << resultsGroupsResults[2] << resultsGroupsResults[3] ;
            if (resultsGroupsResults[1] == "A")
            {
              //qDebug() << "Adding a HypAbsentFromRef " << resultsGroupsResults[2] << " " << resultsGroupsResults[3];
              result->getRefAbsentFromHyp().insert(resultsGroupsResults[2],resultsGroupsResults[3]);
            }
            else if (resultsGroupsResults[1] == "F")
            {
              //qDebug() << "Adding a RefAbsentFromHyp " << resultsGroupsResults[2] << " " << resultsGroupsResults[3];
              result->getHypAbsentFromRef().insert(resultsGroupsResults[2],resultsGroupsResults[3]);
            }
            resultsGroupsRegexpMatchIndex += resultsGroupsRegexp.matchedLength();
          }
        }
        allRelationsMatchIndex += allRelationsRegexp.matchedLength();
    }
    return result;
}
