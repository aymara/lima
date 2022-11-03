// Copyright 2002-2020 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       basicConstraintFunctions.cpp
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Wed Mar 16 2005
 * copyright   Copyright (C) 2005-2020 by CEA LIST
 *
 ***********************************************************************/

#include "basicConstraintFunctions.h"
#include "constraintFunctionFactory.h"
#include "common/Data/strwstrtools.h"
#include "common/MediaticData/mediaticData.h"
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"

#include <algorithm>

using namespace std;
using namespace Lima::Common::MediaticData;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

namespace Lima
{
namespace LinguisticProcessing
{
namespace Automaton
{

//**********************************************************************
// factories for constraints defined in this file
ConstraintFunctionFactory<AgreementConstraint>
AgreementConstraintFactory(AgreementConstraintId);

ConstraintFunctionFactory<GenderAgreement>
GenderAgreementFactory(GenderAgreementId);

ConstraintFunctionFactory<NumberAgreement>
NumberAgreementFactory(NumberAgreementId);

ConstraintFunctionFactory<LinguisticPropertyIs>
LinguisticPropertyIsFactory(LinguisticPropertyIsId);

ConstraintFunctionFactory<LengthInInterval>
LengthInIntervalFactory(LengthInIntervalId);

ConstraintFunctionFactory<NumericValueInInterval>
NumericValueInIntervalFactory(NumericValueInIntervalId);

ConstraintFunctionFactory<NoSpaceWith>
NoSpaceWithFactory(NoSpaceWithId);

//**********************************************************************
class CheckIfEmptyPredicate
{
public:
  CheckIfEmptyPredicate(const Common::PropertyCode::PropertyAccessor& propAcc) : m_acc(propAcc) {}
  bool operator()(const LinguisticCode& code) { return m_acc.empty(code); }
private:
  const Common::PropertyCode::PropertyAccessor& m_acc;
};


AgreementConstraint::
AgreementConstraint(MediaId language,
                    const LimaString& complement):
    ConstraintFunction(language,complement),
    m_categoryForAgreementAccessor(0)
{
  if (complement == Common::Misc::utf8stdstring2limastring("PERSON"))
  {
    m_categoryForAgreementAccessor=
      &(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager().getPropertyAccessor("PERSON"));
  }
  else if (complement == Common::Misc::utf8stdstring2limastring("GENDER"))
  {
    m_categoryForAgreementAccessor=
      &(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager().getPropertyAccessor("GENDER"));
  }
  else if (complement == Common::Misc::utf8stdstring2limastring("NUMBER"))
  {
    m_categoryForAgreementAccessor=
      &(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager().getPropertyAccessor("NUMBER"));
  }
}

bool AgreementConstraint::
operator()(const AnalysisGraph& anagraph,
           const LinguisticGraphVertex& vertex1,
           const LinguisticGraphVertex& vertex2,
           AnalysisContent& /*ac*/ ) const
{
  const LinguisticGraph& graph = *(anagraph.getGraph());

  if (vertex1 == anagraph.firstVertex() ||
      vertex1 == anagraph.firstVertex() ||
      vertex2 == anagraph.lastVertex()  ||
      vertex2 == anagraph.lastVertex() )
  {
    return false;
  }

  // compare the categories of the two vertices
  MorphoSyntacticData* data1=get(vertex_data,graph,vertex1);
  MorphoSyntacticData* data2=get(vertex_data,graph,vertex2);

  MorphoSyntacticData::const_iterator
  it1=data1->begin(),it1_end=data1->end();
  for (; it1!=it1_end; it1++)
  {
    MorphoSyntacticData::const_iterator
    it2=data2->begin(),it2_end=data2->end();
    for (; it2!=it2_end; it2++)
    {
      if (m_categoryForAgreementAccessor->empty((*it1).properties)
          || m_categoryForAgreementAccessor->empty((*it2).properties)
          || m_categoryForAgreementAccessor->equal((*it1).properties,
              (*it2).properties))
      {
        return true;
      }
    }
  }
  return false;
}

//**********************************************************************
GenderAgreement::
GenderAgreement(MediaId language,
                const LimaString& /* unused complement*/ ):
AgreementConstraint(language,Common::Misc::utf8stdstring2limastring("GENDER")) {}

NumberAgreement::
NumberAgreement(MediaId language,
                const LimaString& /* unused complement */):
AgreementConstraint(language,Common::Misc::utf8stdstring2limastring("NUMBER")) {}

//**********************************************************************
LinguisticPropertyIs::
LinguisticPropertyIs(MediaId language,
                     const LimaString& complement):
    ConstraintFunction(language,complement),
    m_propertyAccessor(0),
    m_values(0)
{
  //AULOGINIT;
  //complement contains the name of the property, the value
  //to test and the language, separated by a comma
  std::string str=Common::Misc::limastring2utf8stdstring(complement);
  //LDEBUG << "init constraint LinguisticPropertyIs with complement " << str;

  auto j(string::npos),k(string::npos);
  auto i=str.find(",");
  if (i!=string::npos)
  {
    j=str.find(",",i+1);
  }

  if (i==string::npos || j==string::npos)
  {
    AULOGINIT;
    LIMA_EXCEPTION( "Constraint LinguisticPropertyIs : invalid complement \""
                    << str.c_str() << "\": three arguments needed");
  }

  string propertyString(str,0,i);
  string valueString(str,i+1,j-i-1);
  string lang(str,j+1);

  const auto& manager = static_cast<const Common::MediaticData::LanguageData&>(
    Common::MediaticData::MediaticData::single().mediaData(lang))
      .getPropertyCodeManager().getPropertyManager(propertyString);

  m_propertyAccessor=&(manager.getPropertyAccessor());
  i=0;
  j=valueString.find("|");
  if (j==std::string::npos) j=valueString.size();
  for (;i<valueString.size();)
  {
    //LDEBUG << "read part " << valueString.substr(i,j-i);
    pair<LinguisticCode,LinguisticProcessing::LinguisticAnalysisStructure::MorphoSyntacticType> value;
    k=valueString.find("#",i);
    if (k!=string::npos && k<j)
    {
      //LDEBUG << "found # : " << valueString.substr(i,k-i) << " # " << valueString.substr(k+1,j-k-1);
      value.first=manager.getPropertyValue(valueString.substr(i,k-i));
      std::string mtype=valueString.substr(k+1,j-k-1);
      if (mtype == "SIMPLE_WORD") { value.second=SIMPLE_WORD; }
      else if (mtype == "ABBREV_ALTERNATIVE") { value.second=ABBREV_ALTERNATIVE; }
      else if (mtype == "HYPHEN_ALTERNATIVE") { value.second=HYPHEN_ALTERNATIVE; }
      else if (mtype == "IDIOMATIC_EXPRESSION") { value.second=IDIOMATIC_EXPRESSION; }
      else if (mtype == "CONCATENATED_ALTERNATIVE") { value.second=CONCATENATED_ALTERNATIVE; }
      else if (mtype == "HYPERWORD_ALTERNATIVE") { value.second=HYPERWORD_ALTERNATIVE; }
      else if (mtype == "UNKNOWN_WORD") { value.second=UNKNOWN_WORD; }
      else if (mtype == "CAPITALFIRST_WORD") { value.second=CAPITALFIRST_WORD; }
      else if (mtype == "AGGLUTINATED_WORD") { value.second=AGGLUTINATED_WORD; }
      else if (mtype == "DESAGGLUTINATED_WORD") { value.second=DESAGGLUTINATED_WORD; }
      else if (mtype == "CHINESE_SEGMENTER") { value.second=CHINESE_SEGMENTER; }
      else if (mtype == "SPECIFIC_ENTITY") { value.second=SPECIFIC_ENTITY; }
      else if (mtype == "SPELLING_ALTERNATIVE") { value.second=SPELLING_ALTERNATIVE; }
      else
      {
        AULOGINIT;
        LIMA_EXCEPTION( "Constraint LinguisticPropertyIs : invalid morphosyntactic type  \""
                          << mtype.c_str() << "\" !");
      }
    } else {
      value.first=manager.getPropertyValue(valueString.substr(i,j-i));
      value.second=NO_MORPHOSYNTACTICTYPE;
    }
    //LDEBUG << "add value pair (linguisticCode=" << value.first << ",type=" << value.second << ")";
    m_values.push_back(value);
    i=j+1;
    j=valueString.find("|",i);
    if (j==string::npos) j=valueString.size();
  }
}

bool LinguisticPropertyIs::
operator()(const LinguisticAnalysisStructure::AnalysisGraph& graph,
           const LinguisticGraphVertex& v,
           AnalysisContent& /*ac*/ ) const
{
  MorphoSyntacticData* data=get(vertex_data,*(graph.getGraph()),v);

  MorphoSyntacticData::const_iterator
  it=data->begin(),
     it_end=data->end();

  for (; it!=it_end; it++)
  {
    for (std::vector<pair<LinguisticCode,LinguisticProcessing::LinguisticAnalysisStructure::MorphoSyntacticType> >::const_iterator pItr=m_values.begin();
         pItr!=m_values.end();
         pItr++)
    {
      if (m_propertyAccessor->equal((*it).properties,pItr->first) &&
          (pItr->second==NO_MORPHOSYNTACTICTYPE || it->type==pItr->second))
      {
        return true;
      }
    }
  }
  return false;
}

//***********************************************************************
LengthInInterval::
LengthInInterval(MediaId language,
                 const LimaString& complement):
    ConstraintFunction(language,complement),
    m_min(0),
    m_max(0)
{
  //complement contains min and max values for length of the token
  //(max value is optional)
  std::string str=Common::Misc::limastring2utf8stdstring(complement);
  auto i=str.find(",");
  if (i==string::npos)
  {
    m_min=atoi(str.c_str());
    m_max=m_min;
  }
  else
  {
    m_min=atoi(string(str,0,i).c_str());
    m_max=atoi(string(str,i+1).c_str());
  }
}

bool LengthInInterval::
operator()(const LinguisticAnalysisStructure::AnalysisGraph& graph,
           const LinguisticGraphVertex& v,
           AnalysisContent& /*ac*/ ) const
{
  Token* token = get(vertex_token,*(graph.getGraph()),v);
  if (token == 0)
  {
    AULOGINIT;
    LERROR << "Null token on vertex " << v;
    return false;
  }

  uint64_t length=token->length();

  //   AULOGINIT;
  //   LDEBUG << "testing length of token " << *token
  //          << "(" <<  length << ") with interval ["
  //          << min << "-" << max << "]";

  //std::cerr << "testing length(" << token->stringForm().toUtf8().constData()
  //          << "(" <<  length << ") with interval [" << m_min << "-" << m_max << "]" << endl;
  return (length >= m_min && length <= m_max);
}

//***********************************************************************
NumericValueInInterval::
NumericValueInInterval(MediaId language,
                 const LimaString& complement):
    ConstraintFunction(language,complement),
    m_language(language),
    m_min(0),
    m_max(0)
{
  //complement contains min and max values for length of the token
  //(max value is optional)
  std::string str=Common::Misc::limastring2utf8stdstring(complement);
  std::string::size_type i=str.find(",");
  if (i==std::string::npos)
  {
    m_min=atoi(str.c_str());
    m_max=m_min;
  }
  else
  {
    string minString(str,0,i), maxString(str,i+1);
    if (minString.empty()) {
      m_min=0;
    }
    else {
      m_min=atoi(minString.c_str());
    }
    if (maxString.empty()) {
      m_max=0;
    }
    else {
      m_max=atoi(maxString.c_str());
    }
  }
}

bool NumericValueInInterval::
operator()(const LinguisticAnalysisStructure::AnalysisGraph& graph,
           const LinguisticGraphVertex& v,
           AnalysisContent& /*ac*/ ) const
{
  Token* token = get(vertex_token,*(graph.getGraph()),v);
  if (token == 0)
  {
    AULOGINIT;
    LERROR << "Null token on vertex " << v;
    return false;
  }

  uint64_t numValue(0);

  const TStatus& status=token->status();
  if (status.getNumeric() == T_INTEGER) {
    numValue=token->stringForm().toULong();
  }
  else {
    // get the normalized form : contains the numeric form
    // (normalized form corresponding to a macro_micro identifying a number)
    MorphoSyntacticData* data = get(vertex_data,*(graph.getGraph()),v);
    for (MorphoSyntacticData::const_iterator  it=data->begin(),
           it_end=data->end(); it!=it_end; it++) {
      numValue=Common::MediaticData::MediaticData::single().stringsPool(m_language)[(*it).normalizedForm].toULong();
      if (numValue!=0) {
        break;
      }
    }
  }
  if (numValue==0) { // cannot determine the numeric value
    return false;
  }

  if (m_max == 0) {
    return ( numValue >= m_min);
  }
  else {
    return ( numValue >= m_min && numValue <= m_max);
  }
}

//***********************************************************************
NoSpaceWith::NoSpaceWith(MediaId language,
                         const LimaString& complement):
ConstraintFunction(language,complement)
{
}

bool NoSpaceWith::operator()(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                             const LinguisticGraphVertex& v1,
                             const LinguisticGraphVertex& v2,
                             AnalysisContent& /*analysis*/) const 
{
  Token* token1 = get(vertex_token,*(graph.getGraph()),v1);
  Token* token2 = get(vertex_token,*(graph.getGraph()),v2);
  if (token1 == 0)
  {
    AULOGINIT;
    LERROR << "Null token on vertex " << v1;
    return false;
  }
  if (token2 == 0)
  {
    AULOGINIT;
    LERROR << "Null token on vertex " << v2;
    return false;
  }
  
  if (token2->position() > token1->position()) {
    return token2->position()==token1->position()+token1->length();
  }
  else {
    return token1->position()==token2->position()+token2->length();
  }  
}

} // end namespace
} // end namespace
} // end namespace
