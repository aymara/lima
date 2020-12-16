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

  LinguisticMetaData* metadata=static_cast<LinguisticMetaData*>(analysis.getData("LinguisticMetaData"));
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
