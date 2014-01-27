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
/***************************************************************************
 *   Copyright (C) 2008 by CEA LIST                              *
 *                                                                         *
 ***************************************************************************/

#include "CorefSolvingNormalizedXmlLogger.h"
#include "coreferentAnnotation.h"
#include "linguisticProcessing/core/Automaton/SpecificEntityAnnotation.h"

#include "common/Data/strwstrtools.h"
#include "common/MediaticData/mediaticData.h"
#include "common/time/traceUtils.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/Token.h"

#include <boost/regex.hpp>
#include <fstream>

using namespace std;
using namespace Lima::Common::Misc;
using namespace Lima::Common::AnnotationGraphs;

namespace Lima
{
  namespace LinguisticProcessing
  {
    namespace Coreferences
    {

      SimpleFactory<MediaProcessUnit,CorefSolvingNormalizedXmlLogger> CorefSolvingNormalizedXmlLoggerFactory(COREFSOLVINGNORMALIZEDXMLLOGGER_CLASSID);

      CorefSolvingNormalizedXmlLogger::CorefSolvingNormalizedXmlLogger() :
          AbstractLinguisticLogger(".output.xml"),
          m_language(0),
          m_graph("PosGraph")
      {}

      CorefSolvingNormalizedXmlLogger::~CorefSolvingNormalizedXmlLogger()
      {}

      void CorefSolvingNormalizedXmlLogger::init(
        Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
        Manager* manager)
      
      {
//         COREFSOLVERLOGINIT;
        AbstractLinguisticLogger::init(unitConfiguration,manager);

        m_language=manager->getInitializationParameters().media;
      }
      LimaStatusCode CorefSolvingNormalizedXmlLogger::process(
        AnalysisContent& analysis) const
      {
//         COREFSOLVERLOGINIT;
        TimeUtils::updateCurrentTime();
        AnnotationData* annotationData = static_cast<AnnotationData*>(analysis.getData("AnnotationData"));
        const LinguisticAnalysisStructure::AnalysisGraph& graph = *(static_cast<LinguisticAnalysisStructure::AnalysisGraph*>(analysis.getData(m_graph)));

//         LinguisticGraph* lingGraph = const_cast<LinguisticGraph*>(graph.getGraph());
        LinguisticMetaData* metadata=static_cast<LinguisticMetaData*>(analysis.getData("LinguisticMetaData"));
        if (metadata == 0)
        {
          COREFSOLVERLOGINIT;
          LERROR << "no LinguisticMetaData ! abort" << LENDL;
          return MISSING_DATA;
        }

        ofstream out;
        if (!openLogFile(out,metadata->getMetaData("FileName")))
        {
          COREFSOLVERLOGINIT;
          LERROR << "Can't open log file " << LENDL;
          return UNKNOWN_ERROR;
        }

        out << "<coreferences>" << endl;


        //   LDEBUG << "CorefSolvingNormalizedXmlLogger on graph " << m_graph << LENDL;
        AnnotationGraphVertexIt itv, itv_end;
        boost::tie(itv, itv_end) = vertices(annotationData->getGraph());
        for (; itv != itv_end; itv++)
        {
          // process
          //LDEBUG << "CorefSolvingNormalizedXmlLogger on annotation vertex " << *itv << LENDL;
          if (annotationData->hasAnnotation(*itv,utf8stdstring2limastring("Coreferent")))
            //if (annotationData->hasAnnotation(*itv,utf8stdstring2limastring("Coreferent")))
          {
            CoreferentAnnotation* annot ;
            try
            {
              annot = annotationData->annotation(*itv,utf8stdstring2limastring("Coreferent"))
                      .pointerValue<CoreferentAnnotation>();
            }
            catch (const boost::bad_any_cast& )
            {
              COREFSOLVERLOGINIT;
              LERROR << "One annotation on vertex " << *itv << " you are trying to cast is not a Coreference; Coreference not logged" << LENDL;
              for (int i = 0; i < 19 ; i++)
              {
                LERROR << "annot "<< i << " : " << limastring2utf8stdstring(annotationData->annotationName(i)) << LENDL ;
              }
              continue;
            }
            LinguisticProcessing::LinguisticAnalysisStructure::Token* token = get(vertex_token, *graph.getGraph(), annot->morphVertex());
            if (token == 0)
            {
              COREFSOLVERLOGINIT;
              LERROR << "Vertex " << *itv << " has no entry in the analysis graph token map. This should not happen !!" << LENDL;
            }
            else
            {
              CoreferentAnnotation* antecedent;
//               bool hasAntecedent = false;
              AnnotationGraphOutEdgeIt it, it_end;
              boost::tie(it, it_end) = boost::out_edges(static_cast<AnnotationGraphVertex>(*itv), annotationData->getGraph());

              for (; it != it_end; it++)
              {
                if (annotationData->hasAnnotation(target(*it,annotationData->getGraph()),utf8stdstring2limastring("Coreferent")))
                {
                  try
                  {
                    antecedent = annotationData->annotation(target(*it, annotationData->getGraph()), utf8stdstring2limastring("Coreferent")).pointerValue<CoreferentAnnotation>();
//                     hasAntecedent = true;
                  }
                  catch (const boost::bad_any_cast& )
                  {
                    COREFSOLVERLOGINIT;
                    LERROR << "One annotation on vertex you are trying to cast resulting from an edge out of " << *itv << " is not a Coreference; Coreference not logged" << LENDL;
                    continue;
                  }
                }
              }
              out << "  <reference>\n"
              << "    <pos>" << get(vertex_token,*graph.getGraph(),annot->morphVertex())->position() << "</pos>\n"
              << "    <len>" << token->stringForm().length() << "</len>\n"
              << "    <string>"<< limastring2utf8stdstring(transcodeToXmlEntities(token->stringForm())) << "</string>\n"
              << "    <npId>" << annot->id() << "</npId>\n"
              << "    <posVertex>" << annot->morphVertex() << "</posVertex>\n";
              //if (hasAntecedent)
              if (false)
              {
                out << "    <npRef>" << antecedent->id() << "</npRef>\n";
                out << "    <refPosVertex>" << antecedent->morphVertex() << "</refPosVertex>\n";
              }
              out << "    <categ>" << annot->categ() << "</categ>\n"
                    << "  </reference>\n"
              << endl;
            }
          }
        }
        out << "</coreferences>" << endl;
        out.close();

        TimeUtils::logElapsedTime("CorefSolvingNormalizedXmlLogger");
        return SUCCESS_ID;

      }


    } // Coreferences
  } // LinguisticProcessing
} // Lima
