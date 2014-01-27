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
/** AgglutinatedToken is a fulltoken that is a agglutinated compound word.
  *
  * @file       AgglutinatedToken.h
  * @author     Benoit Mathieu (CEA)  <mathieub@zoe.cea.fr>
  *             Copyright  (c) 2003 by CEA
  * @date       Created on Jul, 8 2002
  *
  */

#ifndef LIMA_LINGUISTICPROCESSINGS_LINGUISTICANALYSISSTRUCTURE_AGGLUTINATEDTOKEN_H
#define LIMA_LINGUISTICPROCESSINGS_LINGUISTICANALYSISSTRUCTURE_AGGLUTINATEDTOKEN_H

#include "LinguisticAnalysisStructureExport.h"
#include "common/Data/LimaString.h"
#include "Token.h"
#include "MorphoSyntacticData.h"

#include <vector>

namespace Lima
{
namespace LinguisticProcessing
{
namespace LinguisticAnalysisStructure
{

/**
*  AgglutinatedToken is owner of all Token* and MorphoSyntacticData* in cuttings.
*  destructor deletes all.
*/
class LIMA_LINGUISTICANALYSISSTRUCTURE_EXPORT AgglutinatedToken : public Token
{

public:

  class Cutting : public std::vector< std::pair< Token*,MorphoSyntacticData* > >
  {};

  typedef std::vector<Cutting> T_CUTTINGS;

  AgglutinatedToken(const Token& ft);

  virtual ~AgglutinatedToken();

  void setCuttings(const T_CUTTINGS& cuttings);

  const T_CUTTINGS& getCuttings() const;

  virtual void outputXml(std::ostream& XmlStream,const Common::PropertyCode::PropertyCodeManager& pcm,const FsaStringsPool& sp) const;

private:

  T_CUTTINGS m_cuttings;

};

} // closing namespace Data
} // colsing namespace LinguisticProcessing
} // closing namespace Lima

#endif
