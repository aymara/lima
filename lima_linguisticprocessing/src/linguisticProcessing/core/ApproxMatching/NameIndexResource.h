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
 *   Copyright (C) 2004-2017 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_MORPHOLOGICANALYSIS_NAMEINDEXRESOURCE_H
#define LIMA_MORPHOLOGICANALYSIS_NAMEINDEXRESOURCE_H

#include "linguisticProcessing/core/LinguisticResources/AbstractResource.h"
#include <QReadWriteLock>

namespace Lima
{
namespace LinguisticProcessing
{
namespace MorphologicAnalysis
{
typedef std::multimap< std::basic_string<wchar_t>, std::basic_string<wchar_t> > NameIndex;
class ApproxStringMatcher;
class LIMA_ANALYSISDICT_EXPORT NameIndexResource : public AbstractResource, NameIndex
{
/*
 *  Q_OBJECT
*/
public:
/*
  NameIndexResource(QObject* parent = 0);
*/  
    friend class ApproxStringMatcher;
    NameIndexResource();

    virtual ~NameIndexResource();

    virtual void init(
      Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
      Manager* manager) override;
    bool withIndex() const { return m_withIndex; }
      
    static std::basic_string<wchar_t> LimaStr2wcharStr( const QString& limastr );

/*    
private Q_SLOTS:
  void accessFileChanged ( const QString & path );
*/

protected:

private:
  void readNames( const std::string& filepath );
  bool m_withIndex;
  // A read/write locker to prevent accessing m_dicoData during its loading
/*
  QReadWriteLock m_lock;
*/  
};

}

}

}

#endif
