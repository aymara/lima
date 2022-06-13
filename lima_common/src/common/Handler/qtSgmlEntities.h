/****************************************************************************
**
** Code below is
** Copyright (C) 2016 The Qt Company Ltd.
**
** It is used and modified by CEA LIST under the terms of the
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 3 as published by the Free Software Foundation
**
****************************************************************************/
#ifndef QTSGMLENTITIES_H
#define QTSGMLENTITIES_H

#include "qtsgmlentities_export.h"

#include <QString>


// parses an entity after "&", and returns it
QTSGMLENTITIES_EXPORT QString parseEntity(QStringView entity);

/** END OF CODE FROM Qt */

#endif
