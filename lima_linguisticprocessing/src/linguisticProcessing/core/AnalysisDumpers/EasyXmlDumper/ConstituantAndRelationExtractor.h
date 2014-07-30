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
/**
 *
 * @file       ConstituantAndRelationExtractor.h
 * @author     Damien Nouvel <Damien.Nouvel@cea.fr> 

 *             Copyright (C) 2004 by CEA LIST
 * @author     Gael de Chalendar <Gael.de-Chalendar@cea.fr>
 * @author     Benoit Mathieu <mathieub@zoe.cea.fr>
 * @date       Mon Oct 07 2008
 * Project     s2lp
 *
 * @brief      extracts forms and relations from boost graph (origninally, from XML file)
 *
 */

#ifndef LIMA_LINGUISTICPROCESSINGS_ANALYSISDUMPERS_EASYXMLDUMPER_CONSTITUANTANDRELATIONEXTRACTOR_H
#define LIMA_LINGUISTICPROCESSINGS_ANALYSISDUMPERS_EASYXMLDUMPER_CONSTITUANTANDRELATIONEXTRACTOR_H

#include "chaine.h"
#include "poslong.h"
#include "relation.h"
#include "forme.h"
#include "groupe.h"

#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/common/PropertyCode/PropertyCodeManager.h"
#include "linguisticProcessing/common/PropertyCode/PropertyManager.h"
#include "linguisticProcessing/common/PropertyCode/PropertyAccessor.h"

#include "common/MediaProcessors/MediaAnalysisDumper.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/SyntacticAnalysis/SyntacticData.h"
#include "linguisticProcessing/core/Automaton/SpecificEntityAnnotation.h"
#include "linguisticProcessing/core/MorphologicAnalysis/IdiomaticExpressionAnnotation.h"

#include <limits>
#include <string>
#include <set>
#include <vector>
#include <map>
#include <cstdint>

#include <boost/tuple/tuple.hpp>
#include <boost/graph/properties.hpp>

#include "common/misc/gregoriannowarn.hpp"

namespace Lima {
namespace LinguisticProcessing {
namespace AnalysisDumpers {
namespace EasyXmlDumper {

class ConstituantAndRelationExtractor
{

public:

  ConstituantAndRelationExtractor(const Common::PropertyCode::PropertyCodeManager* propertyCodeManager);
  virtual ~ConstituantAndRelationExtractor();

  void visitBoostGraph(const LinguisticGraphVertex& v,
                       const LinguisticGraphVertex& end,
                       const LinguisticGraph& anaGraph,
                       const LinguisticGraph& posGraph,
                       const Common::AnnotationGraphs::AnnotationData& annotationData,
                       const SyntacticAnalysis::SyntacticData& syntacticData,
                       std::map< LinguisticAnalysisStructure::Token*, uint64_t >& fullTokens,
                       std::vector< bool >& alreadyDumpedTokens,
                       const MediaId& language);

  const std::map<Chaine,std::vector<uint64_t> >& getChaines() const { return m_chaines;}
  const std::map<uint64_t,uint64_t>& getVertexToFormeIds() const { return m_vertexToFormeIds;}
  const std::map<uint64_t,Forme*>& getFormesIndex() const { return m_formesIndex;}
  const std::vector<Relation*>& getRelations() const { return m_outRelations;}
  const std::vector<Relation*>& getInRelations() const { return m_inRelations;}
  const std::map<uint64_t, Groupe>& getGroupes() const { return m_groupes;}
  const std::map< uint64_t, uint64_t >& positionsFormsIds() const { return m_positionsFormsIds; };
  const std::set< uint64_t >& inGroupFormsPositions() const { return m_inGroupFormsPositions; };

  void constructionDesGroupes();
  void constructionDesRelationsEntrantes();
  void replaceSEWithCompounds();
  void splitCompoundTenses();
  void addLastFormsInGroups();

protected:

  Forme* extractVertex(const LinguisticGraphVertex& v,
                       const LinguisticGraph& graph,
                       bool checkFullTokens,
                       std::map< LinguisticAnalysisStructure::Token*, uint64_t >& fullTokens,
                       std::vector< bool >& alreadyDumpedTokens,
                       MediaId language);

  Relation* extractEdge(const LinguisticGraphEdge& e,
                        const LinguisticGraph& posGraph,
                        const DependencyGraph& depGraph,
                        std::map< LinguisticAnalysisStructure::Token*, uint64_t >& fullTokens,
                        const SyntacticAnalysis::SyntacticData& syntacticData,
                        MediaId language);

  const Common::PropertyCode::PropertyAccessor m_macroA;
  const Common::PropertyCode::PropertyManager m_macroPm;
  const Common::PropertyCode::PropertyAccessor m_microA;
  const Common::PropertyCode::PropertyManager m_microPm;

private:

//   std::string getName(const QString& localName,
//                       const QString& qName);

  Groupe* createGroupe(const Forme* forme,
                      const std::set<std::string>& relationsToFollow,
                      const std::string& groupType);
  Groupe* createGroupe(const Forme* forme,
                      const std::set<std::string>& relationsToFollow,
                      const std::string& groupType,
                      bool mayBeUnique);

  void insertGroup(const Groupe& groupe);
  bool addToGroupIfIsInsideAGroup(const Forme* forme);

  template <typename AnnotationType>
  void splitCompoundAnalysisAnnotation(
    LinguisticGraphVertex v,
    Forme& forme,
    LimaString& annotationTypeStr,
    const Common::AnnotationGraphs::AnnotationData& annotationData,
    const LinguisticGraph& anaGraph,
    std::map< LinguisticAnalysisStructure::Token*, uint64_t >& fullTokens,
    std::vector< bool >& alreadyDumpedTokens,
    const MediaId& language)
  {
    Common::AnnotationGraphs::GenericAnnotation genAnnot = annotationData.annotation(v, annotationTypeStr);
    AnnotationType genAnnotVect = genAnnot.value<AnnotationType>();
    m_namedEntitiesVertices.insert(std::make_pair(forme.id, forme));
    m_posAnaMatching.insert(std::make_pair(forme.id, v));
    std::vector< LinguisticGraphVertex >::iterator genAnnotVectIt, genAnnotVectIt_end;
    genAnnotVectIt = genAnnotVect.m_vertices.begin(); genAnnotVectIt_end = genAnnotVect.m_vertices.end();
    std::vector<uint64_t> genAnaVertices;
    for(; genAnnotVectIt != genAnnotVectIt_end; genAnnotVectIt++)
    {
      Forme* anaForme = extractVertex(*genAnnotVectIt, anaGraph, false, fullTokens, alreadyDumpedTokens, language);
      if(anaForme != 0)
      {
        // if corresponding AnalysisGraph vertex, link it
        DUMPERLOGINIT;
        LDEBUG << "ConstituantAndRelationExtractor:: got analysis " << *genAnnotVectIt << ", " << anaForme->forme;
        m_anaGraphVertices[*genAnnotVectIt] = anaForme;
        genAnaVertices.push_back(*genAnnotVectIt);
      }
    }
    m_seCompounds[v] = genAnaVertices;
  }

  std::map<Chaine,std::vector<uint64_t> > m_chaines;
  std::map<uint64_t,uint64_t> m_vertexToFormeIds;
  std::map<uint64_t,uint64_t> m_formeIdsToVertex;

  std::map<uint64_t,Forme*> m_formesIndex; // id in pos graph -> forme
  std::map<uint64_t,Forme*> m_anaGraphVertices; // id in analysis graph -> forme

  std::vector<Relation*> m_outRelations;
  std::vector<Relation*> m_inRelations;

  std::map<uint64_t,std::vector<uint64_t> > m_seCompounds; // List of specific entitites
  std::map<uint64_t, Forme> m_namedEntitiesVertices; // List of named entitites
  std::map< uint64_t, std::pair< uint64_t, uint64_t > > m_compoundTenses; // compound -> (aux id, participle id)

  std::map<uint64_t,uint64_t> m_posAnaMatching; // id in pos graph -> id in ana graph
  std::map<uint64_t,uint64_t> m_posAnnotMatching; // id in pos graph -> id in annot graph
  std::map<uint64_t,uint64_t> m_annotPosMatching; // id in annot graph -> id in pos graph

  std::map<uint64_t, Groupe> m_groupes; // position -> groupe
  std::map< uint64_t, uint64_t > m_positionsFormsIds; // position -> id
  std::set< uint64_t > m_inGroupFormsPositions;   // positions

};

} // end namespace EasyXmlDumper
} // end namespace AnalysisDumpers
} // end namespace LinguisticProcessings
} // end namespace Lima

#endif // LIMA_LINGUISTICPROCESSINGS_ANALYSISDUMPERS_EASYXMLDUMPER_CONSTITUANTANDRELATIONEXTRACTOR_H
