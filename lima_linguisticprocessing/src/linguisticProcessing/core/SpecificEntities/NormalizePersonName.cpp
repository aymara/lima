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
 * @file       NormalizePersonName.cpp
 * @author     Besancon Romaric (romaric.besancon@cea.fr)
 * @date       Tue Jun 13 2006
 * copyright   Copyright (C) 2006-2012 by CEA LIST
 * 
 ***********************************************************************/

#include "NormalizePersonName.h"
#include "NormalizationUtils.h"
#include "MicrosForNormalization.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/core/Automaton/constraintFunctionFactory.h"
#include "common/Data/strwstrtools.h"

using namespace Lima::Common::MediaticData;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::LinguisticProcessing::Automaton;
using namespace std;

namespace Lima {
namespace LinguisticProcessing {
namespace SpecificEntities {

#define FIRSTNAME_FEATURE_NAME "firstname"
#define LASTNAME_FEATURE_NAME "lastname"

//**********************************************************************
// factories for actions defined in this file
Automaton::ConstraintFunctionFactory<NormalizePersonName>
NormalizePersonNameFactory(NormalizePersonNameId);

// utility function to normalize the case
LimaString capitalizeWord(const LimaString& str) {
  // capitalize single word
  return str.left(1).toUpper()+str.mid(1).toLower();
}

LimaString capitalize(const LimaString& str) {
  //capitalize each word, separated by a space or '-'
  QRegExp sep("[ -]");
  QString capitalized;
  int current=0;
  int i=str.indexOf(sep,current);
  while (i!=-1) {
    capitalized+=capitalizeWord(str.mid(current,i-current))+str[i];
    current=i+1;
    i=str.indexOf(sep,current);
  }
  // last one
  capitalized+=capitalizeWord(str.mid(current));
  return capitalized;
}


//**********************************************************************
NormalizePersonName::
NormalizePersonName(MediaId language,
                    const LimaString& complement):
Automaton::ConstraintFunction(language,complement),
m_language(language),
m_firstname(),
m_lastname(),
m_microsForFirstname(0),
m_microAccessor(0)
{
  m_microAccessor=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager().getPropertyAccessor("MICRO"));

  if (language != UNDEFLANG) {
    try {
      AbstractResource* res = LinguisticResources::single().getResource(language,"microsForPersonNameNormalization");
      MicrosForNormalization* micros=static_cast<MicrosForNormalization*>(res);
      m_microsForFirstname=micros->getMicros("FirstnameMicros");
    }
    catch (exception& e) {
      SELOGINIT;
      LWARN << "Exception caught: " << e.what();
      LWARN << "-> micros for person name normalization are not initialized";
    }
  }
  
  if (!complement.isEmpty()) {
    //uint64_t i=complement.find(LimaChar(',')); portage 32 64
    int i=complement.indexOf(LimaChar(','));
    if (i==-1) {
      m_lastname=complement;
    }
    else {
      m_firstname=complement.left(i);
      m_lastname=complement.mid(i+1);
    }
  }    
}

// function to normalize person names using a simple heuristic on result
// to separate firstname from lastname
/**
 * compute the normalized form of a person name
 * heuristic :
 * - if one or several elements of the entity are identified as
 *   firstnames in the dictionary => set them as firstname,
 *   set the other elements as lastname
 * - if no elements are identified as firstnames in the dictionary :
 *   if there are only one element : set it as lastname
 *   if there are only two elements: set first as firstname, second as lastname
 *
 *   if there is an initial : set whatever is before the initial + the initial
 *      as firstname, whatever is after as lastname
 *      (this one is not applied for the moment)
 *
 * @param m the match for the person name to normalize
 */
bool NormalizePersonName::
operator()(RecognizerMatch& m,
           AnalysisContent& /*unused analysis*/) const 
{
  // if firstname or lastname were given as arguments to the action
  if (!m_firstname.isEmpty() || !m_lastname.isEmpty()) {
    m.features().setFeature(FIRSTNAME_FEATURE_NAME,m_firstname);
    std::vector<EntityFeature>::iterator firstnameFeatureIt = m.features().find(FIRSTNAME_FEATURE_NAME);
    (*firstnameFeatureIt).setPosition(0);
    (*firstnameFeatureIt).setLength(0);
    m.features().setFeature(LASTNAME_FEATURE_NAME,m_lastname);
    std::vector<EntityFeature>::iterator lastnameFeatureIt = m.features().find(LASTNAME_FEATURE_NAME);
    (*lastnameFeatureIt).setPosition(0);
    (*lastnameFeatureIt).setLength(0);
    // modified stored normalized string to given normalization:
    m.features().setFeature(DEFAULT_ATTRIBUTE,m_firstname+LimaChar(' ')+m_lastname);
    return true;
  }

  // modified stored normalized string to inflected form :
  // ensure no normalization is applied to person names
  m.features().setFeature(DEFAULT_ATTRIBUTE,capitalize(m.getString()));

  LimaString firstname;
  LimaString lastname;

  for (RecognizerMatch::const_iterator i(m.begin()); i!=m.end(); i++) {
    if (! (*i).isKept()) {
      continue;
    }
    Token* t = m.getToken(i);
    MorphoSyntacticData* data = m.getData(i);
    // test if is firstname
    if (testMicroCategory(m_microsForFirstname,m_microAccessor,data)) {
      if (!firstname.isEmpty()) { firstname += LimaChar(' '); }
      firstname += t->stringForm();
    }
    else {
      if (!lastname.isEmpty()) { lastname += LimaChar(' '); }
      lastname += t->stringForm();
    }
  }

  // if firstname and lastname are attributed (for at least two words)
  if (((!firstname.isEmpty()) && (!lastname.isEmpty()))
      || m.size() == 1) {
    m.features().setFeature(FIRSTNAME_FEATURE_NAME,capitalize(firstname));
    RecognizerMatch::const_iterator i(m.begin());
    Token* t = m.getToken(i);
    uint64_t pos = (int64_t)(t->position());
    std::vector<EntityFeature>::iterator firstnameFeatureIt = m.features().find(FIRSTNAME_FEATURE_NAME);
    uint64_t len = (int64_t)(t->length());
    (*firstnameFeatureIt).setPosition(pos);
    (*firstnameFeatureIt).setLength(len);
    m.features().setFeature(LASTNAME_FEATURE_NAME,capitalize(lastname));
    return true;
  }

  // if there isn't a firstname or a lastname, use a heuristic

  //if only two elements, first is firstname, second is lastname
  if (m.size() == 2) {
    RecognizerMatch::const_iterator i(m.begin());
    Token* t = m.getToken(i);
    firstname = t->stringForm();
    m.features().setFeature(FIRSTNAME_FEATURE_NAME,capitalize(firstname));
    std::vector<EntityFeature>::iterator firstnameFeatureIt = m.features().find(FIRSTNAME_FEATURE_NAME);
    uint64_t pos = (int64_t)(t->position());
    (*firstnameFeatureIt).setPosition(pos);
    uint64_t len = (int64_t)(t->length());
    (*firstnameFeatureIt).setLength(len);
    i++;
    t = m.getToken(i);
    lastname = t->stringForm();
    m.features().setFeature(LASTNAME_FEATURE_NAME,capitalize(lastname));
    std::vector<EntityFeature>::iterator lastnameFeatureIt = m.features().find(LASTNAME_FEATURE_NAME);
    pos = (int64_t)(t->position());
    (*lastnameFeatureIt).setPosition(pos);
    len = (int64_t)(t->length());
    (*lastnameFeatureIt).setLength(len);
    return true;
  }

  // else loop again on the elements : last element is the lastname
  // all others are firstname, unless something indicates we are passing
  // in the lastname part (initial, "de")

  lastname=LimaString();
  firstname=LimaString();
  uint64_t lastnamePos = 2001;
  uint64_t lastnameLen = 2002;
  uint64_t firstnamePos = 2003;
  uint64_t firstnameLen = 2004;
  bool inLastname(false);
  bool initial(false);
  RecognizerMatch::const_iterator next;
  for (RecognizerMatch::const_iterator i(m.begin()); i!=m.end(); i++) {
    Token* t = m.getToken(i);
    if (initial) {
      inLastname=1;
      initial=false;
      if (t->form() == Common::MediaticData::MediaticData::changeable().stringsPool(m_language)[Common::Misc::utf8stdstring2limastring(".")]) 
      {
        firstname += t->stringForm();
        continue;
      }
    }
    if ((t->length() == 1 ||
         (t->length() == 2 && t->stringForm()[1]==LimaChar('.')))
        && ! inLastname) {
      initial=true;
      if (!firstname.isEmpty()) { firstname += LimaChar(' '); }
      firstname += t->stringForm();
      continue;
    }
    else if (t->stringForm() == Common::Misc::utf8stdstring2limastring("de") || 
             t->stringForm() == Common::Misc::utf8stdstring2limastring("De")) {
      inLastname=1;
    }
    // if last element -> last name
    next=i;
    next++;
    if (next == m.end()) { inLastname=1; }
    else { // peek to find if next elt is something like "Jr"
      Token* nextToken = m.getToken(next);
      const LimaString& str=nextToken->stringForm();
      if (str == Common::Misc::utf8stdstring2limastring("Jr") ||
          str == Common::Misc::utf8stdstring2limastring("sr") ||
          str == Common::Misc::utf8stdstring2limastring("II") ||
          str == Common::Misc::utf8stdstring2limastring("III")) {
        inLastname=1;
      }
    }
    
    if (inLastname) {
      if (!lastname.isEmpty() &&
          !(t->stringForm() == Common::Misc::utf8stdstring2limastring("."))) { 
        lastname += LimaChar(' '); 
      }
      lastname += t->stringForm();
      lastnamePos = t->position();
      lastnameLen = t->length();
    }
    else {
      if (!firstname.isEmpty()) { firstname += LimaChar(' '); }
      firstname += t->stringForm();
      firstnamePos = t->position();
      firstnameLen = t->length();
    }
  }
  
  if (firstname.isEmpty() && lastname.isEmpty()) {
    
    const FsaStringsPool& sp=Common::MediaticData::MediaticData::single().stringsPool(m_language);
    m.features().setFeature(LASTNAME_FEATURE_NAME,m.getNormalizedString(sp));
  } 
  else {
    m.features().setFeature(FIRSTNAME_FEATURE_NAME,capitalize(firstname));
    std::vector<EntityFeature>::iterator featureIt = m.features().find(FIRSTNAME_FEATURE_NAME);
    (*featureIt).setPosition(firstnamePos);
    (*featureIt).setLength(firstnameLen);
    m.features().setFeature(LASTNAME_FEATURE_NAME,capitalize(lastname));
    featureIt = m.features().find(LASTNAME_FEATURE_NAME);
    (*featureIt).setPosition(lastnamePos);
    (*featureIt).setLength(lastnameLen);
  }
  return true;
}

} // end namespace
} // end namespace
} // end namespace
