// Copyright 2017 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/**
 * \file    FileTextExtractor.cpp
 * \author  Jocelyn Vernay
 * \date    Wed, Sep 06 2017
 *
 */

#include "FileTextExtractor.h"

#include <algorithm>
#include <iostream>
#include <cstring>

#include <QFile>
#include <QString>
#include <QTextStream>
#include <QMessageBox>

namespace Lima
{
namespace Gui
{
namespace Tools
{

template<class T> // T can be std::string or Qstring
inline T concatenate(const std::vector<T> &v, const T &separator, const T &quotation_left, const T &quotation_right)
{
  T s;
  for (auto i = v.begin(); i != v.end(); i++)
  {
    if (s.size() > 0)
      s += quotation_right + separator + quotation_left;
    s += *i;
  }
  return s;
}

bool extractTextFromFile(const std::string &path, QString &fileContent, const std::string &extension)
{
  std::vector<std::string> textHandledTypes = {"txt", "md"};
  QString errorMessage;

  bool textHandled = (extension.size()==0 || (std::find(textHandledTypes.begin(), textHandledTypes.end(), extension) != textHandledTypes.end()));

  if (textHandled)
  {
    QFile file(QString(path.c_str()));
    if (file.open(QFile::ReadOnly))
    {
      QTextStream qts(&file);
      fileContent = qts.readAll();
      file.close();
      return true;
    }
    else
      errorMessage = std::strerror(errno);
  }
  else
    errorMessage = QString::asprintf("Can't handle \".%s\" files. Only text files (\".%s\") are supported",
                                     extension.c_str(), concatenate<std::string>(textHandledTypes, ", ", "\".", "\"").c_str());

  std::cout << "Error opening file \"" << path << "\": " << errorMessage.toStdString() << std::endl;

  QMessageBox msgBox;
  msgBox.setWindowTitle("Error");
  msgBox.setInformativeText(QString::asprintf("Error opening file \"%s\".", path.c_str()));
  msgBox.setText(errorMessage);
  msgBox.exec();

  return false;
}

} // Tools
} // Gui
} // Lima
