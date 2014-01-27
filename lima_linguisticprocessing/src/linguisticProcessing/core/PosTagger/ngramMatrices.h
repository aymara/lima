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
/**              Defines Bigrams and Trigrams matrices  *
  * @file        ngramMatrices.h
  * @author      Benoit Mathieu <mathieub@zoe.cea.fr>

  *              Copyright (c) 2003 by CEA
  * @date        Created on  Oct, 27 2003
  * @version     $Id$
  *
  */

#ifndef LIMA_POSTAGGER_NGRAMMATRICES_H
#define LIMA_POSTAGGER_NGRAMMATRICES_H

#include "PosTaggerExport.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "linguisticProcessing/core/LinguisticResources/AbstractResource.h"
#include <string>
#include <map>
#include <limits>

#include "linguisticProcessing/common/PropertyCode/PropertyManager.h"

#ifdef WIN32
#undef min
#undef max
#endif

namespace Lima
{
namespace LinguisticProcessing
{
namespace PosTagger
{

#define TRIGRAMMATRIX_CLASSID "TrigramMatrix"

class LIMA_POSTAGGER_EXPORT TrigramMatrix : public AbstractResource
{

public:

  TrigramMatrix() : 
      m_language(std::numeric_limits< unsigned char >::max()),
      m_trigrams() {}

  virtual ~TrigramMatrix() {}

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    AbstractResource::Manager* manager)
  ;

  void readTrigramMatrixFile(const std::string& fileName);

  bool exists(LinguisticCode cat1,LinguisticCode cat2,LinguisticCode cat3) const;

  float freq(LinguisticCode cat1,LinguisticCode cat2,LinguisticCode cat3) const;

private:
  MediaId m_language;
  const Common::PropertyCode::PropertyManager* m_microManager;
  std::map< LinguisticCode, std::map<LinguisticCode, std::map<LinguisticCode,float> > > m_trigrams;

};

#define BIGRAMMATRIX_CLASSID "BigramMatrix"

class LIMA_POSTAGGER_EXPORT BigramMatrix : public AbstractResource
{
public:

  BigramMatrix() : 
      m_language(std::numeric_limits< unsigned char >::max()),
      m_bigrams() {}

  virtual ~BigramMatrix() {}

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    AbstractResource::Manager* manager)
  ;
  
  void readBigramMatrixFile(const std::string& fileName);

  bool exists(LinguisticCode cat1,LinguisticCode cat2) const;

  float freq(LinguisticCode cat1,LinguisticCode cat2) const;

private:

  MediaId m_language;
  const Common::PropertyCode::PropertyManager* m_microManager;
  std::map< LinguisticCode, std::map<LinguisticCode, float> > m_bigrams;

};

} // closing namespace PosTagger
} // closing namespace LinguisticProcessing
} // closing namespace Lima

#endif
