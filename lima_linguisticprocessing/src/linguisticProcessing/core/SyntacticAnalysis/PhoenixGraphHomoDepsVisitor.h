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
  * @file        PhoenixGraphHomeDepsVisitor.h
  * @author      Gael de Chalendar <Gael.de-Chalendar@cea.fr> 

  *              Copyright (c) 2003 by CEA
  * @date        Created on Oct, 8 2003
  * @version     $Id$
  *
  */

#ifndef LIMA_SYNTACTICANALYSIS_PHOENIXGRAPHHOMODEPSVISITOR_H
#define LIMA_SYNTACTICANALYSIS_PHOENIXGRAPHHOMODEPSVISITOR_H

#include "SyntacticData.h"
#include "DependencyGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticDataUtils.h"
#include "linguisticProcessing/core/Automaton/recognizer.h"

#include "common/misc/depth_first_searchnowarn.hpp"

#include <string>

namespace Lima
{
namespace LinguisticProcessing
{
namespace SyntacticAnalysis
{

class PhoenixGraphHomoDepsVisitor : public boost::default_dfs_visitor
{
protected:
  typedef boost::color_traits < boost::default_color_type > Color;

  MediaId m_language;
  const Common::PropertyCode::PropertyAccessor* m_genderAccessor;
  const Common::PropertyCode::PropertyAccessor* m_numberAccessor;
  Automaton::Recognizer* m_homoDepRecognizer;
  SyntacticData* m_data;
  LinguisticGraphVertex m_homoDepsStartVertex;
  LinguisticGraphVertex m_homoDepsStopVertex;
  AnalysisContent* m_analysis;

public:
  PhoenixGraphHomoDepsVisitor(
    MediaId language,
    Automaton::Recognizer* homoDepRecognizer,
    SyntacticData* data,
    LinguisticGraphVertex homoDepsStartVertex,
    LinguisticGraphVertex homoDepsStopVertex,
    AnalysisContent* analysis,
    bool estOnDicoWord ) :
      m_language(language),
      m_homoDepRecognizer(homoDepRecognizer),
      m_data(data),
      m_homoDepsStartVertex(homoDepsStartVertex),
      m_homoDepsStopVertex(homoDepsStopVertex),
      m_analysis(analysis)
  {
    LIMA_UNUSED(estOnDicoWord);
    m_genderAccessor=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyAccessor("GENDER"));
    m_numberAccessor=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyAccessor("NUMBER"));
  }
  ;

  bool enforcePropertiesConstraints(
    Common::MediaticData::SyntacticRelationId type,
    LinguisticGraphVertex src, LinguisticGraphVertex dest);

  void addRelationBetweenForChain(
    Common::MediaticData::SyntacticRelationId type,
    LinguisticGraphVertex src, LinguisticGraphVertex dest,
    const LinguisticAnalysisStructure::ChainIdStruct& chain);

  template < typename Graph >
  void examine_edge(LinguisticGraphEdge e, const Graph& g);

  template < typename Graph >
  void discover_vertex(LinguisticGraphVertex v, const Graph& g);

  template < typename Graph >
  void finish_vertex(LinguisticGraphVertex v, const Graph& g);

class StartFinishedException : public std::exception
    {}
  ;

};

template < typename Graph >
void PhoenixGraphHomoDepsVisitor::examine_edge(LinguisticGraphEdge e, const Graph& g)
{
//   SAPLOGINIT;
//   LDEBUG << "PhoenixGraphHomoDepsVisitor::examine_edge " << e << " (stop vertex is: " << m_homoDepsStopVertex << ")";

  VertexColorLinguisticGraphPropertyMap c_map = get(boost::vertex_color, const_cast< Graph& >(g));

  // use graph attached to the syntacticData
  Lima::LinguisticProcessing::LinguisticAnalysisStructure::AnalysisGraph*
  analysisGraph=m_data->iterator();

  LinguisticGraphVertex v = target(e,g);

//   LDEBUG << "dest/stop is: " << v << "/" << m_homoDepsStopVertex;
  if (v == m_homoDepsStopVertex)
  {
//     LDEBUG << "Vertex " << v << " color was: " << c_map[v];
    c_map[v] = Color::black();
//     LDEBUG << "Vertex " << v << " new color is: " << c_map[v];
  }

  if ( v == analysisGraph->firstVertex() || v == analysisGraph->lastVertex()) return;


  // mettre �jour les chaines en cours
  // faire un test d'automates avec l'ensemble de r�les ad�uat
  std::vector< Lima::LinguisticProcessing::Automaton::RecognizerMatch > recognizerResult;

//   LDEBUG << "Searching a relation from " << v;
  m_data->clearStoredRelations();
  LinguisticGraphVertex v_copy = v;
  bool relationFound = false;
//   LDEBUG << "Calling testOnVertex " << v_copy;
  relationFound = m_homoDepRecognizer->
                  testOnVertex(*analysisGraph, v_copy,
                               m_homoDepsStartVertex, m_homoDepsStopVertex, *m_analysis,recognizerResult);
  std::set< std::string > alreadyCreatedRelations;
  while (relationFound)
  {
//     LDEBUG << "Automaton results found: " << recognizerResult.size();
    LinguisticGraphVertex src, dest;
    std::string relation;
    boost::tie(src,dest,relation) = m_data->relation();
    std::ostringstream oss; oss << src << "/" << dest << "/" << relation;
    std::string foundRel = oss.str();
//     LDEBUG << "Working on relation found " << foundRel;
    while (!((src == m_data->iterator()->firstVertex()) &&
             (dest == m_data->iterator()->lastVertex()) ))
    {
      Lima::LinguisticProcessing::Automaton::RecognizerMatch matchResult = *(recognizerResult.begin());
      Common::MediaticData::SyntacticRelationId relType;
      if (relation != "")
      {
        relType = static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getSyntacticRelationId(relation);
      }
      else
      {
        relType = matchResult.getType();
        relation = static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getSyntacticRelationName(relType);;
      }
      if (alreadyCreatedRelations.find(foundRel) != alreadyCreatedRelations.end())
      {
//         LDEBUG << foundRel << " already found. Getting next";
        boost::tie(src,dest,relation) = m_data->relation();
        if (relation != "")
        {
          relType = static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getSyntacticRelationId(relation);
        }
        else
        {
          relType = matchResult.getType();
          relation = static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getSyntacticRelationName(relType);;
        }
        std::ostringstream noss ; noss << src << "/" << dest << "/" << relation;
        foundRel = noss.str();
//         LDEBUG << "Next is " << foundRel;
        if (src == 0 && dest == 1)
        {
//           LDEBUG << "There is no more relation found from " << v << "; breaking";
          m_data->clearStoredRelations();
//           LDEBUG << "Leaving PhoenixGraphHomoDepsVisitor::examine_edge(" << e << ")";
          return;
        }
        else
        {
//           LDEBUG << "There is more ; continuing";
          continue;
        }
      }
      else
      {
//         LDEBUG << foundRel << " not already found. Using it";
        alreadyCreatedRelations.insert(foundRel);
      }
      std::set< Lima::LinguisticProcessing::LinguisticAnalysisStructure::ChainIdStruct > intersection;
      VertexChainIdPropertyMap chainMap = get( vertex_chain_id, *(m_data-> graph()));
      std::set_intersection(
        chainMap[src].begin(), chainMap[src].end(),
        chainMap[dest].begin(), chainMap[dest].end(),
        std::insert_iterator< std::set< Lima::LinguisticProcessing::LinguisticAnalysisStructure::ChainIdStruct > >(intersection, intersection.end())
        );

      bool created = false;
//       LDEBUG << "Intersection size: " << intersection.size();
      if (intersection.empty())
      {
        SAPLOGINIT;
        LWARN << "In homosyntagmatic dependence relations extraction: ends ("
        <<src<<" and "<<dest<<") seems to not belong to the same syntagmatic chain ;";
        //        LWARN << "aborting this dependence relation";
        //        break;
        Lima::LinguisticProcessing::LinguisticAnalysisStructure::ChainIdStruct cis;
        addRelationBetweenForChain(relType,src, dest, cis);
        created = true;
      }
      else
      {
        std::set< Lima::LinguisticProcessing::LinguisticAnalysisStructure::ChainIdStruct >::iterator intersectionIt, intersectionIt_end;
        intersectionIt = intersection.begin();
        intersectionIt_end = intersection.end();

        //      bool created = false;
        for (;intersectionIt != intersectionIt_end; intersectionIt++)
        {
//           LDEBUG << "Intersection= " << (*intersectionIt).chainId() << " ; "<< relType;
          if ( //((*intersectionIt).chainId() != 0) &&
            ( static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).isTypeARelationForChain(
                (*intersectionIt).chainType(), relType) ) )
          {
//             LDEBUG;
//             LDEBUG << "Creating relation: " << relType << " " << src << " / " << dest;
//             LDEBUG;
            created = true;
            try
            {
              addRelationBetweenForChain(relType,src, dest, (*intersectionIt));
            }
            catch (const std::runtime_error& e)
            {
              created = false;
            }
          }
          else
          {
            SAPLOGINIT;
            LINFO << "Relation type " << relType << " is not defined as a relation for chain type " << (*intersectionIt).chainType();
            LINFO << "Cannot search other relations from " << v << " or will loop.";
//             LDEBUG << "No more relation found from " << v;
//             LDEBUG << "Leaving PhoenixGraphHomoDepsVisitor::examine_edge(" << e << ")";
            m_data->clearStoredRelations();
            return;
          }
        }
      }
//       if (!created)
//       {
//         LDEBUG << "Creation of relation: " << relType << " " << src << " / " << dest << " aborted.";
//       }
      boost::tie(src,dest,relation) = m_data->relation();
    }
    recognizerResult.clear();
//     LDEBUG << "Searching another relation from " << v;
    v_copy = v;
//     LDEBUG << "Calling testOnVertex " << v_copy;
    relationFound = m_homoDepRecognizer->
                    testOnVertex(*analysisGraph, v_copy,
                                 m_homoDepsStartVertex, m_homoDepsStopVertex, *m_analysis, recognizerResult);
  }
  m_data->clearStoredRelations();

//   LDEBUG << "No more relation found from " << v;
//   LDEBUG << "Leaving PhoenixGraphHomoDepsVisitor::examine_edge(" << e << ")";
}

bool PhoenixGraphHomoDepsVisitor::enforcePropertiesConstraints(
  Common::MediaticData::SyntacticRelationId type,
  LinguisticGraphVertex src, LinguisticGraphVertex dest)
{
  VertexDataPropertyMap map =  get(vertex_data, *(m_data->graph()));
  LinguisticAnalysisStructure::MorphoSyntacticData* data1 = map[src];
  LinguisticAnalysisStructure::MorphoSyntacticData* data2 = map[dest];
  if (data1 == 0 || data1->empty() || data2 == 0 || data2->empty())
  {
    return false;
  }
  bool result = true;

  Common::MediaticData::SyntacticRelationId R_DETSUB = static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getSyntacticRelationId("DETSUB");
  Common::MediaticData::SyntacticRelationId R_SUBADJPOST = static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getSyntacticRelationId("SUBADJPOST");

  // contrainte sur le genre: art->nom , adj->nom
  // @todo completer la liste a charger depuis config
  if ( (type == R_DETSUB) || (type == R_SUBADJPOST) )
  {
    std::set< LinguisticCode > genres1 = data1->allValues(*m_genderAccessor);
    std::set< LinguisticCode > genres2 = data2->allValues(*m_genderAccessor);
    if (genres1.size()!=0 || genres2.size()!=0)
    {
      std::set< LinguisticCode > commonGenders;
      std::set_intersection(genres1.begin(), genres1.end(),
                            genres2.begin(), genres2.end(),
                            std::insert_iterator< std::set< LinguisticCode> >(commonGenders, commonGenders.end()));
      LinguisticAnalysisStructure::ExcludePropertyPredicate epp(m_genderAccessor,commonGenders);
      data1->erase(remove_if(data1->begin(),data1->end(),epp),data1->end());
      data2->erase(remove_if(data2->begin(),data2->end(),epp),data2->end());
    }
  }

  // contrainte sur le nombre: art->nom , adj->nom
  if ( (type == R_DETSUB) || (type == R_SUBADJPOST) )
  {
    std::set< LinguisticCode > numbers1 = data1->allValues(*m_numberAccessor);
    std::set< LinguisticCode > numbers2 = data2->allValues(*m_numberAccessor);
    std::set< LinguisticCode > commonNumbers;
    std::set_intersection(
      numbers1.begin(), numbers1.end(),
      numbers2.begin(), numbers2.end(),
      std::insert_iterator< std::set< LinguisticCode> >(commonNumbers, commonNumbers.end()));
    if (!(numbers1.size()==0 && numbers2.size()==0) &&
        !( (type == R_SUBADJPOST) && (numbers1.size()==1) && (m_numberAccessor->empty(*(numbers1.begin())))))
    {
      LinguisticAnalysisStructure::ExcludePropertyPredicate epp(m_numberAccessor,commonNumbers);
      data1->erase(remove_if(data1->begin(),data1->end(),epp),data1->end());
      data2->erase(remove_if(data2->begin(),data2->end(),epp),data2->end());
    }
  }
  return result;
}


void PhoenixGraphHomoDepsVisitor::addRelationBetweenForChain(
  Common::MediaticData::SyntacticRelationId type,
  LinguisticGraphVertex src, LinguisticGraphVertex dest,
  const LinguisticAnalysisStructure::ChainIdStruct& chain)
{
//   SAPLOGINIT;
//   LDEBUG << "Adding relation " << type << " between " << src << " and " << dest << " for chain: " << chain;
  /*
    if (enforcePropertiesConstraints(type,src,dest))
    {
  */
  DependencyGraphVertex v1 = m_data->depVertexForTokenVertex(src);
  DependencyGraphVertex v2 = m_data->depVertexForTokenVertex(dest);
  DependencyGraphEdge edge;
  bool success;
  boost::tie(edge, success) = add_edge(v1, v2, *(m_data->dependencyGraph()));
  if (success)
  {
//     put( edge_depchain_id, *(m_data->dependencyGraph()), edge, chain);
    put( edge_deprel_type, *(m_data->dependencyGraph()), edge, type);
  }
  else
  {
    SAPLOGINIT;
    LERROR << "Was not able to add relation (" << type << ") between " << src << " and " << dest << " for chain: " << chain;
    throw std::runtime_error("Was not able to add relation");
  }
  /*
    }
    else
    {
      LWARN << "Was not able to enforce properties constraints when adding relation (" << type << ") between " << src << " and " << dest << " for chain: " << chain << ". Adding aborted.";
      throw std::runtime_error("Was not able to enforce properties constraints when adding relation");
    }
  */
}


template < typename Graph >
void PhoenixGraphHomoDepsVisitor::discover_vertex(LinguisticGraphVertex v, const Graph& g)
{
//   SAPLOGINIT;
//   LDEBUG << "PhoenixGraphHomoDepsVisitor::discover_vertex " << v << " (stop vertex is: " << m_homoDepsStopVertex << ") for g: " << &g;
}

template < typename Graph >
void PhoenixGraphHomoDepsVisitor::finish_vertex(LinguisticGraphVertex v, const Graph& g)
{
//   SAPLOGINIT;
//   LDEBUG << "PhoenixGraphHomoDepsVisitor::finish_vertex " << v << " for g: " << &g;
  if (v == m_homoDepsStartVertex)
  {
//     LDEBUG << "START finished. raising StartFinishedException" << v;
    throw StartFinishedException();
  }
}


} // closing namespace SyntacticAnalysis
} // closing namespace LinguisticProcessing
} // closing namespace Lima

#endif
