// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Tue Feb  12 2013
 * @brief      accessors for various features of a word, stored in a BoW format
 *             (same features as the WordFeatureExtractors)
 ***********************************************************************/

#ifndef BOWFEATUREEXTRACTOR_H
#define BOWFEATUREEXTRACTOR_H

#include "AnalysisDumpersExport.h"
#include "linguisticProcessing/common/PropertyCode/PropertyAccessor.h"
#include "linguisticProcessing/common/BagOfWords/bowToken.h"
#include "common/AbstractFactoryPattern/RegistrableFactory.h"

namespace Lima {
namespace LinguisticProcessing {

//----------------------------------------------------------------------
// Abstract feature extractor class
//
// BoW feature extractors are needed for generic dumper because compounds are stored in
// BoWTerm and we need compatible feature extractors on LinguisticVertex and on BoWTerm
// to have a similar output for both
class LIMA_ANALYSISDUMPERS_EXPORT AbstractBoWFeatureExtractor
{
public:
  AbstractBoWFeatureExtractor(MediaId language, const std::string& complement="");
  virtual ~AbstractBoWFeatureExtractor() {}

  virtual std::string getValue(const Common::BagOfWords::AbstractBoWElement* token) const=0;
  const std::string& getName() { return m_name; }
  void setName(const std::string& name) { m_name=name; }

protected:
  MediaId m_language;
  std::string m_name;
};

//----------------------------------------------------------------------
// Asbtract factory for abstract feature extractor
class LIMA_ANALYSISDUMPERS_EXPORT AbstractBoWFeatureExtractorFactory :
  public RegistrableFactory<AbstractBoWFeatureExtractorFactory>
{
public:
  AbstractBoWFeatureExtractorFactory(const std::string& factoryId);
  virtual AbstractBoWFeatureExtractor* create(MediaId language, const std::string& complement="")=0;
private:
};

//----------------------------------------------------------------------
// template for factories for actual instances of feature extractors
template<typename BoWFeatureExtractor>
class LIMA_ANALYSISDUMPERS_EXPORT BoWFeatureExtractorFactory :
  public AbstractBoWFeatureExtractorFactory
{
public:
  BoWFeatureExtractorFactory(const std::string& factoryId):
  AbstractBoWFeatureExtractorFactory(factoryId) {}
  AbstractBoWFeatureExtractor* create(MediaId language, const std::string& complement="") override {
    AbstractBoWFeatureExtractor* newFeature=new BoWFeatureExtractor(language,complement);
    return newFeature;
  }
private:
};

//----------------------------------------------------------------------
// a class for list of features
//----------------------------------------------------------------------
class LIMA_ANALYSISDUMPERS_EXPORT BoWFeatures : public std::vector<AbstractBoWFeatureExtractor*> {
public:
  BoWFeatures();
  BoWFeatures(MediaId language);
  ~BoWFeatures();

  void setLanguage(MediaId language) { m_language=language; }
  void initialize(const std::deque<std::string>& featureNames);
private:
  MediaId m_language;
};

//----------------------------------------------------------------------
// instances of feature extractors
//----------------------------------------------------------------------
#define BoWFeaturePosition_ID "position"
class LIMA_ANALYSISDUMPERS_EXPORT BoWFeaturePosition : public AbstractBoWFeatureExtractor {
public:
  BoWFeaturePosition(MediaId language,const std::string& complement="");
  ~BoWFeaturePosition() {}

  std::string getValue(const Common::BagOfWords::AbstractBoWElement* token) const override;
};

//----------------------------------------------------------------------
#define BoWFeatureToken_ID "word"
class LIMA_ANALYSISDUMPERS_EXPORT BoWFeatureToken : public AbstractBoWFeatureExtractor {
public:
   BoWFeatureToken(MediaId language,const std::string& complement="");
  ~BoWFeatureToken() {}

  std::string getValue(const Common::BagOfWords::AbstractBoWElement* token) const override;
};

//----------------------------------------------------------------------
#define BoWFeatureInflectedForm_ID "inflectedForm"
class LIMA_ANALYSISDUMPERS_EXPORT BoWFeatureInflectedForm : public AbstractBoWFeatureExtractor {
public:
   BoWFeatureInflectedForm(MediaId language,const std::string& complement="");
  ~BoWFeatureInflectedForm() {}

  std::string getValue(const Common::BagOfWords::AbstractBoWElement* token) const override;
};

//----------------------------------------------------------------------
#define BoWFeatureLemma_ID "lemma"
class LIMA_ANALYSISDUMPERS_EXPORT BoWFeatureLemma : public AbstractBoWFeatureExtractor {
public:
  BoWFeatureLemma(MediaId language,const std::string& complement="");
  ~BoWFeatureLemma() {}

  std::string getValue(const Common::BagOfWords::AbstractBoWElement* token) const override;
private:
};

//----------------------------------------------------------------------
#define BoWFeatureProperty_ID "property"
class LIMA_ANALYSISDUMPERS_EXPORT BoWFeatureProperty : public AbstractBoWFeatureExtractor {
public:
  BoWFeatureProperty(MediaId language, const std::string& complement="");
  ~BoWFeatureProperty() {}

  std::string getValue(const Common::BagOfWords::AbstractBoWElement* token) const override;

private:
  std::string m_propertyName;
  const Common::PropertyCode::PropertyAccessor* m_propertyAccessor;
  const Common::PropertyCode::PropertyManager* m_propertyManager;
};

//----------------------------------------------------------------------
#define BoWFeatureTstatus_ID "tstatus"
class LIMA_ANALYSISDUMPERS_EXPORT BoWFeatureTstatus : public AbstractBoWFeatureExtractor {
public:
  BoWFeatureTstatus(MediaId language,const std::string& complement="");
  ~BoWFeatureTstatus() {}

  std::string getValue(const Common::BagOfWords::AbstractBoWElement* token) const override;
};

} // end namespace
} // end namespace

#endif
