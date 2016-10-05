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
/************************************************************************
 *
 * @file       LimaCommon.h (from s2Common.h)
 * @author     Gael de Chalendar <Gael.de-Chalendar@cea.fr> 

 *             Benoit Mathieu <mathieub@zoe.cea.fr> 

 *             Hervé Le Borgne <herve.le-borgne@cea.fr> 

 * @date       mar déc 18 2007
 * copyright   Copyright (C) 2003-2012 by CEA LIST
 * Project     mm_common
 *
 * @brief      (short description)
 *
 ***********************************************************************/
#ifndef LIMA_MMCOMMONS_H
#define LIMA_MMCOMMONS_H

#include <QtCore/QMutex>
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
#ifdef ANTINNO_SPECIFIC
// FWI 19/05/2016 ajout 2 includes
#include <QtCore/QString>
#include <QtCore/QStringList>

#ifdef WIN32

#ifdef LIMA_COMMON_EXPORTING
#define LIMA_COMMON_EXPORT    __declspec(dllexport)
#else
#define LIMA_COMMON_EXPORT    __declspec(dllimport)
#endif


#else // Not WIN32

#define LIMA_COMMON_EXPORT

#endif
namespace Lima
{
#ifdef _DEBUG
  class LIMA_COMMON_EXPORT StopAnalyze
  {
    bool _v;
  public:
    StopAnalyze(bool v);
    StopAnalyze(StopAnalyze const&);
    operator bool() const;
    StopAnalyze& operator=(StopAnalyze const& o);
    bool operator==(StopAnalyze const& o);
    bool operator!=(StopAnalyze const& o);
  };
#else
  typedef bool LIMA_COMMON_EXPORT StopAnalyze;
#endif
  extern LIMA_COMMON_EXPORT StopAnalyze defaultStopAnalyze;
}

#define LTRACE \
  if ( logger.loggingLevel() <= QsLogging::TraceLevel ) \
    QsLogging::antinno::LogHelper(QsLogging::TraceLevel, logger.zone()).stream()
#define LDEBUG \
  if ( logger.loggingLevel() <= QsLogging::DebugLevel ) \
     QsLogging::antinno::LogHelper(QsLogging::DebugLevel, logger.zone()).stream()
#define LINFO \
  if ( logger.loggingLevel() <= QsLogging::InfoLevel ) \
     QsLogging::antinno::LogHelper(QsLogging::InfoLevel, logger.zone()).stream()
#define LNOTICE \
  if ( logger.loggingLevel() <= QsLogging::InfoLevel ) \
     QsLogging::antinno::LogHelper(QsLogging::InfoLevel, logger.zone()).stream()
#define LWARN \
  if ( logger.loggingLevel() <= QsLogging::WarnLevel ) \
     QsLogging::antinno::LogHelper(QsLogging::WarnLevel, logger.zone()).stream()
#define LERROR \
  if ( logger.loggingLevel() <= QsLogging::ErrorLevel ) \
     QsLogging::antinno::LogHelper(QsLogging::ErrorLevel, logger.zone()).stream()
#define LFATAL \
  if ( logger.loggingLevel() <= QsLogging::FatalLevel ) \
     QsLogging::antinno::LogHelper(QsLogging::FatalLevel, logger.zone()).stream()

#else

#define LTRACE QLOG_TRACE()
#define LDEBUG QLOG_DEBUG()
#define LINFO QLOG_INFO()
#define LNOTICE QLOG_INFO()
#define LWARN QLOG_WARN()
#define LERROR QLOG_ERROR()
#define LFATAL QLOG_FATAL()

#endif

// #define LOGINIT(X) QsLogging::Logger& logger = QsLogging::Logger::instance(X); 
// logger.setLoggingLevel( QsLogging::Categories::instance().levelFor( X ) );

class LogInit
{
public:
  LogInit(char const* x)
  {
    // initialisation thread-safe
    static QMutex mutex;
    QMutexLocker locker(&mutex);
    pLogger = &QsLogging::Logger::instance(x);
#ifndef DEBUG_CD
    QsLogging::Level level = QsLogging::Categories::instance().levelFor(x);
    pLogger->setLoggingLevel(level);
#endif

  }
  QsLogging::Logger* pLogger;
};
#ifndef DEBUG_CD
#define LOGINIT(X) \
  static LogInit logInit(X); /*initialisation exécutée une seul fois*/\
  auto& logger = *(logInit.pLogger);
#else
#define LOGINIT(X) \
  static LogInit logInit(X); /*initialisation exécutée une seul fois*/\
  auto& logger = *(logInit.pLogger); \
  logger.setLoggingLevel(QsLogging::Categories::instance().levelFor( X ));
#endif

//QsLogging::DestinationPtr debugDestination(  QsLogging::DestinationFactory::MakeDebugOutputDestination() );
//logger.addDestination(debugDestination.get());
#ifdef ANTINNO_SPECIFIC
// FWI 07/10/2015 ajout pour les logger
static std::ostream& operator<<(std::ostream &os, const QString& s)
{
  os << s.toUtf8().constData();
  return os;
}

static ::std::ostream& operator<<(::std::ostream& out, QStringList const& o)
{
  bool isFirst = true;
  for(auto it=o.constBegin(); it!=o.constEnd(); ++it)
  {
    out << (isFirst?L"":L",") << *it;
    isFirst = false;
  }
  return out;
}
#endif



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

namespace Lima
{

enum LimaStatusCode {
    SUCCESS_ID,
    CANNOT_OPEN_FILE_ERROR,
    OUT_OF_RANGE_ERROR,
    UNKNOWN_ERROR,
    UNSUPPORTED_LANGUAGE,
    INVALID_CONFIGURATION,
    MISSING_DATA
#ifdef ANTINNO_SPECIFIC
    // FWI 22/02/2016 ajout TIME_OVERFLOW pour stopAnalyze
    ,TIME_OVERFLOW
#endif
};

#ifdef ANTINNO_SPECIFIC
BOOST_STRONG_TYPEDEF(unsigned int, ReformulationType)
#endif


BOOST_STRONG_TYPEDEF(uint32_t, LinguisticCode);
#ifdef ANTINNO_SPECIFIC
// FWI 25/05/2016 : on spécialise max() pour le type LinguisticCode sinon le max() d'origine renvoit 0 (ce qui est un bug de la lib std)
}
namespace std {
  template <> Lima::LinguisticCode numeric_limits<Lima::LinguisticCode>::max() { return Lima::LinguisticCode(::std::numeric_limits<uint32_t>::max()); }
}
namespace Lima {
#endif

BOOST_STRONG_TYPEDEF(char, NoParameters);
#ifdef ANTINNO_SPECIFIC
// FWI 25/05/2016 : on spécialise max() pour le type LinguisticCode sinon le max() d'origine renvoit 0 (ce qui est un bug de la lib std)
}
namespace std {
  template <> Lima::NoParameters numeric_limits<Lima::NoParameters>::max() { return Lima::NoParameters(::std::numeric_limits<char>::max()); }
}
namespace Lima {
#endif

#define UNDEFLANG std::numeric_limits<uint8_t>::max()

BOOST_STRONG_TYPEDEF(uint8_t, MediaId);
#ifdef ANTINNO_SPECIFIC
// FWI 25/05/2016 : on spécialise max() pour le type MediaId sinon le max() d'origine renvoit 0 (ce qui est un bug de la lib std)
}
namespace std {
  template <> Lima::MediaId numeric_limits<Lima::MediaId>::max() { return Lima::MediaId(::std::numeric_limits<uint8_t>::max()); }
}
namespace Lima {
#endif

class LimaException : public std::exception
{
public:
    LimaException() : std::exception(),m_reason() {}
    LimaException(const std::string& mess) : std::exception(), m_reason(mess) {}
    virtual ~LimaException() throw() {}
    virtual const char * what () const throw() {
        return m_reason.c_str();
    }
protected:
  LimaException& operator=(const LimaException&) {return  *this;}
  const std::string m_reason;
};

class InvalidConfiguration : public LimaException
{
  public:
    InvalidConfiguration() : LimaException() {};
    InvalidConfiguration(const std::string& mess) : LimaException(mess) {}
private:
  InvalidConfiguration& operator=(const InvalidConfiguration&) {return  *this;}
};
class MediaNotInitialized : public LimaException
{
public :
    MediaNotInitialized(MediaId medId) : LimaException(),m_medId(medId),m_med(),m_num(true) {};
    MediaNotInitialized(const std::string& med) : LimaException(),m_medId(0),m_med(med),m_num(false) {};
    virtual ~MediaNotInitialized() throw() {};
    const char* what() const throw()
    {
        if (m_num)
        {
            std::ostringstream oo;
            oo << "uninitialized media " << (int)m_medId;
            return oo.str().c_str();
        }
        else
        {
            return (std::string("uninitialized media ")+m_med).c_str();
        }
    };
private:
  MediaNotInitialized& operator=(const MediaNotInitialized&) {return  *this;}
  MediaId m_medId;
    std::string  m_med;
    bool m_num;
};

class LanguageNotInitialized : public LimaException {
public :
    LanguageNotInitialized(MediaId langId) : LimaException(),m_langId(langId),m_lang(),m_num(true) {};
    LanguageNotInitialized(const std::string& lang) : LimaException(),m_langId(0),m_lang(lang),m_num(false) {};
    virtual ~LanguageNotInitialized() throw() {};
    const char* what() const throw() {
        if (m_num) {
            std::ostringstream oo;
            oo << "uninitialized language " << (int)m_langId;
            return oo.str().c_str();
        } else {
            return (std::string("uninitialized language ")+m_lang).c_str();
        }
    };
private:
  LanguageNotInitialized& operator=(const LanguageNotInitialized&) {return  *this;}
  MediaId m_langId;
    std::string  m_lang;
    bool m_num;
};

class AccessByStringNotInitialized : public LimaException {
public :
    AccessByStringNotInitialized(const std::string& reason) : LimaException(), m_reason(reason) {};
    virtual ~AccessByStringNotInitialized() throw() {};
    const char* what() const throw() {
        std::ostringstream oo;
        oo << "Fsa not initialized because of " << m_reason;
        return oo.str().c_str();
    };
private:
  AccessByStringNotInitialized& operator=(const AccessByStringNotInitialized&) {return  *this;}
  std::string  m_reason;
};

class AccessByStringOutOfRange : public LimaException {
public :
    AccessByStringOutOfRange(const std::string& reason) : LimaException(), m_reason(reason) {};
    virtual ~AccessByStringOutOfRange() throw() {};
    const char* what() const throw() {
        std::ostringstream oo;
        oo << "parameter out of range " << m_reason;
        return oo.str().c_str();
    };
private:
  AccessByStringOutOfRange& operator=(const AccessByStringOutOfRange&) {return  *this;}
  std::string  m_reason;
};

class IncompleteResources : public LimaException {
public :
    IncompleteResources(const std::string& reason) : LimaException(), m_reason(reason) {}
    virtual ~IncompleteResources() throw() {}
    const char* what() const throw() {
        return (std::string("incomplete ressources:  ") + m_reason).c_str() ;
    }
private:
  IncompleteResources& operator=(const IncompleteResources&) {return  *this;}
  std::string  m_reason;
};

class XMLException : public std::runtime_error
{
public:
  explicit XMLException(const std::string& msg = "") : std::runtime_error(msg) {}
  const char* getMessage() const {return this->what();}
private:
  XMLException& operator=(const XMLException&) {return  *this;}
};


} // closing namespace Lima

#endif // LIMA_MMCOMMONS_H
