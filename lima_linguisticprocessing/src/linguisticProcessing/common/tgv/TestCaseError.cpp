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
                             ErrorTypes type,
                             const std::string& reason,
                             const std::string& pipeline,
                              const TestCase::TestUnit& tu):
  m_type(type), m_reason(reason), m_conditional(tu.conditional)
{
  std::ostringstream out;
  if (type == TestCaseFailed)
  {
    out << "  test unit : " << tu.id << std::endl;
    out << "  pipeline : " << pipeline << std::endl;
    out << "  reason : " << reason << std::endl;
    out << "  type : " << tc.type <<   ", id : " 
        << tc.id.toUtf8().constData() << std::endl;
    for(auto paramIt = tc.simpleValCallParams.cbegin() ;
        paramIt != tc.simpleValCallParams.cend() ; 
        paramIt++ ) 
    {
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
    out << "TestCase " << tc.id.toUtf8().constData() 
        << " is invalid ! " << std::endl;
    out << reason;
    out << "  explanation : " << tc.explanation << std::endl;
    out << "in TestUnit " << tu.id.toUtf8().constData() 
        << " on trace " << tu.trace << std::endl;
    out << "  comment : " << tu.comment << std::endl;
    out << "  left : " << tu.left << std::endl;
    out << "  operator : " << tu.op << std::endl;
    out << "  right : " << tu.right << std::endl;
    out << "  conditional : " << tu.conditional << std::endl;
    out << "operator '" << tu.op << "' is unknown !" << std::endl;
  }
  else if (type == InvalidKey)
  {
    out << "TestCase " << tc.id.toUtf8().constData()
        << " failed ! " << std::endl;
    out << reason;
    out << "  explanation : " << tc.explanation << std::endl;
    out << "in pipeline : " << pipeline << std::endl;
    out << "in TestUnit " << tu.id.toUtf8().constData()
        << " on trace " << tu.trace << std::endl;
    out << "  comment : " << tu.comment << std::endl;
    out << "  left : " << tu.left << std::endl;
    out << "  operator : " << tu.op << std::endl;
    out << "  right : " << tu.right << std::endl;
    out << "  conditional : " << tu.conditional << std::endl;
    out << m_reason << std::endl;
    m_reason += out.str();
  }
  m_reason = out.str();
}


} // TGV
} // Common
} // Lima
