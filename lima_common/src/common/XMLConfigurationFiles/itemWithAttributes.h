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
/************************************************************************
 *
 * @file       itemWithAttributes.h
 * @author     besancon (romaric.besancon@cea.fr)
 * @date       Thu Dec  8 2005
 * @version    $Id$
 * copyright   Copyright (C) 2005-2012 by CEA LIST
 * Project     XMLConfigurationFile
 *
 * @brief      generic named item with attributes
 *
 *
 ***********************************************************************/

#ifndef ITEMWITHATTRIBUTES_H
#define ITEMWITHATTRIBUTES_H

#include "common/LimaCommon.h"
#include <map>
#include <string>

namespace Lima {
namespace Common {
namespace XMLConfigurationFiles {

class ItemWithAttributesPrivate;
class LIMA_XMLCONFIGURATIONFILES_EXPORT ItemWithAttributes
{
 public:
  ItemWithAttributes();
  ItemWithAttributes(const std::string& name);
  ItemWithAttributes(const QString& name);
  ItemWithAttributes(const ItemWithAttributes& item);
  ItemWithAttributes& operator=(const ItemWithAttributes& item);
  ~ItemWithAttributes();

  const std::string& getName() const;
  const std::string& getAttribute(const std::string& attributeName) const;
  bool hasAttribute(const std::string& attributeName) const;
    uint64_t nbAttributes() const;
  void addAttribute(const std::string& attributeName,
                    const std::string& value);
  const std::map<std::string,std::string>& getAttributes() const;

  bool hasAttribute(const QString& attributeName) const;
  void addAttribute(const QString& attributeName,
                    const QString& value);

private:
  ItemWithAttributesPrivate* m_d;
};

} // end namespace
} // end namespace
} // end namespace

#endif
