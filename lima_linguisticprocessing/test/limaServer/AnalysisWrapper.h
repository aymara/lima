/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) <year>  <name of author>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

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
