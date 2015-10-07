/*
    Copyright 2002-2013 CEA LIST

    This file is part of LIMA.

    LIMA is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    LIMA is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with LIMA.  If not, see <http://www.gnu.org/licenses/>
*/
/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/

#include "AbstractResource.h"

namespace Lima {
namespace LinguisticProcessing {

class AbstractResourcePrivate
{
friend  class AbstractResource;

  AbstractResourcePrivate() {}
  ~AbstractResourcePrivate() {}
  AbstractResourcePrivate(const AbstractResourcePrivate&) {}
    
  LimaFileSystemWatcher m_resourceFileWatcher;
};

AbstractResource::AbstractResource( QObject* parent ) : 
    QObject( parent ), 
    InitializableObject<AbstractResource,ResourceInitializationParameters>(), 
    m_d(new AbstractResourcePrivate())
{
  connect(&m_d->m_resourceFileWatcher,SIGNAL(fileChanged(QString)),this,SIGNAL(resourceFileChanged(QString)));
}

AbstractResource::~AbstractResource()
{
  delete m_d;
}

AbstractResource::AbstractResource(const AbstractResource& r) : 
    QObject(r.parent()), 
    InitializableObject<AbstractResource,ResourceInitializationParameters>(), 
    m_d(new AbstractResourcePrivate(*r.m_d))
{
  connect(&m_d->m_resourceFileWatcher,SIGNAL(fileChanged(QString)),this,SIGNAL(resourceFileChanged(QString)));
}

LimaFileSystemWatcher& AbstractResource::resourceFileWatcher()
{
  return m_d->m_resourceFileWatcher;
}
// void AbstractResource::resourceFileChanged ( const QString & path )
// {
//   std::cerr << "AbstractResource::resourceFileChanged" << path.toUtf8().constData();
// //   configure(path);
// }


} // LinguisticProcessing
} // Lima

