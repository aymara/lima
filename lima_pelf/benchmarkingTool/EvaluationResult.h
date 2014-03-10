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
#ifndef LIMA_PELF_EVALUATIONRESULT_H
#define LIMA_PELF_EVALUATIONRESULT_H

#include <QtCore/QtDebug>
#include <QtGui/QColor>

#include "EvaluationResultDimension.h"

namespace Lima {
namespace Pelf {

class EvaluationResult
{

public:

    enum DIMENSION_ID {
        DIMENSION_ID_CONST_ALL = 0,
        DIMENSION_ID_REL_ALL = 1,
        DIMENSION_ID_CONST_GN = 2,
        DIMENSION_ID_CONST_NV = 3,
        DIMENSION_ID_CONST_GA = 4,
        DIMENSION_ID_CONST_GR = 5,
        DIMENSION_ID_CONST_GP = 6,
        DIMENSION_ID_CONST_PV = 7,
        DIMENSION_ID_REL_SUJV = 8,
        DIMENSION_ID_REL_AUXV = 9,
        DIMENSION_ID_REL_CODV = 10,
        DIMENSION_ID_REL_CPLV = 11,
        DIMENSION_ID_REL_MODV = 12,
        DIMENSION_ID_REL_COMP = 13,
        DIMENSION_ID_REL_ATBSO = 14,
        DIMENSION_ID_REL_MODN = 15,
        DIMENSION_ID_REL_MODA = 16,
        DIMENSION_ID_REL_MODR = 17,
        DIMENSION_ID_REL_MODP = 18,
        DIMENSION_ID_REL_COORD = 19,
        DIMENSION_ID_REL_APPOS = 20,
        DIMENSION_ID_REL_JUXT = 21
    };
    static QMap<DIMENSION_ID, EvaluationResultDimension*> dimensions;

    double fc; // found and correct
    double fp; // found - precision
    double cr; // correct - recall

    static void initDimensions ();
    static void replaceDimension (QString name, QString key, QColor color, DIMENSION_ID dimensionId, bool rel);
    static QMap<DIMENSION_ID, EvaluationResultDimension*>& getDimensions ();
    static QList<EvaluationResultDimension*> getDimensionsVisible ();
    static DIMENSION_ID getDimensionGroup (DIMENSION_ID dimensionId);

    /// Accessor for @see refAbsentFromHyp
    inline QMultiMap<QString, QString>& getRefAbsentFromHyp() {return refAbsentFromHyp;}
    /// Accessor for @see refAbsentFromHyp
    inline const QMultiMap<QString, QString>& getRefAbsentFromHyp() const {return refAbsentFromHyp;}

    inline QMultiMap<QString, QString>& getHypAbsentFromRef() {return hypAbsentFromRef;}
    inline const QMultiMap<QString, QString>& getHypAbsentFromRef() const {return hypAbsentFromRef;}

    inline QMultiMap<QString, QString>& getTypeError() {return typeError;}
    inline const QMultiMap<QString, QString>& getTypeError() const {return typeError;}

private:
  /** pairs of sentence/group or sentence/relation ids that are absent in hypothesis while present in reference */
  QMultiMap<QString, QString> refAbsentFromHyp;

  /** pairs of sentence/group or sentence/relation ids that are false because present in hypothesis while present in reference */
  QMultiMap<QString, QString> hypAbsentFromRef;

  /** pairs of sentence/group or sentence/relation ids that are present in both hypothesis and reference but with different types */
  QMultiMap<QString, QString> typeError;

};

} // End namespace Lima
} // End namespace Pelf

#endif // LIMA_PELF_EVALUATIONRESULT_H
