// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004 by CEA LIST                       *
 *                                                                         *
 ***************************************************************************/

#ifndef LIMA_COMMON_PROPERTYCODE_XMLPROPERTYHANDLER_H
#define LIMA_COMMON_PROPERTYCODE_XMLPROPERTYHANDLER_H

#include "linguisticProcessing/LinguisticProcessingCommon.h"

#include <string>
#include <vector>
#include <map>

namespace Lima
{
namespace Common
{
namespace PropertyCode
{

class XmlPropertyReaderPrivate;
class LIMA_PROPERTYCODE_EXPORT XmlPropertyReader
{
public:
  // -----------------------------------------------------------------------
  //  Constructors
  // -----------------------------------------------------------------------
  XmlPropertyReader();
  ~XmlPropertyReader();

  struct PropertyDescription
  {
    std::string name;
    std::vector<std::string> values;
  };

  struct SubPropertyDescription
  {
    std::string name;
    std::string parentName;
    std::vector<std::pair<std::string,std::vector<std::string> > > values;
  };

  bool parse(QIODevice *device);

  QString errorString() const;

  const std::vector<PropertyDescription>& getProperties();
  const std::vector<SubPropertyDescription>& getSubProperties();

private:

  XmlPropertyReaderPrivate* m_d;
};

} // closing namespace PropertyCode
} // closing namespace Common
} // closing namespace Lima

#endif
