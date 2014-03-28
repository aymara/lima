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
