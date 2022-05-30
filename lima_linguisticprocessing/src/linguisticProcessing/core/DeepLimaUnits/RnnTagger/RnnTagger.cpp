/*
    Copyright 2002-2021 CEA LIST

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

#include <QtCore/QTemporaryFile>
#include <QtCore/QRegularExpression>
#include <QDir>

#include "common/misc/Exceptions.h"
#include "common/Data/strwstrtools.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "common/tools/FileUtils.h"
#include "common/MediaticData/mediaticData.h"
#include "common/time/timeUtilsController.h"

#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/common/linguisticData/LimaStringText.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/TextSegmentation/SegmentationData.h"
#include "linguisticProcessing/common/helpers/ConfigurationHelper.h"
#include "linguisticProcessing/common/helpers/DeepTokenizerBase.h"
#include "linguisticProcessing/common/helpers/LangCodeHelpers.h"

#include "RnnTagger.h"
#include "deeplima/ner.h"

using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::Common::PropertyCode;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::Misc;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
namespace Lima::LinguisticProcessing::DeepLimaUnits::RnnTagger {

RnnTagger::RnnTagger() {

}

RnnTagger::~RnnTagger() = default;

void RnnTagger::init(
        Lima::Common::XMLConfigurationFiles::GroupConfigurationStructure &unitConfiguration,
        Lima::InitializableObject<Lima::MediaProcessUnit, Lima::MediaProcessUnitInitializationParameters>::Manager *manager) {

}

Lima::LimaStatusCode
RnnTagger::process(Lima::AnalysisContent &analysis) const {
    return Lima::MISSING_DATA;
}

}