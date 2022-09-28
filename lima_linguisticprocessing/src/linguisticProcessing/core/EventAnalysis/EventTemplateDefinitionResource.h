// Copyright (C) 2011 by CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef EVENTTEMPLATEDEFINITIONRESOURCE_H
#define EVENTTEMPLATEDEFINITIONRESOURCE_H

#include "EventAnalysisExport.h"
#include "EventTemplateStructure.h"
#include "linguisticProcessing/core/LinguisticResources/AbstractResource.h"

namespace Lima {
namespace LinguisticProcessing {
namespace EventAnalysis {

#define EVENTTEMPLATEDEFINITIONRESOURCE_CLASSID "EventTemplateDefinition"

class LIMA_EVENTANALISYS_EXPORT EventTemplateDefinitionResource : public AbstractResource
{
  Q_OBJECT
 public:
  EventTemplateDefinitionResource(); 
  ~EventTemplateDefinitionResource();

  void init(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
            Manager* manager) override;
  
  // mapping is oriented, return 1 if mapping elt1 -> elt2, -1 if mapping elt2 -> elt1, 0 otherwise
  int existsMapping(const std::string& eltName1, const std::string& eltName2) const;
  const std::string& getName()  const;
  const std::string& getMention()  const;
  const std::map<std::string,Common::MediaticData::EntityType>& getStructure()  const;
  unsigned int getCardinality(const std::string& role) const;
    
 private:
  MediaId m_language;
  EventTemplateStructure m_structure;
  std::map<std::string, std::set<std::string> > m_elementMapping;
  std::map<std::string, unsigned int> m_cardinalities;
};

} // end namespace
} // end namespace
} // end namespace

#endif
