// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_LINGUISTICPROCESSING_ANALYSISDICTFSAACCESSRESOURCE_H
#define LIMA_LINGUISTICPROCESSING_ANALYSISDICTFSAACCESSRESOURCE_H

#include "AnalysisDictExport.h"
#include "linguisticProcessing/core/LinguisticResources/AbstractAccessResource.h"

#include <QReadWriteLock>

namespace Lima {

namespace LinguisticProcessing {

namespace AnalysisDict {

/**
@author Benoit Mathieu
*/
class LIMA_ANALYSISDICT_EXPORT FsaAccessResource : public AbstractAccessResource
{
  Q_OBJECT
public:
    FsaAccessResource(QObject* parent = 0);

    virtual ~FsaAccessResource();

    virtual void init(
      Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
      Manager* manager) override;
    
    virtual std::shared_ptr<Common::AbstractAccessByString> getAccessByString() const override;
    
private Q_SLOTS:
  void accessFileChanged ( const QString & path );


protected:
  std::shared_ptr<Common::AbstractAccessByString> m_fsaAccess;

private:
  // A read/write locker to prevent accessing m_dicoData during its loading
  QReadWriteLock m_lock;
};

}

}

}

#endif
