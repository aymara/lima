/*
    Copyright 2019 CEA LIST

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
#ifndef LIMA_LINGUISTICPROCESSING_POSTAGGER_SVMTOOLPOSTAGGER_H
#define LIMA_LINGUISTICPROCESSING_POSTAGGER_SVMTOOLPOSTAGGER_H

#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/MediaticData/mediaticData.h"
#include "common/time/traceUtils.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"

#include "common/MediaProcessors/MediaProcessUnit.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticDataUtils.h"

#include <list>
#include <vector>
#include <set>
#include <map>
#include <queue>

#include <sstream>

class tagger;

namespace Lima
{

namespace LinguisticProcessing
{

namespace PosTagger
{

#define PERLSVMTOOLPOSTAGGER_CLASSID "PerlSvmToolPosTagger"

class PerlSvmToolPosTaggerPrivate;
/**
 * @brief A PoS tagger using SVMTool, Perl version, as a backend
 * @author Gael de Chalendar
*/
class PerlSvmToolPosTagger : public MediaProcessUnit
{
  friend class SvmToolPosTaggerPrivate;
public:
  PerlSvmToolPosTagger();

  virtual ~PerlSvmToolPosTagger();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;

  LimaStatusCode process(AnalysisContent& analysis) const override;

private:

  PerlSvmToolPosTaggerPrivate* m_d;
};

class PerlSvmToolPosTaggerFactory : public InitializableObjectFactory<MediaProcessUnit>
{
public:

  virtual MediaProcessUnit* create(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    MediaProcessUnit::Manager* manager) const override ;

private:
  PerlSvmToolPosTaggerFactory(const std::string& id);
  static PerlSvmToolPosTaggerFactory* s_instance;

};


}

}

}

#endif
