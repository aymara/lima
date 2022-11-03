// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/

#ifndef LIMA_LINGUISTICPROCESSING_LINGUISTICPROCESSINGEXCEPTION_H
#define LIMA_LINGUISTICPROCESSING_LINGUISTICPROCESSINGEXCEPTION_H

#include "common/LimaCommon.h"
#include <string>

namespace Lima {
namespace LinguisticProcessing {

class LinguisticProcessingException : public LimaException  {
  public:
    explicit LinguisticProcessingException(const char* mess):
      LimaException(mess) {};
    explicit LinguisticProcessingException(const std::string& mess):
      LimaException(mess) {};
    explicit LinguisticProcessingException(const QString& mess):
      LimaException(mess) {};
};

/**
 * This macro writes the message @ref X to a previously configured error stream
 * before throwing a LinguisticProcessingException with the same message
 */
#define LIMA_LP_EXCEPTION(X) { \
    QString errorString; \
    QTextStream qts(&errorString); \
    qts << __FILE__ << ":" << __LINE__ << ": " << X ; \
    LERROR << errorString; \
    throw LinguisticProcessingException(errorString); \
}

} // LinguisticProcessing
} // Lima



#endif
