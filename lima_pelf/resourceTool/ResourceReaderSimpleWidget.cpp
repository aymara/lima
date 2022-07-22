// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "ResourceReaderSimpleWidget.h"
using namespace Lima::Pelf;

ResourceReaderSimpleWidget::ResourceReaderSimpleWidget() :
  resourceModel(0)
{
}

void ResourceReaderSimpleWidget::init (ResourceReaderSimpleModel* rrsm)
{
    resourceModel = rrsm;
}

void ResourceReaderSimpleWidget::confirmInstall ()
{
    if(confirmInstallDlg())
        resourceModel->install();
}

bool ResourceReaderSimpleWidget::confirmInstallDlg ()
{
    if(!checkInstallingReady())
        return false;
    QWidget* qWidgetThis = dynamic_cast<QWidget*>(this);
    return QMessageBox::question(
       qWidgetThis,
       "Install resource ?",
       "Resource will be saved, compiled and installed, this operation may take several minutes, do you want to do it now ?",
       QMessageBox::Ok | QMessageBox::Cancel,
       QMessageBox::Cancel
    ) == QMessageBox::Ok;
}

bool ResourceReaderSimpleWidget::checkInstallingReady ()
{
    QWidget* qWidgetThis = dynamic_cast<QWidget*>(this);
    if(resourceModel->installStatus == ResourceReaderSimpleModel::INSTALL_PROCESSING)
    {
        QMessageBox::warning(
            qWidgetThis,
            "Installation already in progress",
            "Installation for this resource is already in progress, please wait for installation process to terminate before installing it again."
        );
        return false;
    }
    return true;
}
