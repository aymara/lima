/*
    Copyright 2002-2020 CEA LIST

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
#include "MorphoSyntacticData.h"
#include "MorphoSyntacticDataUtils.h"

#include "common/MediaticData/mediaticData.h"
#include "common/Data/strwstrtools.h"

#include <algorithm>

using namespace Lima::Common::MediaticData;

namespace Lima
{

namespace LinguisticProcessing
{

namespace LinguisticAnalysisStructure
{

bool LinguisticElement::operator==(const LinguisticElement& le) const
{
  return ((properties==le.properties) &&
          (inflectedForm==le.inflectedForm) &&
          (lemma==le.lemma) &&
          (normalizedForm==le.normalizedForm) &&
          (type==le.type));
}

bool LinguisticElement::operator<(const LinguisticElement& le) const
{
  if (inflectedForm!=le.inflectedForm) return inflectedForm<le.inflectedForm;
  if (lemma!=le.lemma) return lemma<le.lemma;
  if (normalizedForm!=le.normalizedForm) return normalizedForm<le.normalizedForm;
  if (properties!=le.properties) return properties<le.properties;
  return type<le.type;
}


MorphoSyntacticData::MorphoSyntacticData()
{}

MorphoSyntacticData::~MorphoSyntacticData()
{}

bool MorphoSyntacticData::hasUniqueMicro(
    const Common::PropertyCode::PropertyAccessor& microAccessor,
    const std::list<LinguisticCode>& microFilter)
{
  LinguisticCode micro(0);
  for (auto it = cbegin(); it != cend(); it++)
  {
    auto tmp = microAccessor.readValue(it->properties);
    if (micro != static_cast<LinguisticCode>(0))
    {
      if (micro != tmp)
      {
        return false;
      }
    }
    else
    {
      micro = tmp;
    }
    bool found=false;
    for (auto filterItr = microFilter.cbegin(); filterItr != microFilter.cend();
         filterItr++)
    {
      if (tmp==*filterItr)
      {
        found=true;
        break;
      }
    }
    if (!found)
    {
      return false;
    }
  }
  // if micro is 0, then there was no micros, return false
  return micro!=static_cast<LinguisticCode>(0);
}

uint64_t MorphoSyntacticData::countValues(
      const Common::PropertyCode::PropertyAccessor& propertyAccessor)
{
  std::set<LinguisticCode> values;
  allValues(propertyAccessor, values);
  return values.size();
}

void MorphoSyntacticData::allValues(
    const Common::PropertyCode::PropertyAccessor& propertyAccessor,
    std::set<LinguisticCode>& result) const
{
  for (auto it = cbegin(); it != cend(); it++)
  {
    if (!propertyAccessor.empty(it->properties))
    {
      result.insert(propertyAccessor.readValue(it->properties));
    }
  }
}

void MorphoSyntacticData::outputXml(
    std::ostream& xmlStream,
    const Common::PropertyCode::PropertyCodeManager& pcm,
    const FsaStringsPool& sp) const
{
  xmlStream << "    <data>" << std::endl;

  if (!empty())
  {
    // trie pour avoir les donnees groupees par strings
    MorphoSyntacticData tmp(*this);
    sort(tmp.begin(),tmp.end(),ltString());
    MorphoSyntacticType type(NO_MORPHOSYNTACTICTYPE);
    StringsPoolIndex form(0);
    StringsPoolIndex lemma(0);
    StringsPoolIndex norm(0);
    std::string currentType;
    bool firstEntry = true;
    for (auto it = tmp.cbegin(); it != tmp.cend(); it++)
    {
      if (it->type != type)
      {
        // Changement type
        if (!firstEntry)
        {
          xmlStream << "        </form>" << std::endl;
          xmlStream << "      </" << currentType << ">" << std::endl;
        }
        type=it->type;
        switch ( type )
        {
            case SIMPLE_WORD :
            currentType = "simple_word";
            break;
            case IDIOMATIC_EXPRESSION :
            currentType = "idiomatic_expression";
            break;
            case UNKNOWN_WORD :
            currentType = "unknown_word";
            break;
            case ABBREV_ALTERNATIVE :
            currentType = "abbrev_alternative";
            break;
            case HYPHEN_ALTERNATIVE :
            currentType = "hyphen_alternative";
            break;
            case CONCATENATED_ALTERNATIVE :
              currentType = "concatenated_alternative";
              break;
            case SPELLING_ALTERNATIVE :
              currentType = "spelling_alternative";
              break;
            case CAPITALFIRST_WORD :
            currentType = "capitalfirst_word";
            break;
            case AGGLUTINATED_WORD:
            currentType = "agglutinated_word";
            break;
            case DESAGGLUTINATED_WORD :
            currentType = "desagglutinated_word";
            break;
            case SPECIFIC_ENTITY :
            currentType = "specific_entity";
            break;
            case HYPERWORD_ALTERNATIVE:
            currentType = "hyperwordstemmer_alternative";
            break;
            case CHINESE_SEGMENTER:
            currentType = "chinese_segmenter";
            break;
            default :
            currentType = "UNKNOWN_MORPHOSYNTACTIC_TYPE";
            break;
        }
        xmlStream << "      <" << currentType << ">" << std::endl;
      }
      if ((it->inflectedForm != form) || (it->lemma != lemma)
          || (it->normalizedForm != norm))
      {
        if (!firstEntry)
        {
          xmlStream << "        </form>" << std::endl;
        }
        form = it->inflectedForm;
        lemma = it->lemma;
        norm = it->normalizedForm;
        xmlStream << "        <form infl=\""
                  << Common::Misc::transcodeToXmlEntities(sp[form]).toStdString()
                  << "\" ";
        xmlStream << "lemma=\""
                  << Common::Misc::transcodeToXmlEntities(sp[lemma]).toStdString()
                  << "\" ";
        xmlStream << "norm=\""
                  << Common::Misc::transcodeToXmlEntities(sp[norm]).toStdString()
                  << "\">" << std::endl;
      }
      const auto& managers = pcm.getPropertyManagers();
      xmlStream << "          <property>" << std::endl;
      for (auto propItr = managers.cbegin(); propItr != managers.cend();
           propItr++)
      {
        if (!propItr->second.getPropertyAccessor().empty(it->properties))
        {
          xmlStream << "            <p prop=\"" << propItr->first
                    << "\" val=\""
                    << propItr->second.getPropertySymbolicValue(it->properties)
                    << "\"/>" << std::endl;
        }
      }
      xmlStream << "          </property>" << std::endl;
      firstEntry=false;
    }
    xmlStream << "        </form>" << std::endl;
    xmlStream << "      </" << currentType << ">" << std::endl;
  }
  xmlStream << "    </data>" << std::endl;
}

std::set<StringsPoolIndex> MorphoSyntacticData::allInflectedForms() const
  {
    std::set<StringsPoolIndex> forms;
    for (auto it = cbegin(); it != cend(); it++)
    {
      forms.insert(it->inflectedForm);
    }
    return forms;
  }

std::set<StringsPoolIndex> MorphoSyntacticData::allLemma() const
  {
    std::set<StringsPoolIndex> lemma;
    for (auto it = cbegin(); it != cend(); it++)
    {
      lemma.insert(it->lemma);
    }
    return lemma;
  }

std::set<StringsPoolIndex> MorphoSyntacticData::allNormalizedForms() const
  {
    std::set<StringsPoolIndex> norms;
    for (auto it = cbegin(); it != cend(); it++)
    {
      norms.insert(it->normalizedForm);
    }
    return norms;
  }

LinguisticCode MorphoSyntacticData::firstValue(
    const Common::PropertyCode::PropertyAccessor& propertyAccessor) const
{
  for (auto it = cbegin(); it != cend(); it++)
  {
    if (!propertyAccessor.empty(it->properties))
    {
      return propertyAccessor.readValue(it->properties);
    }
  }
  return static_cast<LinguisticCode>(0);
}



} // LinguisticAnalysisStructure

} // LinguisticProcessing

} // Lima
