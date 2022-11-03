// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                              *
 *                                                                         *
 ***************************************************************************/
#include "MorphoSyntacticDataHandler.h"

using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

namespace Lima
{

namespace LinguisticProcessing
{

namespace MorphologicAnalysis
{

MorphoSyntacticDataHandler::MorphoSyntacticDataHandler(
  LinguisticAnalysisStructure::MorphoSyntacticData& output,
  MorphoSyntacticType type) :
    m_output(output),
    m_current(),
    m_inConcat(false)
{
  m_current.type=type;
}

MorphoSyntacticDataHandler::~MorphoSyntacticDataHandler() {}

void MorphoSyntacticDataHandler::startEntry(StringsPoolIndex form)
{
  m_current.inflectedForm=form;
}

void MorphoSyntacticDataHandler::endEntry()
{}

void MorphoSyntacticDataHandler::foundLingInfos(StringsPoolIndex lemma,StringsPoolIndex norm)
{
  m_current.lemma=lemma;
  m_current.normalizedForm=norm;
}

void MorphoSyntacticDataHandler::endLingInfos()
{}

void MorphoSyntacticDataHandler::foundAccentedForm(StringsPoolIndex form)
{
#ifdef DEBUG_LP
  MORPHOLOGINIT;
  LDEBUG << "MorphoSyntacticDataHandler::foundAccentedForm" << form;
#endif
  m_current.inflectedForm=form;
}

void MorphoSyntacticDataHandler::endAccentedForm()
{
}

void MorphoSyntacticDataHandler::foundProperties(LinguisticCode lings)
{
#ifdef DEBUG_LP
  MORPHOLOGINIT;
  LDEBUG << "MorphoSyntacticDataHandler::foundProperties" << lings
          << "("<<m_inConcat<<")";
#endif
  if (!m_inConcat) {
    m_current.properties=lings;
    m_output.push_back(m_current);
  }
}

void MorphoSyntacticDataHandler::foundConcatenated()
{
  m_inConcat=true;
}

void MorphoSyntacticDataHandler::endConcatenated()
{
  m_inConcat=false;
}



}

}

}
