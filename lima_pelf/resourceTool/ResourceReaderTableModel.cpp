// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "ResourceReaderTableModel.h"
using namespace Lima::Pelf;

int ResourceReaderTableModel::sortedHeaderColumn = 0;
Qt::SortOrder ResourceReaderTableModel::sortedHeaderOrder = Qt::AscendingOrder;

ResourceReaderTableModel::ResourceReaderTableModel (QObject * parent) :
    QAbstractTableModel(parent),
    ResourceReaderSimpleModel()
{
  qDebug() << "ResourceReaderTableModel::ResourceReaderTableModel ";
  connect(m_installProcess, 
          SIGNAL(finished(int,QProcess::ExitStatus)),
          this, 
          SLOT(installFinished(int,QProcess::ExitStatus)));
  connect(m_installProcess, 
          SIGNAL(error(QProcess::ProcessError)),
          this, 
          SLOT(installError(QProcess::ProcessError)));
  connect(&m_watcher, 
          SIGNAL(fileChanged(QString)), 
          this, 
          SIGNAL(resourceFileModified(QString)));

}

ResourceReaderTableModel::~ResourceReaderTableModel()
{
}

int ResourceReaderTableModel::searchEntries (QStringList args)
{
  int entriesCount = ResourceReaderSimpleModel::searchEntries(args);
  beginResetModel();
  endResetModel();
  return entriesCount;
}

int ResourceReaderTableModel::rowCount (const QModelIndex& ) const
{
    return foundData.size();
}

int ResourceReaderTableModel::columnCount (const QModelIndex& ) const
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
    std::sort(availableData.begin(), availableData.end(), headerLessThan);
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
