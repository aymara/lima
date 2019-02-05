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
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/

#ifndef LIMA_LINGUISTICPROCESSING_LINGUISTICANALYSISSTRUCTUREMORPHOSYNTACTICDATAUTILS_H
#define LIMA_LINGUISTICPROCESSING_LINGUISTICANALYSISSTRUCTUREMORPHOSYNTACTICDATAUTILS_H

#include "LinguisticAnalysisStructureExport.h"
#include "common/LimaCommon.h"
#include "linguisticProcessing/common/PropertyCode/PropertyAccessor.h"
#include "MorphoSyntacticData.h"

namespace Lima {

namespace LinguisticProcessing {

namespace LinguisticAnalysisStructure {

class LIMA_LINGUISTICANALYSISSTRUCTURE_EXPORT ltString 
{
  public:
  bool operator()(const LinguisticElement& elem1,const LinguisticElement& elem2) const;
};

class LIMA_LINGUISTICANALYSISSTRUCTURE_EXPORT ltProperty 
{
  public:
  ltProperty(const Common::PropertyCode::PropertyAccessor& prop);
  bool operator()(const LinguisticElement& elem1,const LinguisticElement& elem2) const;
  private:
  Common::PropertyCode::PropertyAccessor m_prop;
};

class LIMA_LINGUISTICANALYSISSTRUCTURE_EXPORT ltNormProperty 
{
  public:
  ltNormProperty(const Common::PropertyCode::PropertyAccessor& prop);
  bool operator()(const LinguisticElement& elem1,const LinguisticElement& elem2) const;
  private:
  Common::PropertyCode::PropertyAccessor m_prop;
};

class LIMA_LINGUISTICANALYSISSTRUCTURE_EXPORT CheckEqualPropertyPredicate 
{
  public:
  CheckEqualPropertyPredicate(const Common::PropertyCode::PropertyAccessor& prop,LinguisticCode value);
  bool operator()(const LinguisticElement& elem) const;
  private:
  Common::PropertyCode::PropertyAccessor m_property;
  LinguisticCode m_value;
};

class LIMA_LINGUISTICANALYSISSTRUCTURE_EXPORT CheckDifferentPropertyPredicate 
{
  public:
  CheckDifferentPropertyPredicate(const Common::PropertyCode::PropertyAccessor& prop,
                                  LinguisticCode value);
  bool operator()(const LinguisticElement& elem) const;
  private:
  Common::PropertyCode::PropertyAccessor m_property;
  LinguisticCode m_value;
};

class LIMA_LINGUISTICANALYSISSTRUCTURE_EXPORT IncludePropertyPredicate 
{
  public:
  IncludePropertyPredicate(const Common::PropertyCode::PropertyAccessor& prop,
                           const std::set<LinguisticCode>& value);
  bool operator()(const LinguisticElement& elem) const;
private:
  IncludePropertyPredicate& operator=(const IncludePropertyPredicate&) {return *this;}
  Common::PropertyCode::PropertyAccessor m_property;
  const std::set<LinguisticCode>& m_values;
};

class LIMA_LINGUISTICANALYSISSTRUCTURE_EXPORT ExcludePropertyPredicate 
{
  public:
  ExcludePropertyPredicate(const Common::PropertyCode::PropertyAccessor& prop,
                           const std::set<LinguisticCode>& value);
  ExcludePropertyPredicate(const ExcludePropertyPredicate& epp);
  
  bool operator()(const LinguisticElement& elem) const;
  
private:
  ExcludePropertyPredicate& operator=(const ExcludePropertyPredicate&) {return *this;}
  Common::PropertyCode::PropertyAccessor m_property;
  const std::set<LinguisticCode>& m_values;
};


} // LinguisticAnalysisStructure 

} // LinguisticProcessing 

} // Lima

#endif
