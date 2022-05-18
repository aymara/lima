// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/

#include "AbstractResource.h"

namespace Lima {
namespace LinguisticProcessing {

AbstractResource::AbstractResource( QObject* parent ) : 
    AbstractResourceBase( parent ), 
    InitializableObject<AbstractResource,ResourceInitializationParameters>()
{
}

AbstractResource::~AbstractResource()
{
}

AbstractResource::AbstractResource(const AbstractResource& r) : 
    AbstractResourceBase(r.parent()), 
    InitializableObject<AbstractResource,ResourceInitializationParameters>()
{
}


} // LinguisticProcessing
} // Lima

