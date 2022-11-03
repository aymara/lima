// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/**
  *
  * @file        annotationGraphTestProcessUnit.cpp
  * @author      Gael de Chalendar (Gael.de-Chalendar@cea.fr) 

  *              Copyright (c) 2004 by CEA
  * @date        Created on Nov, 8 2004
  * @version     $Id$
  *
  */

#include "AnnotationGraphTestProcessUnit.h"

#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "common/time/traceUtils.h"
#include "common/Data/strwstrtools.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"

#include <stack>
#include <iostream>

using namespace std;
using namespace boost;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

namespace Lima
{
namespace LinguisticProcessing
{
namespace AnnotationGraphs
{

SimpleFactory<MediaProcessUnit,AnnotationGraphTestProcessUnit> annotationGraphTestFactory(ANNOTATIONGRAPHTESTPO_CLASSID);

AnnotationGraphTestProcessUnit::AnnotationGraphTestProcessUnit() :
    m_language()
{}

void AnnotationGraphTestProcessUnit::init(
                                           Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  LIMA_UNUSED(unitConfiguration);
//  SALOGINIT;
  m_language=manager->getInitializationParameters().media;
//  LanguageResources* lres=LinguisticResources::single().getLanguageResources(language);
}

LimaStatusCode AnnotationGraphTestProcessUnit::process(
  AnalysisContent& analysis) const
{
  TimeUtils::updateCurrentTime();
  SALOGINIT;
  LINFO << "start AnnotationGraphTestProcessUnit";
  // create syntacticData
  if (std::dynamic_pointer_cast<AnalysisGraph>(analysis.getData("AnalysisGraph"))==0)
  {
    LERROR << "no AnalysisGraph ! abort";
    return MISSING_DATA;
  }
  
  /** Creation of an annotation graph if necessary*/
  auto annotationData = std::dynamic_pointer_cast< AnnotationData >(analysis.getData("AnnotationData"));
  if (annotationData==0)
  {
    annotationData=new AnnotationData();
    /** Creates a node in the annotation graph for each node of the 
      * morphosyntactic graph. Each new node is annotated with the name mrphv and
      * associated to the morphosyntactic vertex number */
    if (std::dynamic_pointer_cast<AnalysisGraph>(analysis.getData("AnalysisGraph")) != 0)
    {
      std::dynamic_pointer_cast<AnalysisGraph>(analysis.getData("AnalysisGraph"))->populateAnnotationGraph(annotationData, "AnalysisGraph");
    }
    if (std::dynamic_pointer_cast<AnalysisGraph>(analysis.getData("PosGraph")) != 0)
    {
      std::dynamic_pointer_cast<AnalysisGraph>(analysis.getData("PosGraph"))->populateAnnotationGraph(annotationData, "PosGraph");
    }
    
    analysis.setData("AnnotationData",annotationData);
  }
  
  /** Creation of an object pt that will be used as an annotation */
  Point pt; pt.x = 3; pt.y = 5;
  
  /** Creation of an annotation for the object pt */
  GenericAnnotation ga(pt);
  
  /** Creation of a new vertex (a new annotation anchor) in the annotation 
    * graph. */
  AnnotationGraphVertex v = annotationData->createAnnotationVertex();
  
  /** The new vertex is annotated with the annotation containing pt */ 
  annotationData->annotate(v, Common::Misc::utf8stdstring2limastring("Point"), ga);
  
  /** To be able to dump the content of an annotation, a function pointer with
    * a precise signature has to be givent to the annotation graph. See
    * @ref{annotationGraphTestProcessUnit.h} for the details of the dumpPoint 
    * function */
  if (annotationData->dumpFunction("Point") == 0)
  {
    annotationData->dumpFunction("Point", new DumpPoint());
  }
  
  TimeUtils::logElapsedTime("AnnotationData");
  return SUCCESS_ID;
}

/** @brief Definition of a function suitable to be used as a dumper for Point
  * annotations of an annotation graph 
  * @param @b os <I>std::ostream&amp;</I> the stream on which to dump the point 
  * @param @b ga <I>GenericAnnotation&amp;</I> the point annotation to be 
  *        dumped. 
  * @return <I>int</I> If succesful, SUCCESS_ID is returned. An error is 
  *         displayed in the case where the given annotation is not a Point 
  *         annotation and UNKNOWN_ERROR is returned .
  */
int DumpPoint::dump(std::ostream& os, GenericAnnotation& ga) const
{
  PROCESSORSLOGINIT;
  try
  {
    ga.value<Point>().dump(os);
    return SUCCESS_ID;
  }
  catch (const boost::bad_any_cast& e)
  {
    LERROR << "This annotation is not a Point ; nothing dumped";
    return UNKNOWN_ERROR;
  }
}


} // closing namespace AnnotationGraphs
} // closing namespace LinguisticProcessing
} // closing namespace Lima
