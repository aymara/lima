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
 * \file    FileTextExtractor.h
 * \author  Jocelyn Vernay
 * \date    Wed, Sep 06 2017
 * 
 */

#ifndef FILETEXTEXTRACTOR_H
#define FILETEXTEXTRACTOR_H

#include <string>
#include <vector>

namespace Lima 
{
namespace Gui 
{
namespace Tools 
{

/// \brief To handle different types of files, this class sets the base for
/// file text extraction.
/// You can inherit from this class for each extension (pdf, docx) to create
/// a new handler for a type of file
/// You'll always need to modify the code of LimaGuiApplication::openFile, though ...
/// So it's not ideal
  
// update : a class or just a function ?
  
  
//class FileTextExtractor {
//  virtual void load(const std::string& path) = 0;
//  virtual std::string text() = 0;
//}

std::string extractTextFromFile(const std::string& path, const std::string& extension = "");

} // Tools
} // Gui
} // Lima

#endif // FILETEXTEXTRACTOR_H
