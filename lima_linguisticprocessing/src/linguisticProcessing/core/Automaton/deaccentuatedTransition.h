// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
  std::shared_ptr<FlatTokenizer::CharChart> m_charchart;
};

} // end namespace
} // end namespace
} // end namespace

#endif
