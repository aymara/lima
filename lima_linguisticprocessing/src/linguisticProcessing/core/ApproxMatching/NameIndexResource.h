// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
