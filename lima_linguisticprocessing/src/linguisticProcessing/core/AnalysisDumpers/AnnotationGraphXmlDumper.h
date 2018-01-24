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
/**
  *
  * @file       annotationGraphXmlDumper.h
  * @author     Gael de Chalendar <Gael.de-Chalendar@cea.fr> 

  *             Copyright (C) 2004 by CEA LIST
  * @date       Mon Nov  8 2004
  * @version    $Id$
  * Project     s2lp
  * 
  * @brief      dump the content of the annotation graph in XML format
  * 
  * 
  */

#ifndef LIMA_LINGUISTICPROCESSINGS_ANALYSISDUMPERS_ANNOTATIONGRAPHXMLDUMPER_H
#define LIMA_LINGUISTICPROCESSINGS_ANALYSISDUMPERS_ANNOTATIONGRAPHXMLDUMPER_H

#include "AnalysisDumpersExport.h"

#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "common/MediaProcessors/MediaProcessUnit.h"

#include <boost/tuple/tuple.hpp>
#include <boost/graph/properties.hpp>
#include "common/misc/depth_first_searchnowarn.hpp"
#include <boost/graph/breadth_first_search.hpp>

#include <map>
#include <iostream>
#include <set>
// #include <ext/hash_set>

namespace Lima {
namespace LinguisticProcessing {
namespace AnalysisDumpers {
  class FullXmlDumper;
}
}
}

namespace Lima {
namespace LinguisticProcessing {
namespace AnalysisDumpers {

#define ANNOTATIONGRAPHXMLDUMPER_CLASSID "AnnotationGraphXmlDumper"

class LIMA_ANALYSISDUMPERS_EXPORT AnnotationGraphXmlDumper : public MediaProcessUnit 
{
friend class Lima::LinguisticProcessing::AnalysisDumpers::FullXmlDumper;

public:

  AnnotationGraphXmlDumper();
  
  virtual ~AnnotationGraphXmlDumper();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;

  LimaStatusCode process(
    AnalysisContent& analysis) const override;

  LimaStatusCode dump(
    AnalysisContent& analysis, const std::string& prologue = "" ) const;

private:
  // constructor to be used by FullXmlDumper
  AnnotationGraphXmlDumper( const MediaId& language, const std::string& handler );
protected:
    /**
    * a visitor for going through the graph between two points
    * and storing a BoWToken for every vertex that are not part
    * of a complex token
    */
    class DumpGraphVisitor : public boost::default_dfs_visitor
    {
        const AnnotationGraphXmlDumper& m_dumper;
        const Common::AnnotationGraphs::AnnotationData& m_data;
        std::ostream& m_os;
        MediaId m_language;
        
        public:
        DumpGraphVisitor(const AnnotationGraphXmlDumper& dumper,
                         const Common::AnnotationGraphs::AnnotationData& annotData,
                        std::ostream& outputStream,
                        MediaId language):
              m_dumper(dumper),
              m_data(annotData),
              m_os(outputStream), 
              m_language(language)
        {
        }
        
        void discover_vertex(AnnotationGraphVertex v, const AnnotationGraph& g);
        void examine_edge(AnnotationGraphEdge e, const AnnotationGraph& g);
        void examine_vertex(AnnotationGraphVertex v, const AnnotationGraph& g);
        class EndOfSearch : public std::runtime_error 
        {
        public:
            EndOfSearch():std::runtime_error("") {}
        };
    };
    
    void dumpLimaData(std::ostream& os,
                       const Common::AnnotationGraphs::AnnotationData& annotData) const;
                            
    void outputVertex(const AnnotationGraphVertex v,
                      const AnnotationGraph& graph,
                      const Common::AnnotationGraphs::AnnotationData& annotData,
                      std::ostream& xmlStream) const;
                      
    void outputEdge(const AnnotationGraphEdge e,
                    const AnnotationGraph& graph,
                    const Common::AnnotationGraphs::AnnotationData& annotData,
                    std::ostream& xmlStream) const;
                    
    void outputVertexIAnnotations(const AnnotationGraphVertex v,
                      const AnnotationGraph& graph,
                      const Common::AnnotationGraphs::AnnotationData& annotData,
                      std::ostream& xmlStream) const;
                      
    void outputVertexSAnnotations(const AnnotationGraphVertex v,
                      const AnnotationGraph& graph,
                      const Common::AnnotationGraphs::AnnotationData& annotData,
                      std::ostream& xmlStream) const;
                      
    void outputVertexGAnnotations(const AnnotationGraphVertex v,
                      const AnnotationGraph& graph,
                      const Common::AnnotationGraphs::AnnotationData& annotData,
                      std::ostream& xmlStream) const;
                      
    void outputEdgeIAnnotations(const AnnotationGraphEdge e,
                    const AnnotationGraph& graph,
                    const Common::AnnotationGraphs::AnnotationData& annotData,
                    std::ostream& xmlStream) const;
                    
    void outputEdgeSAnnotations(const AnnotationGraphEdge e,
                    const AnnotationGraph& graph,
                    const Common::AnnotationGraphs::AnnotationData& annotData,
                    std::ostream& xmlStream) const;
    
    void outputEdgeGAnnotations(const AnnotationGraphEdge e,
                    const AnnotationGraph& graph,
                    const Common::AnnotationGraphs::AnnotationData& annotData,
                    std::ostream& xmlStream) const;
    
                    MediaId m_language;
    std::string m_handler;
};

} // end namespace AnalysisDumpers
} // end namespace LinguisticProcessings
} // end namespace Lima

#endif // LIMA_LINGUISTICPROCESSINGS_ANALYSISDUMPERS_ANNOTATIONGRAPHXMLDUMPER_H
