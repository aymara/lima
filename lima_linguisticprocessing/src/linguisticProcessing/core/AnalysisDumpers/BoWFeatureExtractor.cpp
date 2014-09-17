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

/************************************************************************
 *
 * @file       BoWFeatures.cpp
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Tue Feb  12 2013
 * copyright   Copyright (C) 2011 by CEA LIST
 * 
 ***********************************************************************/

#include "BoWFeatureExtractor.h"
#include "linguisticProcessing/common/linguisticData/languageData.h"
#include "linguisticProcessing/common/PropertyCode/PropertyCodeManager.h"
#include "common/MediaticData/mediaticData.h"
#include "common/Data/strwstrtools.h"
#include <sstream>

using namespace std;
using namespace boost;
using namespace Lima::Common;
using namespace Lima::Common::BagOfWords;

namespace Lima {
namespace LinguisticProcessing {

//***********************************************************************
// factories for feature extractors
AbstractBoWFeatureExtractor::AbstractBoWFeatureExtractor(MediaId language, const std::string& /*complement*/):
m_language(language)
{
}

AbstractBoWFeatureExtractorFactory::
AbstractBoWFeatureExtractorFactory(const std::string& factoryId):
RegistrableFactory<AbstractBoWFeatureExtractorFactory>(factoryId)
{
}

BoWFeatureExtractorFactory<BoWFeaturePosition> BoWFeaturePositionFactory(BoWFeaturePosition_ID);
BoWFeatureExtractorFactory<BoWFeatureToken> BoWFeatureTokenFactory(BoWFeatureToken_ID);
BoWFeatureExtractorFactory<BoWFeatureLemma> BoWFeatureLemmaFactory(BoWFeatureLemma_ID);
BoWFeatureExtractorFactory<BoWFeatureProperty> BoWFeaturePropertyFactory(BoWFeatureProperty_ID);
BoWFeatureExtractorFactory<BoWFeatureTstatus> BoWFeatureTstatusFactory(BoWFeatureTstatus_ID);

//***********************************************************************
// BoWFeature list
//***********************************************************************
BoWFeatures::BoWFeatures() : vector<AbstractBoWFeatureExtractor*>(),
m_language(0)
{
}

BoWFeatures::BoWFeatures(MediaId language) : vector<AbstractBoWFeatureExtractor*>(),
m_language(language)
{
}

BoWFeatures::~BoWFeatures()
{
  for (BoWFeatures::iterator it=begin(),it_end=end(); it!=it_end; it++) {
    if (*it) {
      delete (*it);
      *it=0;
    }
  }
}

void BoWFeatures::initialize(const deque<std::string>& featureNames)
{
  for (deque<std::string>::const_iterator it=featureNames.begin(),
         it_end=featureNames.end();it!=it_end;it++) 
  {
    string featureName=(*it);
    string complement;
    string::size_type i=featureName.find(":");
    if (i!=string::npos) {
      complement=string(featureName,i+1);
      featureName=string(featureName,0,i);
    }
    DUMPERLOGINIT;
    LDEBUG << "BoWFeatures: initialize feature" << featureName << LENDL;
    push_back(BoWFeatureLemmaFactory.getFactory(featureName)->create(m_language,complement));
    back()->setName(featureName);
  }
}

//***********************************************************************
// BoWFeature accessors
//***********************************************************************
BoWFeaturePosition::BoWFeaturePosition(MediaId language, const std::string& complement):
AbstractBoWFeatureExtractor(language,complement)
{}

std::string BoWFeaturePosition::
getValue(const Common::BagOfWords::BoWToken* token) const
{
  ostringstream oss;
  oss << token->getPosition();
  return oss.str();
}

//***********************************************************************
BoWFeatureToken::BoWFeatureToken(MediaId language, const std::string& complement):
AbstractBoWFeatureExtractor(language,complement)
{}

std::string BoWFeatureToken::
getValue(const BagOfWords::BoWToken* token) const
{
  return Common::Misc::limastring2utf8stdstring(token->getInflectedForm());
}

//***********************************************************************
BoWFeatureLemma::BoWFeatureLemma(MediaId language, const std::string& complement):
AbstractBoWFeatureExtractor(language,complement)
{
}

std::string BoWFeatureLemma::
getValue(const BagOfWords::BoWToken* token) const
{
  return Common::Misc::limastring2utf8stdstring(token->getLemma());
}

//***********************************************************************
BoWFeatureProperty::BoWFeatureProperty(MediaId language,const std::string& complement):
AbstractBoWFeatureExtractor(language),
m_propertyName(complement),
m_propertyAccessor(0),
m_propertyManager(0)
{
  const Common::PropertyCode::PropertyCodeManager& codeManager=
  static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager();
  // for BoW, only property available is MACRO
  m_propertyAccessor=&codeManager.getPropertyAccessor("MACRO");
  m_propertyManager=&codeManager.getPropertyManager("MACRO");
}

std::string BoWFeatureProperty::
getValue(const BagOfWords::BoWToken* token) const
{
  ostringstream oss;
  oss << m_propertyManager->getPropertySymbolicValue(token->getCategory());
  return oss.str();
}

//***********************************************************************
BoWFeatureTstatus::BoWFeatureTstatus(MediaId language, const std::string& complement):
AbstractBoWFeatureExtractor(language,complement)
{}

std::string BoWFeatureTstatus::
getValue(const BagOfWords::BoWToken* /*token*/) const
{
  return "";
}

} // end namespace
} // end namespace
