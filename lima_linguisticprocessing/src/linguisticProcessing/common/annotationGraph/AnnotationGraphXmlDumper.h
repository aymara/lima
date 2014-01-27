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
/** @brief      dump the full content of the annotation graph in XML format
  *
  * @file       AnnotationGraphXmlDumper.h
  * @author     Gael de Chalendar <Gael.de-Chalendar@cea.fr> 

  *             Copyright (C) 2005-2012 by CEA LIST
  * @date       Mon Nov  23 2005
  * Project     s2common
  *
  */

#ifndef LIMA_COMMON_ANNOTATIONGRAPHS_ANNOTATIONGRAPHXMLDUMPER_H
#define LIMA_COMMON_ANNOTATIONGRAPHS_ANNOTATIONGRAPHXMLDUMPER_H

#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "AnnotationGraph.h"
#include "AnnotationData.h"
#include "common/Data/LimaString.h"
#include "common/misc/fsaStringsPool.h"

#include "common/misc/depth_first_searchnowarn.hpp"



#include <map>
#include <iostream>
#include <set>


namespace Lima {
namespace Common {
namespace AnnotationGraphs {

/** @brief This class dumps an annotation graph to a given stream in an XML format.
 * It uses a dpeth first search to output the graph and the annotations dump
 * functions if available. Next, it dumps the stored matchings.
*/
class LIMA_ANNOTATIONGRAPH_EXPORT AnnotationGraphXmlDumper
{
public:

  AnnotationGraphXmlDumper();

  virtual ~AnnotationGraphXmlDumper();

  void dump(std::ostream& os, const AnnotationGraph* graph,
            const AnnotationData& annotationData) const;

  protected:
    /**
    * a visitor for going through the graph between two points
    * and storing a BoWToken for every vertex that are not part
    * of a complex token
    */
    class DumpGraphVisitor : public boost::default_dfs_visitor
    {
        std::ostream& m_os;
        const AnnotationData& m_annotData;

     public:
        DumpGraphVisitor(std::ostream& os, const AnnotationData& ad) :
          m_os(os), m_annotData(ad)
        {
        }

        void discover_vertex(AnnotationGraphVertex v,
                             const AnnotationGraph& graph);

        void examine_edge(AnnotationGraphEdge e, const AnnotationGraph& g);
	private:
		DumpGraphVisitor& operator=(const DumpGraphVisitor& ) {return *this;}
    };

};

} // end namespace AnnotationGraphs
} // end namespace Common
} // end namespace Lima

#endif // LIMA_COMMON_ANNOTATIONGRAPHS_ANNOTATIONGRAPHXMLDUMPER_H
