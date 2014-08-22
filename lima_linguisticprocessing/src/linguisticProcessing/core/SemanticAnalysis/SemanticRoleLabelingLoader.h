/*
    Copyright 2002-2014 CEA LIST

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
/************************************************************************
 *
 * @file       SemanticRoleLabelingLoader.h
 * @author     Clémence Filmont <clemence.filmont@cea.fr>
 * @date       2014-04-17
 * copyright   Copyright (C) 2014 by CEA LIST
 * Project     mm_linguisticprocessing
 *
 * @brief      an Semantic Role Labeling Loader class
 *
 *
 ***********************************************************************/

#ifndef SEMANTICROLELABELINGLOADER_H
#define SEMANTICROLELABELINGLOADER_H

#include "linguisticProcessing/core/LinguisticProcessors/AnalysisLoader.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "LimaConllTokenIdMapping.h"
#include "QStringList"
#include <QString>
#include <QFile>
#include <map>


namespace Lima {
namespace LinguisticProcessing {
namespace SemanticAnalysis {

#define SEMANTICROLELABELINGLOADER_CLASSID "SemanticRoleLabelingLoader"

class SemanticRoleLabelingLoader : public AnalysisLoader
{
 public:
  SemanticRoleLabelingLoader();
  virtual ~SemanticRoleLabelingLoader();

  void init(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
            Manager* manager);

  LimaStatusCode process(AnalysisContent& analysis) const;

 private:
  MediaId m_language;
  std::string m_graph;
  std::string m_suffix;

  // Conll handler
  class ConllHandler
  {
  public:
    QRegExp m_descriptorSeparator;
    QRegExp m_tokenSeparator;
    std::pair<LinguisticGraphVertex,QString> *m_verbalClasses;
    std::vector<std::pair<LinguisticGraphVertex,QString>> *m_semanticRoles;
    int m_verbalClassNb;

    ConllHandler(MediaId language, AnalysisContent& analysis, LinguisticAnalysisStructure::AnalysisGraph* graph);
    virtual ~ConllHandler();

    /**
     * @brief extract semantic annotations associated to token
     * @param sentenceIndex the index of the current sentence
     * @param limaConllMapping the chosen lima conll token id mapping
     * @param sentence the current sentence
     * @return true if any verbal class is found, false otherwise
     */
    bool extractSemanticInformations(int sentenceIndex, LimaConllTokenIdMapping* limaConllMapping, const QString & sentence);

    /**
     * @brief split a text into different types segments
     * @param segment the segment to split
     * @param separator the separator used to split
     * @return the segment split
     */
    QStringList splitSegment(const QString & segment, QRegExp separator);

    /**
     * @brief get the lima token id matching any conll token one from the same text
     * @param conllTokenId the conll token id one search the matched lima id
     * @param sentenceNb the index of the current sentence
     * @param limaConllMapping the chosen lima conll token id mapping
     * @return the lima token id
     * @note function to put in the LimaConllTokenIdMapping class?
     */
    LinguisticGraphVertex getLimaTokenId(int conllTokenId, int sentenceIndex, LimaConllTokenIdMapping* limaConllMapping);


  private:
    MediaId m_language;
    AnalysisContent& m_analysis;
    LinguisticAnalysisStructure::AnalysisGraph* m_graph;


  };

};

} // end namespace
} // end namespace
} // end namespace

#endif
