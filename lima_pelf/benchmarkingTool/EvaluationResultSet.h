// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_PELF_EVALUATIONRESULTSET_H
#define LIMA_PELF_EVALUATIONRESULTSET_H

#include <QtCore/QtDebug>
#include <QtCore/QRegularExpression>
#include <QtCore/QStringList>

#include "EvaluationResult.h"

namespace Lima {
namespace Pelf {

class EvaluationResultSet : public QMap<EvaluationResult::DIMENSION_ID, EvaluationResult*>
{

public:
    void replaceResult (EvaluationResult::DIMENSION_ID dimensionId, EvaluationResult* result);
    void findEvaluationResults(QString cmdOutput);
    EvaluationResult* findResultsType(QString cmdOutput, QString typeKey, QString typeName);

};

} // End namespace Lima
} // End namespace Pelf

#endif // LIMA_PELF_EVALUATIONRESULTSET_H
