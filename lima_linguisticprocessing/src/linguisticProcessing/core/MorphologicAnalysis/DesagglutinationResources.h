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
