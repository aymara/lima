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
#include "ResourceReaderTableModel.h"
using namespace Lima::Pelf;

int ResourceReaderTableModel::sortedHeaderColumn = 0;
Qt::SortOrder ResourceReaderTableModel::sortedHeaderOrder = Qt::AscendingOrder;

ResourceReaderTableModel::ResourceReaderTableModel (QObject * parent) :
    QAbstractTableModel(parent),
    ResourceReaderSimpleModel()
{
  qDebug() << "ResourceReaderTableModel::ResourceReaderTableModel ";
  connect(m_installProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
      this, SLOT(installFinished(int, QProcess::ExitStatus)));
  connect(m_installProcess, SIGNAL(error(QProcess::ProcessError)),
      this, SLOT(installError(QProcess::ProcessError)));
  connect(&m_watcher, SIGNAL(fileChanged(QString)), this, SIGNAL(resourceFileModified(QString)));

}

ResourceReaderTableModel::~ResourceReaderTableModel()
{
}

int ResourceReaderTableModel::searchEntries (QStringList args)
{
    int entriesCount = ResourceReaderSimpleModel::searchEntries(args);
    reset();
    return entriesCount;
}

int ResourceReaderTableModel::rowCount (const QModelIndex& parent) const
{
    return foundData.size();
}

int ResourceReaderTableModel::columnCount (const QModelIndex& parent) const
{
    return columnCountPerEntry;
}

QVariant ResourceReaderTableModel::headerData (int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole)
        return QVariant();
    if(orientation == Qt::Horizontal)
        return QVariant(columnHeaders.at(section));
    if(orientation == Qt::Vertical)
        return QVariant("#"+QString::number(availableData[foundData[section]]->sourceLine));
    return QVariant();
}

void ResourceReaderTableModel::sortByHeader (int column, Qt::SortOrder order)
{
    sortedHeaderColumn = column;
    sortedHeaderOrder = order;
    qSort(availableData.begin(), availableData.end(), headerLessThan);
    Q_EMIT dataChanged();
}

bool ResourceReaderTableModel::headerLessThan (AbstractResourceEntry* entry1, AbstractResourceEntry* entry2)
{
    return entry1->headerLessThan(entry2, sortedHeaderColumn, sortedHeaderOrder);
}

QVariant ResourceReaderTableModel::data (const QModelIndex& index, int role) const
{
    if(role != Qt::DisplayRole)
        return QVariant();
    int column = index.column();
    AbstractResourceEntry* entry = getFoundEntry(index.row());
    if(entry == 0)
        return QVariant();
    return QVariant(entry->getColumnData(column));
}

#include "ResourceReaderTableModel.moc"
