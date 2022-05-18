// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2003 by  CEA                                            *
 *   author Olivier MESNARD olivier.mesnard@cea.fr                         *
 *                                                                         *
 *  composed dictionnary                                                   *
 ***************************************************************************/

#include "AbstractAnalysisDictionary.h"

namespace Lima {
namespace LinguisticProcessing {
namespace AnalysisDict {

AbstractAnalysisDictionary::AbstractAnalysisDictionary(QObject* parent) :
    AbstractResource(parent)
{
}

AbstractAnalysisDictionary::~AbstractAnalysisDictionary()
{
}

} // namespace AnalysisDict
} // namespace LinguisticProcessing
} // namespace Lima

