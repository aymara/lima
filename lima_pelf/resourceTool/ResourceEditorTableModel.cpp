// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "ResourceEditorTableModel.h"

#include <algorithm>

using namespace Lima::Pelf;

ResourceEditorTableModel::ResourceEditorTableModel () :
    ResourceReaderTableModel() {}

void ResourceEditorTableModel::init (QString rp, QString ic)
{
    checkDataModified = true;
    editedResourcePath = rp;
    ResourceReaderSimpleModel::init (QStringList() << rp, ic);
}

void ResourceEditorTableModel::saveData ()
{
    int oldSortedHeaderColumn = ResourceReaderTableModel::sortedHeaderColumn;
    ResourceReaderTableModel::sortedHeaderColumn = -1;
    std::sort(availableData.begin(), availableData.end(), ResourceReaderTableModel::headerLessThan);
    qDebug() << "Saving resource data into " << editedResourcePath;
    QFile resourceFile(editedResourcePath);
    if(!resourceFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "Error opening resource file for writing";
        return;
    }
    QString encodedEntries = "";
    for(int i = 0; i < availableData.size(); i++)
        encodedEntries += availableData.at(i)->encodeToString()+"\n";
    QByteArray encodedEntriesUtf8 = encodedEntries.toUtf8();
    int encodedEntriesUtf8Size = qstrlen(encodedEntriesUtf8);
    resourceFile.resize(encodedEntriesUtf8Size);
    resourceFile.write(encodedEntriesUtf8, encodedEntriesUtf8Size);
    resourceFile.close();
    ResourceReaderTableModel::sortedHeaderColumn = oldSortedHeaderColumn;
    std::sort(availableData.begin(), availableData.end(), ResourceReaderTableModel::headerLessThan);
}

void ResourceEditorTableModel::install ()
{
    if(dataModified)
        saveData();
    ResourceReaderTableModel::install();
}

void ResourceEditorTableModel::addEntry (AbstractResourceEntry* de)
{
    dataModified = true;
    availableData << de;
    Q_EMIT dataChanged();
}

void ResourceEditorTableModel::deleteEntries (QModelIndexList indexList)
{
    dataModified = true;
    std::sort(indexList.begin(),indexList.end());
    QList<AbstractResourceEntry*> entriesList;
    for(int i = 0; i < indexList.size(); i++)
        if(AbstractResourceEntry* entry = getFoundEntry(indexList.at(i).row()))
            entriesList << entry;
    for(int i = 0; i < entriesList.size(); i++)
        availableData.removeAll(entriesList.at(i));
    Q_EMIT dataChanged();
}

#include "ResourceEditorTableModel.moc"
