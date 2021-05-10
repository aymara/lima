/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
#ifndef BRATJSONDUMPERBRATDUMPER_H
#define BRATJSONDUMPERBRATDUMPER_H

#include "linguisticProcessing/core/AnalysisDumpers/AbstractIEDumper.h"

namespace Lima {
namespace LinguisticProcessing {
namespace AnalysisDumpers {

#define BRATJSONDUMPER_CLASSID "BratJSONDumper"

// output results in JSON compatible with the embedded Brat
// see docData in https://brat.nlplab.org/embed.html
class LIMA_ANALYSISDUMPERS_EXPORT BratJSONDumper : public AbstractIEDumper
{
public:
  BratJSONDumper();
  virtual ~BratJSONDumper();

  void init(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
            Manager* manager) override;
  
  void outputGlobalHeader(std::ostream& os, const std::string& sourceFile="", 
                          const LimaStringText& originalText=LimaStringText("")) const;
  void outputGlobalFooter(std::ostream& os) const;
  void outputEntitiesHeader(std::ostream& os) const;
  void outputEntitiesFooter(std::ostream& os) const;
  void outputRelationsHeader(std::ostream& os) const;
  void outputRelationsFooter(std::ostream& os) const;
  void outputEventsHeader(std::ostream& os) const;
  void outputEventsFooter(std::ostream& os) const;
  
  void outputEntityString(std::ostream& out,
                          unsigned int entityId,
                          const std::string& entityType,
                          const std::string& entityString,
                          const std::vector<std::pair<uint64_t,uint64_t> >& positions,
                          const Automaton::EntityFeatures& entityFeatures,
                          bool noNorm=false) const override;

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
  //bool addEventMentionAsEntity() const override { return true; }
  bool addEventMentionAsEntity() const override { return false; }

private:
  // set members mutable (modified in the inherited const functions)
  mutable bool m_firstEntity;
  mutable bool m_firstRelation;
  mutable bool m_firstEvent;
  // attributes and norms are printed with the entities in AbstractIEDumper: need them to be printed
  // separately in JSON: store them to print them at the end (could do that with all -> better ?)
  mutable std::vector<std::string> m_attributes;
  mutable std::vector<std::string> m_norms;
  // event triggers are different from the entities: same format but stored associated with a 'triggers' key
  // have to wait until the events to choose where to put them, store them until that point
  mutable std::map<unsigned int,std::string> m_entities;
  mutable std::vector<std::string> m_eventTriggers;
};

} // AnalysisDumpers
} // LinguisticProcessing
} // Lima

#endif
