// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_PELF_RESOURCEREADERTABLEMODEL_H
#define LIMA_PELF_RESOURCEREADERTABLEMODEL_H

#include <QtCore/QtDebug>
#include <QtCore/QAbstractTableModel>
#include <algorithm>

#include "ResourceReaderSimpleModel.h"
#include "AbstractResourceEntry.h"

namespace Lima {
namespace Pelf {

class ResourceReaderTableModel : 
    public QAbstractTableModel, 
    public ResourceReaderSimpleModel
{

Q_OBJECT

public:

    static int sortedHeaderColumn;
    static Qt::SortOrder sortedHeaderOrder;

    int columnCountPerEntry;
    QStringList columnHeaders;

    ResourceReaderTableModel (QObject * parent = 0 );
    virtual ~ResourceReaderTableModel();
    
    int searchEntries (QStringList args);
    int rowCount (const QModelIndex& parent = QModelIndex()) const override;
    int columnCount (const QModelIndex& parent = QModelIndex()) const override;
    QVariant headerData (int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    void sortByHeader (int column, Qt::SortOrder order);
    static bool headerLessThan (AbstractResourceEntry* entry1, AbstractResourceEntry* entry2);
    QVariant data (const QModelIndex& index, int role) const override;
    void emitDataInstalled (bool success) override { Q_EMIT dataInstalled(success); }; // ResourceReaderSimpleModel needed Q_EMIT function

Q_SIGNALS:

    void dataChanged ();
    void dataInstalled (bool success); // ResourceReaderSimpleModel needed signal
    void resourceFileModified(const QString & path);

public Q_SLOTS:

    // ResourceReaderSimpleModel needed slots
    virtual void installFinished (int ec, QProcess::ExitStatus es) override { ResourceReaderSimpleModel::installFinished(ec, es); };
    virtual void installError (QProcess::ProcessError e) override { ResourceReaderSimpleModel::installError(e); };

};

} // End namespace Lima
} // End namespace Pelf

#endif // LIMA_PELF_RESOURCEREADERTABLEMODEL_H
