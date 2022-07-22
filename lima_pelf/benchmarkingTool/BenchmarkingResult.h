// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_PELF_BECNHMARKINGRESULT_H
#define LIMA_PELF_BECNHMARKINGRESULT_H

#include <QtCore/QtDebug>
#include <QtCore/QDateTime>

#include "PipelineUnit.h"
#include "EvaluationResult.h"

namespace Lima {
namespace Pelf {

class BenchmarkingResult
{

public:

    QDateTime time;
    QString comment;
    QMap<PipelineUnit*, EvaluationResultSet*> resultUnits;

    BenchmarkingResult ();

};

} // End namespace Lima
} // End namespace Pelf

#endif // LIMA_PELF_BECNHMARKINGRESULT_H
