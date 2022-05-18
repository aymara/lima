// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_COMMON_MEDIAPROCESSORSPROCESSUNITPIPELINE_H
#define LIMA_COMMON_MEDIAPROCESSORSPROCESSUNITPIPELINE_H

#include "MediaProcessUnit.h"
#include "common/ProcessUnitFramework/ProcessUnitPipeline.h"
#include "common/LimaCommon.h"

#include <list>

namespace Lima
{

// namespace ObjetProcessing
// {

template class Lima::ProcessUnitPipeline<MediaProcessUnit>;
LIMA_MEDIAPROCESSORS_EXPORT typedef Lima::ProcessUnitPipeline<MediaProcessUnit> MediaProcessUnitPipeline;


// } // ObjetProcessing

} // Lima

#endif
