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

#include "SentenceBoundariesUpdater.h"

#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "common/MediaticData/mediaticData.h"
#include "common/time/timeUtilsController.h"
#include "SegmentationData.h"
#include "linguisticProcessing/common/helpers/ConfigurationHelper.h"

#include <string>

using namespace std;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::Common;

namespace Lima
{
namespace LinguisticProcessing
{
namespace TextSegmentation
{
namespace SentenceBoundariesUpdater
{

#define DEBUG_THIS_FILE true
#define LOG_ERROR_AND_THROW(msg, exc) { \
                                        SENTENCEBOUNDARIESUPDATERLOGINIT; \
                                        LERROR << msg; \
                                        throw exc; \
                                      }

#define LOG_ERROR_AND_EXIT(msg, err_code) { \
                                            SENTENCEBOUNDARIESUPDATERLOGINIT; \
                                            LERROR << msg; \
                                            return err_code; \
                                          }

namespace
{
inline string THIS_FILE_LOGGING_CATEGORY()
{
  SENTENCEBOUNDARIESUPDATERLOGINIT;
  return logger.zone().toStdString();
}
}

#if defined(DEBUG_LP) && defined(DEBUG_THIS_FILE)
  #define LOG_MESSAGE(stream, msg) { stream << msg; }
  #define LOG_MESSAGE_WITH_PROLOG(stream, msg) SENTENCEBOUNDARIESUPDATERLOGINIT; LOG_MESSAGE(stream, msg);
#else
  #define LOG_MESSAGE(stream, msg) ;
  #define LOG_MESSAGE_WITH_PROLOG(stream, msg) ;
#endif

static SimpleFactory<MediaProcessUnit, SentenceBoundariesUpdater> sentenceboundariesupdaterFactory(SENTENCEBOUNDARIESUPDATER_CLASSID); // clazy:exclude=non-pod-global-static

class SentenceBoundariesUpdaterPrivate : public ConfigurationHelper
{
public:
  SentenceBoundariesUpdaterPrivate();
  virtual ~SentenceBoundariesUpdaterPrivate();

  void init(XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration);
  LimaStatusCode process(AnalysisContent& analysis) const;

private:
  size_t updateBoundaries(SegmentationData& sb, const AnalysisGraph& graph) const;

  string m_graphName;
  string m_sbLayerName;
};

SentenceBoundariesUpdaterPrivate::SentenceBoundariesUpdaterPrivate()
  : ConfigurationHelper("SentenceBoundariesUpdater", THIS_FILE_LOGGING_CATEGORY())
{
}

SentenceBoundariesUpdaterPrivate::~SentenceBoundariesUpdaterPrivate()
{
}

SentenceBoundariesUpdater::SentenceBoundariesUpdater() : m_d(new SentenceBoundariesUpdaterPrivate())
{
}

SentenceBoundariesUpdater::~SentenceBoundariesUpdater()
{
  delete m_d;
}

void SentenceBoundariesUpdater::init(
  XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)
{
  LIMA_UNUSED(manager);
  m_d->init(unitConfiguration);
}

LimaStatusCode SentenceBoundariesUpdater::process(AnalysisContent& analysis) const
{
  TimeUtilsController SentenceBoundariesUpdaterProcessTime("SentenceBoundariesUpdater");

  LimaStatusCode code = m_d->process(analysis);

  TimeUtils::logElapsedTime("SentenceBoundariesUpdater::process");

  return code;
}

void SentenceBoundariesUpdaterPrivate::init(XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration)
{
  m_graphName = getStringParameter(unitConfiguration, "graph", ConfigurationHelper::REQUIRED | ConfigurationHelper::NOT_EMPTY);
  m_sbLayerName = getStringParameter(unitConfiguration, "data", ConfigurationHelper::REQUIRED | ConfigurationHelper::NOT_EMPTY);
}

LimaStatusCode SentenceBoundariesUpdaterPrivate::process(AnalysisContent& analysis) const
{
  AnalysisGraph* graph = static_cast<AnalysisGraph*>(analysis.getData(m_graphName));
  if (graph == nullptr)
  {
    LOG_ERROR_AND_EXIT("SentenceBoundariesUpdaterPrivate::process graph" << m_graphName << "has not been produced: check pipeline",
                       MISSING_DATA);
  }

  SegmentationData* sb = static_cast<SegmentationData*>(analysis.getData(m_sbLayerName));
  if (sb == nullptr)
  {
    LOG_ERROR_AND_EXIT("SentenceBoundariesUpdaterPrivate::process segmentation data" << m_sbLayerName << "has not been produced: check pipeline",
                       MISSING_DATA);
  }

  updateBoundaries(*sb, *graph);

  return SUCCESS_ID;
}

size_t SentenceBoundariesUpdaterPrivate::updateBoundaries(SegmentationData& sb, const AnalysisGraph& graph) const
{
  SENTENCEBOUNDARIESUPDATERLOGINIT;
  size_t segmentsMerged = 0;
  vector<Segment>& segments = sb.getSegments();
  map<LinguisticGraphVertex, vector<Segment>::iterator> lastVerticies;

  for (auto it = segments.begin(); it != segments.end(); it++)
  {
    LinguisticGraphVertex v = it->getLastVertex();
    if (lastVerticies.end() != lastVerticies.find(v))
    {
      LOG_ERROR_AND_EXIT("SentenceBoundariesUpdaterPrivate::updateBoundaries: vertex" << v << "mentioned twice as segment's end",
                         UNKNOWN_ERROR);
    }
    lastVerticies[v] = it;
  }

  list<pair<vector<Segment>::iterator, vector<Segment>::iterator>> segments_to_merge;
  for (auto it = segments.begin(); it != segments.end(); it++)
  {
    set<LinguisticGraphVertex> visited;
    list<LinguisticGraphVertex> toVisit;
    toVisit.push_back(it->getFirstVertex());

    while (!toVisit.empty())
    {
      LinguisticGraphVertex currentVertex = toVisit.front();
      toVisit.pop_front();
      visited.insert(currentVertex);

      auto iter = lastVerticies.find(currentVertex);
      if (lastVerticies.end() != iter)
      {
        if (iter->second > it)
        {
          // This is the end of one of following segments
          segments_to_merge.push_back(make_pair(it, iter->second));
          it = iter->second;
          break;
        }
        else if (iter->second == it)
        {
          break;
        }
      }

      for ( LinguisticGraphVertex next : getFollowingNodes<SetOfLinguisticGraphVertices>(graph, currentVertex) )
      {
        if (visited.end() == visited.find(next))
        {
          toVisit.push_back(next);
        }
      }
    }
  }

  segments_to_merge.reverse();
  for (const pair<vector<Segment>::iterator, vector<Segment>::iterator>& p : segments_to_merge)
  {
    segmentsMerged += p.second - p.first;

    // Update FirstVertex
    p.second->setFirstVertex(p.first->getFirstVertex());
    uint64_t c = 0;
    for (vector<Segment>::iterator i = p.first; i != p.second; i++)
    {
      c += i->getLength();
    }

    // Update Length
    p.second->setLength(p.second->getLength() + c);

    // Erase vertices starting from p.first until p.second (p.second is not erased)
    segments.erase(p.first, p.second);
  }

  LDEBUG << "segmentsMerged==" << segmentsMerged;
  return segmentsMerged;
}

} // namespace SentenceBoundariesUpdater
} // namespace TextSegmentation
} // namespace LinguisticProcessing
} // namespace Lima
