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
    Manager* manager)
  ;

  LimaStatusCode process(
    AnalysisContent& analysis) const;

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
