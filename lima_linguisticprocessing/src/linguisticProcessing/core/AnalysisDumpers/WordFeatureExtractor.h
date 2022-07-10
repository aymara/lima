// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Mon Feb  7 2011
 * copyright   Copyright (C) 2011 by CEA LIST
 * @brief      accessors for various features of a word
 ***********************************************************************/

#ifndef WORDFEATUREEXTRACTOR_H
#define WORDFEATUREEXTRACTOR_H

#include "AnalysisDumpersExport.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/common/PropertyCode/PropertyAccessor.h"
#include "common/AbstractFactoryPattern/RegistrableFactory.h"

namespace Lima {
namespace LinguisticProcessing {

//----------------------------------------------------------------------
// Abstract feature extractor class
class LIMA_ANALYSISDUMPERS_EXPORT AbstractFeatureExtractor
{
public:
  AbstractFeatureExtractor(MediaId language, const std::string& complement="");
  virtual ~AbstractFeatureExtractor() {}

  virtual std::string
    getValue(const LinguisticAnalysisStructure::AnalysisGraph* graph,
             LinguisticGraphVertex v, AnalysisContent &analysis) const=0;
  const std::string& getName() { return m_name; }
  void setName(const std::string& name) { m_name=name; }

protected:
  MediaId m_language;
  std::string m_name;
};

//----------------------------------------------------------------------
// Asbtract factory for abstract feature extractor
class LIMA_ANALYSISDUMPERS_EXPORT AbstractFeatureExtractorFactory :
  public RegistrableFactory<AbstractFeatureExtractorFactory>
{
public:
  AbstractFeatureExtractorFactory(const std::string& factoryId);
  virtual AbstractFeatureExtractor* create(MediaId language, const std::string& complement="")=0;
private:
};

//----------------------------------------------------------------------
// template for factories for actual instances of feature extractors
template<typename FeatureExtractor>
class LIMA_ANALYSISDUMPERS_EXPORT FeatureExtractorFactory :
  public AbstractFeatureExtractorFactory
{
public:
  FeatureExtractorFactory(const std::string& factoryId):
  AbstractFeatureExtractorFactory(factoryId) {}
  AbstractFeatureExtractor* create(MediaId language, const std::string& complement="") override {
    AbstractFeatureExtractor* newFeature=new FeatureExtractor(language,complement);
    return newFeature;
  }
private:
};

//----------------------------------------------------------------------
// a class for list of features
//----------------------------------------------------------------------
class LIMA_ANALYSISDUMPERS_EXPORT WordFeatures : public std::vector<AbstractFeatureExtractor*> {
public:
  WordFeatures();
  WordFeatures(MediaId language);
  ~WordFeatures();

  void setLanguage(MediaId language) { m_language=language; }
  void initialize(const std::deque<std::string>& featureNames);
private:
  MediaId m_language;
};

//----------------------------------------------------------------------
// instances of feature extractors
//----------------------------------------------------------------------
#define FeaturePosition_ID "position"
class LIMA_ANALYSISDUMPERS_EXPORT FeaturePosition : public AbstractFeatureExtractor {
public:
  FeaturePosition(MediaId language,const std::string& complement="");
  ~FeaturePosition() {}

  std::string
    getValue(const LinguisticAnalysisStructure::AnalysisGraph* graph,
             LinguisticGraphVertex v,
             AnalysisContent & ) const override;
};

//----------------------------------------------------------------------
#define FeatureToken_ID "word"
class LIMA_ANALYSISDUMPERS_EXPORT FeatureToken : public AbstractFeatureExtractor {
public:
   FeatureToken(MediaId language,const std::string& complement="");
  ~FeatureToken() {}

  std::string
    getValue(const LinguisticAnalysisStructure::AnalysisGraph* graph,
             LinguisticGraphVertex v,
             AnalysisContent &
            ) const override;
};

//----------------------------------------------------------------------
// inflectedForm is different from word: word is the token in the text, inflectedForm is the inflected form in the dictionary
// (e.g. possible corrections on accents)
#define FeatureInflectedForm_ID "inflectedForm"
class LIMA_ANALYSISDUMPERS_EXPORT FeatureInflectedForm : public AbstractFeatureExtractor {
public:
   FeatureInflectedForm(MediaId language,const std::string& complement="");
  ~FeatureInflectedForm() {}

  std::string
    getValue(const LinguisticAnalysisStructure::AnalysisGraph* graph,
             LinguisticGraphVertex v,
             AnalysisContent &
            ) const override;
private:
  const FsaStringsPool* m_sp;
};

//----------------------------------------------------------------------
#define FeatureLemma_ID "lemma"
class LIMA_ANALYSISDUMPERS_EXPORT FeatureLemma : public AbstractFeatureExtractor {
public:
  FeatureLemma(MediaId language,const std::string& complement="");
  ~FeatureLemma() {}

  std::string
    getValue(const LinguisticAnalysisStructure::AnalysisGraph* graph,
             LinguisticGraphVertex v,
             AnalysisContent &
            ) const override;
private:
  const FsaStringsPool* m_sp;
};

//----------------------------------------------------------------------
#define FeatureProperty_ID "property"
class LIMA_ANALYSISDUMPERS_EXPORT FeatureProperty : public AbstractFeatureExtractor {
public:
  FeatureProperty(MediaId language, const std::string& complement="");
  ~FeatureProperty() {}

  std::string
    getValue(const LinguisticAnalysisStructure::AnalysisGraph* graph,
             LinguisticGraphVertex v,
             AnalysisContent &
            ) const override;
private:
  std::string m_propertyName;
  const Common::PropertyCode::PropertyAccessor* m_propertyAccessor;
  const Common::PropertyCode::PropertyManager* m_propertyManager;
};

//----------------------------------------------------------------------
#define FeatureTstatus_ID "tstatus"
class LIMA_ANALYSISDUMPERS_EXPORT FeatureTstatus : public AbstractFeatureExtractor {
public:
  FeatureTstatus(MediaId language,const std::string& complement="");
  ~FeatureTstatus() {}

  std::string
    getValue(const LinguisticAnalysisStructure::AnalysisGraph* graph,
             LinguisticGraphVertex v,
             AnalysisContent &
            ) const override;
};

//--------------------------------------------------------
#define FeatureSpecificEntity_ID "specificEntity"
class LIMA_ANALYSISDUMPERS_EXPORT FeatureSpecificEntity : public AbstractFeatureExtractor
{
 public:
  FeatureSpecificEntity(MediaId language, const std::string& complement="");
  ~FeatureSpecificEntity() {}

  std::string getValue(const LinguisticAnalysisStructure::AnalysisGraph* graph,
             LinguisticGraphVertex v,
             AnalysisContent &) const override;
};

//--------------------------------------------------------
#define FeatureLemmaSpecificEntity_ID "lemmaSpecificEntity"
class LIMA_ANALYSISDUMPERS_EXPORT FeatureLemmaSpecificEntity : public AbstractFeatureExtractor
{
public:
  FeatureLemmaSpecificEntity(MediaId language, const std::string& complement="");
  ~FeatureLemmaSpecificEntity() {}

  std::string getValue(const LinguisticAnalysisStructure::AnalysisGraph* graph,
                       LinguisticGraphVertex v,
                       AnalysisContent &) const override;
};

//--------------------------------------------------------
#define FeatureStoredData_ID "storedData"
class LIMA_ANALYSISDUMPERS_EXPORT FeatureStoredData : public AbstractFeatureExtractor
{
public:
  FeatureStoredData(MediaId language, const std::string& complement="");
  ~FeatureStoredData() {}

  std::string getValue(const LinguisticAnalysisStructure::AnalysisGraph* graph,
                       LinguisticGraphVertex v,
                       AnalysisContent &) const override;
};

} // end namespace
} // end namespace

#endif
