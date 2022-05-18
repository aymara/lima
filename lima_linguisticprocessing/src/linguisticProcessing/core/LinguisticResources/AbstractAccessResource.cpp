// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
#include "AbstractAccessResource.h"

namespace Lima {
namespace LinguisticProcessing {
namespace AnalysisDict {

AbstractAccessResource::AbstractAccessResource(QObject* parent) :
    AbstractResource(parent),
    m_isMainKeys(false)
{
}

AbstractAccessResource::~AbstractAccessResource() 
{
}


}
}
}
