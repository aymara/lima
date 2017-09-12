/************************************************************************
 *
 * @file       EventTemplateDefinitionResource.h
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Fri Sep  2 2011
 * copyright   Copyright (C) 2011 by CEA LIST
 * Project     MM
 * 
 * @brief      Linguistic resource storing the defintion of event templates
 * 
 * 
 ***********************************************************************/

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
  const std::string& getMention(const std::string)  const;
  const std::map<std::string,Common::MediaticData::EntityType>& getStructure(const std::string)  const;
    
 private:
  MediaId m_language;
  std::vector<EventTemplateStructure> m_templates;
  std::map<std::string, std::set<std::string> > m_elementMapping;
  
};

} // end namespace
} // end namespace
} // end namespace

#endif
