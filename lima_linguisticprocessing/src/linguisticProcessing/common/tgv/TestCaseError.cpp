// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004 by Benoit Mathieu                                  *
 *   mathieub@zoe.cea.fr                                                   *
 *                                                                         *
 ***************************************************************************/
#include "TestCaseError.hpp"

namespace Lima
{
namespace Common
{
namespace TGV
{


TestCaseError::TestCaseError(const TestCase& tc,
                             const std::string& reason,
                             const std::string& pipeline) :
    m_type(NoError), m_reason(""), m_conditional(false)
{
  std::ostringstream out;
  out << "TestCase " << tc.id << "  type : " << tc.type << " succeeded ! " << std::endl;
  out << "  explanation : " << tc.explanation << std::endl;
  out << "in pipeline : " << pipeline << std::endl;
  out << reason << std::endl;
  m_reason = out.str();
}

TestCaseError::TestCaseError(const TestCase& tc,
                             ErrorTypes type,
                             const std::string& reason,
                             const std::string& pipeline,
                              const TestCase::TestUnit& tu):
  m_type(type), m_reason(reason), m_conditional(tu.conditional)
{
  std::ostringstream out;
  if (type == NoError)
  {
    out << "TestCase " << tc.id << " succeeded ! " << std::endl;
    out << reason;
    out << "  explanation : " << tc.explanation << std::endl;
    out << "in TestUnit " << tu.id << " on trace " << tu.trace << std::endl;
    out << "  comment : " << tu.comment << std::endl;
    out << "  left : " << tu.left << std::endl;
    out << "  operator : " << tu.op << std::endl;
    out << "  right : " << tu.right << std::endl;
    out << "  conditional : " << tu.conditional << std::endl;
    out << "operator '" << tu.op << "' is unknown !" << std::endl;
  }
  else if (type == TestCaseFailed)
  {
    out << "  test unit : " << tu.id << std::endl;
    out << "  pipeline : " << pipeline << std::endl;
    out << "  reason : " << reason << std::endl;
    out << "  type : " << tc.type <<   ", id : " << tc.id << std::endl;
    for( SimpleValCallParams::const_iterator paramIt = tc.simpleValCallParams.begin() ;
    paramIt != tc.simpleValCallParams.end() ; paramIt++ ) {
      out << " " << (*paramIt).first << " = " << (*paramIt).second<< std::endl;
    }
    for( MultiValCallParams::const_iterator paramIt = tc.multiValCallParams.begin() ;
    paramIt != tc.multiValCallParams.end() ; paramIt++ ) {
      out << " " << (*paramIt).first << " = ";
      const std::list<std::string>& list = (*paramIt).second;
      for( std::list<std::string>::const_iterator valIt = list.begin() ;
        valIt != list.end() ; valIt++ ) {
        out << *valIt << ",";
      }
      out << std::endl;
    }
    for( MapValCallParams::const_iterator paramIt = tc.mapValCallParams.begin() ;
      paramIt != tc.mapValCallParams.end() ; paramIt++ ) {
      const std::map<std::string,std::string>& map = (*paramIt).second;
      out << " " << (*paramIt).first  << " = ";
      for( std::map<std::string,std::string>::const_iterator valueIt = map.begin() ;
        valueIt != map.end() ; valueIt++ ) {
        out << " attr='" << (*valueIt).first << "' val='" << (*valueIt).second << "'";
      }
      out << std::endl;
    }
  }
  else if (type == InvalidOperator)
  {
    out << "TestCase " << tc.id << " is invalid ! " << std::endl;
    out << reason;
    out << "  explanation : " << tc.explanation << std::endl;
    out << "in TestUnit " << tu.id << " on trace " << tu.trace << std::endl;
    out << "  comment : " << tu.comment << std::endl;
    out << "  left : " << tu.left << std::endl;
    out << "  operator : " << tu.op << std::endl;
    out << "  right : " << tu.right << std::endl;
    out << "  conditional : " << tu.conditional << std::endl;
    out << "operator '" << tu.op << "' is unknown !" << std::endl;
  }
  else if (type == InvalidKey)
  {
    out << "TestCase " << tc.id << " failed ! " << std::endl;
    out << reason;
    out << "  explanation : " << tc.explanation << std::endl;
    out << "in pipeline : " << pipeline << std::endl;
    out << "in TestUnit " << tu.id << " on trace " << tu.trace << std::endl;
    out << "  comment : " << tu.comment << std::endl;
    out << "  left : " << tu.left << std::endl;
    out << "  operator : " << tu.op << std::endl;
    out << "  right : " << tu.right << std::endl;
    out << "  conditional : " << tu.conditional << std::endl;
  }
  m_reason = out.str();
}


} // TGV
} // Common
} // Lima
