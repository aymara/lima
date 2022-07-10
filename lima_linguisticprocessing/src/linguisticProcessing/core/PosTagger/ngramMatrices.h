// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
  Q_OBJECT

public:

  TrigramMatrix() : 
      m_language(std::numeric_limits< unsigned char >::max()),
      m_trigrams() {}

  virtual ~TrigramMatrix() {}

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    AbstractResource::Manager* manager) override;

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
  Q_OBJECT
public:

  BigramMatrix() : 
      m_language(std::numeric_limits< unsigned char >::max()),
      m_bigrams() {}

  virtual ~BigramMatrix() {}

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    AbstractResource::Manager* manager) override;
  
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
