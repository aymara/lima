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
/** @brief       Debug logger for dependency graphs
  *
  * @file        SyntacticAnalysisXmlLogger.h
  * @author      Gael de Chalendar (Gael.de-Chalendar@cea.fr) 

  *              Copyright (c) 2005 by CEA
  * @date        
  *
  */

#ifndef SYNTACTICANALYSISXMLLOGGERSPECIFICENTITIESXMLLOGGER_H
#define SYNTACTICANALYSISXMLLOGGERSPECIFICENTITIESXMLLOGGER_H

#include "SyntacticAnalysisExport.h"
#include "linguisticProcessing/core/LinguisticProcessors/AbstractTextualAnalysisDumper.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/Token.h"


namespace Lima
{
namespace LinguisticProcessing
{
namespace SyntacticAnalysis
{

class SyntacticData;

#define SYNTACTICANALYSISXMLLOGGER_CLASSID "SyntacticAnalysisXmlLogger"

/** 
 * @brief This MediaProcessUnit is a dependency graph XML logger 
 */
class LIMA_SYNTACTICANALYSIS_EXPORT SyntacticAnalysisXmlLogger : public AbstractTextualAnalysisDumper
{
public:
  /** @brief Default constructor */
  SyntacticAnalysisXmlLogger();

  /** @brief Default destructor. Nothing to do. */
  virtual ~SyntacticAnalysisXmlLogger();

  /**
   * @brief MediaProcessUnit initialization method
   * @param unitConfiguration @b IN <I>GroupConfigurationStructure&</I> Access
   * to configuration file data.
   * @param manager @b IN <I>Manager*</I>Access to the analysis data (language 
   * in particular).
   * @throws InvalidConfiguration In case of errors in the configuration file.
   * @throws std::exception Other errors.
   */
  virtual void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override
  ;

  /**
   * @brief Do the job of logging the dependency graph of the given text 
   * analysis.
   * @param analysis @b IN <I>AnalysisContent&</I> The analysis data containing 
   * the dependency graph to log.
   * @return <I>LimaStatusCode</I> The error status of the logging.
   */
  virtual LimaStatusCode process(
    AnalysisContent& analysis) const override;

private:
  
  /**
    * @brief The main dumper method called on each sentence.
    * @param os @b OUT <I>std::ostream&</I> The stream on which to dump.
    * @param begin @b IN <I>LinguisticGraphVertex</I>The first vertex of the 
    * sentence to be dumped.
    * @param end  @b IN <I>LinguisticGraphVertex</I>The last vertex of the 
    * sentence to be dumped.
    * @param pit @b IN <I>const LinguisticAnalysisStructure::AnalysisGraph*</I>The morphologic 
    * analysis graph
    * @param syntacticData @b IN <I>const SyntacticData*</I>The structure 
    * giving access to the dependency graph of the analysis.
    * @param offsetBegin @b IN <I>const uint64_t</I>The offset of the 
    * sentence first word (defaulted to 0) 
    */
  void dumpLimaData(std::ostream& os,
                  const LinguisticGraphVertex begin,
                  const LinguisticGraphVertex end,
                  const LinguisticAnalysisStructure::AnalysisGraph* pit,
                  const SyntacticAnalysis::SyntacticData* syntacticData,
                  const uint64_t offsetBegin=0) const;
  /**
  * @brief A visitor for going through the graph between two points and 
    * storing a BoWToken for every vertex that are not part of a complex token
    * 
    * This is a Boost Graph Library breadth first search visitor 
    * (http://www.boost.org/). An exception is thrown to interrupt the search 
    * on the stop vertex. 
  */
  class DumpGraphVisitor : public boost::default_bfs_visitor
  {
    /** @brief A reference to the logger using this visitor.*/
    const SyntacticAnalysisXmlLogger& m_dumper;
    /** @brief The stream on which to dump.*/
    std::ostream& m_os;
    /** @brief The vertex on which to stop the visit.*/
    LinguisticGraphVertex m_lastVertex;
    /** @brief The structure containing the dependency graph currently dumped. */
    const SyntacticAnalysis::SyntacticData* m_syntacticData;
    /** @brief The language of the analyzed text.*/
    MediaId m_language;
    /** @brief The offset of the sentence first word.*/
    uint64_t m_offsetBegin;
    /** @brief Associates a pointer to a FullToken to an int id. Allows to 
      * uniqly reference a same FullToken all along a sentence.*/
    std::map< LinguisticAnalysisStructure::Token*, uint64_t >& m_tokens;
    /** @brief For each FullToken of a sentence identified by its id, stores 
      * true if it has already been dumped and false otherwise.
      *
      * Usefull as a same FullToken can appear more than one time in a 
      * disambiguated analysis.
      */
    std::vector< bool >& m_alreadyDumpedTokens;
    
    public:
    /** @brief Uniq constructor. See the class members for the definition of 
      * its arguments.*/
    DumpGraphVisitor(const SyntacticAnalysisXmlLogger& dumper,
                    std::ostream& outputStream,
                    const LinguisticGraphVertex& end,
                    const SyntacticAnalysis::SyntacticData* syntacticData,
                    std::map< LinguisticAnalysisStructure::Token*, uint64_t >& tokens,
                    std::vector< bool >& alreadyDumpedTokens,
                    MediaId language,
                    const uint64_t offsetBegin):
          m_dumper(dumper),
          m_os(outputStream), 
          m_lastVertex(end), 
          m_syntacticData(syntacticData),
    m_language(language), 
    m_offsetBegin(offsetBegin), 
    m_tokens(tokens),
    m_alreadyDumpedTokens(alreadyDumpedTokens)
    {
    }
    
    /**
      * @brief Callback called by the BGL breadth first search algorithm when 
      * finding a vertex for the first time.
      * @param v @b IN <I>LinguisticGraphVertex</I> The vertex discovered.
      * @param g @b IN <I>LinguisticGraph&</I> The graph being searched.
      */
    void discover_vertex(LinguisticGraphVertex v, const LinguisticGraph& g);
    
    /**
      * @brief Callback called by the BGL breadth first search algorithm when 
      * reaching a vertex.
      * @param v @b IN <I>LinguisticGraphVertex</I> The vertex discovered.
      * @param g @b IN <I>LinguisticGraph&</I> The graph being searched.
      */
    void examine_vertex(LinguisticGraphVertex v, const LinguisticGraph& g);

    /**
      * @brief Callback called by the BGL breadth first search algorithm when 
      * traversing an edge.
      * @param v @b IN <I>LinguisticGraphEdge</I> The edge traversed.
      * @param g @b IN <I>LinguisticGraph&</I> The graph being searched.
      */
    void examine_edge(LinguisticGraphEdge e, const LinguisticGraph& g);
    
    /** @brief Exception used to interrupt the search when reaching the stop
      * vertex. It is needed as the BGL BFS doesn't give other interruption
      * mean.
      */
    class EndOfSearch : public std::runtime_error 
    {
    public:
        EndOfSearch():std::runtime_error("") {}
    };
  };
  

  /**
   * @brief Converts the sum of a position and an offset into an effective 
   * position string
   * @param position @b IN <I>const uint64_t</I>The position.
   * @param offsetBegin  @b IN <I>const uint64_t</I>The offest.
   * @return <I>LimaString</I>string representation of the sum of 
   * @ref position and @ref offsetBegin
   */
  static LimaString getPosition(const uint64_t position,
                                  const uint64_t offsetBegin);
  
  /**
   * @brief Dumps a vertex.
   * @param v @b IN <I></I> The vertex to dump.
   * @param graph @b IN <I></I> The graph containing this vertex.
   * @param offsetBegin @b IN <I>const uint64_t</I>The offset of the 
   * sentence first word (defaulted to 0) 
   * @param syntacticData @b IN <I>const SyntacticData*</I>The structure 
   * giving access to the dependency graph of the analysis.
   * @param xmlStream @b OUT <I>std::ostream&</I> The stream on which to dump.
   * @param fullTokens @b IN/OUT <I>std::map\< LinguisticAnalysisStructure::Token*, 
   * uint64_t \>&</I> The map which associates pointers toward FullTokens 
   * to their uniq integer id.
   * @param alreadyDumpedFullTokens @b IN/OUT <I>std::vector< bool >&</I> 
   * The vector identifying already dumped tokens. The indices of the vector 
   * are the FullTokens ids.
   */
  void outputVertex(const LinguisticGraphVertex v,
                    const LinguisticGraph& graph,
                    const uint64_t offsetBegin,
                    const SyntacticAnalysis::SyntacticData* syntacticData,
                    std::ostream& xmlStream,
                    std::map< LinguisticAnalysisStructure::Token*, uint64_t >& fullTokens,
                    std::vector< bool >& alreadyDumpedFullTokens) const;
                    
  /**
   * @brief Dumps an edge.
   * @param e @b IN <I>const LinguisticGraphEdge</I> The edge to dump.
   * @param graph @b IN <I>const LinguisticGraph&</I> The graph containing this edge.
   * @param xmlStream @b OUT <I>std::ostream&</I> The stream on which to dump.
   */
  void outputEdge(const LinguisticGraphEdge e,
                  const LinguisticGraph& graph,
                  std::ostream& xmlStream) const;
  
  /** @brief The language of the analyzed text.*/
  MediaId m_language;
  
  /** @brief The suffix used to name the output file.
   * 
   * It is taken at init time from the configuration file. The beginning of the 
   * filename is the text file name found in the text metadata.
   */
  const Common::PropertyCode::PropertyCodeManager* m_propertyCodeManager;

};

} // SyntacticAnalysis
} // LinguisticProcessing
} // Lima

#endif
