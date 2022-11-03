// Copyright 2002-2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_TENSORFLOWUNITS_LANGCODEHELPERS_H
#define LIMA_TENSORFLOWUNITS_LANGCODEHELPERS_H

namespace Lima
{
namespace LinguisticProcessing
{

// lang_str must indicate annotation model (only ud is supported now)
// udlang must indicate the particular language code (afr, fra, eng, ...)
// This function parses several different representations of the data mentioned above:
// ud-fra, fra

bool fix_lang_codes(QString &lang_str, std::string &udlang)
{
  if (lang_str != QString("ud") || udlang.find("ud-") == 0)
  {
    if (udlang.size() == 0 && lang_str.size() > 0 && lang_str != QString("ud"))
    {
      // This block helps to use tokenizer in non-UD pipelines.
      if (lang_str.startsWith("ud-"))
      {
        udlang = lang_str.right(lang_str.size() - 3).toStdString();
      }
      else
      {
        udlang = lang_str.toStdString();
      }
      lang_str = "ud";
    }
    else if (udlang.size() >= 4 && udlang.find(lang_str.toStdString()) == 0 && udlang[lang_str.size()] == '-')
    {
      udlang = udlang.substr(3);
    }
    else
    {
      // parse lang codes like 'eng.ud'
      if (udlang.size() == 0 && lang_str.size() >= 4 && lang_str.indexOf(".ud") == lang_str.size() - 3)
      {
        udlang = lang_str.left(3).toStdString();
        lang_str = "ud";
      }
      else
      {
        return false;
      }
    }
  }
  return true;
}

} // namespace LinguisticProcessing
} // namespace Lima

#endif
