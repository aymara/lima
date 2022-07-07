// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
