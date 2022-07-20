// Copyright 2002-2022 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_LINGUISTICPROCESSING_COMPOUNDSCOMPOUNDSBUILDERFROMSYNTACTICDATA_H
#define LIMA_LINGUISTICPROCESSING_COMPOUNDSCOMPOUNDSBUILDERFROMSYNTACTICDATA_H

#include "CompoundsExport.h"
#include "common/MediaProcessors/MediaProcessUnit.h"

#include "common/MediaticData/mediaData.h"

#include <boost/serialization/strong_typedef.hpp>
#include <boost/cstdint.hpp>


namespace Lima
{

namespace LinguisticProcessing
{

namespace Compounds
{

#define COMPOUNDSBUILDERFROMSYNTACTICDATAFACTORY_CLASSID "CompoundsBuilderFromSyntacticData"

class CompoundsBuilderFromSyntacticDataPrivate;

/**
@author Benoit Mathieu
*/
class LIMA_COMPOUNDS_EXPORT CompoundsBuilderFromSyntacticData : public MediaProcessUnit
{
public:
  CompoundsBuilderFromSyntacticData();

  virtual ~CompoundsBuilderFromSyntacticData();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;

  LimaStatusCode process(AnalysisContent& analysis) const override;

  Lima::Common::MediaticData::ConceptType getConceptType(const std::string& typeName) const;
  const std::string& getConceptName(const Lima::Common::MediaticData::ConceptType& type) const;
  Lima::Common::MediaticData::ConceptType getConceptForMacro(const LinguisticCode& macro) const;

  bool isACompoundRel(const std::string& rel) const;

private:
  CompoundsBuilderFromSyntacticData(const CompoundsBuilderFromSyntacticData& b);
  CompoundsBuilderFromSyntacticData& operator=(const CompoundsBuilderFromSyntacticData& b);
  CompoundsBuilderFromSyntacticDataPrivate* m_d;
};

} // Compounds

} // LinguisticProcessing

} // Lima

#endif
