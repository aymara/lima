// Copyright 2002-2019 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "WordUnit.h"
#include "KnnSearcher.h"

#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <boost/algorithm/string.hpp>

#include <errno.h>
#include "common/time/traceUtils.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"




using namespace std;

namespace Lima
{
namespace LinguisticProcessing
{
namespace WordSenseDisambiguation
{

  WordUnit::WordUnit()
  {
  }

  WordUnit::~WordUnit()
  {
  }

  WordUnit::WordUnit(KnnSearcher* searcher,
         const Lemma2Index& lemma2Index,
         const Index2Lemma& index2Lemma,
         string lemma,
         Mode mode,
         string sensesPath)
  : m_lemma(lemma), m_mode(mode)
  {
    LOGINIT("WordSenseDisambiguator");
     if (lemma2Index.find(lemma) != lemma2Index.end())
     {
      m_lemmaId = lemma2Index.find(lemma)->second;
     } else {
  LWARN << "no lemmaId for "<< lemma << " in lemma2Index ";
     }
     m_nbSenses = loadSenses(searcher, lemma2Index, index2Lemma, sensesPath);
  }


  int WordUnit::loadSenses(KnnSearcher* searcher,
         const Lemma2Index& lemma2Index,
         const Index2Lemma& index2Lemma,
         string sensesPath)
  {
    LOGINIT("WordSenseDisambiguator");
    int nbSenses = 0;
#ifndef WIN32
    set<uint64_t> senseMembersIds;
    int formerNbSenses = -1;
    int nbTotalElems = 0;
    string sensesFile;
    ifstream ifs;
    int ifd;
    string s = "";
    switch (m_mode) {
      case B_MOST_FREQUENT:
      case B_JAWS_MOST_FREQUENT:
      case B_ROMANSEVAL_MOST_FREQUENT:
    sensesFile = sensesPath+"/"+m_lemma[0]+"/"+m_lemma+".senses";
    cerr << "Loading senses from " << sensesFile << endl;
    ifs.open(sensesFile.c_str(), std::ifstream::binary);
    if ( !ifs.good() ) {
      LERROR << "File " << sensesFile << " not read" ;
      if ( ifs.eof() ) {
        LERROR << "(eof)" ;
      } else if ( ifs.fail() ) {
        LWARN << "(fail)" ;
        LWARN << " Probably file "<< sensesFile <<" does not exist" ;
        return 0;
      } else if ( ifs.bad() ) {
        LERROR << "(bad)" ;
      } else {
        LERROR << "(reason unknown)" ;
      }
      LERROR << ". ";
      return nbSenses;
    }
    while (getline(ifs, s)) {
      vector<string> strs;
      boost::split(strs, s, boost::is_any_of(";"));
      vector<string> members;
      boost::split(members, strs.at(1), boost::is_any_of("*"));
      for (vector<string>::iterator it = members.begin(); it != members.end(); it++)
      {
        if (lemma2Index.find(*it)!=lemma2Index.end())
        {
    senseMembersIds.insert(lemma2Index.find(*it)->second);
        }
      }
      m_senses.insert(WordSenseUnit(nbSenses, m_lemmaId, m_mode, s, senseMembersIds, 20-nbSenses));
      nbSenses++;
    }
    for (set<WordSenseUnit>::iterator itDebug = m_senses.begin();
             itDebug != m_senses.end();
              itDebug++)
              {
                LDEBUG << "inserted : " << itDebug->senseId();
              }
    ifs.close();
    break;
      case S_WSI_MRD:
  //sensesFile = sensesPath+"/"+lemma[0]+"/"+lemma+".bin";
  sensesFile = sensesPath+"/"+m_lemma+".bin";
        LINFO << "Opening file : " << sensesFile;
  ifd = open(sensesFile.c_str(), O_RDONLY);
  if (ifd ==-1) {
    LERROR << "an error occured while opening file " << sensesFile;
    switch (errno) {
      case      ENOENT :
        LERROR << "noentry ";
        break;
      case     EMFILE  :
        LERROR << "too many files open ";
        break;
      case      EACCES :
        LERROR << "eaccess ";
        break;
      default :
        LERROR << "unrecognized error : impossible to open : "<< sensesFile;
        exit(EXIT_FAILURE);
        break;
    }
    return nbSenses;
  }
  if (read(ifd, &nbTotalElems, sizeof(nbTotalElems)) == -1) {
    LERROR << "Call to function read(...) failed";
    exit(EXIT_FAILURE);
  }
  LDEBUG << "NbTotalElements : "<< nbTotalElems ;
  for (int i = 0; i < nbTotalElems; i++) {
    if (formerNbSenses!=nbSenses && i !=0) {
      LDEBUG << "one more sense for " << m_lemma << " : " << s ;
      m_senses.insert(WordSenseUnit(formerNbSenses, m_lemmaId, m_mode, s, senseMembersIds));
      s="";
      senseMembersIds.clear();
    }
    formerNbSenses=nbSenses;
    uint id = 0;
    if (read(ifd, &id, sizeof(id)) == -1) {
      LERROR << "Call to function read(...) failed";
      exit(EXIT_FAILURE);
    }
    if (read(ifd, &nbSenses, sizeof(nbSenses)) == -1) {
      LERROR << "Call to function read(...) failed";
      exit(EXIT_FAILURE);
    }

/*    LDEBUG << "formerNbSense : " << formerNbSenses;
    LDEBUG << "NbSense : " << nbSenses; */
    if (searcher!=NULL) {
      senseMembersIds.insert(searcher->sys2Owner(id));
      //TOFIX : id are internal knn_searchids ?? seems ok
      //LDEBUG << "Ids : "<< id ;
      //LDEBUG << "lemmaId : " << searcher->sys2Owner(id);
      if (index2Lemma.find(searcher->sys2Owner(id)) != index2Lemma.end())
      {
        s+= index2Lemma.find(searcher->sys2Owner(id))->second + "|" ;
      }
    }
  }
  m_senses.insert(WordSenseUnit(formerNbSenses, m_lemmaId, m_mode, s, senseMembersIds));
  break;
      default :
  break;
    }
    LDEBUG << "NbSenses for " << m_lemma << " : "<< nbSenses ;


    /*
    LDEBUG << "test size : " << m_senses.size();
  for(set<WordSenseUnit>::iterator itSenses = m_senses.begin();
           itSenses!= m_senses.end();
           itSenses++ ) {

  LDEBUG << "Checking 3 Sense id "<< m_lemma << " : " << itSenses->senseId();
  }
    */




#endif


    return nbSenses;
  }

  std::ostream& operator << (std::ostream& os, const WordUnit& wu)
  {
    os << wu.lemmaId() <<":"<< wu.lemma() << "(" << wu.mode() << "):" << wu.nbSenses() ;
    return os;
  }

  QDebug& operator << (QDebug& os, const WordUnit& wu)
  {
    os << wu.lemmaId() <<":"<< wu.lemma() << "(" << wu.mode() << "):" << wu.nbSenses() ;
    return os;
  }



}  // namespace WordSenseDisambiguation
}  // namespace LinguisticProcessing
}  // namespace Lima
