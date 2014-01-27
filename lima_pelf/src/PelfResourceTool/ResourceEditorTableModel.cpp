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
#include "ResourceEditorTableModel.h"
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
    qSort(availableData.begin(), availableData.end(), ResourceReaderTableModel::headerLessThan);
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
    qSort(availableData.begin(), availableData.end(), ResourceReaderTableModel::headerLessThan);
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
    emit dataChanged();
}

void ResourceEditorTableModel::deleteEntries (QModelIndexList indexList)
{
    dataModified = true;
    qSort(indexList);
    QList<AbstractResourceEntry*> entriesList;
    for(int i = 0; i < indexList.size(); i++)
        if(AbstractResourceEntry* entry = getFoundEntry(indexList.at(i).row()))
            entriesList << entry;
    for(int i = 0; i < entriesList.size(); i++)
        availableData.removeAll(entriesList.at(i));
    emit dataChanged();
}

#include "ResourceEditorTableModel.moc"
