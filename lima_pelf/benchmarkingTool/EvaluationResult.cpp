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
#include "EvaluationResult.h"
using namespace Lima::Pelf;

void EvaluationResult::initDimensions ()
{
    replaceDimension(
        "ALL CONSTITUANTS",
        "CONSTITUENT EVAL SUBCORPUS_PELF ALL_CONSTITUENTS",
        QColor("#ff3232"),
        DIMENSION_ID_CONST_ALL,
        false
    );
    replaceDimension(
        "GN",
        "CONSTITUENT EVAL SUBCORPUS_PELF GN",
        QColor("#ff32cb"),
        DIMENSION_ID_CONST_GN,
        false
    );
    replaceDimension(
        "NV",
        "CONSTITUENT EVAL SUBCORPUS_PELF NV",
        QColor("#9832ff"),
        DIMENSION_ID_CONST_NV,
        false
    );
    replaceDimension(
        "GA",
        "CONSTITUENT EVAL SUBCORPUS_PELF GA",
        QColor("#32feff"),
        DIMENSION_ID_CONST_GA,
        false
    );
    replaceDimension(
        "GR",
        "CONSTITUENT EVAL SUBCORPUS_PELF GR",
        QColor("#32ff66"),
        DIMENSION_ID_CONST_GR,
        false
    );
    replaceDimension(
        "GP",
        "CONSTITUENT EVAL SUBCORPUS_PELF GP",
        QColor("#99ff32"),
        DIMENSION_ID_CONST_GP,
        false
    );
    replaceDimension(
        "PV",
        "CONSTITUENT EVAL SUBCORPUS_PELF PV",
        QColor("#ffcc32"),
        DIMENSION_ID_CONST_PV,
        false
    );
    replaceDimension(
        "ALL RELATIONS",
        "EVAL SUBCORPUS_PELF ALL_RELATIONS",
        QColor("#3265ff"),
        DIMENSION_ID_REL_ALL,
        true
    );
    replaceDimension(
        "SUJ-V",
        "EVAL SUBCORPUS_PELF SUJ_V",
        QColor("#dd5454"),
        DIMENSION_ID_REL_SUJV,
        true
    );
    replaceDimension(
        "AUX-V",
        "EVAL SUBCORPUS_PELF AUX_V",
        QColor("#dd54ba"),
        DIMENSION_ID_REL_AUXV,
        true
    );
    replaceDimension(
        "COD-V",
        "EVAL SUBCORPUS_PELF COD_V",
        QColor("#9854dd"),
        DIMENSION_ID_REL_CODV,
        true
    );
    replaceDimension(
        "CPL-V",
        "EVAL SUBCORPUS_PELF CPL_V",
        QColor("#5476dd"),
        DIMENSION_ID_REL_CPLV,
        true
    );
    replaceDimension(
        "MOD-V",
        "EVAL SUBCORPUS_PELF MOD_V",
        QColor("#54dcdd"),
        DIMENSION_ID_REL_MODV,
        true
    );
    replaceDimension(
        "COMP",
        "EVAL SUBCORPUS_PELF COMP",
        QColor("#54dd77"),
        DIMENSION_ID_REL_COMP,
        true
    );
    replaceDimension(
        "ATB-SO",
        "EVAL SUBCORPUS_PELF ATB_SO",
        QColor("#99dd54"),
        DIMENSION_ID_REL_ATBSO,
        true
    );
    replaceDimension(
        "MOD-N",
        "EVAL SUBCORPUS_PELF MOD_N",
        QColor("#ddbb54"),
        DIMENSION_ID_REL_MODN,
        true
    );
    replaceDimension(
        "MOD-A",
        "EVAL SUBCORPUS_PELF MOD_A",
        QColor("#bb7676"),
        DIMENSION_ID_REL_MODA,
        true
    );
    replaceDimension(
        "MOD-R",
        "EVAL SUBCORPUS_PELF MOD_R",
        QColor("#bb76a9"),
        DIMENSION_ID_REL_MODR,
        true
    );
    replaceDimension(
        "MOD-P",
        "EVAL SUBCORPUS_PELF MOD_P",
        QColor("#9876bb"),
        DIMENSION_ID_REL_MODP,
        true
    );
    replaceDimension(
        "COORD",
        "EVAL SUBCORPUS_PELF COORD",
        QColor("#7687bb"),
        DIMENSION_ID_REL_COORD,
        true
    );
    replaceDimension(
        "APPOS",
        "EVAL SUBCORPUS_PELF APPOS",
        QColor("#76bbbb"),
        DIMENSION_ID_REL_APPOS,
        true
    );
    replaceDimension(
        "JUXT",
        "EVAL SUBCORPUS_PELF JUXT",
        QColor("#76bb88"),
        DIMENSION_ID_REL_JUXT,
        true
    );
}

void EvaluationResult::replaceDimension (QString name, QString key, QColor color, DIMENSION_ID dimensionId, bool rel)
{
  if (dimensions[dimensionId] != 0)
  {
    delete dimensions[dimensionId];
  }
    dimensions[dimensionId] = new EvaluationResultDimension(name, key, color, dimensionId, rel);
}

QMap<EvaluationResult::DIMENSION_ID, EvaluationResultDimension*>& EvaluationResult::getDimensions ()
{
    return dimensions;
}

QList<EvaluationResultDimension*> EvaluationResult::getDimensionsVisible ()
{
    QList<EvaluationResultDimension*> dimensionsVisible;
    bool masked = false;
    Q_FOREACH(EvaluationResultDimension* dimension, dimensions)
    {
        if(dimension->visibilityState == Qt::PartiallyChecked)
            masked = true;
    }
    Q_FOREACH(EvaluationResultDimension* dimension, dimensions)
    {
        int dimensionGroupId = getDimensionGroup((DIMENSION_ID)dimension->id);
        bool groupChecked = dimensionGroupId == -1 || dimensions[(DIMENSION_ID)dimensionGroupId]->visibilityState == Qt::Checked;
        if(dimension->visibilityState == Qt::PartiallyChecked || (!masked && groupChecked && dimension->visibilityState == Qt::Checked))
            dimensionsVisible.push_back(dimension);
    }
    return dimensionsVisible;
}

EvaluationResult::DIMENSION_ID EvaluationResult::getDimensionGroup (DIMENSION_ID dimensionId)
{
    switch(dimensionId)
    {
    case DIMENSION_ID_CONST_GN:
    case DIMENSION_ID_CONST_NV:
    case DIMENSION_ID_CONST_GA:
    case DIMENSION_ID_CONST_GR:
    case DIMENSION_ID_CONST_GP:
    case DIMENSION_ID_CONST_PV:
        return DIMENSION_ID_CONST_ALL;
    case DIMENSION_ID_REL_SUJV:
    case DIMENSION_ID_REL_AUXV:
    case DIMENSION_ID_REL_CODV:
    case DIMENSION_ID_REL_CPLV:
    case DIMENSION_ID_REL_MODV:
    case DIMENSION_ID_REL_COMP:
    case DIMENSION_ID_REL_ATBSO:
    case DIMENSION_ID_REL_MODN:
    case DIMENSION_ID_REL_MODA:
    case DIMENSION_ID_REL_MODR:
    case DIMENSION_ID_REL_MODP:
    case DIMENSION_ID_REL_COORD:
    case DIMENSION_ID_REL_APPOS:
    case DIMENSION_ID_REL_JUXT:
        return DIMENSION_ID_REL_ALL;
    default:
        return (DIMENSION_ID)-1;
    }
}

QMap<EvaluationResult::DIMENSION_ID, EvaluationResultDimension*> EvaluationResult::dimensions;
