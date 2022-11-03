// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
