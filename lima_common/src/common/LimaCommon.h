/*
    Copyright 2002-2021 CEA LIST

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
/************************************************************************
 *
 * @file       LimaCommon.h (from s2Common.h)
 * @author     Gael de Chalendar <Gael.de-Chalendar@cea.fr>

 *             Benoit Mathieu <mathieub@zoe.cea.fr>

 *             Hervé Le Borgne <herve.le-borgne@cea.fr>

 * @date       mar déc 18 2007
 * copyright   Copyright (C) 2003-2021 by CEA LIST
 * Project     mm_common
 *
 * @brief      (short description)
 *
 ***********************************************************************/
#ifndef LIMA_MMCOMMONS_H
#define LIMA_MMCOMMONS_H

#include <QtGlobal>

#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
#include <QtCore/QMutex>
#define QTENDL endl
#else
#include <QtCore/QRecursiveMutex>
#define QTENDL Qt::endl
#endif

#include <cstdint>

#ifdef WIN32

#pragma warning( disable : 4512 )

// Avoids compilation errors redefining struc sockaddr in ws2def.h
#define _WINSOCKAPI_

#undef min
#undef max
typedef __int16 int16_t;
typedef unsigned __int16 uint16_t;
typedef __int32 int32_t;
typedef unsigned __int32 uint32_t;
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
#endif


#include <boost/serialization/strong_typedef.hpp>

#ifdef WIN32


#ifdef LIMA_DATA_EXPORTING
   #define LIMA_DATA_EXPORT    __declspec(dllexport)
#else
   #define LIMA_DATA_EXPORT    __declspec(dllimport)
#endif

#ifdef LIMA_DATAHANDLER_EXPORTING
   #define LIMA_DATAHANDLER_EXPORT    __declspec(dllexport)
#else
   #define LIMA_DATAHANDLER_EXPORT    __declspec(dllimport)
#endif

#ifdef LIMA_FSAACCESS_EXPORTING
  #define LIMA_FSAACCESS_EXPORT    __declspec(dllexport)
#else
  #define LIMA_FSAACCESS_EXPORT    __declspec(dllimport)
#endif

#ifdef LIMA_MEDIAPROCESSORS_EXPORTING
   #define LIMA_MEDIAPROCESSORS_EXPORT    __declspec(dllexport)
#else
   #define LIMA_MEDIAPROCESSORS_EXPORT    __declspec(dllimport)
#endif

#ifdef LIMA_MEDIATICDATA_EXPORTING
   #define LIMA_MEDIATICDATA_EXPORT    __declspec(dllexport)
#else
   #define LIMA_MEDIATICDATA_EXPORT    __declspec(dllimport)
#endif

#ifdef LIMA_COMMONMISC_EXPORTING
   #define LIMA_COMMONMISC_EXPORT    __declspec(dllexport)
#else
   #define LIMA_COMMONMISC_EXPORT    __declspec(dllimport)
#endif

#ifdef LIMA_COMMONTOOLS_EXPORTING
   #define LIMA_COMMONTOOLS_EXPORT    __declspec(dllexport)
#else
   #define LIMA_COMMONTOOLS_EXPORT    __declspec(dllimport)
#endif

#ifdef LIMA_PROCESSUNITFRAMEWORK_EXPORTING
   #define LIMA_PROCESSUNITFRAMEWORK_EXPORT    __declspec(dllexport)
#else
   #define LIMA_PROCESSUNITFRAMEWORK_EXPORT    __declspec(dllimport)
#endif

#ifdef LIMA_TIME_EXPORTING
   #define LIMA_TIME_EXPORT    __declspec(dllexport)
#else
   #define LIMA_TIME_EXPORT    __declspec(dllimport)
#endif

#ifdef LIMA_XMLCONFIGURATIONFILES_EXPORTING
   #define LIMA_XMLCONFIGURATIONFILES_EXPORT    __declspec(dllexport)
#else
   #define LIMA_XMLCONFIGURATIONFILES_EXPORT    __declspec(dllimport)
#endif

#ifdef LIMA_COMMON_EXPORTING
   #define LIMA_COMMON_EXPORT    __declspec(dllexport)
#else
   #define LIMA_COMMON_EXPORT    __declspec(dllimport)
#endif

#else // Not WIN32

#define LIMA_DATA_EXPORT
#define LIMA_DATAHANDLER_EXPORT
#define LIMA_FSAACCESS_EXPORT
#define LIMA_MEDIAPROCESSORS_EXPORT
#define LIMA_MEDIATICDATA_EXPORT
#define LIMA_COMMONMISC_EXPORT
#define LIMA_COMMONTOOLS_EXPORT
#define LIMA_PROCESSUNITFRAMEWORK_EXPORT
#define LIMA_TIME_EXPORT
#define LIMA_XMLCONFIGURATIONFILES_EXPORT
#define LIMA_COMMON_EXPORT

#endif

#include <string>
#include <stdexcept>

#ifndef LIMA_DEBUG
#define LIMA_DEBUG 0
#endif

// standard include
#include <sstream>

#include <common/QsLog/QsLog.h>
#include <common/QsLog/QsLogCategories.h>
#include "common/QsLog/QsLogDest.h"

#define LTRACE QLOG_TRACE()
#define LDEBUG QLOG_DEBUG()
#define LINFO QLOG_INFO()
#define LNOTICE QLOG_INFO()
#define LWARN QLOG_WARN()
#define LERROR QLOG_ERROR()
#define LFATAL QLOG_FATAL()

// #define LOGINIT(X) QsLogging::Logger& logger = QsLogging::Logger::instance(X);
// logger.setLoggingLevel( QsLogging::Categories::instance().levelFor( X ) );

class LogInit
{
public:
  LogInit(char const* x)
  {
    // initialisation thread-safe
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
    static QMutex mutex(QMutex::Recursive);
#else
    static QRecursiveMutex mutex;
#endif
    QMutexLocker locker(&mutex);
    pLogger = &QsLogging::Logger::instance(QLatin1String(x));
#ifndef DEBUG_CD
    QsLogging::Level level = QsLogging::Categories::instance().levelFor(x);
    pLogger->setLoggingLevel(level);
#endif
  }
  QsLogging::Logger* pLogger;
};
#define LOGINIT(X) \
  static LogInit logInit(X); /*initialisation exécutée une seul fois*/\
  auto& logger = *(logInit.pLogger); \
  logger.setLoggingLevel(QsLogging::Categories::instance().levelFor( X ));

//QsLogging::DestinationPtr debugDestination(  QsLogging::DestinationFactory::MakeDebugOutputDestination() );
//logger.addDestination(debugDestination.get());


#define LENDL ". Note: LENDL is deprecated. It will be removed from a future release."

#define ABSTRACTFACTORYPATTERNLOGINIT LOGINIT("Common::AbstractFactoryPattern")
#define ABSTRACTPROCESSINGCLIENTLOGINIT LOGINIT("Common::AbstractProcessingClient")
#define AGLOGINIT LOGINIT("Common::AnnotationGraph")
#define BOWLOGINIT LOGINIT("Common::BOW");
#define CLIENTFACTORYLOGINIT LOGINIT("Common::ClientFactory")
#define COMPSTRACCESSLOGINIT LOGINIT("Common::CompStrAccess")
#define FSAAHASHLOGINIT LOGINIT("Common::FsaAccessHash")
#define FSAAIOLOGINIT LOGINIT("Common::FsaAccessIO")
#define FSAALOGINIT LOGINIT("Common::FsaAccess")
#define HANDLERLOGINIT LOGINIT("Common::Handler")
#define LDATALOGINIT LOGINIT("Common::LanguageData")
#define MDATALOGINIT LOGINIT("Common::MediaticData")
#define MISCLOGINIT LOGINIT("Common::Misc")
#define PROCESSORSLOGINIT LOGINIT("Common::Processors")
#define PROCESSUNITFRAMEWORKLOGINIT LOGINIT("Common::ProcessUnitFramework")
#define PROPERTYCODELOGINIT LOGINIT("Common::PropertyCode")
#define STRINGMAPLOGINIT LOGINIT("Common::StringMap")
#define STRPOOLLOGINIT LOGINIT("Common::StringPool")
#define TGVLOGINIT LOGINIT("Common::TGV")
#define XMLCFGLOGINIT LOGINIT("Common::XMLConfigurationFiles")
#define DYNAMICLIBMANAGERLOGINIT LOGINIT("Common::DynamicLibrariesManager")

QDebug&  operator<< (QDebug&  qd, const std::string& str );


#ifndef LIMA_UNUSED
#define LIMA_UNUSED(x) (void)x;
#endif

#define USE_STD_BITSET_FOR_LINGUISTIC_CODE

#ifdef USE_STD_BITSET_FOR_LINGUISTIC_CODE
#include "LinguisticCode/StdBitset.h"
#endif

namespace Lima
{

enum LimaStatusCode {
    SUCCESS_ID,
    UNKNOWN_ERROR,
    CANNOT_OPEN_FILE_ERROR,
    OUT_OF_RANGE_ERROR,
    UNSUPPORTED_LANGUAGE,
    INVALID_CONFIGURATION,
    MISSING_DATA,
    UNKNOWN_FORMAT
};

BOOST_STRONG_TYPEDEF(char, NoParameters);

#define UNDEFLANG std::numeric_limits<uint8_t>::max()

BOOST_STRONG_TYPEDEF(uint8_t, MediaId);

/**
 * The main LIMA exception class. Throw this exception to signal a problem with
 * a precise associated message.
 *
 * Instead of directly throwing this exception, prefer using one of the
 * @ref LIMA_EXCEPTION macros to automatically include location information useful
 * for debugging.
 */
class LimaException : public std::exception
{
public:
    explicit LimaException(const char* message = "") :
        std::exception(), m_reason(message)
    {
    }
    explicit LimaException(const QString& message) :
        std::exception(), m_reason(message.toStdString())
    {
    }
    explicit LimaException(const std::string& message) :
        std::exception(), m_reason(message)
    {
    }
    LimaException(const LimaException&) = default;
    LimaException& operator=(const LimaException& e) = delete;
    virtual ~LimaException() throw() = default;

    virtual const char * what () const throw() override
    {
        return m_reason.c_str();
    }

protected:
    std::string m_reason;
};

/**
 * This macro writes the message @ref X to a previously configured error stream
 * before throwing a LimaException with the same message
 */
#define LIMA_EXCEPTION(X) { \
    QString errorString; \
    QTextStream qts(&errorString); \
    qts << __FILE__ << ":" << __LINE__ << ": " << X ; \
    LERROR << errorString; \
    throw LimaException(errorString); \
}

/**
 * This macro writes the message @ref X to a previously configured error stream
 * before throwing a @ref Y exception (@ref Y is an exception class name) with
 * the same message.
 */
#define LIMA_EXCEPTION_SELECT(X,Y) { \
    QString errorString; \
    QTextStream qts(&errorString); \
    qts << __FILE__ << ":" << __LINE__ << ": " << X ; \
    LERROR << errorString; \
    throw Y(errorString); \
}

/**
 * This macro writes the message @ref Y to the error stream configured by its
 * first parameter @ref X, before throwing LimaException with the same message.
 */
#define LIMA_EXCEPTION_LOGINIT(X,Y) { \
    X ; \
    QString errorString; \
    QTextStream qts(&errorString); \
    qts << __FILE__ << ":" << __LINE__ << ": " << Y ; \
    LERROR << errorString; \
    throw Lima::LimaException(errorString); \
}

/**
 * This macro writes the message @ref Y to the error stream configured by its
 * first parameter @ref X, before throwing a @ref Z exception (@ref Z is an
 * exception class name) with the same message.
 */
#define LIMA_EXCEPTION_SELECT_LOGINIT(X,Y,Z) { \
    X ; \
    QString errorString; \
    QTextStream qts(&errorString); \
    qts << __FILE__ << ":" << __LINE__ << ": " << Y ; \
    LERROR << errorString; \
    throw Z(errorString); \
}

/**
 * Use this exception to signal an error in one of the configuration files
 */
class InvalidConfiguration : public LimaException
{
public:
    explicit InvalidConfiguration(const char* message = "") :
        LimaException(std::string(message))
    {
    }
    explicit InvalidConfiguration(const std::string& message) :
        LimaException(message)
    {
    }
    explicit InvalidConfiguration(const QString& message) :
        LimaException(message)
    {
    }
    virtual ~InvalidConfiguration() throw() = default;
    InvalidConfiguration(const InvalidConfiguration&) = default;
    InvalidConfiguration& operator=(const InvalidConfiguration& e) = delete;
};

/**
 * Use this exception to signal that a media is used which has not been
 * initialized.
 */
class MediaNotInitialized : public LimaException
{
public :
    MediaNotInitialized(MediaId medId) :
        LimaException(),
        m_medId(medId),
        m_med(),
        m_num(true)
    {
      if (m_num)
      {
          std::ostringstream oo(m_reason);
          oo << "uninitialized media " << (int)m_medId;
      }
      else
      {
        m_reason = std::string("uninitialized media ")+m_med;
      }
    }

    explicit MediaNotInitialized(const QString& media) :
        LimaException(media),
        m_medId(0),
        m_med(media.toStdString()),
        m_num(false)
    {
    }
    explicit MediaNotInitialized(const std::string& media) :
        LimaException(media),
        m_medId(0),
        m_med(media),
        m_num(false)
    {
      if (m_num)
      {
          std::ostringstream oo(m_reason);
          oo << "uninitialized media " << (int)m_medId;
      }
      else
      {
        m_reason = std::string("uninitialized media ")+m_med;
      }

    };

    MediaNotInitialized(const MediaNotInitialized&) = default;
    MediaNotInitialized& operator=(const MediaNotInitialized&) = delete;
    virtual ~MediaNotInitialized() throw() = default;

private:
  MediaId m_medId;
  std::string  m_med;
  bool m_num;
};

/**
 * Use this exception to signal the used of a wrongly initialized LIMA
 * dictionary.
 */
class AccessByStringNotInitialized : public LimaException
{
public :
    AccessByStringNotInitialized(const QString& reason) :
        LimaException(reason)
    {
        m_reason = std::string("Fsa not initialized because of ") + reason.toStdString();
    }
    AccessByStringNotInitialized(const std::string& reason) :
        LimaException(reason)
    {
        m_reason = std::string("Fsa not initialized because of ") + reason;
    }
    AccessByStringNotInitialized(const AccessByStringNotInitialized&) = default;
    AccessByStringNotInitialized& operator=(const AccessByStringNotInitialized&) = delete;
    virtual ~AccessByStringNotInitialized() throw() {};

};

/**
 * Use this exception to signal a wrong access to a LIMA dictionary.
 */
class AccessByStringOutOfRange : public LimaException
{
public :
    AccessByStringOutOfRange(const QString& reason) : LimaException()
    {
      m_reason = std::string("parameter out of range ") + reason.toStdString();
    }
    AccessByStringOutOfRange(const std::string& reason) : LimaException()
    {
      m_reason = std::string("parameter out of range ") + reason;
    }
    AccessByStringOutOfRange(const AccessByStringOutOfRange&) = default;
    AccessByStringOutOfRange& operator=(const AccessByStringOutOfRange&) = delete;
    virtual ~AccessByStringOutOfRange() throw() {};

};

/**
 * Throw this exception when encountering problems with XML files opening or parsing
 */
class XMLException : public Lima::LimaException
{
public:
  explicit XMLException(const char* message="") :
      Lima::LimaException()
  {
    m_reason = std::string("XMLException: ") + message;
  }
  explicit XMLException(const QString& message) :
      Lima::LimaException()
  {
    m_reason = std::string("XMLException: ") + message.toStdString();
  }
  explicit XMLException(const std::string& message) :
      Lima::LimaException()
  {
    m_reason = std::string("XMLException: ") + message;
  }
  XMLException(const XMLException&) = default;
  XMLException& operator=(const XMLException&) = delete;
};


} // closing namespace Lima

#endif // LIMA_MMCOMMONS_H
