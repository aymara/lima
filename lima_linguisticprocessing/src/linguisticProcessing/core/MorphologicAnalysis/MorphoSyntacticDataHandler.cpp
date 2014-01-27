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
  MORPHOLOGINIT;
  LDEBUG << "MorphoSyntacticDataHandler::foundAccentedForm" << form;
  m_current.inflectedForm=form;
}

void MorphoSyntacticDataHandler::endAccentedForm()
{
}

void MorphoSyntacticDataHandler::foundProperties(LinguisticCode lings)
{
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
