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

#ifndef LIMA_LINGUISTICPROCESSING_FLATTOKENIZER_ReturnStatus_H
#define LIMA_LINGUISTICPROCESSING_FLATTOKENIZER_ReturnStatus_H

#include "FlatTokenizerExport.h"
#include "common/Data/LimaString.h"

#include <vector>

namespace Lima
{
namespace LinguisticProcessing
{
namespace FlatTokenizer
{

class LIMA_FLATTOKENIZER_EXPORT ReturnStatus {

public:
    ReturnStatus();
    virtual ~ReturnStatus();

    // inserts a new return status in the list. If specified
    // status already exists, 0 is returned, its code otherwise.
    Lima::LimaChar addStatus(const Lima::LimaString& name);

protected:

private:
    class StatusEntry : public Lima::LimaString {
    public :
        StatusEntry(const Lima::LimaString& name, const Lima::LimaChar code) :
           Lima::LimaString(name),
           _code(code) {
        }
        virtual ~StatusEntry() {}
        Lima::LimaChar _code;
    };
    std::vector<StatusEntry> _statusList;
    Lima::LimaChar _currentCode;
///////////////////////////////////////////////////////////
};

} //namespace FlatTokenizer
} // namespace LinguisticProcessing
} // namespace Lima


#endif
