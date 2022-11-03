// Copyright 2002-2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef SYMBOLICCODEXMLREADER_H
#define SYMBOLICCODEXMLREADER_H

#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "linguisticProcessing/common/PropertyCode/PropertyCodeManager.h"

#include <string>
#include <map>


class SymbolicCodeXmlReaderPrivate;
class LIMA_PROPERTYCODE_EXPORT SymbolicCodeXmlReader
{
public:
  // -----------------------------------------------------------------------
  //  Constructors
  // -----------------------------------------------------------------------
  SymbolicCodeXmlReader(const Lima::Common::PropertyCode::PropertyCodeManager& pcm,
                        std::map<std::string,Lima::LinguisticCode>& outputMap);
  ~SymbolicCodeXmlReader();

  bool parse(QIODevice *device);

  QString errorString() const;

private:
  SymbolicCodeXmlReaderPrivate* m_d;
};

#endif
