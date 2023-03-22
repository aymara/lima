// Copyright 2002-2020 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_MORPHOLOGICANALYSIS_APPROXSTRINGMATCHER_H
#define LIMA_MORPHOLOGICANALYSIS_APPROXSTRINGMATCHER_H

#include "linguisticProcessing/core/MorphologicAnalysis/MorphologicAnalysisExport.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "linguisticProcessing/core/FlatTokenizer/CharChart.h"
#include "common/MediaProcessors/MediaProcessUnit.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "linguisticProcessing/core/LinguisticResources/AbstractAccessResource.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
//#include "linguisticProcessing/core/Automaton/SpecificEntityAnnotation.h"

#include "NameIndexResource.h"

namespace Lima
{
namespace LinguisticProcessing
{
namespace MorphologicAnalysis
{

#define APPROX_STRING_MATCHER_CLASSID "ApproxStringMatcher"

// TODO  : à mettre dans le .cpp avec une implémentation private
typedef struct _Suggestion {
  // Position of candidate (number of unicode character) in search string
  int startPosition;
  // length of candidate (number of unicode character)
  int endPosition;
  // number of errors (delete/add) to match
  int nb_error;
  // id of target in lexicon ??
  // long unsigned int match_id;
} Suggestion;

LIMA_DATA_EXPORT  std::ostream& operator<<(std::ostream &os, const Suggestion& node);
LIMA_DATA_EXPORT  QDebug& operator<<(QDebug& os, const Suggestion& node);

// TODO  : à mettre dans le .cpp avec une implémentation private
typedef struct _Solution {
  // position of matched string in text
  Suggestion suggestion;
  // list of vertex in graph
  std::deque<LinguisticGraphVertex> vertices;
  // matched string
  LimaString form;
  // normalized string = matched element in lexicon
  LimaString normalizedForm;
  // length of matched string in text
  uint64_t length;
} Solution;

typedef struct _SolutionCompare {
  bool operator() (const Solution& s1, const Solution& s2) const;
} SolutionCompare;

LIMA_DATA_EXPORT  std::ostream& operator<<(std::ostream &os, const Solution& solution);
LIMA_DATA_EXPORT  QDebug& operator<<(QDebug& os, const Solution& solution);

class LIMA_MORPHOLOGICANALYSIS_EXPORT ApproxStringMatcher : public MediaProcessUnit
{
  typedef std::pair< std::basic_string<wchar_t>, std::basic_string<wchar_t> > Regex;
  typedef std::vector<Regex> RegexMap;
  typedef std::set<Solution,SolutionCompare> OrderedSolution;

public:
  ApproxStringMatcher();
  virtual ~ApproxStringMatcher();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;

  LimaStatusCode process(
    AnalysisContent& analysis) const override;


private:
  void matchApproxTokenAndFollowers(
    LinguisticGraph& g,
    LinguisticGraphVertex vStart,
    LinguisticGraphVertex vEnd,
    std::pair<NameIndex::const_iterator,NameIndex::const_iterator> nameRange,
    OrderedSolution& result) const;

  void computeVertexMatches(
    const LinguisticGraph& g,
    const LinguisticGraphVertex vStart,
    const LinguisticGraphVertex vEnd,
    const Suggestion& suggestion, Solution& tempResult) const;

  int findApproxPattern(
    const std::basic_string<wchar_t>& pattern, LimaString text,
    std::vector<Suggestion>& suggestions, int nbMaxError) const;

  void createVertex(
    LinguisticGraph& g,
    LinguisticGraphVertex vStart,
    LinguisticGraphVertex vEnd,
    const Solution& solution,
    Lima::Common::AnnotationGraphs::AnnotationData* annotationData ) const;

  QString wcharStr2LimaStr(const std::basic_string<wchar_t>& wstring) const;
  std::basic_string<wchar_t> LimaStr2wcharStr(const QString& limastr) const;
  std::basic_string<wchar_t> buildPattern(const std::basic_string<wchar_t>& normalizedForm) const;

  MediaId m_language;
  LinguisticCode m_neCode;
  LinguisticCode m_neMicroCode;
  Common::AbstractAccessByString *m_lexicon;
  FsaStringsPool* m_sp;
  int m_nbMaxNumError;
  int m_nbMaxDenError;
  Common::MediaticData::EntityType m_entityType;
  Common::MediaticData::EntityGroupId m_entityGroupId;
  RegexMap m_regexes;
  std::shared_ptr<NameIndexResource> m_nameIndex;
};
} // MorphologicAnalysis
} // LinguisticProcessing
} // Lima

#endif
