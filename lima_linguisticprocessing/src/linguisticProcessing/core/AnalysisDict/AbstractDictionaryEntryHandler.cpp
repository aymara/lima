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

#include "AbstractDictionaryEntryHandler.h"

namespace Lima {
namespace LinguisticProcessing {
namespace AnalysisDict {

// To avoid warning for unused parameters when specializing  
void AbstractDictionaryEntryHandler::startEntry(StringsPoolIndex /*form*/) {}
void AbstractDictionaryEntryHandler::endEntry() {}
void AbstractDictionaryEntryHandler::foundLingInfos(StringsPoolIndex,StringsPoolIndex) {}
void AbstractDictionaryEntryHandler::deleteLingInfos(StringsPoolIndex,StringsPoolIndex) {}
void AbstractDictionaryEntryHandler::endLingInfos() {}
void AbstractDictionaryEntryHandler::foundConcatenated() {}
void AbstractDictionaryEntryHandler::deleteConcatenated() {}
void AbstractDictionaryEntryHandler::foundComponent(uint64_t, uint64_t,StringsPoolIndex) {}
void AbstractDictionaryEntryHandler::endComponent() {}
void AbstractDictionaryEntryHandler::endConcatenated() {}
void AbstractDictionaryEntryHandler::foundAccentedForm(StringsPoolIndex) {}
void AbstractDictionaryEntryHandler::deleteAccentedForm(StringsPoolIndex) {}
void AbstractDictionaryEntryHandler::endAccentedForm() {}
void AbstractDictionaryEntryHandler::foundProperties(LinguisticCode) {}

} // AnalysisDict
} // LinguisticProcessing
} // Lima
