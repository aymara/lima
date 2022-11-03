// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "MetaDataDumper.h"
#include "common/time/traceUtils.h"
#include "common/Data/strwstrtools.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"

using namespace std;
using namespace Lima::Common;

namespace Lima {
namespace LinguisticProcessing {

//***********************************************************************
SimpleFactory<MediaProcessUnit,MetaDataDumper>
MetaDataDumperFactory(METADATADUMPER_CLASSID);

MetaDataDumper::MetaDataDumper() :
AbstractTextualAnalysisDumper()
{
}

MetaDataDumper::~MetaDataDumper() {
}

//***********************************************************************
void MetaDataDumper::
init(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
     Manager* manager)

{
  AbstractTextualAnalysisDumper::init(unitConfiguration,manager);
}

LimaStatusCode MetaDataDumper::process(AnalysisContent& analysis) const
{
  LOGINIT("LP::EventAnalysis");
  LDEBUG << "MetaDataDumper::process";
  TimeUtils::updateCurrentTime();

  // initialize output
  auto dstream=AbstractTextualAnalysisDumper::initialize(analysis);
  ostream& out=dstream->out();

  auto metadata = std::dynamic_pointer_cast<LinguisticMetaData>(analysis.getData("LinguisticMetaData"));
  if (metadata == 0) {
    DUMPERLOGINIT;
    LERROR << "no LinguisticMetaData ! abort";
    return MISSING_DATA;
  }

  // serialize map<string,string>
  // must be compatible with the reading part in the MetaDataHandler
  const map<string,string>& m=metadata->getAllMetaData();
  out << m.size() << endl;
  for (const auto& it: m) {
    out << it.first << endl;
    out << it.second << endl;
  }
  TimeUtils::logElapsedTime("MetaDataDumper");
  return SUCCESS_ID;
}

} // end namespace
} // end namespace
