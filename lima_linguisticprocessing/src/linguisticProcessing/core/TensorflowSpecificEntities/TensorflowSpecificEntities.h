/** @brief  TensorflowSpecificEntities is the implementation of the module responsible of recognizing specific entities (ORGANIZATION, MISCELLANEAOUS, PERSON and LOCATION). 
  * @author      Victoria Bourgeais <victoria.bourgeais@cea.fr>  
  *
  *              Copyright (c) 2017 by CEA-LIST
  *
  */

#ifndef LIMA_LINGUISTICPROCESSING_TENSORFLOWSPECIFICENTITIES_TENSORFLOWSPECIFICENTITIES_H
#define LIMA_LINGUISTICPROCESSING_TENSORFLOWSPECIFICENTITIES_TENSORFLOWSPECIFICENTITIES_H

// includes from projec
#include "common/MediaProcessors/MediaProcessUnit.h"
#include "linguisticProcessing/core/TensorflowSpecificEntities/TensorflowSpecificEntitiesExport.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/Automaton/recognizerMatch.h"

// includes system
// #include <memory>

namespace Lima
{
namespace LinguisticProcessing
{
namespace TensorflowSpecificEntities
{
#define TENSORFLOWSPECIFICENTITIES_CLASSID "TensorflowSpecificEntities"

class TensorflowSpecificEntitiesPrivate;
  
/**
 * @class TensorflowSpecificEntities
 * @brief This is a @ref MediaProcessUnit that is usually the third element of the pipeline. Each token will be attributed an entity or not. Some tokens can be gathered in the same entity. 
 * Thus, the 3 main graphs will be updated.
 * 
  * As a ProcessUnit, it has an init and a process function. See @ref ProcessUnit for details.
  * 
  * 
 */
class LIMA_TENSORFLOWSPECIFICENTITIES_EXPORT TensorflowSpecificEntities : public MediaProcessUnit
{
public:
//   TensorflowSpecificEntities():m_d(std::make_unique<TensorflowSpecificEntitiesPrivate>()){}

  TensorflowSpecificEntities();
  virtual ~TensorflowSpecificEntities();
  
  /** @copydoc Lima::MediaProcessUnit::init()
   * 
   * @details 
   * It accepts the following parameters:
   * - param:graphOutputFile
   * - param:charValuesFile
   * - param:wordValuesFile
   * - param:tagValuesFile
   */
  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;

  /**  @brief @copybrief Lima::MediaProcessUnit::process()
   * run the tensorflow model on the sentences built by the previous MediaProcessUnit @ref Lima::LinguisticProcessing::LinguisticAnalysisStructure::SentenceBoundariesFinder to identify entities
   * 
   * @details
   * 1. initializes a tensorflow session and loads the model responsible for searching entities
   * 2. gets sentence bounds and pre-process them by calling some functions from an external library
   * the idea is to transform sentences into int according to external vocabularies which associate an identifier to words and characters
   * 3. runs the model by using minibatching (groups of 20 sentences max)
   * 4. calls @ref updateAnalysisData function to update the three graph  : Syntactic Graph, Analysis Graph and AnnotationGraph batch by batch
   * 
   * @param[in,out] analysis AnalysisContent object on which to process
   * 
   * @return  LimaStatusCode
   * @retval SUCCESS_ID if success
   * @retval UNKNOWN_ERROR if it is a tensorflow error
   * @retval MISSING_DATA if some data are missing
   */
  LimaStatusCode process(
    AnalysisContent& analysis) const override;  
  
private:
  
  //   std::unique_ptr<TensorflowSpecificEntitiesPrivate> m_d;  
  TensorflowSpecificEntitiesPrivate* m_d;
  
  TensorflowSpecificEntities(const TensorflowSpecificEntities&)=delete;
  TensorflowSpecificEntities& operator=(const TensorflowSpecificEntities&)=delete;
   
   /**  @brief check where entities have been created and calls @ref createSpecificEntity function to update each graph when entities have been found
   * 
   * @details
   * 1. looks for entities taking into account the sentences' order
   * when an entity is recognized, it keeps on searching if some tokens take part from the same one.  
   * 2. a specific object is created and analysed next by the @ref createSpecificEntity function
   * 
   * @param[in,out] analysis AnalysisContent object on which to process
   * 
   * @return  bool 1 for success
   */
  bool updateAnalysisData(AnalysisContent& analysis) const;
  
    /**  @brief update the three graphs by creating new @ref LinguisticGraphVertex and modifying edges
   * 
   * @details
   * each LinguisticGraphVertex recognized as an entity is associated to a new @ref LinguisticGraphVertex based on the original one's properties (lemma...) and micros properties are updated by calling @ref addMicrosToMorphoSyntacticData.
   * the edges in and out from the original one are linked to the new one.
   * if an entity gathers several LinguisticGraphVertex, only one is created. 
   * @ref clearUnreachableVertices is finally called to remove edges between original LinguisticGraphVertex gathered in a new one.
   * 
   * @note original LinguisticGraphVertex are unremoved.
   * Main algorithm comes from @ref Lima::LinguisticProcessing::SpecificEntities::CreateSpecificEntity::operator()
   * 
   * @param[in,out] analysis AnalysisContent object on which to process
   * 
   * @return  bool 1 for success
   */
  bool createSpecificEntity(
    Automaton::RecognizerMatch& entityFound,
    AnalysisContent& analysis) const;

   /**  @brief algorithm to set the new vertex linguistic properties
   * 
   * @details
   * If a list of micro-categories @ref micros is given in the complement and the entity head @ref oldMorphData have one (or more) of these micro categories, then the @ref newMorphData is composed of them.
   * Elsewhere, any micro is found in the entity head @ref oldMorphData, all micros are affected to the @ref newMorphData
   * 
   * @note each micro affected is first added to @ref elem which is pushed in the container @ref newMorphData
   * 
   * @param[in,out] newMorphData new MorphoSyntacticData from the new LinguisticGraphVertex created by the recognization of a entity
   * @param[in] oldMorphData MorphoSyntacticData from the original LinguisticGraphVertex of the entity's head 
   * @param[in] micros list of entity's micro-categories 
   * @param[in,out] elem new LinguisticElement created by the recognization of a entity
   */
  void addMicrosToMorphoSyntacticData(
     LinguisticAnalysisStructure::MorphoSyntacticData* newMorphData,
     const LinguisticAnalysisStructure::MorphoSyntacticData* oldMorphData,
     const std::set<LinguisticCode>& micros,
     LinguisticAnalysisStructure::LinguisticElement& elem) const;
  
  /**  @brief clear all edges attached to old vertex @from which don't belong to the main path
   * the root of the path is not the root of the main path
   * the node at the end of the path is not the last vertex of the main path
   * 
   * @param[in,out] anagraph 
   * @param[in,out] from 
   */
  void clearUnreachableVertices(
    LinguisticAnalysisStructure::AnalysisGraph* anagraph,
    LinguisticGraphVertex from) const;
};

} // closing namespace TensorflowSpecificEntities
} // closing namespace LinguisticProcessing
} // closing namespace Lima

#endif // LIMA_LINGUISTICPROCESSING_TENSORFLOWSPECIFICENTITIES_TENSORFLOWSPECIFICENTITIES_H
