/*
 *    Copyright 2002-2013 CEA LIST
 * 
 *    This file is part of LIMA.
 * 
 *    LIMA is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Affero General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 * 
 *    LIMA is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Affero General Public License for more details.
 * 
 *    You should have received a copy of the GNU Affero General Public License
 *    along with LIMA.  If not, see <http://www.gnu.org/licenses/>
 */
#include "EvaluationResultSet.h"
using namespace Lima::Pelf;

void EvaluationResultSet::replaceResult (EvaluationResult::DIMENSION_ID dimensionId, EvaluationResult* result)
{
    if(result != 0)
        (*this)[dimensionId] = result;
}

void EvaluationResultSet::findEvaluationResults(QString cmdOutput)
{
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
    qDebug() << "EvaluationResultSet::findResultsType  typeKey=" << typeKey.split(" ").back();
    QString resultsRegexpOld = "\\s+p=((0|1)?\\.?\\d+)\\s+r=((0|1)?\\.?\\d+)\\s+f=((0|1)?\\.?\\d+)";
    QString resultsRegexp = "\\s+fc=(\\d+)\\s+fp=(\\d+)\\s+cr=(\\d+)";
    QRegExp allRelationsRegexp(typeKey+resultsRegexpOld+resultsRegexp);
    int allRelationsMatchIndex = 0;
    EvaluationResult* result = 0;
    while((allRelationsMatchIndex = allRelationsRegexp.indexIn(cmdOutput, allRelationsMatchIndex)) != -1)
    {
        QStringList allRelationsResults = allRelationsRegexp.capturedTexts();
        QStringList::iterator allRelationsResultsIt = allRelationsResults.begin();
        int resultCount = 0;
        double fc = -1, fp = -1, cr = -1;
        while(allRelationsResultsIt != allRelationsResults.end())
        {
            switch(resultCount)
            {
                case 7:
                    fc = (*allRelationsResultsIt).toDouble();
                    break;
                case 8:
                    fp = (*allRelationsResultsIt).toDouble();
                    break;
                case 9:
                    cr = (*allRelationsResultsIt).toDouble();
                    break;
            }
            ++allRelationsResultsIt;
            ++resultCount;
        }
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
