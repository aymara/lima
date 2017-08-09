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
  virtual ~AbstractTextualAnalysisHandler() {};
  
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
                    Manager* manager)
  {
    LIMA_UNUSED(manager);
    try {
      m_suffix = unitConfiguration.getParamsValueAtKey("suffix");
    }
    catch (Common::XMLConfigurationFiles::NoSuchParam& e) {
      throw InvalidConfiguration(std::string("No such param 'suffix': ") + e.what());
    }
  }

  inline virtual const std::string& suffix() const {return m_suffix;}
  inline virtual void setSuffix(const std::string& suffix) {m_suffix = suffix;}
  private:
  std::string m_suffix;
};


} // Lima

#endif
