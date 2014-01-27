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

#ifndef LIMA_MORPHOLOGICANALYSIS_DOTGRAPHWRITER_H
#define LIMA_MORPHOLOGICANALYSIS_DOTGRAPHWRITER_H

#include "PosTaggerExport.h"
#include "common/MediaProcessors/MediaProcessUnit.h"
#include "ngramMatrices.h"

namespace Lima
{
namespace LinguisticProcessing
{

#define DOTGRAPHWRITER_CLASSID "DotGraphWriter"

class LIMA_POSTAGGER_EXPORT DotGraphWriter : public MediaProcessUnit
{

public:
  DotGraphWriter();
  virtual ~DotGraphWriter();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager)
  ;

  LimaStatusCode process(AnalysisContent& analysis) const;

protected:

  std::string m_outputSuffix;
  MediaId m_language;
  const Lima::LinguisticProcessing::PosTagger::TrigramMatrix* m_trigramMatrix;
  const Lima::LinguisticProcessing::PosTagger::BigramMatrix* m_bigramMatrix;
  std::deque<std::string> m_vertexDisplay;
  std::string m_graphId;

  std::map<std::string,std::string> m_graphDotOptions;
  std::map<std::string,std::string> m_nodeDotOptions;
  std::map<std::string,std::string> m_edgeDotOptions;
  
};

struct LIMA_POSTAGGER_EXPORT LimaGraphGraphvizGraphWriter 
{
  LimaGraphGraphvizGraphWriter(
                               const std::map<std::string,std::string>& graphDotOptions,
                               const std::map<std::string,std::string>& nodeDotOptions,
      const std::map<std::string,std::string>& edgeDotOptions):
      m_graphDotOptions(graphDotOptions),
                        m_nodeDotOptions(nodeDotOptions),
                                         m_edgeDotOptions(edgeDotOptions)
                                         {
                                         }

                                         void operator()(std::ostream& out) const 
                                         {
                                           out << "graph [";
                                           std::map<std::string,std::string>::const_iterator git, git_end;
                                           git = m_graphDotOptions.begin(); git_end = m_graphDotOptions.end();
                                           if (git != git_end)
                                           {
                                             out << (*git).first << "=" << (*git).second;
                                             git++;
                                           }
                                           for (; git != git_end; git++)
                                           {
                                             out << "," << (*git).first << "=" << (*git).second;
                                           }
                                           out << "]" << std::endl;

                                           out << "node [";
                                           std::map<std::string,std::string>::const_iterator nit, nit_end;
                                           nit = m_nodeDotOptions.begin(); nit_end = m_nodeDotOptions.end();
                                           if (nit != nit_end)
                                           {
                                             out << (*nit).first << "=" << (*nit).second;
                                             nit++;
                                           }
                                           for (; nit != nit_end; nit++)
                                           {
                                             out << "," << (*nit).first << "=" << (*nit).second;
                                           }
                                           out << "]" << std::endl;

                                           out << "edge [";
                                           std::map<std::string,std::string>::const_iterator eit, eit_end;
                                           eit = m_edgeDotOptions.begin(); eit_end = m_edgeDotOptions.end();
                                           if (eit != eit_end)
                                           {
                                             out << (*eit).first << "=" << (*eit).second;
                                             eit++;
                                           }
                                           for (; eit != eit_end; eit++)
                                           {
                                             out << "," << (*eit).first << "=" << (*eit).second;
                                           }
                                           out << "]" << std::endl;
                                         }
  
  private:
    const std::map<std::string,std::string>& m_graphDotOptions;
    const std::map<std::string,std::string>& m_nodeDotOptions;
    const std::map<std::string,std::string>& m_edgeDotOptions;

};

} // LinguisticProcessing
} // Lima

#endif
