/************************************************************************
 *
 * @file       AbstractIEDumper.h
 * @author     Faiza Gara, Romaric Besançon (romaric.besancon@cea.fr)
 * @date       Wed Nov 23 2016
 * copyright   Copyright (C) 2016 by CEA - LIST
 * Project     AMOSE
 *
 * @brief      Abstract dumper for extracted information: entities, relations and events: do not implement the exact output format of these things but contains all the common part consisting of gathering the information.
 *
 *
 ***********************************************************************/
#ifndef ABSTRACTIEDUMPER_H
#define ABSTRACTIEDUMPER_H

#include "linguisticProcessing/core/AnalysisDumpers/AnalysisDumpersExport.h"
#include "linguisticProcessing/core/EventAnalysis/EventAnalysisExport.h"
#include "linguisticProcessing/core/SemanticAnalysis/SemanticRelationAnnotation.h"
#include "linguisticProcessing/core/EventAnalysis/EventTemplateData.h"
#include "linguisticProcessing/core/Automaton/SpecificEntityAnnotation.h"
#include "linguisticProcessing/core/EventAnalysis/EventTemplateDefinitionResource.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/LinguisticProcessors/AbstractTextualAnalysisDumper.h"
#include "linguisticProcessing/core/AnalysisDumpers/OffsetMapping.h"
#include "linguisticProcessing/common/linguisticData/LimaStringText.h"

namespace Lima {
namespace LinguisticProcessing {
namespace AnalysisDumpers {

class LIMA_ANALYSISDUMPERS_EXPORT AbstractIEDumper : public AbstractTextualAnalysisDumper
{
public:
  AbstractIEDumper();

  virtual ~AbstractIEDumper();

  virtual void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override
    ;

  virtual LimaStatusCode process(AnalysisContent& analysis) const override;

  /**@{
   * @brief specific output functions to deal with actual output format
   * these functions are called inside the process() function.
   */
  virtual void outputGlobalHeader(std::ostream& /*os*/, const std::string& /*sourceFile*/="") const {}
  virtual void outputGlobalFooter(std::ostream& /*os*/) const {}
  virtual void outputEntitiesHeader(std::ostream& /*os*/) const {}
  virtual void outputEntitiesFooter(std::ostream& /*os*/) const {}
  virtual void outputRelationsHeader(std::ostream& /*os*/) const {}
  virtual void outputRelationsFooter(std::ostream& /*os*/) const {}
  virtual void outputEventsHeader(std::ostream& /*os*/) const {}
  virtual void outputEventsFooter(std::ostream& /*os*/) const {}

  virtual void outputEntityString(std::ostream& out,
                                  unsigned int entityId,
                                  const std::string& entityType,
                                  const std::string& entityString,
                                  const std::vector<std::pair<uint64_t,uint64_t> >& positions,
                                  const Automaton::EntityFeatures& entityFeatures,
                                  bool noNorm=false) const=0;

  virtual void outputAttributesString(std::ostream& out,
                                  unsigned int entityId,
                                  std::map <std::tuple <std::size_t, std::string , std::string >,  std::size_t >& mapAttributes) const=0;

  virtual unsigned int outputRelationString(std::ostream& out,
                                    unsigned int relationId,
                                    const std::string& relationType,
                                    const std::string& sourceArgString,
                                    const std::string& targetArgString) const=0;

  virtual void outputEventString(std::ostream& out,
                                 unsigned int eventId,
                                 unsigned int eventMentionId,
                                 const std::string& eventMentionType,
                                 const std::vector<unsigned int>& eventRoleId,
                                 const std::vector<std::string>& eventRoleType) const=0;

  virtual void outputEntityNorm(std::ostream& out,
                                 unsigned int entityId,
                                 const std::string& entityNorm) const=0;

  /**@}
   */

  // virtual function to indicate if event mention must be printed separately as an entity: to be redefined in child classes if needed
  virtual bool addEventMentionAsEntity() const { return false; }

protected:
  MediaId m_language;
  std::string m_graph;
  bool m_followGraph;
  std::string m_domain;
  std::deque< std::string > m_attributes;
  bool m_all_attributes;
  std::map<std::string,EventAnalysis::EventTemplateDefinitionResource*> m_templateDefinitions;
  std::set<std::string> m_templateNames;
  // small hack: make it mutable so that process() can be const
  // @todo: pass it as argument to all the internal functions where needed
  mutable OffsetMapping* m_offsetMapping;
  bool m_outputGroups; // ouptut full entity types with the groups

  //member private members
  const SemanticAnalysis::SemanticRelationAnnotation*
    getSemanticRelationAnnotation(LinguisticGraphVertex v,
                                  const Common::AnnotationGraphs::AnnotationData* annotationData) const;


  const SpecificEntities::SpecificEntityAnnotation*
    getSpecificEntityAnnotation(LinguisticGraphVertex v,
                                const Common::AnnotationGraphs::AnnotationData* annotationData) const;

  bool
    outputEntity(std::ostream& out,
                 LinguisticGraphVertex v,
                 const SpecificEntities::SpecificEntityAnnotation* annot,
                 const VertexTokenPropertyMap& tokenMap,
                 uint64_t offset,
                 LimaStringText originalText,
                 std::map <std::tuple <uint64_t, uint64_t, std::string >, std::size_t >& mapEntities,
                 std::map <std::tuple <std::size_t, std::string , std::string >,  std::size_t >& mapAttributes
      ) const;

  void outputSemanticRelations(std::ostream& out,
                               const Common::AnnotationGraphs::AnnotationData* annotationData,
                               const VertexTokenPropertyMap& tokenMap,
                               std::map<std::tuple<std::uint64_t,std::uint64_t,std::string>,std::size_t > mapEntities,
                               uint64_t offset
    ) const;
  void outputEventData(std::ostream& out,
                       const EventAnalysis::EventTemplateData* eventData,
                       const Common::AnnotationGraphs::AnnotationData* annotationData,
                       const VertexTokenPropertyMap& tokenMap,
                       std::map<std::tuple<std::uint64_t,std::uint64_t,std::string>,std::size_t > mapEntities,
                       uint64_t offset,
                       LimaStringText originalText
    ) const;
  std::string outputSemanticRelationArg(const std::string& vertexRole,
                                        const AnnotationGraphVertex& vertex,
                                        const VertexTokenPropertyMap& tokenMap,
                                        std::map<std::tuple<std::uint64_t,std::uint64_t,std::string>,std::size_t >  mapEntities,
                                        const Common::AnnotationGraphs::AnnotationData* annotationData,
                                        uint64_t offset) const;

   void computePositions(std::vector<std::pair<uint64_t,uint64_t> >& positions, LimaString& stringForm, uint64_t pos, uint64_t len) const;

   void adjustPosition(std::uint64_t& position) const;
   void adjustPositions(std::vector<std::pair<uint64_t,uint64_t> >& positions) const;
   

};

} // AnalysisDumpers
} // LinguisticProcessing
} // Lima

#endif
