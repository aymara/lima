// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/******************************************************************************
*
* File        : tokenFilter.cpp
* Author      : Gael de Chalendar (Gael.de-Chalendar@cea.fr)
* Created on  : Mon Aug 04 2003
* Copyright   : (c) 2003 by CEA
* Version     : $Id$
*
******************************************************************************/

#include "TokenFilter.h"
#include "common/MediaticData/mediaticData.h"

using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

namespace Lima {
namespace LinguisticProcessing {
namespace SyntacticAnalysis {

TokenFilter::TokenFilter(const std::string& id, const std::string& name,
        const std::set< StringsPoolIndex >& attributes,
        const std::set< LinguisticCode >& categories) :
    m_id(id), m_name(name), m_attributes(attributes), m_categories(categories)
{
}

TokenFilter::TokenFilter(const TokenFilter& filter) :
    m_id(filter.m_id), m_name(filter.m_name),
    m_attributes(filter.m_attributes), m_categories(filter.m_categories)
{
}

TokenFilter::TokenFilter(const LinguisticAnalysisStructure::MorphoSyntacticData* data,
            const Common::PropertyCode::PropertyAccessor* microAccessor) :
    m_id(), 
    m_name(),
    m_attributes(data->allNormalizedForms()), 
    m_categories(data->allValues(*microAccessor))
{}

TokenFilter::~TokenFilter()
{
    m_attributes.clear();
    m_categories.clear();
}

void TokenFilter::operator=(const TokenFilter& filter)
{
    m_id = filter.m_id;
    m_name = filter.m_name;
    m_attributes = filter.m_attributes;
    m_categories = filter.m_categories;
}

bool TokenFilter::operator==(const TokenFilter& filter) const
{
    return ( (m_id == filter.m_id) && (m_name == filter.m_name) &&
            (m_attributes == filter.m_attributes) && (m_categories == filter.m_categories) );
}

/*
bool TokenFilter::operator==(const DicoWord& word) const
{
    bool checkAttributes;
    bool checkCategories;

    if (m_attributes.size() == 0) checkAttributes = true;
    else checkAttributes = (m_attributes.find(word.lemmaIndex()) != m_attributes.end());
    checkCategories = (m_categories.find(word.microCategory()) != m_categories.end());
    return ( checkAttributes && checkCategories );
}
*/

/** Used to search a real TokenFilter inside syntagmatic structure */
bool tfless::operator()(const TokenFilter& x, const TokenFilter& y) const
{
//    std::cerr << "In tfless(" << x << ", " << y << ")..." << std::endl;
    if ( (x.id() != "") && (y.id() != "") ) {
      return (x.id() < y.id());
    } else  if (x.attributes() != y.attributes()) {
      return (x.attributes() < y.attributes());
    } else {
      return (x.categories() < y.categories());
    }
}

std::ostream& operator<<(std::ostream &os, const TokenFilter& d)
{
    os << d.id() << " ";
    os << d.name() << " " ;
    std::set< StringsPoolIndex >::const_iterator it_attr, it_attr_end;
    boost::tie(it_attr, it_attr_end) = d.attributesIterators();
    for (; it_attr != it_attr_end; it_attr++)
    {

    // language not available, write stringPoolIndexes instead of string, to be fixed later     
       os << *it_attr << " ";
    }
    std::set< LinguisticCode >::const_iterator it_cat, it_cat_end;
    boost::tie(it_cat, it_cat_end) = d.categoriesIterators();
    for (; it_cat != it_cat_end; it_cat++)
        os << (*it_cat) << " ";
    return os;
}

} // namespace SyntacticAnalysis
} // namespace LinguisticProcessing
} // namespace Lima
