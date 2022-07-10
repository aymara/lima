// Copyright 2002-2019 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *                         buildCompoundDict.cpp                           *
 *  begin                : mon nov 7 2005                                  *
 *  copyright            : (C) 2003-2005 by CEA                            *
 *  email                : olivier.mesnard@cea.fr                          *
 *  description                                                            *
 *  test of compoundStringAccess                                           *
 *  building  of dictionary                                                *
 *                                                                         *
 ***************************************************************************/
// for set locale
#include <locale.h>

#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <list>
#include <string>

#include "common/LimaCommon.h"
#include "common/time/traceUtils.h"

// string and file handling Utilities
#include "common/Data/strwstrtools.h"

// dictionaries
#include "common/FsaAccess/FsaAccessBuilder16.h"
#include "common/FsaAccess/FsaAccessSpare16.h"
#include "common/misc/AbstractAccessByString.h"
#include "common/FsaAccess/CompoundStringAccess.h"

#include <QtCore/QCoreApplication>

using namespace std;
using namespace Lima;
using namespace Lima::Common;

#define COMPOUND_SEPARATOR ';'

// options
typedef struct ParamStruct {
  std::string listOfWords;
  std::string outputDico;
  std::string inputDico;
  bool doPrint;
  bool test;
} Param;


// Utilis�pour la premi�e passe (mots simples)
void addListOfSimpleWords( const std::string& listOfWords,
   Lima::Common::FsaAccess::FsaAccessBuilder16& dico) {

  if( !listOfWords.size() )
    return;

  std::ifstream wList(listOfWords.c_str(), std::ios::in | std::ios::binary );
  if ( !wList.is_open() ) {
    std::cerr <<  "Cannot open list of words " << listOfWords << std::endl;
    return;
  }
  std::cerr <<  "Read list of simple words" << std::endl;
  char strbuff[400];

  for( int counter = 0 ; ; counter++ ) {
    if( (counter%10000) == 0 ) {
      std::cout << "\raddListOfWords counter = " << counter << std::flush;
    }
    // lecture d'une ligne du fichier
    wList.getline(strbuff, 400, '\n' );
    string line(strbuff);
    if( line.size() == 0 ) {
      std::cout <<  "end of list of words. counter=" << counter << std::endl;
      break;
    }
    else {
      // test si mot simple
      string::size_type separator_pos = line.find(COMPOUND_SEPARATOR);
//      std::cerr <<  "add " << line << "?";
      if( separator_pos == string::npos) {
//        std::cerr <<  "OK" << std::endl;
        Lima::LimaString word = Lima::Common::Misc::utf8stdstring2limastring(line);
//        std::cerr <<  "OK2 " << word << std::endl;
//        Lima::Common::Misc::icu_string2wstring(word,line);
        dico.addWord( word );
      }
//      std::cerr <<  "NO" << std::endl;
    }
  }
  std::cout << std::endl << std::flush;
  dico.pack();
}

void addListOfCompoundsWords( const std::string& listOfWords,
   Lima::Common::FsaAccess::CompoundStringAccess<Lima::Common::FsaAccess::FsaAccessSpare16>&dico ) {

  if( !listOfWords.size() )
    return;

  std::ifstream wList(listOfWords.c_str(), std::ios::in | std::ios::binary );
  if ( !wList.is_open() ) {
    std::cerr <<  "Cannot open list of words " << listOfWords << std::endl;
    return;
  }
  std::cerr <<  "Read list of compounds words" << std::endl;
  char strbuff[200];

  for( int counter = 0 ; ; counter++ ) {
    if( (counter%10000) == 0 ) {
      std::cout << "\raddListOfWords counter = " << counter << std::flush;
    }
    // lecture d'une ligne du fichier
    wList.getline(strbuff, 200, '\n' );
    string line(strbuff);
    if( line.size() == 0 ) {
      std::cout <<  "end of list of words. counter=" << counter << std::endl
                << std::flush;
      break;
    }
    else {
      // test si mot simple
      string::size_type component_pos = 0;
      string::size_type separator_pos = line.find(COMPOUND_SEPARATOR);
      if( separator_pos != string::npos) {
        vector<LimaString > compound;
        for( ; ; ) {
          string component( line, component_pos, separator_pos - component_pos );
          Lima::LimaString word = Lima::Common::Misc::utf8stdstring2limastring(component);
//    Lima::Common::Misc::icu_string2wstring(word,component);
          compound.push_back(word);
//          std::cerr << "compound.push_back(" << word << ")" << std::endl;
          component_pos = separator_pos+1;
          separator_pos = line.find(COMPOUND_SEPARATOR, component_pos);
          if( separator_pos == string::npos ) {
            string lastComponent( line, component_pos, separator_pos - component_pos );
            Lima::LimaString word = Lima::Common::Misc::utf8stdstring2limastring(lastComponent);
//      Lima::Common::Misc::icu_string2wstring(word,lastComponent);
//            std::cerr << "compound.push_back(" << word << ")" << std::endl;
            compound.push_back(word);
            break;
          }
        }
        dico.addWord(compound);
      }
    }
  }
  std::cout << std::endl << std::flush;
}

void testListOfWords( const std::string& listOfWords,
   Lima::Common::FsaAccess::CompoundStringAccess<Lima::Common::FsaAccess::FsaAccessSpare16>&dico,
   bool doPrint ) {

  if( !listOfWords.size() )
    return;

  std::ifstream wList(listOfWords.c_str(), std::ios::in | std::ios::binary );
  if ( !wList.is_open() ) {
    std::cerr <<  "Cannot open list of words " << listOfWords << std::endl;
    return;
  }
  std::cerr <<  "Read list of compounds words for test" << std::endl;
  char strbuff[200];

//  int64_t tId0 = -1;
  for( int counter = 0 ; ; counter++ ) {
    if( (counter%10000) == 0 ) {
      std::cout << "\raddListOfWords counter = " << counter << std::flush;
    }
    // lecture d'une ligne du fichier
    wList.getline(strbuff, 200, '\n' );
    string line(strbuff);
    if( line.size() == 0 ) {
      std::cout <<  "end of list of words. counter=" << counter << std::endl
                << std::flush;
      break;
    }
    else {
      string::size_type component_pos = 0;
      string::size_type separator_pos = line.find(COMPOUND_SEPARATOR);
      vector<LimaString > compound;
      for( ; separator_pos != string::npos ; ) {
        string component( line, component_pos, separator_pos - component_pos );
        Lima::LimaString word = Lima::Common::Misc::utf8stdstring2limastring(component);
//        Lima::Common::Misc::icu_string2wstring(word,component);
        compound.push_back(word);
        component_pos = separator_pos+1;
        separator_pos = line.find(COMPOUND_SEPARATOR, component_pos);
      }
      string lastComponent( line, component_pos, separator_pos - component_pos );
      Lima::LimaString word = Lima::Common::Misc::utf8stdstring2limastring(lastComponent);
//      Lima::Common::Misc::icu_string2wstring(word,lastComponent);
      compound.push_back(word);
      int64_t tId = dico.getIndex(compound);
      cerr << "dico.getIndex(" << line << ")=" << tId << endl;
      std::vector<LimaString > compoundSpelling;
      dico.getSpelling(tId, compoundSpelling);
      if( doPrint ) {
        std::vector<LimaString >::iterator compoundsIt =
           compoundSpelling.begin();
        cout << line << "|" << tId << "|";
        if( compoundsIt != compoundSpelling.end() ) {
          string spelling = Lima::Common::Misc::limastring2utf8stdstring(*compoundsIt);
          cout << spelling;
          compoundsIt++;
        }
        for( ; compoundsIt != compoundSpelling.end() ; compoundsIt++ ) {
          std::string spelling = Lima::Common::Misc::limastring2utf8stdstring(*compoundsIt);
          std::cout << ";" << spelling;
        }
        std::cout << std::endl << std::flush;
      }
//      assert(tId==tId0+1);
//      tId0 = tId;
    }
  }
}

void outputListOfWords0(
  Lima::Common::FsaAccess::CompoundStringAccess<Lima::Common::FsaAccess::FsaAccessSpare16>&dico,
  int startIndex,
  bool /*doPrint*/ ) {
    for( int index = startIndex ; ; index++ ) {
      std::vector<LimaString > compoundSpelling;
      dico.getSpelling(index, compoundSpelling);
        std::vector<LimaString >::iterator compoundsIt =
           compoundSpelling.begin();
        if( compoundsIt != compoundSpelling.end() ) {
          string spelling = Lima::Common::Misc::limastring2utf8stdstring(*compoundsIt);
          cout << spelling;
          compoundsIt++;
        }
        for( ; compoundsIt != compoundSpelling.end() ; compoundsIt++ ) {
          string spelling = Lima::Common::Misc::limastring2utf8stdstring(*compoundsIt);
          cout << ";" << spelling;
        }
        cout << "|" << index << endl;
     }
}

void outputListOfWords(
  Lima::Common::FsaAccess::CompoundStringAccess<Lima::Common::FsaAccess::FsaAccessSpare16>&dico,
  bool doPrint ) {
  try {
    outputListOfWords0( dico, 0, doPrint );
   }
   catch( AccessByStringOutOfRange &e ) {
      cerr << "outputListOfWords catch AccessByStringOutOfRange" << endl;
      cerr << e.what() << endl;
      cerr << "<<<<<<<<<<<<<<< end of simple words >>>>>>>>>>>>>" << endl;
   }
  try {
    outputListOfWords0( dico, MAX_SIMPLE_TID, doPrint );
   }
   catch( AccessByStringOutOfRange &e ) {
      cerr << "outputListOfWords catch AccessByStringOutOfRange" << endl;
      cerr << e.what() << endl;
   }
}

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

  cerr << argv[0] << " begin..." << endl;

  setlocale(LC_ALL, "");

#ifdef DEBUG_CD
  FSAALOGINIT;
  LDEBUG << argv[0] <<  " begin...";
#endif

  // options reading
  Param param = {
    std::string(),  // listOfWords
    std::string(),  // outputDico
    std::string(),  // inputDico
    false,           // doPrint
    false           // test
  };

  for (int i = 1 ; i < argc; i++) {
    std::string arg(argv[i]);
    int pos = -1;
    if (arg == "--help")
    {
      std::cerr << "usage: " << argv[0]
                << " --help" << std::endl;
      std::cerr << "       " << argv[0]
                << " [--output=<filename>]"
                << " [--input=<filename>]"
                << " [--listOfWords=<filename>]"
                << " [--test]"
                << std::endl;
      return 0;
    }
    else if ( (pos = arg.find("--input=")) != -1 ){
      param.inputDico = arg.substr(pos+8);
    }
    else if ( (pos = arg.find("--output=")) != -1 ){
      param.outputDico = arg.substr(pos+9);
    }
    else if ( (pos = arg.find("--listOfWords=")) != -1 ){
      param.listOfWords = arg.substr(pos+14);
    }
    else if ( (pos = arg.find("--test")) != -1 ){
      param.test = true;
    }
    else if ( (pos = arg.find("--doPrint")) != -1 ){
      param.doPrint = true;
    }
  }

  cerr << argv[0] << ": ";
  if(param.inputDico.size()) {
    cerr << "--input='" << param.inputDico << "' ";
  }
  if(param.outputDico.size()) {
    cerr << "--output='" << param.outputDico << "' ";
  }
  if(param.listOfWords.size()) {
    cerr << "--listOfWords='" << param.listOfWords << "'";
  }
  if(param.doPrint) {
    cerr << "--doPrint";
  }
  cerr << endl;

  if ( param.inputDico.size() == 0 ) {
    // Si pas de dictionnaire en entree, on le construit
    std::cerr <<  "Create FsaAccessBuilder16 dictionary for first pass...." << std::endl;
    Lima::Common::FsaAccess::FsaAccessBuilder16 dico;
    addListOfSimpleWords( param.listOfWords , dico );
    // sauvegarde des mots simples pour les recharger en mode spare
    dico.write(param.outputDico);
  }
  else if( (param.listOfWords.size() != 0 ) && !param.test ) {
    // Puis construction du dictionnaire de mots compos�
    std::cerr <<  "Create CompoundStringAccess<FsaAccessSpare6> dictionary for second pass...." << std::endl;
    Lima::Common::FsaAccess::CompoundStringAccess<Lima::Common::FsaAccess::FsaAccessSpare16> compoundsDico;
    // Lecture des mots simples en mode spare
    compoundsDico.readSimple( param.inputDico );
    std::cerr <<  "add compounds...." << std::endl;
    addListOfCompoundsWords( param.listOfWords, compoundsDico );
    std::cerr <<  "write " << param.inputDico << "..." << std::endl;
//    param.outputDico.append(".cmp");
    compoundsDico.write(param.inputDico);
  }
  else {
    // Creation et chargement du dictionnaire des mots compos�s
    std::cerr <<  "Create CompoundStringAccess<FsaAccessSpare6> dictionary for test...." << std::endl;
    Lima::Common::FsaAccess::CompoundStringAccess<Lima::Common::FsaAccess::FsaAccessSpare16> compoundsDico;
    // Lecture des mots simples  et des mots compos�
    compoundsDico.read ( param.inputDico );
    if( param.listOfWords.length() > 0 ) {
      std::cerr <<  "test compounds...." << std::endl;
      testListOfWords( param.listOfWords, compoundsDico, param.doPrint );
    }
    else {
      std::cerr <<  "output listOfWords...." << std::endl;
      outputListOfWords( compoundsDico, param.doPrint );
    }
  }

  return EXIT_SUCCESS;
}
