// Copyright (c) 2010, Razvan Petru
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:

// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice, this
//   list of conditions and the following disclaimer in the documentation and/or other
//   materials provided with the distribution.
// * The name of the contributors may not be used to endorse or promote products
//   derived from this software without specific prior written permission.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
// OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef QSLOGDEST_H
#define QSLOGDEST_H

#include <memory>
#include <QtCore/QString>
#include <QtCore/QObject>


#include "QsLog_export.h"


namespace QsLogging
{

class LIMA_COMMONQSLOG_EXPORT Destination
{
public:
  virtual ~Destination() = default;
  virtual void write(const QString& message, const QString& zone) = 0;
};

typedef std::shared_ptr<Destination> DestinationPtr;

class DestinationsImpl; // d pointer
class LIMA_COMMONQSLOG_EXPORT Destinations : public QObject
{
Q_OBJECT
  friend class DestinationsImpl;
public:
  static Destinations& instance()
  {
    static Destinations staticDestinations;
    return staticDestinations;
  }

  bool setDefault();
  bool removeDefault();
  bool configure(const QString& fileName, bool reload = false);

  const QMap< QString, DestinationPtr >& destinations() const;

private Q_SLOTS:
  void configureFileChanged ( const QString & path );

private:
  Destinations(QObject* parent = 0);
  Destinations(const Destinations&);
  virtual ~Destinations();
  Destinations& operator=(const Destinations&);

  DestinationsImpl* d;
};

//! Creates logging destinations/sinks. The caller will have ownership of
//! the newly created destinations.
class LIMA_COMMONQSLOG_EXPORT DestinationFactory
{
public:
   static DestinationPtr MakeFileDestination(const QString& filePath);
   static DestinationPtr MakeDebugOutputDestination();
};

//! debugger sink
class LIMA_COMMONQSLOG_EXPORT DebugOutputDestination : public Destination
{
public:
  virtual void write(const QString& message, const QString& zone = QLatin1String("")) override;
};


} // end namespace

#endif // QSLOGDEST_H
