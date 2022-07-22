// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_PELF_RESOURCEREADERTOOLBOXWIDGET_H
#define LIMA_PELF_RESOURCEREADERTOOLBOXWIDGET_H

#include <QtCore/QtDebug>
#include <QtWidgets/QToolBox>

#include "ResourceReaderSimpleWidget.h"
#include "ResourceReaderToolBoxModel.h"
#include "ResourceFileEditDlg.h"

namespace Lima {
namespace Pelf {

class ResourceReaderToolBoxWidget : public QToolBox, public ResourceReaderSimpleWidget
{

Q_OBJECT

public:

    ResourceReaderToolBoxWidget (QWidget* parent);
    void init (ResourceReaderToolBoxModel* rrtbm);
    void moveCursorToSourceLine (QString sourceFile, int sourceLine);

protected:

    QMap<QString,int> sourceIndexes;

private Q_SLOTS:
  void slotResourceFileModified(const QString&);
};

} // End namespace Lima
} // End namespace Pelf

#endif // LIMA_PELF_RESOURCEREADERTOOLBOXWIDGET_H
