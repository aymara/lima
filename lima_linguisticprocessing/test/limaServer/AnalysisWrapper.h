// Copyright (C) <year>  <name of author>
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef ANALYSISWRAPPER_H
#define ANALYSISWRAPPER_H

#include <set>
#include <memory>
#include <QtCore/QThread>
#include <QtCore/QString>

class QObject;
class QHttpRequest;
class QHttpResponse;

namespace Lima
{
  namespace LinguisticProcessing
  {
    class AbstractLinguisticProcessingClient;
  }
}

class AnalysisWrapperPrivate;
/**
 * @brief Analyser wrapper
 */
class AnalysisWrapper : public QObject
{
  Q_OBJECT
public:
    AnalysisWrapper (
        std::shared_ptr< Lima::LinguisticProcessing::AbstractLinguisticProcessingClient > analyzer,
        const std::set<std::string>& langs, QObject* parent = 0 );
    virtual ~AnalysisWrapper();
    
  QString analyze(const QString& text, const QString& language, const QString& pipeline);

private:
  AnalysisWrapperPrivate* m_d;
};

#endif // ANALYSISWRAPPER_H
