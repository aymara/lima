// Copyright (c) 2010, Razvan Petru
// All rights reserved.

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

#ifndef QSLOG_H
#define QSLOG_H

#include "QsLogDest.h"

#ifdef WIN32
#pragma warning(disable: 4127)
#endif
#include <QDebug>
#include <QString>

#include <iostream>

#include "QsLog_export.h"


namespace QsLogging
{
enum Level
{
   TraceLevel = 0,
   DebugLevel,
   InfoLevel,
   WarnLevel,
   ErrorLevel,
   FatalLevel
};


class Destination;
class LoggerImpl; // d pointer
class LIMA_COMMONQSLOG_EXPORT Logger
{
  friend class LoggerImpl;
public:
   static Logger& instance(const QString& zone = QLatin1String(""))
   {
      static QMap<QString,Logger*> staticLog;
      QMap<QString,Logger*>::iterator it = staticLog.find(zone);
      if (it == staticLog.end())
      {
        Logger* logger = new Logger(zone);
        for (auto destination: Destinations::instance().destinations())
        {
          logger->addDestination(destination);
        }
        return **staticLog.insert(zone, logger);
      }
      return **it;
   }

   //! Adds a log message destination. Don't add null destinations.
   void addDestination(std::shared_ptr<Destination> destination);
   //! Logging at a level < 'newLevel' will be ignored
   void setLoggingLevel(Level newLevel);
   //! The default level is INFO
   Level loggingLevel() const;
   const QString& zone() const;

  //! Returns true if logging level is lower than or equal to FatalLevel
  bool isLoggingEnabled() const;
  //! Returns true if logging level is lower than or equal to TraceLevel
  bool isTraceEnabled() const;
  //! Returns true if logging level is lower than or equal to DebugLevel
  bool isDebugEnabled() const;
  //! Returns true if logging level is lower than or equal to InfoLevel
  bool isInfoEnabled() const;
  //! Returns true if logging level is lower than or equal to WarnLevel
  bool isWarnEnabled() const;
  //! Returns true if logging level is lower than or equal to ErrorLevel
  bool isErrorEnabled() const;
  //! Returns true if logging level is lower than or equal to FatalLevel
  bool isFatalEnabled() const;

   //! The helper forwards the streaming to QDebug and builds the final
   //! log message.
   class LIMA_COMMONQSLOG_EXPORT Helper
   {
   public:
      explicit Helper(Level logLevel, const QString& zone) :
            level(logLevel),
            qtDebug(&buffer),
            zone(zone){}
      ~Helper();
      QDebug& stream(){ return qtDebug; }

   private:
      void writeToLog();

      Level level;
      QString buffer;
      QDebug qtDebug;
      QString zone;
   };

   Logger& operator=(const Logger&);
private:
   Logger(const QString& aZone = QLatin1String(""));
   Logger(const Logger&);
   ~Logger();

   void write(const QString& message);

   LoggerImpl* d;
};

LIMA_COMMONQSLOG_EXPORT QDebug&  operator<< (QDebug&  qd, const std::string& str );

} // end namespace

//! Logging macros: define QS_LOG_LINE_NUMBERS to get the file and line number
//! in the log output.
#ifndef QS_LOG_LINE_NUMBERS
   #define QLOG_TRACE() \
   if( logger.loggingLevel() > QsLogging::TraceLevel ){} \
     else QsLogging::Logger::Helper(QsLogging::TraceLevel, logger.zone()).stream()
   #define QLOG_DEBUG() \
   if( logger.loggingLevel() > QsLogging::DebugLevel ){} \
     else QsLogging::Logger::Helper(QsLogging::DebugLevel, logger.zone()).stream()
   #define QLOG_INFO()  \
   if( logger.loggingLevel() > QsLogging::InfoLevel ){} \
     else QsLogging::Logger::Helper(QsLogging::InfoLevel, logger.zone()).stream()
   #define QLOG_WARN()  \
   if( logger.loggingLevel() > QsLogging::WarnLevel ){} \
     else QsLogging::Logger::Helper(QsLogging::WarnLevel, logger.zone()).stream()
   #define QLOG_ERROR() \
   if( logger.loggingLevel() > QsLogging::ErrorLevel ){} \
     else QsLogging::Logger::Helper(QsLogging::ErrorLevel, logger.zone()).stream()
   #define QLOG_FATAL() \
   QsLogging::Logger::Helper(QsLogging::FatalLevel, logger.zone()).stream()
#else
   #define QLOG_TRACE() \
   if( logger.loggingLevel() > QsLogging::TraceLevel ){} \
     else  QsLogging::Logger::Helper(QsLogging::TraceLevel, logger.zone()).stream() << __FILE__ << '@' << __LINE__
   #define QLOG_DEBUG() \
   if( logger.loggingLevel() > QsLogging::DebugLevel ){} \
     else QsLogging::Logger::Helper(QsLogging::DebugLevel, logger.zone()).stream() << __FILE__ << '@' << __LINE__
   #define QLOG_INFO()  \
   if( logger.loggingLevel() > QsLogging::InfoLevel ){} \
     else QsLogging::Logger::Helper(QsLogging::InfoLevel, logger.zone()).stream() << __FILE__ << '@' << __LINE__
   #define QLOG_WARN()  \
   if( logger.loggingLevel() > QsLogging::WarnLevel ){} \
     else QsLogging::Logger::Helper(QsLogging::WarnLevel, logger.zone()).stream() << __FILE__ << '@' << __LINE__
   #define QLOG_ERROR() \
   if( logger.loggingLevel() > QsLogging::ErrorLevel ){} \
     else QsLogging::Logger::Helper(QsLogging::ErrorLevel, logger.zone()).stream() << __FILE__ << '@' << __LINE__
   #define QLOG_FATAL() \
   QsLogging::Logger::Helper(QsLogging::FatalLevel, logger.zone()).stream() << __FILE__ << '@' << __LINE__
#endif

#endif // QSLOG_H
