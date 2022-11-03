// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
                          testContentDict16.cpp  -  description
                             -------------------
    begin                : lun jun 2 2003
    copyright            : (C) 2003 by Olivier Mesnard
    email                : olivier.mesnard@cea.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *  compact dictionnary based on finite state automata                     *
 *  implemented with Boost Graph library                                   *
 *                                                                         *
 ***************************************************************************/

#include <iostream>
#include <iomanip>

#include <string>
using namespace std;
#include "common/LimaCommon.h"

#include "common/time/traceUtils.h"

// string and file handling Utilities
#include "common/Data/strwstrtools.h"

#include "common/FsaAccess/FsaAccessSpare16.h"
#include "common/StringMap/IndirectDataDico.h"
#include "common/StringMap/SimpleDataDico.h"

#include <QtCore/QCoreApplication>

//#include "common/linguisticData/linguisticData.h"

using namespace Lima;
using namespace Lima::Common;
using namespace Lima::Common::FsaAccess;
using namespace Lima::Common::StringMap;
//using namespace Lima::Common::LinguisticData;

// options
typedef struct ParamStruct {
  std::string key;
  int offset;
  std::string keyFileName;
  std::string dataFileName;
  std::string accessMethod;
  bool withAssert;
  std::string contentType;
} Param;

void testSimpleDataDico(const Param& param );
void testAnalysisDico(const Param& param );

#include "common/tools/LimaMainTaskRunner.h"
#include "common/AbstractFactoryPattern/AmosePluginsManager.h"
#include <QtCore/QTimer>

int run(int aargc,char** aargv);

int main(int argc, char **argv)
{
  QCoreApplication a(argc, argv);

  // Task parented to the application so that it
  // will be deleted by the application.
  LimaMainTaskRunner* task = new LimaMainTaskRunner(argc, argv, run, &a);

  // This will cause the application to exit when
  // the task signals finished.
  QObject::connect(task, SIGNAL(finished(int)), &a, SLOT(quit()));

  // This will run the task from the application event loop.
  QTimer::singleShot(0, task, SLOT(run()));

  return a.exec();

}


int run(int argc,char** argv)
{
  QsLogging::initQsLog();
  // Necessary to initialize factories
  Lima::AmosePluginsManager::single();
  
  cerr << "testContentDict16 begin..." << endl;

  setlocale(LC_ALL, "");

#ifdef DEBUG_CD
  FSAALOGINIT;
  LDEBUG <<  "testContentDict16 begin...";
#endif

  // options reading
  Param param = {
    std::string(),  // key
    0,              // offset
    std::string(),  // keyFileName
    std::string(),  // dataFileName
    std::string(),  // accessMethod
    false,          // withAssert
    std::string()  // contentType
  };

  for (int i = 1 ; i < argc; i++) {
    std::string arg(argv[i]);
    std::string::size_type pos = std::string::npos;
    if (arg == "--help")
    {
      std::cerr << "usage: " << argv[0]
                << " --help" << std::endl;
      std::cerr << "       " << argv[0]
                << " [--key=<word>]"
                << " [--offset=<int>]"
                << " [--keyFileName=<filename>]"
                << " [--dataFileName=<filename>]"
                << " [--accessMethod=Fsa|Tree]"
                << " [--contentType=simple|indirect]"
                << " [--codetaFileName=<filename>]"
                << " [--language=fre|eng]"
                << " [--withAssert]"
                << std::endl;
      return 0;
    }
    else if ( (pos = arg.find("--keyFileName=")) != std::string::npos ){
      param.keyFileName = arg.substr(pos+14);
    }
    else if ( (pos = arg.find("--dataFileName=")) != std::string::npos ){
      param.dataFileName = arg.substr(pos+15);
    }
    else if ( (pos = arg.find("--key=")) != std::string::npos ){
      param.key = arg.substr(pos+6);
    }
    else if ( (pos = arg.find("--offset=")) != std::string::npos ){
      param.offset = atoi( (arg.substr(pos+9)).c_str() );
    }
    else if ( arg.compare("--withAssert") == 0 ){
      param.withAssert = true;
    }
    else if ( (pos = arg.find("--accessMethod=")) != std::string::npos ){
      param.accessMethod = arg.substr(pos+15);
    }
    else if ( (pos = arg.find("--contentType=")) != std::string::npos ){
      std::cerr << "arg = " << arg << std::endl;
      param.contentType = arg.substr(pos+14);
    }
  }

  cerr << "testContentDict16: ";
  if(param.keyFileName.size()) {
    cerr << "--keyFileName='" << param.keyFileName << "' ";
  }
  if(param.dataFileName.size()) {
    cerr << "--dataFileName='" << param.dataFileName << "' ";
  }
  if(param.withAssert) {
    cerr << "--withAssert ";
  }
  if(param.accessMethod.size()) {
    cerr << "--accessMethod='" << param.accessMethod << "' ";
  }
  if(param.contentType.size()) {
    cerr << "--contentType='" << param.contentType << "' ";
  }
  cerr << "--key="<< param.key << " ";
  cerr << "--offset="<< param.offset << " ";
  cerr << endl;

  if( !param.contentType.compare(std::string("simple")) ) {
    // Test dictionnaire simple (type dictionnaire de fr�uence)
    testSimpleDataDico(param);
  }
  if( !param.contentType.compare(std::string("indirect")) ) {
    // Test dictionnaire d'analyse
    testAnalysisDico( param );
  }

  return EXIT_SUCCESS;
}

void testSimpleDataDico(const Param& param ) {
  typedef int simpleDataElement;
  typedef std::vector<simpleDataElement> simpleStoredSet;
  typedef Lima::Common::FsaAccess::FsaAccessSpare16 accessMethod;
  typedef Lima::Common::StringMap::SimpleDataDico<accessMethod, simpleDataElement,simpleStoredSet> SimpleDicoType;

  SimpleDicoType* dico = new SimpleDicoType(-1);
  dico->parseAccessMethod(param.keyFileName);
  dico->parseData(param.dataFileName);
  dico->getSize();
  Lima::LimaString limaKey(Misc::utf8stdstring2limastring(param.key));
  simpleDataElement val = dico->getElement(limaKey);
  std::cout << "simpleDico->getElement(" << param.key << ") =" << val << std::endl;
}

typedef unsigned char* analysisDataElement;
typedef FsaAccessSpare16 accessMethod;
typedef IndirectDataDico<accessMethod,analysisDataElement> MyAnalysisBaseDico;

class MyAnalysisDico : public MyAnalysisBaseDico {
public:
  MyAnalysisDico(const analysisDataElement& defaultValue)
    : MyAnalysisBaseDico(defaultValue) {}
  analysisDataElement getEntry( const Lima::LimaString& word) const override;
};

analysisDataElement MyAnalysisDico::getEntry(
const Lima::LimaString& word) const{
  int64_t index = -1;
  std::cerr <<  "MyAnalysisDico::getEntry().." << std::endl;
#ifdef DEBUG_CD
  STRINGMAPLOGINIT;
  LDEBUG <<  "MyAnalysisDico::getEntry(" << word << ")";
#endif

  // Look in FsaDictionary (or tree or..)
  index = m_accessMethod.getIndex(word);
#ifdef DEBUG_CD
  LDEBUG <<  "index = " << index;
#endif

  index = m_accessMethod.getIndex(word);
//  if( index > 0 ) ???
  if( index >= 0 )
  {
#ifdef DEBUG_CD
    LDEBUG <<  "index = " << index;
#endif
    analysisDataElement entry = m_data + m_index2Data[index];
    return entry;
//    return analysisDataElement(word, m_index2Data[index], m_dicoCode );
  }
  else
    return m_emptyElement;
}


void testAnalysisDico(const Param& param ) {
  string resourcesPath=qEnvironmentVariableIsEmpty("LIMA_RESOURCES")
      ?"/usr/share/apps/lima/resources"
      :string(qgetenv("LIMA_RESOURCES").constData());
  string commonConfigFile=string("lima-common.xml");
  string configDir=qEnvironmentVariableIsEmpty("LIMA_CONF")
      ?"/usr/share/config/lima"
      :string(qgetenv("LIMA_CONF").constData());

  MyAnalysisDico* dico = new MyAnalysisDico(analysisDataElement(0));
  dico->parseAccessMethod(param.keyFileName);
  dico->parseData(param.dataFileName);
  dico->getSize();
  Lima::LimaString limaKey(Misc::utf8stdstring2limastring(param.key));
  analysisDataElement entry = dico->getEntry(limaKey);

  int v1 = *entry;
  int v2 = *(entry+1);
  std::cout << "analysisDico->getElement(" << param.key << ") ="
            << std::setbase(16) << v1 << "," << v2 << std::setbase(10) << std::endl;
}
