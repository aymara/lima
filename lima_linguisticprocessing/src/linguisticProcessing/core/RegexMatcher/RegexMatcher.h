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
/** @brief      A process unit able to match regex against analysed text
 * 
 * @file        RegexMatcher.h
 * @author      Gael de Chalendar (Gael.de-Chalendar@cea.fr) 

 *              Copyright (c) 2011 by CEA LIST
 * @date        Created on Nov, 1 2011
 * @version     $Id:  $
 *
 */

#ifndef LIMA_LINGUISTICPROCESSING_REGEXREPLACER_H
#define LIMA_LINGUISTICPROCESSING_REGEXREPLACER_H

#include "RegexMatcherExport.h"
#include "common/Data/LimaString.h"

#include "common/MediaProcessors/MediaProcessUnit.h"

namespace Lima
{
namespace LinguisticProcessing
{

namespace RegexMatching
{

#define REGEXREPLACER_CLASSID "RegexMatcher"

class RegexMatcherPrivate;

/** @brief This is a @ref MediaProcessUnit that matches regexes against the text itself and replaces corresponding tokens in the analysis graph by one token
 * 
 * As a ProcessUnit, it has an init and a process function. See @ref ProcessUnit for details.
 *
 * IOPES:
 * - Input: an AnalysisContent and the following parameters in the configuration file:
 *   - a map named @b regexes where each key is a standard regular expression and the value is a tokenization status to associate to created tokens.
 * - Output: an AnalysisContent
 * - Preconditions: the AnalysisContent must contain an AnalysisData of type AnalysisGraph named "AnalysisGraph". This graph must be a simple string of tokens: each vertex has one and only one next vertex (except for the last one). This is checked.
 * - Effects: Tokens matching the regexes are replaced by one token with the given tokenization status and annotations are created in the annotation graph (this one is created if necessary) to match the new vertex with the old ones.
 */
class LIMA_REGEXMATCHER_EXPORT RegexMatcher : public MediaProcessUnit
{
friend class RegexMatcherPrivate;

public:
  RegexMatcher();
  virtual ~RegexMatcher();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager)
  ;

  LimaStatusCode process(
    AnalysisContent& analysis) const;

private:
  RegexMatcherPrivate* m_d;
};

} //namespace RegexReplace
} // namespace LinguisticProcessing
} // namespace Lima

#endif
