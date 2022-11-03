// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/** @brief      Declaration of DesagglutinationResources
  *
  * @file       DesagglutinationResources.h
  * @author     Benoit Mathieu <mathieub@zoe.cea.fr>
  * @date       Created on January 2004
  * @version    $Id$
  *
  */

#ifndef LIMA_MORPHOLOGICANALYSIS_DESAGGLUTINATIONRESOURCES_H
#define LIMA_MORPHOLOGICANALYSIS_DESAGGLUTINATIONRESOURCES_H

#include "MorphologicAnalysisExport.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "linguisticProcessing/core/LinguisticResources/AbstractResource.h"
#include "common/Data/LimaString.h"
#include "common/Data/strwstrtools.h"

#include <map>
#include <set>

namespace Lima
{
namespace LinguisticProcessing
{
namespace MorphologicAnalysis
{
namespace Desagglutination
{

struct CompoundCategories
{
  LinguisticCode begin;
  LinguisticCode part;
  LinguisticCode end;
};

typedef std::set<Lima::LimaString> Delimiters;

typedef std::map<LinguisticCode,CompoundCategories> CategoriesMapping;

#define DESAGGLUTINATIONRESOURCES_CLASSID "DesagglutinationResources"

class LIMA_MORPHOLOGICANALYSIS_EXPORT DesagglutinationResources : public AbstractResource
{
  Q_OBJECT

public:
  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager)
  ;
  
  const CategoriesMapping& getCategoriesMapping() const;
  const Delimiters& getDelimiters() const;
  
private:

  MediaId m_language;
  CategoriesMapping m_categoriesMapping;
  Delimiters m_delimiters;  
  
  void loadMicroCategoriesMappingFromFile(const std::string& microcategoriesMappingFile);
  void loadDelimitersFromFile(const std::string& delimitersFile);
  
};

inline const CategoriesMapping& DesagglutinationResources::getCategoriesMapping() const
{
  return m_categoriesMapping;
}

inline const Delimiters& DesagglutinationResources::getDelimiters() const
{
  return m_delimiters;
}



} // Desagglutination
} // MorphologicAnalysis
} // LinguisticProcessing
} // Lima

#endif
