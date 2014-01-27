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
/**
  *
  * @file        SelectionalPreferences.h
  * @author      Gael de Chalendar (Gael.de-Chalendar@cea.fr)
  * @date        Created on  : Sat May 22 2010
  *              Copyright   : (c) 2010 by CEA
  * @version     $Id: $
  *
  */

#ifndef SELECTIONALPREFERENCES_H
#define SELECTIONALPREFERENCES_H

#include "SyntacticAnalysisExport.h"
#include "linguisticProcessing/core/LinguisticResources/AbstractResource.h"
#include "common/Data/LimaString.h"

#include <boost/tuple/tuple.hpp>

#include <set>
#include <map>

namespace Lima
{
namespace LinguisticProcessing
{
namespace SyntacticAnalysis
{

#define SELECTIONALPREFERENCES_CLASSID "SelectionalPreferences"

class LIMA_SYNTACTICANALYSIS_EXPORT SelectionalPreferences : public AbstractResource
{
public:
  SelectionalPreferences();

  virtual ~SelectionalPreferences();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager)
  ;

  void loadFromFile(const std::string& fileName);

  inline MediaId language() const {return m_language;};

  double dependencyProbability(std::string target, LinguisticCode targetCode, std::string dependency, std::string source, LinguisticCode sourceCode) const;

private:
  class compareTuple : public std::less< boost::tuple< std::string, LinguisticCode, std::string, std::string, LinguisticCode > >
  {
    public:
    bool operator()(const boost::tuple< std::string, LinguisticCode, std::string, std::string, LinguisticCode >& t1,
                    const boost::tuple< std::string, LinguisticCode, std::string, std::string, LinguisticCode >& t2) const;
  };
                    
  MediaId m_language;
  std::map< boost::tuple< std::string, LinguisticCode, std::string, std::string, LinguisticCode >, double, compareTuple > m_preferences;

};

} // namespace SyntacticAnalysis
} // namespace LinguisticProcessing
} // namespace Lima

#endif
