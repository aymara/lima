/*
 *    Copyright 2002-2013 CEA LIST
 * 
 *    This file is part of LIMA.
 * 
 *    LIMA is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Affero General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 * 
 *    LIMA is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Affero General Public License for more details.
 * 
 *    You should have received a copy of the GNU Affero General Public License
 *    along with LIMA.  If not, see <http://www.gnu.org/licenses/>
 */
#ifndef LIMA_PELF_RESOURCEREADERTABLEMODEL_H
#define LIMA_PELF_RESOURCEREADERTABLEMODEL_H

#include <QtCore/QtDebug>
#include <QtCore/QAbstractTableModel>
#include <algorithm>

#include "ResourceReaderSimpleModel.h"
#include "AbstractResourceEntry.h"

namespace Lima {
namespace Pelf {

class ResourceReaderTableModel : public QAbstractTableModel, public ResourceReaderSimpleModel
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
    int rowCount (const QModelIndex& parent = QModelIndex()) const;
    int columnCount (const QModelIndex& parent = QModelIndex()) const;
    QVariant headerData (int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    void sortByHeader (int column, Qt::SortOrder order);
    static bool headerLessThan (AbstractResourceEntry* entry1, AbstractResourceEntry* entry2);
    QVariant data (const QModelIndex& index, int role) const;
    void emitDataInstalled (bool success) { Q_EMIT dataInstalled(success); }; // ResourceReaderSimpleModel needed Q_EMIT function

Q_SIGNALS:

    void dataChanged ();
    void dataInstalled (bool success); // ResourceReaderSimpleModel needed signal
    void resourceFileModified(const QString & path);

public Q_SLOTS:

    // ResourceReaderSimpleModel needed slots
    virtual void installFinished (int ec, QProcess::ExitStatus es) { ResourceReaderSimpleModel::installFinished(ec, es); };
    virtual void installError (QProcess::ProcessError e) { ResourceReaderSimpleModel::installError(e); };

};

} // End namespace Lima
} // End namespace Pelf

#endif // LIMA_PELF_RESOURCEREADERTABLEMODEL_H
