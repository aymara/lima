// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_LINGUISTICPROCESSING_ANALYSISDICTACCESSRESSOURCE_H
#define LIMA_LINGUISTICPROCESSING_ANALYSISDICTACCESSRESSOURCE_H

#include "linguisticProcessing/core/LinguisticResources/AbstractResource.h"

namespace Lima {
namespace LinguisticProcessing {
namespace AnalysisDict {

/**
@author Benoit Mathieu
*/
class AccessRessource : public AbstractResource
{
  Q_OBJECT
public:
    AccessRessource();
    virtual ~AccessRessource();

};

}
}
}

#endif
