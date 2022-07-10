// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

// NAUTITIA
//
// jys 23-JUL-2002
//
// ReturnStatus manages the list of status returned by an
// inner automaton to its master state.
// Return status are declared at the beginning of state
// definition, before inner automatons and transitions.

#include "ReturnStatus.h"

#include "common/misc/Exceptions.h"

using namespace Lima;

namespace Lima
{
namespace LinguisticProcessing
{
namespace FlatTokenizer
{


ReturnStatus::ReturnStatus() :
    _statusList(),
    _currentCode(0) {}

ReturnStatus::~ReturnStatus() 
{
    _statusList.clear();//AndDestroy();
}

// inserts a new return status in the list. If specified
// status already exists, 0 is returned, its code otherwise.
Lima::LimaChar ReturnStatus::addStatus(const LimaString& name) {
    _statusList.push_back(StatusEntry(name, _currentCode));
    return LimaChar(_currentCode.unicode()--);
}

} //namespace FlatTokenizer
} // namespace LinguisticProcessing
} // namespace Lima
