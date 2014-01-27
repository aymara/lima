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
/** @brief       Data used for the simplification of sentences allowing easier heterosyntagmatic analysis
  *
  * @file        SimplificationData.h
  * @author      Gael de Chalendar (Gael.de-Chalendar@cea.fr) 

  *              Copyright (c) 2005 by CEA
  * @date        Created on Mar, 23 2005
  * @version     $Id$
  *
  */

#ifndef LIMA_SYNTACTICANALYSIS_SIMPLIFICATIONDATA_H
#define LIMA_SYNTACTICANALYSIS_SIMPLIFICATIONDATA_H

#include "SyntacticAnalysisExport.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "common/ProcessUnitFramework/AnalysisContent.h"

// To use tuples
#include "boost/tuple/tuple.hpp"
 //Comparison operators can be included with:
#include "boost/tuple/tuple_comparison.hpp"
// To use tuple input and output operators,
#include "boost/tuple/tuple_io.hpp"

namespace Lima {
namespace LinguisticProcessing {
namespace SyntacticAnalysis {

/**
  * @brief 
  */
class LIMA_SYNTACTICANALYSIS_EXPORT SimplificationData : public AnalysisData
{
public:

  /** @brief 
    * @param graph @b IN/OUT <I>LinguisticGraph*</I> the graph containing
    *        morphological data
    */
  explicit SimplificationData(LinguisticAnalysisStructure::AnalysisGraph* pit);

  virtual ~SimplificationData();

  inline const std::list< boost::tuple< LinguisticGraphVertex, LinguisticGraphVertex, Common::MediaticData::SyntacticRelationId > >& subSentBounds() const {return m_subSentBounds;}

  void subSentBounds(const boost::tuple<LinguisticGraphVertex, LinguisticGraphVertex, Common::MediaticData::SyntacticRelationId >& bounds);
  
  inline void clearBounds() {m_subSentBounds.clear();}
  
  inline bool simplificationDone() const {return m_simplificationDone;}
  inline void simplificationDone(bool value) {m_simplificationDone = value;}
  
  void addSimplification(LinguisticGraphEdge first, LinguisticGraphEdge last, Common::MediaticData::SyntacticRelationId type);
  
  std::list< boost::tuple< LinguisticGraphEdge, LinguisticGraphEdge, Common::MediaticData::SyntacticRelationId > >& simplifications(LinguisticGraphVertex sent);
  
  inline void sentence(LinguisticGraphVertex sent) {m_sentence = sent;}

  inline std::map< std::string, std::vector< LinguisticGraphVertex > >& coordStringsDefs() {return m_coordStringsDefs;}
  inline std::map< std::string, std::vector< std::vector< LinguisticGraphVertex > > >& coordStrings() {return  m_coordStrings;}

private:
  /** @brief  */
  LinguisticAnalysisStructure::AnalysisGraph* m_anagraph;

  std::list< boost::tuple< LinguisticGraphVertex, LinguisticGraphVertex, Common::MediaticData::SyntacticRelationId > > m_subSentBounds;

  /** true if a simplification has been done ; 
    * reinitialized to false before each subsentence search */
  bool m_simplificationDone;
  
  std::map< LinguisticGraphVertex,
    std::list< boost::tuple< LinguisticGraphEdge, LinguisticGraphEdge, Common::MediaticData::SyntacticRelationId > > > m_simplifications;
  
  LinguisticGraphVertex m_sentence;

  std::map< std::string, std::vector< LinguisticGraphVertex > > m_coordStringsDefs;
  std::map< std::string, std::vector< std::vector< LinguisticGraphVertex > > > m_coordStrings;
};

} // closing namespace SyntacticAnalysis
} // closing namespace LinguisticProcessing
} // closing namespace Lima

#endif // LIMA_SYNTACTICANALYSIS_SIMPLIFICATIONDATA_H
