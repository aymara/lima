// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_PELF_RESOURCEREADERSIMPLEWIDGET_H
#define LIMA_PELF_RESOURCEREADERSIMPLEWIDGET_H

#include <QtCore/QtDebug>
#include <QtWidgets/QMessageBox>

#include "ResourceReaderSimpleModel.h"

namespace Lima {
namespace Pelf {

class ResourceReaderSimpleWidget
{

public:

    ResourceReaderSimpleWidget ();
    virtual ~ResourceReaderSimpleWidget () {};
    void init (ResourceReaderSimpleModel* rrsm);
    void confirmInstall ();
    bool confirmInstallDlg ();
    bool checkInstallingReady ();

protected:

    ResourceReaderSimpleModel* resourceModel;

};

} // End namespace Lima
} // End namespace Pelf

#endif // LIMA_PELF_RESOURCEREADERSIMPLEWIDGET_H
