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
 *   Copyright (C) 2004-2012 by CEA LIST                              *
 *                                                                         *
 ***************************************************************************/
#include "UnknownWordLogger.h"

#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "common/time/traceUtils.h"
#include "common/Data/strwstrtools.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticData.h"

#include <fstream>


using namespace boost;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace std;

namespace Lima
{

namespace LinguisticProcessing
{

namespace MorphologicAnalysis
{

SimpleFactory<MediaProcessUnit,UnknownWordLogger> unknownWordLoggerFactory(UNKNOWNWORDLOGGER_CLASSID);

UnknownWordLogger::UnknownWordLogger()
    : MediaProcessUnit(),m_out(0)
{}

UnknownWordLogger::~UnknownWordLogger()
{}

void UnknownWordLogger::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)
{
  MORPHOLOGINIT;
  MediaId language = manager->getInitializationParameters().media;
  try
  {
    std::string outputFile=unitConfiguration.getParamsValueAtKey("outputFile");
    m_out=new std::ofstream(outputFile.c_str(), std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
  }
  catch (NoSuchParam& )
  {
    LERROR << "no param 'outputFile' in UnknownWordLogger group for language " << (int) language << LENDL;
    throw InvalidConfiguration();
  }
}

LimaStatusCode UnknownWordLogger::process(
  AnalysisContent& analysis) const
{
  TimeUtils::updateCurrentTime();

  AnalysisGraph* tokenList=static_cast<AnalysisGraph*>(analysis.getData("AnalysisGraph"));
  LinguisticGraph* g=tokenList->getGraph();

  try
  {

    LinguisticGraphVertexIt v, vEnd;
    boost::tie(v, vEnd) = vertices(*g);

    //    Lima::LimaString dummyString;

    for (; v != vEnd; v++)
    {
      MorphoSyntacticData* currentData=get(vertex_data, *g, *v);
      Token* tok=get(vertex_token, *g, *v);
      if (currentData == 0)
      {
        continue;
      } // NULL for first and last vertex
      
      if (in_degree(*v,*g)==0) {
        // ignore this vertex, it's not in graph
        continue;
      }

      //<if token has any linguistic properties, directly or by its
      // orthographic alternatives, default properties are not applied>
      if ((m_out != 0) && currentData->empty())
      {
        *m_out << Common::Misc::limastring2utf8stdstring(tok->stringForm()) << "\t" << Common::Misc::limastring2utf8stdstring(tok->status().defaultKey()) << std::endl;
      }
    }
  }
  catch (std::exception &exc)
  {
    MORPHOLOGINIT;
    LWARN << "Exception in UnknownWordLogger :  " << exc.what() << LENDL;
    return UNKNOWN_ERROR;
  }

  // Graphe après l'affectation des propriétés par défaut
  // std::cout << std::endl;
  // std::cout << "Graph after Default Properties:" << std::endl;

  // Fin de l'analyse morphologique
  // std::cout << "End of Morphological Analysis" << std::endl;
  // std::cout << std::endl;

  TimeUtils::logElapsedTime("UnknownWordLogger");
  return SUCCESS_ID;
}


}

}

}
