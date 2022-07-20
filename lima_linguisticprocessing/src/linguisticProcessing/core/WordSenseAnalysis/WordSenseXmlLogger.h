// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_WORDSENSEDISAMBIGUATION_WSXMLLOGGER_H
#define LIMA_WORDSENSEDISAMBIGUATION_WSXMLLOGGER_H

/** @brief      xml logger for Word Senses 
  *
  * @file       WordSenseXmlLogger.h
  * @author     Claire Mouton <claire.mouton@cea.fr>
  *             Copyright (c) 2010 by CEA
  * @version    $Id: WordSenseXmlLogger.h 2010-08-18 claire $
  */



#include "WordSenseAnalysisExport.h"
#include "linguisticProcessing/common/PropertyCode/PropertyCodeManager.h"
#include "linguisticProcessing/common/misc/AbstractLinguisticLogger.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationGraph.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/core/SyntacticAnalysis/SyntacticData.h"

namespace Lima
{
namespace LinguisticProcessing
{
namespace WordSenseDisambiguation
{


#define WORDSENSEXMLLOGGER_CLASSID "WordSenseXmlLogger"

  

class LIMA_WORDSENSEANALYSIS_EXPORT DumpXMLAnnotationVisitor : public boost::default_bfs_visitor
{
  std::ostream& m_ostream;
  Common::AnnotationGraphs::AnnotationData* m_ad;
  MediaId m_language;
  LinguisticGraphEdge m_lastEdge;
  std::string m_memo;

public:
  DumpXMLAnnotationVisitor(std::ostream& os,
  Common::AnnotationGraphs::AnnotationData* ad,
  MediaId language):
  m_ostream(os),m_ad(ad),m_language(language), m_lastEdge(LinguisticGraphEdge()),m_memo("") {}



  void examine_edge(LinguisticGraphEdge e,
                    const LinguisticGraph& g); 



};

class LIMA_WORDSENSEANALYSIS_EXPORT WordSenseXmlLogger : public AbstractLinguisticLogger
{

public:

  WordSenseXmlLogger();
  virtual ~WordSenseXmlLogger();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;

  LimaStatusCode process(
    AnalysisContent& analysis) const override;

protected:

private:

  std::string m_outputSuffix;
  std::string m_graphId;
  MediaId m_language;
  DependencyGraphEdge m_lastEdge;
  std::string m_memo;

  void dump(
    std::ostream& fileName, 
    LinguisticAnalysisStructure::AnalysisGraph* g,
    Common::AnnotationGraphs::AnnotationData* ad) const;

};


} // WordSenseDisambiguation
} // LinguisticProcessing
} // Lima


#endif // WORDSENSEXMLLOGGER_H
