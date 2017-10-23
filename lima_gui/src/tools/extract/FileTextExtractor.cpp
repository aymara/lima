/*
    Copyright 2017 CEA LIST

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

namespace Lima 
{
namespace Gui 
{
namespace Tools 
{

std::string extractTextFromFile(const std::string &path, const std::string &extension) 
{
  std::vector<std::string> textHandledTypes = {"txt", "md"};
  std::vector<std::string> handledTypes = {"pdf", "docx"};

  bool textHandled = (extension.size()==0 || (std::find(textHandledTypes.begin(), textHandledTypes.end(), extension) != textHandledTypes.end()));
  bool handled = (std::find(handledTypes.begin(), handledTypes.end(), extension) != handledTypes.end());
  if (textHandled) 
  {
    QString tump;
    QFile file(QString(path.c_str()));
    if (file.open(QFile::ReadOnly)) 
    {
      QTextStream qts(&file);
      tump = qts.readAll();
      file.close();
    }
    else 
    {
      std::cout << "didn't open : " << path << std::endl;
      std::cout << "Error opening file: " << std::strerror(errno) << std::endl;
    }
    return tump.toStdString();
  }
  else if(handled) 
  {
    // put your formats here
    if (extension == "pdf") 
    {
      std::cout << extension << ": can't handle this file extension" 
                << std::endl;
    }
    else if (extension == "docx") 
    {
      std::cout << extension << ": can't handle this file extension" 
                << std::endl;
    }
  }
  else
  {
    std::cout << extension << ": can't handle this file extension" 
              << std::endl;
  }
  return "";
}

} // Tools
} // Gui
} // Lima
