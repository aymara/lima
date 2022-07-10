// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
BoWFeatureExtractorFactory<BoWFeatureInflectedForm> BoWFeatureInflectedormFactory(BoWFeatureInflectedForm_ID);
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
  DUMPERLOGINIT;
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
    try {
      LDEBUG << "BoWFeatures: initialize feature" << featureName;
      push_back(BoWFeatureLemmaFactory.getFactory(featureName)->create(m_language,complement));
      back()->setName(featureName);
    }
    catch (LimaException& e) {
      LERROR << "BoWFeatures: feature" << featureName << "is not defined";
    }
  }
}

//***********************************************************************
// BoWFeature accessors
//***********************************************************************
BoWFeaturePosition::BoWFeaturePosition(MediaId language, const std::string& complement):
AbstractBoWFeatureExtractor(language,complement)
{}

std::string BoWFeaturePosition::
getValue(const AbstractBoWElement* token) const
{
  ostringstream oss;
  if  (dynamic_cast<const BoWToken*>(token) != 0)
    oss << dynamic_cast<const BoWToken*>(token)->getPosition();
  return oss.str();
}

//***********************************************************************
BoWFeatureToken::BoWFeatureToken(MediaId language, const std::string& complement):
AbstractBoWFeatureExtractor(language,complement)
{}

std::string BoWFeatureToken::
getValue(const AbstractBoWElement* token) const
{
  if  (dynamic_cast<const BoWToken*>(token) != 0)
    return Common::Misc::limastring2utf8stdstring(dynamic_cast<const BoWToken*>(token)->getInflectedForm());
  else
    return token->getIdUTF8String();
}

//***********************************************************************
BoWFeatureInflectedForm::BoWFeatureInflectedForm(MediaId language, const std::string& complement):
AbstractBoWFeatureExtractor(language,complement)
{}

std::string BoWFeatureInflectedForm::
getValue(const AbstractBoWElement* token) const
{
  // return the same as word (in WordFeatureExtractor, return the inflected form 
  // from the dictionary, as opposed to the form in the text, but this difference is not stored
  // in the BoWToken)
  if  (dynamic_cast<const BoWToken*>(token) != 0)
    return Common::Misc::limastring2utf8stdstring(dynamic_cast<const BoWToken*>(token)->getInflectedForm());
  else
    return token->getIdUTF8String();
  
}

//***********************************************************************
BoWFeatureLemma::BoWFeatureLemma(MediaId language, const std::string& complement):
AbstractBoWFeatureExtractor(language,complement)
{
}

std::string BoWFeatureLemma::
getValue(const AbstractBoWElement* token) const
{
  if  (dynamic_cast<const BoWToken*>(token) != 0)
    return Common::Misc::limastring2utf8stdstring(dynamic_cast<const BoWToken*>(token)->getLemma());
  else
    return token->getIdUTF8String();
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
getValue(const AbstractBoWElement* token) const
{
  ostringstream oss;
  if  (dynamic_cast<const BoWToken*>(token) != 0)
    oss << dynamic_cast<const BoWToken*>(token)->getCategory();
  return oss.str();
}

//***********************************************************************
BoWFeatureTstatus::BoWFeatureTstatus(MediaId language, const std::string& complement):
AbstractBoWFeatureExtractor(language,complement)
{}

std::string BoWFeatureTstatus::
getValue(const AbstractBoWElement* token) const
{
  LIMA_UNUSED(token);
  return "";
}

} // end namespace
} // end namespace
