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
 *   Copyright (C) 2004-2012 by CEA LIST                              *
 *                                                                         *
 ***************************************************************************/

#include "MorphoSyntacticDataUtils.h"

namespace Lima {

namespace LinguisticProcessing {

namespace LinguisticAnalysisStructure {

bool ltString::operator()(const LinguisticElement& elem1,const LinguisticElement& elem2) const
{
  if (elem1.type != elem2.type) return elem1.type < elem2.type;
  if (elem1.inflectedForm != elem2.inflectedForm) return elem1.inflectedForm < elem2.inflectedForm;
  if (elem1.lemma != elem2.lemma) return elem1.lemma < elem2.lemma;
  if (elem1.normalizedForm != elem2.normalizedForm) return elem1.normalizedForm < elem2.normalizedForm;
  return elem1.properties < elem2.properties;
}

ltProperty::ltProperty(const Common::PropertyCode::PropertyAccessor& prop) :
  m_prop(prop)
{}

bool ltProperty::operator()(const LinguisticElement& elem1,const LinguisticElement& elem2) const
{
  return m_prop.readValue(elem1.properties) < m_prop.readValue(elem2.properties);
}

ltNormProperty::ltNormProperty(const Common::PropertyCode::PropertyAccessor& prop) :
  m_prop(prop)
{}

bool ltNormProperty::operator()(const LinguisticElement& elem1,const LinguisticElement& elem2) const
{
  if (elem1.normalizedForm!=elem2.normalizedForm) return elem1.normalizedForm < elem2.normalizedForm;
  return m_prop.readValue(elem1.properties) < m_prop.readValue(elem2.properties);
}

CheckEqualPropertyPredicate::CheckEqualPropertyPredicate(const Common::PropertyCode::PropertyAccessor& prop,LinguisticCode value) :
  m_property(prop),
  m_value(value)
{}

bool CheckEqualPropertyPredicate::operator()(const LinguisticElement& elem) const
{
  return m_property->equal(elem.properties,m_value);
}

CheckDifferentPropertyPredicate::CheckDifferentPropertyPredicate(const Common::PropertyCode::PropertyAccessor& prop,LinguisticCode value) :
  m_property(prop),
  m_value(value)
{}

bool CheckDifferentPropertyPredicate::operator()(const LinguisticElement& elem) const
{
  return !(m_property->equal(elem.properties,m_value));
}

IncludePropertyPredicate::IncludePropertyPredicate(const Common::PropertyCode::PropertyAccessor& prop,const std::set<LinguisticCode>& values) :
  m_property(prop),
  m_values(values)
{}

bool IncludePropertyPredicate::operator()(const LinguisticElement& elem) const
{
  return (m_values.find(m_property->readValue(elem.properties)) != m_values.end());
}

ExcludePropertyPredicate::ExcludePropertyPredicate(const Common::PropertyCode::PropertyAccessor& prop,const std::set<LinguisticCode>& values) :
  m_property(prop),
  m_values(values)
{}

ExcludePropertyPredicate::ExcludePropertyPredicate(const ExcludePropertyPredicate& epp) :
  m_property(epp.m_property),
  m_values(epp.m_values)
{
}

bool ExcludePropertyPredicate::operator()(const LinguisticElement& elem) const
{
  return (m_values.find(m_property->readValue(elem.properties)) == m_values.end());
}

} // LinguisticAnalysisStructure 

} // LinguisticProcessing 

} // Lima
