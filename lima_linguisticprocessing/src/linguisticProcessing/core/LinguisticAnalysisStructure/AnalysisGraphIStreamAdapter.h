// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2010 by CEA - LIST                                      *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_LINGUISTICPROCESSING_LINGUISTICANALYSISSTRUCTUREANALYSISGRAPHISTREAMADAPTER_H
#define LIMA_LINGUISTICPROCESSING_LINGUISTICANALYSISSTRUCTUREANALYSISGRAPHISTREAMADAPTER_H


#include "AnalysisGraph.h"

namespace Lima
{

namespace LinguisticProcessing
{

namespace LinguisticAnalysisStructure
{

/**
@author Benoit Mathieu
*/
class AnalysisGraphIStreamAdapter : public std::istream
{
public:
  AnalysisGraphIStreamAdapter(const AnalysisGraph* graph);

    virtual ~AnalysisGraphIStreamAdapter();

private:

  /** @brief A pointer towards the linguistic graph */
  AnalysisGraph* m_graph;

  MediaId m_language;
};

}

}

}

#endif
