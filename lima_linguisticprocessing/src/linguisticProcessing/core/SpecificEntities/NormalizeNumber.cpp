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
 * @file       NormalizeNumber.cpp
 * @author     Besancon Romaric (romaric.besancon@cea.fr)
 * @date       Tue Jun 13 2006
 * copyright   Copyright (C) 2006-2012 by CEA LIST
 *
 ***********************************************************************/

#include "NormalizeNumber.h"
#include "NormalizationUtils.h"
#include "MicrosForNormalization.h"
#include "linguisticProcessing/core/Automaton/SpecificEntityAnnotation.h"
#include "common/Data/strwstrtools.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/core/Automaton/automatonCommon.h"
#include "linguisticProcessing/core/Automaton/constraintFunctionFactory.h"
#include <cmath>

using namespace Lima::Common::MediaticData;
using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::LinguisticProcessing::Automaton;
using namespace std;

namespace Lima {
namespace LinguisticProcessing {
namespace SpecificEntities {

#define NUMVALUE_FEATURE_NAME "numvalue"
#define UNIT_FEATURE_NAME "unit"

//**********************************************************************
// factories for actions defined in this file
Automaton::ConstraintFunctionFactory<NormalizeNumber>
NormalizeNumberFactory(NormalizeNumberId);

//**********************************************************************
NormalizeNumber::
NormalizeNumber(MediaId language,
                const LimaString& complement):
Automaton::ConstraintFunction(language,complement),
m_language(language),
m_microsForNumber(0),
m_microsForUnit(0),
m_microsForConjunction(0),
m_microAccessor(0)
{
  m_microAccessor=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager().getPropertyAccessor("MICRO"));

  if (language != UNDEFLANG) {
    try {
      AbstractResource* res = LinguisticResources::single().getResource(language,"microsForNumberNormalization");
      MicrosForNormalization* micros=static_cast<MicrosForNormalization*>(res);
      m_microsForNumber=micros->getMicros("NumberMicros");
      m_microsForUnit=micros->getMicros("UnitMicros");
      m_microsForConjunction=micros->getMicros("ConjCoordMicros");
    }
    catch (exception& e) {
      SELOGINIT;
      LWARN << "Exception caught: " << e.what();
      LWARN << "-> micros for number normalization are not initialized";
    }
  }
}

/***********************************************************************/
// specific helper functions for the normalization of numbers
// not part of the class: are language independant

// some local typedefs
 // use a single double: is 0, indicates it is a conjunction
typedef double NumberPart;
// current state: must add or multiply with following value
typedef enum { ADDITIVE, MULTIPLICATIVE } NumberNormalizationMode;

bool isFraction(Token* t);
bool isIntegerWithCommaOrDot(Token* t);
bool isIntegerWithComma(Token* t);
bool isIntegerWithDot(Token* t);
double getValueHeuristic(const LimaString& str, LimaChar sep);
bool isMultiplierNumber(double n);
double computeNumberValue(vector<NumberPart>& m,
                          vector<NumberPart>::iterator itBegin,
                          vector<NumberPart>::iterator itEnd,
                          NumberNormalizationMode mode=ADDITIVE);

double NormalizeNumber::
getNumberValue(Token* t,MorphoSyntacticData* data) const
{
  if (isInteger(t))
  { // t is in numeric format
    return LimaStringToInt(t->stringForm());
  }
  else if (isIntegerWithComma(t))
  {
    return getValueHeuristic(t->stringForm(),LimaChar(','));
  }
  else if (isIntegerWithDot(t))
  {
    return getValueHeuristic(t->stringForm(),LimaChar('.'));
  }

  // get the normalized form : contains the numeric form
  // (normalized form corresponding to a macro_micro identifying a number)
  MorphoSyntacticData::const_iterator
    it=data->begin(),
    it_end=data->end();
  for (; it!=it_end; it++) {
    if (testMicroCategory(m_microsForNumber,m_microAccessor,(*it).properties)) {
      const LimaString& str=
        Common::MediaticData::MediaticData::single().stringsPool(m_language)[(*it).normalizedForm];
      bool ok;
      double d = str.toDouble(&ok);
      if(ok) {
#ifdef DEBUG_LP
        SELOGINIT;
        LDEBUG << "getNumberValue of " << t->stringForm() << ". Found number "<< d;
#endif
        return d;
      }
    }
  }
#ifdef DEBUG_LP
  SELOGINIT;
  LDEBUG << "getNumberValue of " << t->stringForm() << ". Could not read number.";
#endif
  return 0;
}

/***********************************************************************/
// normalization of a number
/***********************************************************************/
bool NormalizeNumber::
operator()(RecognizerMatch& m,
           AnalysisContent& analysis) const
{
#ifdef DEBUG_LP
  SELOGINIT;
  LDEBUG << "NormalizeNumber " << m;
#endif

  // annotation data is used to get numeric value of already recognized number entities
  AnnotationData* annotationData = static_cast< AnnotationData* >(analysis.getData("AnnotationData"));

  vector<NumberPart> values;

  // a first pass on the match to eliminate what isn't a number
  for (RecognizerMatch::iterator it=m.begin(),it_end=m.end();
       it!=it_end; it++) {

    if (! (*it).isKept()) {
      continue;
    }

    // find if vertex is a specific entity of type number
    // in this case, get the numeric value from the features
    bool hasNumericValue(false);
    std::set< AnnotationGraphVertex > matches = annotationData->matches(m.getGraph()->getGraphId(),(*it).m_elem.first,"annot");
    for (std::set< AnnotationGraphVertex >::const_iterator annot = matches.begin(),
           annot_end=matches.end(); annot != annot_end; annot++) {
      if (annotationData->hasAnnotation(*annot, Common::Misc::utf8stdstring2limastring("SpecificEntity"))) {
#ifdef DEBUG_LP
        LDEBUG << "NormalizeNumber: vertex " << (*it).m_elem.first << " has specific entity annotation";
#endif
        const SpecificEntityAnnotation* se =
          annotationData->annotation(*annot, Common::Misc::utf8stdstring2limastring("SpecificEntity")).
          pointerValue<SpecificEntityAnnotation>();
        const EntityFeatures& features=se->getFeatures();
        for (EntityFeatures::const_iterator f=features.begin(),f_end=features.end();
             f!=f_end; f++) {
#ifdef DEBUG_LP
          LDEBUG << "NormalizeNumber: looking at feature " << (*f).getName();
#endif
          if ((*f).getName()==NUMVALUE_FEATURE_NAME) {
            try {
              double value=boost::any_cast<double>((*f).getValue());
#ifdef DEBUG_LP
              LDEBUG << "NormalizeNumber: add value " << value;
#endif
              values.push_back(value);
              hasNumericValue=true;
            }
            catch (const boost::bad_any_cast& e) {
              // check if it is a string containing a number (may be the case if a setEntityFeature
              // has been used explicitely in a rule
              std::string strval=(*f).getValueString();
              char *end;
              double val = std::strtod(strval.c_str(), &end);
              if (val!=0 || strval=="0") {
                values.push_back(val);
                hasNumericValue=true;
              }
              else {
                SELOGINIT;
                LERROR << "Error: failed to get numeric value for feature" << (*f).getName() << ":" << (*f).getValueString();
              }
            }
          }
        }
      }
    }
    if (hasNumericValue) {
      continue;
    }
    // find the numeric value in the normalization form of tokens
    Token* t = m.getToken(it);
    MorphoSyntacticData* data = m.getData(it);
    double value=getNumberValue(t,data);
    if (value != 0.0) { // it is a number
      values.push_back(value);
    }
    else if (testMicroCategory(m_microsForConjunction,m_microAccessor,data)) {
      // conjunction needed to compute the value
      values.push_back(0.0);
    }
    else if (testMicroCategory(m_microsForUnit,m_microAccessor,data)) {
#ifdef DEBUG_LP
      LDEBUG << "NormalizeNumber: add feature UNIT " << t->stringForm();
#endif
      m.features().addFeature(UNIT_FEATURE_NAME,t->stringForm());
    }
    // ignore other non numbers (can be % or "de"...)
  }

  if (values.empty()) {
    SELOGINIT;
    LWARN << "Warning: cannot normalize number \"" <<  m.getString()
          << "\": no numeric information available for components";
    m.features().setFeature(NUMVALUE_FEATURE_NAME,(double)0.0);
    // return true even if value could not be computed: a value has been set in
    // features
    return true;
  }

  // then compute the number
  double number=computeNumberValue(values,values.begin(),values.end());
#ifdef DEBUG_LP
  LDEBUG << "NormalizeNumber: add feature NUMVALUE " << number;
#endif
  m.features().addFeature(NUMVALUE_FEATURE_NAME,number);
  m.features().addFeature(DEFAULT_ATTRIBUTE,m.getString());
  return true;
}

//**********************************************************************
// definitions of helper functions
bool isIntegerWithCommaOrDot(Token* t) {
  const TStatus& status=t->status();
  switch(status.getNumeric()) {
  case T_COMMA_NUMBER:
  case T_DOT_NUMBER: return true;
  default: return false;
  }
}

bool isIntegerWithComma(Token* t) {
  const TStatus& status=t->status();
  switch(status.getNumeric()) {
  case T_COMMA_NUMBER: return true;
  default: return false;
  }
}

bool isIntegerWithDot(Token* t) {
  const TStatus& status=t->status();
  switch(status.getNumeric()) {
  case T_DOT_NUMBER: return true;
  default: return false;
  }
}

bool isFraction(Token* t) {
  const TStatus& status=t->status();
  return (status.getNumeric() == T_FRACTION);
}

double getValueHeuristic(const LimaString& str, LimaChar sep) {
  // heuristic to find the value of a number with separator '.' or ','
  // -> should be language dependent !... (in TODO list)

  // if there is at least two separators -> 20.000.000 20,000,000
  //uint64_t firstSep(str.find(sep,0)); portage 32 64
  int firstSep(str.indexOf(sep));
  if (firstSep == -1) { // error
    return LimaStringToInt(str);
  }

  //uint64_t nextSep(str.find(sep,firstSep+1)); portage 32 64
  int nextSep(str.indexOf(sep,firstSep+1));

  if (nextSep == -1) {
    // no second separator
    LimaString beforeSep = str.left(firstSep);
    LimaString afterSep = str.mid(firstSep+1);

    if (afterSep.size() == 3) {
      // three digits after separator -> assume 1000
      return ( (LimaStringToInt(beforeSep)*1000)
           +
           LimaStringToInt(afterSep) );
    }
    else { // assume decimal part
      return ( LimaStringToInt(beforeSep) +
           ( (double)LimaStringToInt(afterSep)
         /pow((double)10,(int)afterSep.size())) ) ;
    }
  }
  else {
    // there is at least a second separator -> should be multiplier
    LimaString beforeSep = str.left(firstSep);
    double result(beforeSep.toDouble());
    LimaString afterSep;
    while (nextSep != -1) {
      afterSep=str.mid(firstSep+1,nextSep-firstSep-1);
      if (afterSep.size() != 3 ) {
    // error : cant be multiplier -> don't know what it is
    return 0;
      }
      result = result*1000 + afterSep.toDouble();
      firstSep = nextSep;
      nextSep = str.indexOf(sep,firstSep+1);
    }
    // last part
    afterSep=str.mid(firstSep+1);
    if (afterSep.size() != 3 ) {
      // error : cant be multiplier -> don't know what it is
      return 0;
    }
    result = result*1000 + LimaStringToInt(afterSep);
    return result;
  }
}

// !!! I doubt that this is language independant !!!...
bool isMultiplierNumber(double n) {
  if (n==100     ||
      n==1000    ||
      n==1000000 ||
      n==1000000000) {
    return true;
  }
  return false;
}

// recursive function to compute the value of a composed number
// (for instance "trente trois mille vingt et un" or "22 millions")
double computeNumberValue(vector<NumberPart>& m,
                          vector<NumberPart>::iterator itBegin,
                          vector<NumberPart>::iterator itEnd,
                          NumberNormalizationMode mode)
{

  if (itBegin == itEnd) { // neutral element
    switch(mode) {
    case ADDITIVE: return 0;
    case MULTIPLICATIVE: return 1;
    }
  }

  vector<NumberPart>::iterator tmp(itBegin);
  if (++tmp == itEnd) { // single element
    return *itBegin;
  }

  // search for multipliers -> find the biggest
  // (the biggest is not always the first : trois cent douze mille)
  vector<NumberPart>::iterator ConjunctionPosition(itEnd);
  vector<NumberPart>::iterator biggestMultiplier(itEnd);
  double biggestMultiplierValue(0);

  for (vector<NumberPart>::iterator i(itBegin); i!=itEnd; i++) {
    double value=*i;
    if (*i==0) {
      // is conjunction
      ConjunctionPosition=i;
    }
    if (isMultiplierNumber(value)) {
      if (value > biggestMultiplierValue) {
        biggestMultiplier = i;
        biggestMultiplierValue = value;
      }
    }
  }

  // if there is a multiplier
  if (biggestMultiplier != itEnd) {
    const double d= (computeNumberValue(m,itBegin,biggestMultiplier,MULTIPLICATIVE)*
            biggestMultiplierValue
            +computeNumberValue(m,biggestMultiplier+1,itEnd));
    return d;
  }

  // if there is a conjunction
  if (ConjunctionPosition != itEnd) {
    const double d= ( computeNumberValue(m,itBegin,ConjunctionPosition) +
             computeNumberValue(m,ConjunctionPosition+1,itEnd) );
    return d;
  }
  return 0;
}


} // end namespace
} // end namespace
} // end namespace
