// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004 by Benoit Mathieu                                  *
 *   mathieub@zoe.cea.fr                                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_COMMON_TGV_TESTCASE_H
#define LIMA_COMMON_TGV_TESTCASE_H

#include "linguisticProcessing/LinguisticProcessingCommon.h"

#include <string>
#include <map>
#include <list>
#include <set>
#include <stdexcept>
#include "common/LimaCommon.h"

namespace Lima
{
namespace Common
{
namespace TGV
{
  typedef std::map<std::string,std::string> SimpleValCallParams;
  typedef std::map<std::string,std::list<std::string> > MultiValCallParams;
  typedef std::map<std::string,std::map<std::string,std::string> > MapValCallParams;

/**
@author Benoit Mathieu
*/
struct LIMA_TGV_EXPORT TestCase
{
  TestCase() : tests() {};
  friend LIMA_TGV_EXPORT std::ostream & operator << (std::ostream &os, const TestCase& tc);
  void print(std::ostream &os) const;
  // recuperation de la valeur du parametre key
  std::string getParam( const std::string& key ) const;
  // recuperation de la liste des valeurs pour le parametre multivalue key
  void getList( const std::string& key, std::list<std::string >& list ) const;
  void getMap( const std::string& key, std::map<std::string,std::string >& map ) const;

  struct TestUnit
  {
    TestUnit() : id(),trace(),comment(),left(),op(),right(),conditional(false) {};
    friend LIMA_TGV_EXPORT std::ostream & operator << (std::ostream &os, const TestUnit& tcu);
    void print(std::ostream &os) const;
    std::string id;
    std::string trace;
    std::string comment;
    std::string left;
    std::string op;
    std::string right;
    bool conditional;
  };

  SimpleValCallParams simpleValCallParams;
  MultiValCallParams multiValCallParams;
  MapValCallParams mapValCallParams;
  
  std::string id;
  std::string type;
  std::string explanation;
  std::list<TestUnit> tests;

};

LIMA_TGV_EXPORT std::ostream& operator<<(std::ostream& out,const std::set<std::string>& s);


} // TGV
} // Common
} // Lima

#endif
