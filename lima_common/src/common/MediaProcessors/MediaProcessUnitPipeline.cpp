// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "MediaProcessUnitPipeline.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"

namespace Lima {
namespace ObjetProcessing {

SimpleFactory<MediaProcessUnit,MediaProcessUnitPipeline> pipelineFactory("ProcessUnitPipeline");



} // ObjetProcessing
} // Lima
