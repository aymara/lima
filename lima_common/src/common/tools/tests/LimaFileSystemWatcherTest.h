// Copyright 2015 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMAFILESYSTEMWATCHERTEST_H
#define LIMAFILESYSTEMWATCHERTEST_H

#include <QObject>

class LimaFileSystemWatcherTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void init();
    void cleanup();

    void LimaFileSystemWatcherTest0();
};

#endif // LIMAFILESYSTEMWATCHERTEST_H
