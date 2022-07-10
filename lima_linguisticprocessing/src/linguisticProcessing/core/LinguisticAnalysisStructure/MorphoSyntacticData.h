// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_LINGUISTICPROCESSING_LINGUISTICANALYSISSTRUCTUREMORPHOSYNTACTICDATA_H
#define LIMA_LINGUISTICPROCESSING_LINGUISTICANALYSISSTRUCTUREMORPHOSYNTACTICDATA_H

#include "LinguisticAnalysisStructureExport.h"
#include "common/LimaCommon.h"
#include "common/Data/LimaString.h"
#include "linguisticProcessing/common/PropertyCode/PropertyAccessor.h"

#include "Token.h"

#include <vector>
#include <list>

namespace Lima {

namespace LinguisticProcessing {

namespace LinguisticAnalysisStructure {

enum MorphoSyntacticType {
  NO_MORPHOSYNTACTICTYPE,
  SIMPLE_WORD,
  ABBREV_ALTERNATIVE,
  HYPHEN_ALTERNATIVE,
  IDIOMATIC_EXPRESSION,
  CONCATENATED_ALTERNATIVE,
  HYPERWORD_ALTERNATIVE,
  UNKNOWN_WORD,
  CAPITALFIRST_WORD,
  AGGLUTINATED_WORD,
  DESAGGLUTINATED_WORD,
  CHINESE_SEGMENTER,
  SPECIFIC_ENTITY, // Added to 
  SPELLING_ALTERNATIVE,
};

struct LIMA_LINGUISTICANALYSISSTRUCTURE_EXPORT LinguisticElement {
  StringsPoolIndex inflectedForm;
  StringsPoolIndex lemma;
  StringsPoolIndex normalizedForm;
  LinguisticCode properties;
  MorphoSyntacticType type;
  bool operator==(const LinguisticElement& le) const;
  bool operator<(const LinguisticElement& le) const;
};

/**
  * Holds morphosyntactic informations
  * @author Benoit Mathieu
  */
class LIMA_LINGUISTICANALYSISSTRUCTURE_EXPORT MorphoSyntacticData : public std::vector<LinguisticElement> {
public:
    MorphoSyntacticData();

    virtual ~MorphoSyntacticData();
    
    /**
     * return true if there is only one micro, and this micro is in microfilter
     * @param microAccessor 
     * @param microFilter 
     * @return 
     */
    bool hasUniqueMicro(const Lima::Common::PropertyCode::PropertyAccessor& microAccessor, const std::list< Lima::LinguisticCode >& microFilter);

    uint64_t countValues(const Common::PropertyCode::PropertyAccessor& propertyAccessor);

    /**
     * Return the first non empty value for the given accessor.
     * @param pro
            cout << "hello" << endl;pertyAccessor 
     * @return 
     */
    LinguisticCode firstValue(const Common::PropertyCode::PropertyAccessor& propertyAccessor) const;
    
    std::set<LinguisticCode> allValues(const Common::PropertyCode::PropertyAccessor& propertyAccessor) const;

    void allValues(const Common::PropertyCode::PropertyAccessor& propertyAccessor,std::set<LinguisticCode>& result) const;
    
    std::set<StringsPoolIndex> allInflectedForms() const;
    
    std::set<StringsPoolIndex> allLemma() const;
    
    std::set<StringsPoolIndex> allNormalizedForms() const;
    
    void outputXml(std::ostream& xmlStream,const Common::PropertyCode::PropertyCodeManager& pcm,const FsaStringsPool& sp) const;

};

inline std::set<LinguisticCode> MorphoSyntacticData::allValues(const Common::PropertyCode::PropertyAccessor& propertyAccessor) const
{
  std::set<LinguisticCode> res;
  allValues(propertyAccessor,res);
  return res;
}

}

}

}

#endif
