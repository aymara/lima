// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
  Q_OBJECT
public:
  SelectionalPreferences();

  virtual ~SelectionalPreferences();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;

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
