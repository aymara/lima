// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_PELF_RESOURCEEDITORTABLEMODEL_H
#define LIMA_PELF_RESOURCEEDITORTABLEMODEL_H

#include <QtCore/QtDebug>
#include <QtCore/QFile>

#include "ResourceReaderTableModel.h"
#include "AbstractResourceEntry.h"

namespace Lima {
namespace Pelf {

class ResourceEditorTableModel : public ResourceReaderTableModel
{

Q_OBJECT

public:

    ResourceEditorTableModel ();
    void init (QString rp, QString ic);
    void saveData ();
    void install ();
    void addEntry ();
    void deleteEntries (QModelIndexList indexList);

public Q_SLOTS:

    void addEntry (AbstractResourceEntry* de);

private:

    QString editedResourcePath;

};

} // End namespace Lima
} // End namespace Pelf

#endif // LIMA_PELF_RESOURCEEDITORTABLEMODEL_H
