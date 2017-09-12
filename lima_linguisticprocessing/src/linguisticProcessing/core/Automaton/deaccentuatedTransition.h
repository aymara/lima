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
/************************************************************************
 *
 * @file       deaccentuatedTransition.h
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Mon Jan  3 2005
 * @version    $Id$
 * copyright   Copyright (C) 2005-2012 by CEA LIST
 * Project     Automaton
 * 
 * @brief      
 * 
 * 
 ***********************************************************************/

#ifndef DEACCENTUATEDTRANSITION_H
#define DEACCENTUATEDTRANSITION_H

#include "AutomatonExport.h"
#include "transitionUnit.h"
//#include "linguisticProcessing/core/FlatTokenizer/CharChart.h"

namespace Lima {
namespace LinguisticProcessing {
namespace FlatTokenizer {
class  CharChart;
}
}
}

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

class LIMA_AUTOMATON_EXPORT DeaccentuatedTransition : public TransitionUnit
{
 public:
  DeaccentuatedTransition(); 
  DeaccentuatedTransition(const LimaString& deaccentuatedForm,
                          MediaId language); 
  DeaccentuatedTransition(const DeaccentuatedTransition&);
  ~DeaccentuatedTransition();
  DeaccentuatedTransition& operator = (const DeaccentuatedTransition&);
  
  // functions inherited from TransitionUnit
  DeaccentuatedTransition* clone() const override;
  DeaccentuatedTransition* create() const override;

  TypeTransition type() const override;
  
  std::string printValue() const override;
  bool operator== (const TransitionUnit&) const override;
  
  // comparison with elements of texts (tokens)
  bool compare(const LinguisticAnalysisStructure::AnalysisGraph& graph,
               const LinguisticGraphVertex& vertex,
               AnalysisContent& analysis,
               const LinguisticAnalysisStructure::Token* token,
               const LinguisticAnalysisStructure::MorphoSyntacticData* data) const override;

  // specific member functions
  const LimaString& getDeaccentuatedForm() const 
    { return m_deaccentuatedForm; }
  MediaId getLanguage() const { return m_language; }

 private:
  LimaString m_deaccentuatedForm;
  MediaId m_language;
  FlatTokenizer::CharChart* m_charchart;
};

} // end namespace
} // end namespace
} // end namespace

#endif
