// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/**
 *   Copyright (C) 2007-2012 by CEA LIST
 *
 * @file        .h
 * @author      Claire Mouton (Claire.Mouton@cea.fr)

 *              Copyright (c) 2007 by CEA
 * @date        Created on Apr, 24 2007
 */
#ifndef ABSTRACTTEXTUALANALYSISHANDLER_H
#define ABSTRACTTEXTUALANALYSISHANDLER_H

#include <sstream>
#include "AnalysisHandlersExport.h"
#include "common/ProcessUnitFramework/AnalysisContent.h"
#include "common/Handler/AbstractXmlAnalysisHandler.h"
#include "common/Data/genericDocumentProperties.h"
#include "linguisticProcessing/core/LinguisticResources/AbstractResource.h"

namespace Lima{

//! @brief defines callback interface
class LIMA_ANALYSISHANDLERS_EXPORT AbstractTextualAnalysisHandler :
    public LinguisticProcessing::AbstractResource,
    public AbstractXmlAnalysisHandler
{
  Q_OBJECT
public:
   AbstractTextualAnalysisHandler():
    LinguisticProcessing::AbstractResource(),
    AbstractXmlAnalysisHandler(std::shared_ptr<const ShiftFrom>()) {}
  virtual ~AbstractTextualAnalysisHandler();

  /**
  * @brief initialize with parameters from configuration file.
  * @param unitConfiguration @IN : <group> tag in xml configuration file that
  *        contains parameters to initialize the object.
  * @param manager @IN : manager that asked for initialization and carries init params
  * Use it to initialize other objects of same kind.
  * @throw InvalidConfiguration when parameters are invalids.
  */
  virtual void init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
                    Manager* manager) override;

  inline virtual const std::string& suffix() const {return m_suffix;}
  inline virtual void setSuffix(const std::string& suffix) {m_suffix = suffix;}
  private:
  std::string m_suffix;
};


} // Lima

#endif
