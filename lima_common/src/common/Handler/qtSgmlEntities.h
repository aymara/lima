// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef QTSGMLENTITIES_H
#define QTSGMLENTITIES_H

#include "qtsgmlentities_export.h"

#include <QString>


// parses an entity after "&", and returns it
QTSGMLENTITIES_EXPORT QString parseEntity(QStringView entity);

/** END OF CODE FROM Qt */

#endif
