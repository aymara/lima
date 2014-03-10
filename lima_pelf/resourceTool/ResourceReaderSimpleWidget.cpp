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
