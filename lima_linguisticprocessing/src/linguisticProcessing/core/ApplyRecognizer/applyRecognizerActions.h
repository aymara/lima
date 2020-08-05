/*
    Copyright 2002-2020 CEA LIST

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
 * @file       applyRecognizerActions.h
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Tue Jan 25 2005
 * copyright   Copyright (C) 2005-2020 by CEA LIST
 * Project     s2lp
 *
 * @brief      this class contains actions used by ApplyRecognizer process unit
 *
 *
 ***********************************************************************/

#ifndef APPLYRECOGNIZERACTIONS_H
#define APPLYRECOGNIZERACTIONS_H

#include "ApplyRecognizerExport.h"
#include "linguisticProcessing/core/Automaton/constraintFunction.h"
#include "linguisticProcessing/core/Automaton/recognizerMatch.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"

namespace Lima {
namespace LinguisticProcessing {
namespace ApplyRecognizer {

//**********************************************************************
// ids for actions defined in this filex
#define CreateAlternativeId "CreateAlternative"
#define StoreInDataId "StoreInData"

//**********************************************************************
class LIMA_APPLYRECOGNIZER_EXPORT CreateAlternative : public Automaton::ConstraintFunction
{
public:
  CreateAlternative(MediaId language,
                    const LimaString& complement=LimaString());
  virtual ~CreateAlternative() {}
  virtual bool operator()(Automaton::RecognizerMatch& result,
                          AnalysisContent& analysis) const override;

  bool actionNeedsRecognizedExpression() override { return true; }

private:
  const Common::PropertyCode::PropertyAccessor* m_macroAccessor;
  const Common::PropertyCode::PropertyCodeManager* m_propertyCodeManager;
  FsaStringsPool* m_stringsPool;

protected:
// protected member functions

  /**
   * Creates the new token corresponding to the idiomatic expression using information from the
   * expression and from the head token of the expression.
   * @param recognizedExpression The recognized expression tokens
   */
  std::pair<LinguisticAnalysisStructure::Token*,
    LinguisticAnalysisStructure::MorphoSyntacticData*>
    createAlternativeToken(const Automaton::RecognizerMatch& recognizedExpression) const;

  bool getCompatibleProperties(const LinguisticAnalysisStructure::MorphoSyntacticData* headData,
                               const LinguisticCode& baseProperty,
                               std::set<LinguisticCode>& newProperties) const;
  bool isCompatible(const LinguisticCode& baseProperty,
                    const LinguisticCode& property,
                    LinguisticCode& newProperty) const;
  LinguisticGraphVertex
    addAlternativeVertex(LinguisticAnalysisStructure::Token*,
                         LinguisticAnalysisStructure::MorphoSyntacticData*,
                         LinguisticGraph* graph) const;
  void createBeginAlternative(LinguisticGraphVertex startVertex,
                              LinguisticGraphVertex alternativeFirstVertex,
                              LinguisticGraph& graph) const;
  void attachEndOfAlternative(LinguisticGraphVertex alternativeLastVertex,
                              LinguisticGraphVertex endVertex,
                              LinguisticGraph& graph) const;
};

//**********************************************************************
class LIMA_APPLYRECOGNIZER_EXPORT StoreInData : public Automaton::ConstraintFunction
{
public:
  StoreInData(MediaId language,
              const LimaString& complement);
  ~StoreInData() {}
  bool operator()(Automaton::RecognizerMatch& result,
                  AnalysisContent& analysis) const override;

  bool actionNeedsRecognizedExpression() override { return true; }
private:
};


} // end namespace
} // end namespace
} // end namespace

#endif
