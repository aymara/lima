// Copyright (C) 2004-2012 by CEA LIST                               *
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef XMLIEDUMPERXMLIEDUMPER_H
#define XMLIEDUMPERXMLIEDUMPER_H

#include "linguisticProcessing/core/AnalysisDumpers/AbstractIEDumper.h"

namespace Lima {
namespace LinguisticProcessing {
namespace AnalysisDumpers {

#define XmlIEDUMPER_CLASSID "XmlIEDumper"

class LIMA_ANALYSISDUMPERS_EXPORT XmlIEDumper : public AbstractIEDumper
{
public:
  XmlIEDumper();

  virtual ~XmlIEDumper();

  void init(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
            Manager* manager) override;

  void outputGlobalHeader(std::ostream& os, const std::string& sourceFile="", 
                          const LimaStringText& /*originalText*/=LimaStringText("")) const override;
  void outputGlobalFooter(std::ostream& os) const override;
  void outputEntitiesHeader(std::ostream& os) const override;
  void outputEntitiesFooter(std::ostream& os) const override;
  void outputRelationsHeader(std::ostream& os) const override;
  void outputRelationsFooter(std::ostream& os) const override;
  void outputEventsHeader(std::ostream& os) const override;
  void outputEventsFooter(std::ostream& os) const override;

  void outputEntityString(std::ostream& out,
                          unsigned int entityId,
                          const std::string& entityType,
                          const std::string& entityString,
                          const std::vector<std::pair<uint64_t,uint64_t> >& positions,
                          const Automaton::EntityFeatures& entityFeatures, bool noNorm=false) const override;

  void outputAttributesString(std::ostream& out,
                          unsigned int entityId,
                          std::map <std::tuple <std::size_t, std::string , std::string >,  std::size_t >& mapAttributes) const override;

  unsigned int outputRelationString(std::ostream& out,
                            unsigned int relationId,
                            const std::string& relationType,
                            const std::string& sourceArgString,
                            const std::string& targetArgString) const override;

  void outputEventString(std::ostream& out,
                         unsigned int eventId,
                         unsigned int eventMentionId,
                         const std::string& eventMentionType,
                         const std::vector<unsigned int>& eventRoleId,
                         const std::vector<std::string>& eventRoleType) const override;

  void outputEntityNorm(std::ostream& out,
                        unsigned int entityId,
                        const std::string& entityNorm) const override;

  // add event mention as a separate entity
  bool addEventMentionAsEntity() const override { return false; }

private:

};

} // AnalysisDumpers
} // LinguisticProcessing
} // Lima

#endif
