// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       Features.cpp
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Mon Feb  7 2011
 * copyright   Copyright (C) 2011 by CEA LIST
 * 
 ***********************************************************************/

#include "WordFeatureExtractor.h"
#include "linguisticProcessing/common/linguisticData/languageData.h"
#include "common/Data/strwstrtools.h"
#include <sstream>

#include "common/MediaticData/mediaticData.h"
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"
#include "linguisticProcessing/core/Automaton/SpecificEntityAnnotation.h"

using namespace std;
using namespace boost;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

using namespace Lima::Common::MediaticData;
using namespace Lima::LinguisticProcessing::SpecificEntities;

namespace Lima {
namespace LinguisticProcessing {

//***********************************************************************
// factories for feature extractors
AbstractFeatureExtractor::AbstractFeatureExtractor(MediaId language, const std::string& /*complement*/):
m_language(language)
{
}

AbstractFeatureExtractorFactory::
AbstractFeatureExtractorFactory(const std::string& factoryId):
RegistrableFactory<AbstractFeatureExtractorFactory>(factoryId)
{
}

FeatureExtractorFactory<FeaturePosition> FeaturePositionFactory(FeaturePosition_ID);
FeatureExtractorFactory<FeatureToken> FeatureTokenFactory(FeatureToken_ID);
FeatureExtractorFactory<FeatureInflectedForm> FeatureInflectedFormFactory(FeatureInflectedForm_ID);
FeatureExtractorFactory<FeatureLemma> FeatureLemmaFactory(FeatureLemma_ID);
FeatureExtractorFactory<FeatureProperty> FeaturePropertyFactory(FeatureProperty_ID);
FeatureExtractorFactory<FeatureTstatus> FeatureTstatusFactory(FeatureTstatus_ID);
FeatureExtractorFactory<FeatureSpecificEntity> FeatureSpecificEntityFactory(FeatureSpecificEntity_ID);
FeatureExtractorFactory<FeatureLemmaSpecificEntity> FeatureLemmaSpecificEntityFactory(FeatureLemmaSpecificEntity_ID);
FeatureExtractorFactory<FeatureStoredData> FeatureStoredDataFactory(FeatureStoredData_ID);

//***********************************************************************
// Feature list
//***********************************************************************
WordFeatures::WordFeatures() : vector<AbstractFeatureExtractor*>(),
m_language(0)
{
}

WordFeatures::WordFeatures(MediaId language) : vector<AbstractFeatureExtractor*>(),
m_language(language)
{
}

WordFeatures::~WordFeatures()
{
//   for (WordFeatures::iterator it=begin(),it_end=end(); it!=it_end; it++) {
//     if (*it) {
//       delete (*it);
//       *it=0;
//     }
//   }
}

void WordFeatures::initialize(const deque<std::string>& featureNames)
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
      LDEBUG << "WordFeatures: initialize feature" << featureName;
      push_back(FeatureLemmaFactory.getFactory(featureName)->create(m_language,complement));
      back()->setName(featureName);
    }
    catch (LimaException& e) {
      LERROR << "WordFeatures: feature" << featureName << "is not defined";
    }

  }
}

//***********************************************************************
// Feature accessors
//***********************************************************************
FeaturePosition::FeaturePosition(MediaId language, const std::string& complement):
AbstractFeatureExtractor(language,complement)
{}

std::string FeaturePosition::
getValue(const LinguisticAnalysisStructure::AnalysisGraph* graph,
         LinguisticGraphVertex v,
         AnalysisContent & /*unused*/) const
{
  Token* token=get(vertex_token,*(graph->getGraph()),v);
  if (token==0) {
    return "";
  }
  ostringstream oss;
  oss << token->position();
  return oss.str();
}

//***********************************************************************
FeatureToken::FeatureToken(MediaId language, const std::string& complement):
AbstractFeatureExtractor(language,complement)
{}

std::string FeatureToken::
getValue(const LinguisticAnalysisStructure::AnalysisGraph* graph,
         LinguisticGraphVertex v,
         AnalysisContent & /*unused*/
        ) const
{
  Token* token=get(vertex_token,*(graph->getGraph()),v);
  if (token==0) {
    return "";
  }
  return Common::Misc::limastring2utf8stdstring(token->stringForm());
}

//***********************************************************************
FeatureInflectedForm::FeatureInflectedForm(MediaId language, const std::string& complement):
AbstractFeatureExtractor(language,complement),
m_sp()
{
    m_sp=&(Common::MediaticData::MediaticData::single().stringsPool(m_language));
}

std::string FeatureInflectedForm::
getValue(const LinguisticAnalysisStructure::AnalysisGraph* graph,
         LinguisticGraphVertex v,
         AnalysisContent & /*unused*/
        ) const
{
  MorphoSyntacticData* data=get(vertex_data,*(graph->getGraph()),v);
  if (data==0) {
    return "";
  }
  // take first
  for (MorphoSyntacticData::const_iterator it=data->begin(),it_end=data->end();it!=it_end;it++) {
    return Common::Misc::limastring2utf8stdstring((*m_sp)[(*it).inflectedForm]);
  }
  return "";
}

//***********************************************************************
FeatureLemma::FeatureLemma(MediaId language, const std::string& complement):
AbstractFeatureExtractor(language,complement),
m_sp()
{
  m_sp=&(Common::MediaticData::MediaticData::single().stringsPool(m_language));
}

std::string FeatureLemma::
getValue(const LinguisticAnalysisStructure::AnalysisGraph* graph,
         LinguisticGraphVertex v,
         AnalysisContent & /*unused*/
        ) const
{
  MorphoSyntacticData* data=get(vertex_data,*(graph->getGraph()),v);
  if (data==0) {
    return "";
  }
  // take first
  for (MorphoSyntacticData::const_iterator it=data->begin(),it_end=data->end();it!=it_end;it++) {
    return Common::Misc::limastring2utf8stdstring((*m_sp)[(*it).normalizedForm]);
  }
  return "";
}

//***********************************************************************
FeatureProperty::FeatureProperty(MediaId language,const std::string& complement):
AbstractFeatureExtractor(language),
m_propertyName(complement),
m_propertyAccessor(0),
m_propertyManager(0)
{
  const Common::PropertyCode::PropertyCodeManager& codeManager=
  static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager();
  m_propertyAccessor=&codeManager.getPropertyAccessor(m_propertyName);
  m_propertyManager=&codeManager.getPropertyManager(m_propertyName);
}

std::string FeatureProperty::
getValue(const LinguisticAnalysisStructure::AnalysisGraph* graph,
         LinguisticGraphVertex v, 
         AnalysisContent & /*unused*/) const
{
  MorphoSyntacticData* data=get(vertex_data,*(graph->getGraph()),v);
  if (data==0) {
    return "";
  }
  ostringstream oss;
  // take first / take all ?
  for (MorphoSyntacticData::const_iterator it=data->begin(),it_end=data->end();it!=it_end;it++) {
    LinguisticCode code=m_propertyAccessor->readValue((*it).properties);
    oss << m_propertyManager->getPropertySymbolicValue(code);
    break;
  }
  return oss.str();
}

//***********************************************************************
FeatureTstatus::FeatureTstatus(MediaId language, const std::string& complement):
AbstractFeatureExtractor(language,complement)
{}

std::string FeatureTstatus::
getValue(const LinguisticAnalysisStructure::AnalysisGraph* graph,
         LinguisticGraphVertex v,
         AnalysisContent & /*unused*/) const
{
  Token* token=get(vertex_token,*(graph->getGraph()),v);
  if (token==0) {
    return "";
  }
  return Common::Misc::limastring2utf8stdstring(token->status().defaultKey());
}


//***********************************************************************
FeatureSpecificEntity::FeatureSpecificEntity(MediaId language, const std::string& complement):
AbstractFeatureExtractor(language,complement)
{
}

std::string FeatureSpecificEntity::
getValue(const LinguisticAnalysisStructure::AnalysisGraph* graph, 
         LinguisticGraphVertex v,
         AnalysisContent &analysis
        ) const
{
  std::string typeName("NAN");
  Common::AnnotationGraphs::AnnotationData *annot = static_cast<  Common::AnnotationGraphs::AnnotationData* >(analysis.getData("AnnotationData"));
  
  std::set< AnnotationGraphVertex > matches = annot->matches(graph->getGraphId(),v,"annot"); 
  for (std::set< AnnotationGraphVertex >::const_iterator it = matches.begin(); it != matches.end(); it++)
  {
    if (annot->hasAnnotation(*it, Common::Misc::utf8stdstring2limastring("SpecificEntity")))
    {
      AnnotationGraphVertex vx=*it;
      const SpecificEntityAnnotation* se = annot->annotation(vx, Common::Misc::utf8stdstring2limastring("SpecificEntity")).
      pointerValue<SpecificEntityAnnotation>();
      
      LimaString str= Common::MediaticData::MediaticData::single().getEntityName(se->getType());
      typeName=Common::Misc::limastring2utf8stdstring(str);
    }
  }
  return typeName;
}

//***********************************************************************
FeatureLemmaSpecificEntity::FeatureLemmaSpecificEntity(MediaId language, const std::string& complement):
AbstractFeatureExtractor(language,complement)
{
}

std::string FeatureLemmaSpecificEntity::
getValue(const LinguisticAnalysisStructure::AnalysisGraph* graph, 
         LinguisticGraphVertex v,
         AnalysisContent &analysis
) const
{
  std::string mxvalue("NAN");
  Common::AnnotationGraphs::AnnotationData *annot = static_cast<  Common::AnnotationGraphs::AnnotationData* >(analysis.getData("AnnotationData"));
  
  std::set< AnnotationGraphVertex > matches = annot->matches(graph->getGraphId(),v,"annot"); 
  for (std::set< AnnotationGraphVertex >::const_iterator it = matches.begin(); it != matches.end(); it++)
  {
    if (annot->hasAnnotation(*it, Common::Misc::utf8stdstring2limastring("SpecificEntity")))
    {
      AnnotationGraphVertex vx=*it;
      const SpecificEntityAnnotation* se = annot->annotation(vx, Common::Misc::utf8stdstring2limastring("SpecificEntity")).
      pointerValue<SpecificEntityAnnotation>();
      
      LimaString str= Common::MediaticData::MediaticData::single().getEntityName(se->getType());
      mxvalue=Common::Misc::limastring2utf8stdstring(str);
    }
  }
  // replace NAN values by lemmas
  if (mxvalue == "NAN") {
    MorphoSyntacticData* data=get(vertex_data,*(graph->getGraph()),v);
    // take first
    for (MorphoSyntacticData::const_iterator it=data->begin(),it_end=data->end();it!=it_end;it++) {
      mxvalue = Common::Misc::limastring2utf8stdstring((*&(Common::MediaticData::MediaticData::single().stringsPool(m_language)))[(*it).normalizedForm]);
      break;
    }
  }
  // replace empty lemma values by tokens
  if (mxvalue == "" ) {
    Token* token=get(vertex_token,*(graph->getGraph()),v);
    mxvalue = Common::Misc::limastring2utf8stdstring(token->stringForm());
  }
  
  return mxvalue;
}

//***********************************************************************
FeatureStoredData::FeatureStoredData(MediaId language, const std::string& complement):
AbstractFeatureExtractor(language,complement)
{
}

std::string FeatureStoredData::
getValue(const LinguisticAnalysisStructure::AnalysisGraph* graph, 
         LinguisticGraphVertex v,
         AnalysisContent &analysis) const
{
  LIMA_UNUSED(analysis)
  Token* token=get(vertex_token,*(graph->getGraph()),v);
  if (token==0) {
    return "";
  }
  ostringstream oss;
  oss << token->position() ;
  return oss.str();
}

} // end namespace
} // end namespace
