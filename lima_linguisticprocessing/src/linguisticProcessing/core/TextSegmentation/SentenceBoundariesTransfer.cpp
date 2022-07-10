// Copyright 2019 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "SentenceBoundariesTransfer.h"
#include "SegmentationData.h"
#include "common/MediaticData/mediaticData.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/time/timeUtilsController.h"
#include "common/Data/strwstrtools.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/common/PropertyCode/PropertyManager.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"

using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

namespace Lima {
namespace LinguisticProcessing {
namespace LinguisticAnalysisStructure {

SimpleFactory<MediaProcessUnit,SentenceBoundariesTransfer> sentenceBoundariesTransferFactory(SENTENCEBOUNDARIESTRANSFER_CLASSID);

class SentenceBoundariesTransferPrivate
{
  friend class SentenceBoundariesTransfer;

  SentenceBoundariesTransferPrivate();

  virtual ~SentenceBoundariesTransferPrivate();
};


SentenceBoundariesTransferPrivate::SentenceBoundariesTransferPrivate()
{
}


SentenceBoundariesTransferPrivate::~SentenceBoundariesTransferPrivate()
{
}


SentenceBoundariesTransfer::SentenceBoundariesTransfer() :
    MediaProcessUnit(),
    m_d(new SentenceBoundariesTransferPrivate())
{
}


SentenceBoundariesTransfer::~SentenceBoundariesTransfer()
{
  delete m_d;
}

void SentenceBoundariesTransfer::init(
  GroupConfigurationStructure& unitConfiguration,
  Manager* manager)
{
  LIMA_UNUSED(unitConfiguration);
  LIMA_UNUSED(manager);
}


LimaStatusCode SentenceBoundariesTransfer::process(
    AnalysisContent& analysis) const
{
  Lima::TimeUtilsController timer("SentenceBoundariesTransfer");

  SEGMENTATIONLOGINIT;
  LINFO << "SentenceBoundariesTransfer::process start transfering sentence bounds from AnalysisGraph to PosGraph";

  auto sb = static_cast<SegmentationData*>(analysis.getData("SentenceBoundaries"));
  if (sb == nullptr)
  {
    DUMPERLOGINIT;
    LERROR << "SentenceBoundariesTransfer::process no SentenceBounds";
    return MISSING_DATA;
  }
  if (sb->getGraphId() == "PosGraph")
  {
    DUMPERLOGINIT;
    LINFO << "SentenceBoundariesTransfer: sentence bounds are already on PosGraph. Nothing to do.";
    return SUCCESS_ID;
  }

  AnalysisGraph* anagraph=static_cast<AnalysisGraph*>(
    analysis.getData("AnalysisGraph"));
  if (anagraph == nullptr)
  {
    LERROR << "SentenceBoundariesTransfer::process no graph 'AnalysisGraph' available !";
    return MISSING_DATA;
  }
  AnalysisGraph* posgraph=static_cast<AnalysisGraph*>(
    analysis.getData("PosGraph"));
  if (posgraph == nullptr)
  {
    LERROR << "SentenceBoundariesTransfer::process no graph 'PosGraph' available !";
    return MISSING_DATA;
  }
  AnnotationData* annotationData = static_cast< AnnotationData* >(
    analysis.getData("AnnotationData"));
  if (annotationData == nullptr)
  {
    LERROR << "SentenceBoundariesTransfer::process: no annotation graph available !";
    return MISSING_DATA;
  }

  auto newSb = new SegmentationData("PosGraph");
  for (const auto& segment: sb->getSegments())
  {
    auto firstVxMatches = annotationData->matches("AnalysisGraph",
                                                  segment.getFirstVertex(),
                                                  "PosGraph");
    if (segment.getFirstVertex() == 0)
      firstVxMatches.insert(0);
    if (firstVxMatches.empty())
    {
      LERROR << "SentenceBoundariesTransfer::process: no PoS graph vertex for segment first AnalysisGraph vextex"
             << segment.getFirstVertex();
      continue;
    }
    auto lastVxMatches = annotationData->matches("AnalysisGraph",
                                                 segment.getLastVertex(),
                                                 "PosGraph");
    if (segment.getFirstVertex() == 1)
      firstVxMatches.insert(1);
    if (lastVxMatches.empty())
    {
      LERROR << "SentenceBoundariesTransfer::process: no PoS graph vertex for segment last AnalysisGraph vextex"
             << segment.getLastVertex();
      continue;
    }
    Segment newSegment("sentence",*firstVxMatches.begin(),*lastVxMatches.begin(),posgraph);
    newSb->add(newSegment);
  }

  analysis.removeData("SentenceBoundaries");
  analysis.setData("SentenceBoundaries", newSb);
  return SUCCESS_ID;
}


}

}

}
