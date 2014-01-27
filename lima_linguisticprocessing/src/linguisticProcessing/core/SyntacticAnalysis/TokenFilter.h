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
  * @file        TokenFilter.cpp
  * @author      Gael de Chalendar (Gael.de-Chalendar@cea.fr)
  * @date        Created on  : Mon Aug 04 2003 

  *              Copyright   : (c) 2003 by CEA
  * @version     $Id$
  *
  */

#ifndef TOKENFILTER_H
#define TOKENFILTER_H

#include "SyntacticAnalysisExport.h"
#include "common/Data/LimaString.h"
#include "common/misc/fsaStringsPool.h"
#include "common/Data/strwstrtools.h"
#include "linguisticProcessing/common/PropertyCode/PropertyAccessor.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticData.h"

#include <string>
#include <set>
#include <boost/tuple/tuple.hpp>

namespace Lima {
namespace LinguisticProcessing {
namespace SyntacticAnalysis {

// <!ELEMENT filtre ( nom, attribut*, categorie+)>
// <!ATTLIST filtre ID ID #REQUIRED>
//<!ELEMENT nom (#PCDATA ) >
//<!ELEMENT attribut (#PCDATA ) >
//<!ELEMENT categorie (#PCDATA ) >
//<filtre ID ="F16">
//      <nom>PREP:à,au,aux</nom>
//      <categorie>146</categorie>
//      <attribut>�/attribut>
//      <attribut>au</attribut>
//      <attribut>aux</attribut>
//</filtre>

class LIMA_SYNTACTICANALYSIS_EXPORT TokenFilter
{
public:
    TokenFilter() : m_id(""), m_name(""), m_attributes(), m_categories() {}
    TokenFilter(const std::string& id, const std::string& name,
            const std::set< StringsPoolIndex >& attributes,
            const std::set< LinguisticCode >& categories);
    TokenFilter(const TokenFilter& filter);
    TokenFilter(const LinguisticAnalysisStructure::MorphoSyntacticData* data,const Common::PropertyCode::PropertyAccessor* microAccessor);
    virtual ~TokenFilter();

    void operator=(const TokenFilter& filter);
    bool operator==(const TokenFilter& filter) const;
    bool operator!=(const TokenFilter& filter) const;
    bool operator<(const TokenFilter& filter) const;
//    bool operator==(const Data::DicoWord& word) const;

    std::string& id();
    const std::string& id() const;

    std::string& name(void);
    const std::string& name(void) const;

    std::set< StringsPoolIndex >& attributes();
    const std::set< StringsPoolIndex >& attributes() const;
    std::pair< std::set< StringsPoolIndex >::iterator, std::set< StringsPoolIndex >::iterator > attributesIterators();
    std::pair< std::set< StringsPoolIndex >::const_iterator, std::set< StringsPoolIndex >::const_iterator > attributesIterators() const;

    std::set< LinguisticCode >& categories();
    const std::set< LinguisticCode >& categories() const;
    std::pair< std::set< LinguisticCode >::iterator, std::set< LinguisticCode >::iterator > categoriesIterators();
    std::pair< std::set< LinguisticCode >::const_iterator, std::set< LinguisticCode >::const_iterator > categoriesIterators() const;

private:
    std::string m_id;
    std::string m_name;
    std::set< StringsPoolIndex > m_attributes;
    std::set< LinguisticCode > m_categories;
};

class tfless : public std::less< TokenFilter >
{
public:
    bool operator()(const TokenFilter& x, const TokenFilter& y) const;
};

LIMA_SYNTACTICANALYSIS_EXPORT std::ostream& operator<<(std::ostream &os, const TokenFilter& d);

inline bool TokenFilter::operator!=(const TokenFilter& word) const
{
    return !( operator==(word) );
}

inline bool TokenFilter::operator<(const TokenFilter& word) const
{
    return tfless()(*this, word);
}

inline std::string& TokenFilter::id()
{
    return m_id;
}

inline const std::string& TokenFilter::id() const
{
    return m_id;
}

inline std::string& TokenFilter::name(void)
{
    return m_name;
}

inline const std::string& TokenFilter::name(void) const
{
    return m_name;
}

inline std::set< StringsPoolIndex >& TokenFilter::attributes()
{
    return m_attributes;
}

inline const std::set< StringsPoolIndex >& TokenFilter::attributes() const
{
    return m_attributes;
}

inline std::pair< std::set< StringsPoolIndex >::iterator, std::set< StringsPoolIndex >::iterator > TokenFilter::attributesIterators()
{
    return std::make_pair(m_attributes.begin(), m_attributes.end());
}

inline std::pair< std::set< StringsPoolIndex >::const_iterator, std::set< StringsPoolIndex >::const_iterator > TokenFilter::attributesIterators() const
{
    return std::make_pair(m_attributes.begin(), m_attributes.end());
}

inline std::set< LinguisticCode >& TokenFilter::categories()
{
    return m_categories;
}

inline const std::set< LinguisticCode >& TokenFilter::categories() const
{
    return m_categories;
}

inline std::pair< std::set< LinguisticCode >::iterator, std::set< LinguisticCode >::iterator > TokenFilter::categoriesIterators()
{
    return std::make_pair(m_categories.begin(), m_categories.end());
}

inline std::pair< std::set< LinguisticCode >::const_iterator, std::set< LinguisticCode >::const_iterator > TokenFilter::categoriesIterators() const
{
    return std::make_pair(m_categories.begin(), m_categories.end());
}

} // namespace SyntacticAnalysis
} // namespace LinguisticProcessing
} // namespace Lima

#endif // TOKENFILTER_H
