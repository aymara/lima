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
