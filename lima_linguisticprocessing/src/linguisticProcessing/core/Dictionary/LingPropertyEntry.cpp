// Copyright 2002-2020 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

// NAUTITIA
//
// jys 1-OCT-2002
//
// LingPropertyEntry is the way to access elementary linguistic properties
// Dictionaries in memory are specified in NA2002.r2128 JYS
// Dictionaries iterators are specified in NA2002.r2129 JYS

#include "linguisticProcessing/core/Dictionary/LingPropertyEntry.h"

#include "common/Data/strwstrtools.h"
#include <iostream>
using namespace std;
#include <wchar.h>


namespace Lima
{
namespace LinguisticProcessing
{
namespace Dictionary
{

// Creates LingPropertyEntry as an empty structure.
LingPropertyEntry::LingPropertyEntry() :
m_lingProperty()
{
}

LingPropertyEntry::LingPropertyEntry(const LingPropertyEntry& entry) :
m_lingProperty(entry.m_lingProperty)
{
}

LingPropertyEntry::LingPropertyEntry(const LinguisticCode& property) :
m_lingProperty(property)
{
}

// Destroys LingPropertyEntry
LingPropertyEntry::~LingPropertyEntry() {}

// Assignment operator. Copies the specified LingPropertyEntry
// into self. Returns a reference to self.
LingPropertyEntry& LingPropertyEntry::operator=(const LingPropertyEntry& entry)
{
  m_lingProperty = entry.m_lingProperty;
  return *this;
}

// Returns true if self is empty
bool LingPropertyEntry::isEmpty() const
{
  return (m_lingProperty == L_NONE);
}

// Returns LingPropery code
LinguisticCode LingPropertyEntry::data() const
{
  return m_lingProperty;
}

} // closing namespace Dictionary
} // closing namespace LinguisticProcessing
} // closing namespace Lima
