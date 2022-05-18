// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
  ltProperty(const Common::PropertyCode::PropertyAccessor* prop);
  bool operator()(const LinguisticElement& elem1,const LinguisticElement& elem2) const;
  private:
  const Common::PropertyCode::PropertyAccessor* m_prop;
};

class LIMA_LINGUISTICANALYSISSTRUCTURE_EXPORT ltNormProperty 
{
  public:
  ltNormProperty(const Common::PropertyCode::PropertyAccessor* prop);
  bool operator()(const LinguisticElement& elem1,const LinguisticElement& elem2) const;
  private:
  const Common::PropertyCode::PropertyAccessor* m_prop;
};

class LIMA_LINGUISTICANALYSISSTRUCTURE_EXPORT CheckEqualPropertyPredicate 
{
  public:
  CheckEqualPropertyPredicate(const Common::PropertyCode::PropertyAccessor* prop,LinguisticCode value);
  bool operator()(const LinguisticElement& elem) const;
  private:
  const Common::PropertyCode::PropertyAccessor* m_property;
  LinguisticCode m_value;
};

class LIMA_LINGUISTICANALYSISSTRUCTURE_EXPORT CheckDifferentPropertyPredicate 
{
  public:
  CheckDifferentPropertyPredicate(const Common::PropertyCode::PropertyAccessor* prop,LinguisticCode value);
  bool operator()(const LinguisticElement& elem) const;
  private:
  const Common::PropertyCode::PropertyAccessor* m_property;
  LinguisticCode m_value;
};

class LIMA_LINGUISTICANALYSISSTRUCTURE_EXPORT IncludePropertyPredicate 
{
  public:
  IncludePropertyPredicate(const Common::PropertyCode::PropertyAccessor* prop,const std::set<LinguisticCode>& value);
  bool operator()(const LinguisticElement& elem) const;
private:
  IncludePropertyPredicate& operator=(const IncludePropertyPredicate&) {return *this;}
  const Common::PropertyCode::PropertyAccessor* m_property;
  const std::set<LinguisticCode>& m_values;
};

class LIMA_LINGUISTICANALYSISSTRUCTURE_EXPORT ExcludePropertyPredicate 
{
  public:
  ExcludePropertyPredicate(const Common::PropertyCode::PropertyAccessor* prop,
                           const std::set<LinguisticCode>& value);
  ExcludePropertyPredicate(const ExcludePropertyPredicate& epp);
  
  bool operator()(const LinguisticElement& elem) const;
  
private:
  ExcludePropertyPredicate& operator=(const ExcludePropertyPredicate&) {return *this;}
  const Common::PropertyCode::PropertyAccessor* m_property;
  const std::set<LinguisticCode>& m_values;
};


} // LinguisticAnalysisStructure 

} // LinguisticProcessing 

} // Lima

#endif
