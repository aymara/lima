// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "CharClass.h"
#include "common/Data/LimaString.h"

namespace Lima
{
namespace LinguisticProcessing
{
namespace FlatTokenizer
{

CharClass::CharClass() :
    m_id(),
    m_name(),
    m_super(0)
{
}

CharClass::~CharClass()
{
}


} // Tokenizer
} // LinguisticProcessing
} // Lima
