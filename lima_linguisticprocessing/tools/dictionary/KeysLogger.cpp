// Copyright 2002-2019 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "KeysLogger.h"
#include "common/Data/LimaString.h"
#include "common/Data/strwstrtools.h"

#include <algorithm>

using namespace std;
using namespace Lima::Common::Misc;

namespace Lima
{

KeysLogger::KeysLogger(std::ostream& out,LinguisticProcessing::FlatTokenizer::CharChart* charChart,bool reverseKeys) :
    m_out(out),
    m_charChart(charChart),
    m_reverseKeys(reverseKeys),
    m_current(),
    m_count(0)
{
  S_ENTRY="entry";
  S_K="k";
  S_I="i";
  S_L="l";
  S_N="n";
  S_C="c";
  S_FORM="form";
  S_DESACC="desacc";
  S_OP="op";
  S_DELETE="delete";
  S_NO="no";
}

KeysLogger::~KeysLogger() {}

bool  KeysLogger::startElement ( const QString & , const QString & name, const QString & , const QXmlAttributes & attributes )
{
  if (name==S_ENTRY)
  {
    m_current = attributes.value(S_K);
    if (m_current == 0)
    {
      DICTIONARYLOGINIT;
      LERROR << "invalid entry !" << endl;
      return false;
    }
    LimaString currentStr=m_current;
    if (m_reverseKeys) {
      std::reverse(currentStr.begin(),currentStr.end());
    }
    m_out << limastring2utf8stdstring(currentStr) << "\n";
    QString op=attributes.value(S_OP);
    if (op==0 || op!= S_DELETE)
    {
      LimaString desaccstr=m_charChart->unmark(currentStr);
      if (desaccstr.size()>0 && desaccstr!=currentStr) {
        m_out << limastring2utf8stdstring(desaccstr) << "\n";
      }
    }
    m_count++;
    if ((m_count % 10000)==0)
    {
      //DICTIONARYLOGINIT;
      std::cout << "\rextract keys :  " << m_count << " entries ..."
                << std::flush;
    }
  }
  else if (name == S_I)
  {
    QString l=attributes.value(S_L);
    if ((l != 0) && (m_reverseKeys || m_current!=l))
    {
      m_out << Common::Misc::limastring2utf8stdstring(l) << "\n";
      m_current=l;
    }
    QString n=attributes.value(S_N);
    if ((n != 0) && (l!=n))
    {
      m_out << Common::Misc::limastring2utf8stdstring(n) << "\n";
      m_current=n;
    }
  }
  else if ((name==S_C))
  {
    m_current = attributes.value(S_FORM);
    if (m_current != 0)
    {
      m_out << Common::Misc::limastring2utf8stdstring(m_current) << "\n";
    } else {
      DICTIONARYLOGINIT;
      LERROR << "WARN : no attribute form in tag 'c' !" << endl;
      return true;
    }
  }
  return true;
}

}
