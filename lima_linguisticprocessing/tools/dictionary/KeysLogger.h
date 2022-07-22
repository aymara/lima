// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef KEYLOGGER_H
#define KEYLOGGER_H


#include "linguisticProcessing/core/FlatTokenizer/CharChart.h"

namespace Lima {

class KeysLoggerPrivate;
class KeysLogger
{
public:

  KeysLogger(const QString& file,
             std::ostream& out,
             LinguisticProcessing::FlatTokenizer::CharChart* charChart,
             bool reverseKeys);

  ~KeysLogger();

private:
  KeysLoggerPrivate* m_d;
};

}

#endif
