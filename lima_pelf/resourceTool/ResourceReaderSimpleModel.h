// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_PELF_RESOURCEREADERSIMPLEMODEL_H
#define LIMA_PELF_RESOURCEREADERSIMPLEMODEL_H

#include <QtCore/QtDebug>
#include <QtCore/QProcess>
#include <QtCore/QFile>
#include <QtCore/QFileSystemWatcher>

#include "AbstractResourceEntry.h"

namespace Lima {
namespace Pelf {

class ResourceReaderSimpleModel
{

public:

    enum INSTALL_STATUS
    {
        INSTALL_READY = 0,
        INSTALL_PROCESSING = 1
    };

    INSTALL_STATUS installStatus;
    QStringList resourcePaths;
    QStringList categoryList;
    QList<AbstractResourceEntry*> availableData;
    QList<int> foundData;
    bool dataModified;
    bool checkDataModified;

    ResourceReaderSimpleModel ();
    virtual ~ResourceReaderSimpleModel();
    
    template <class ResourceEntryType> void loadData ()
    {
        QStringList sourceStrings;
        ResourceEntryType* entry;
        availableData.clear();
        categoryList.clear();
        foundData.clear();
        for(int i = 0; i < resourcePaths.size(); i++)
        {
            QString sourcePath = resourcePaths.at(i);
            sourceStrings = loadDataSourceStrings(sourcePath);
            for(int j = 0; j < sourceStrings.size(); j++)
            {
                QString sourceString = sourceStrings.at(j);
                if((entry = ResourceEntryType::factory(sourceString, sourcePath, j)) != 0)
                    availableData << entry;
            }
            if((entry = ResourceEntryType::factoryEndFile()) != 0)
                availableData << entry;
        }
        loadDataCategories(ResourceEntryType::emptyCategoryName);
    }
    void init (QStringList rp, QString ic);
    QStringList loadDataSourceStrings (QString sourcePath);
    void loadDataCategories (QString emptyCategoryName);
    int searchEntries (QStringList args);
    AbstractResourceEntry* getFoundEntry (int entryNo) const;
    void install ();
    virtual void installFinished (int exitCode, QProcess::ExitStatus exitStatus);
    virtual void installError (QProcess::ProcessError error);
    virtual void emitDataInstalled (bool success) = 0; // Virtual Q_EMIT function (needed for class to be polymorphic)

protected:
    QString installComand;
    QProcess* m_installProcess;
    QFileSystemWatcher m_watcher;
};

} // End namespace Lima
} // End namespace Pelf

#endif // LIMA_PELF_RESOURCEREADERSIMPLEMODEL_H
