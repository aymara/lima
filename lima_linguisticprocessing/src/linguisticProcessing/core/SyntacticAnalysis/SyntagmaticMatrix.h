// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/**
  *
  * @file        SyntagmaticMatrix.h
  * @author      Gael de Chalendar (Gael.de-Chalendar@cea.fr)
  * @date        Created on  : Mon Aug 04 2003
  *              Copyright   : (c) 2003 by CEA
  * @version     $Id$
  *
  */

#ifndef SYNTAGMATICMATRIX_H
#define SYNTAGMATICMATRIX_H

#include "SyntacticAnalysisExport.h"
#include "TokenFilter.h"
#include "common/Data/LimaString.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/ChainIdStruct.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticData.h"
#include "linguisticProcessing/core/LinguisticResources/AbstractResource.h"

#include <set>
#include <map>

namespace Lima
{
namespace LinguisticProcessing
{
namespace SyntacticAnalysis
{

#define SYNTAGMDEFSTRUCT_CLASSID "SyntagmDefinitionStructure"

class LIMA_SYNTACTICANALYSIS_EXPORT SyntagmaticMatrixRow
  {
public:
    SyntagmaticMatrixRow() = default;
    virtual ~SyntagmaticMatrixRow() = default;

    SyntagmaticMatrixRow(const SyntagmaticMatrixRow& row) = default;
    SyntagmaticMatrixRow& operator=(const SyntagmaticMatrixRow& row) = default;


    void deleteRow()
    {
      m_filters.clear();
    }

    std::set< TokenFilter, tfless >::const_iterator find(const TokenFilter& f) const;

    std::set< TokenFilter, tfless > m_filters;
  };

inline LIMA_SYNTACTICANALYSIS_EXPORT std::ostream& operator<<(std::ostream &os, const SyntagmaticMatrixRow& r)
{
  std::set< TokenFilter, tfless >::const_iterator it, it_end;
  it = r.m_filters.begin(); it_end = r.m_filters.end();
  for (; it != it_end; it++)
    os << (*it) << " ; ";
  os << std::endl;
  return os;
}

class tf_dwless : public std::less< TokenFilter >
{
public:
    bool operator()(const TokenFilter& x, const TokenFilter& y) const;
    bool operator()(const TokenFilter& x, const std::pair<TokenFilter,SyntagmaticMatrixRow>& p) const;
    bool operator()(const std::pair<TokenFilter,SyntagmaticMatrixRow>& p, const TokenFilter& y) const;
};

typedef std::map< TokenFilter, SyntagmaticMatrixRow, tfless > SyntagmaticMatrixFilter;

class LIMA_SYNTACTICANALYSIS_EXPORT SyntagmaticMatrix
{
public:

  SyntagmaticMatrix() :
      m_filters(SyntagmaticMatrixFilter()),
      m_language()
  {}

  SyntagmaticMatrix(const SyntagmaticMatrix& matrix) :
      m_filters(SyntagmaticMatrixFilter(matrix.m_filters)),
      m_language(matrix.m_language)
  {}

  virtual ~SyntagmaticMatrix() {}

  void deleteMatrix();

  SyntagmaticMatrixFilter& filters();

  const SyntagmaticMatrixFilter& filters() const;

  SyntagmaticMatrixFilter::const_iterator find(const TokenFilter& f) const;

  void display() const;

  inline MediaId language() const
  {
    return m_language;
  }

  inline void language(MediaId lang)
  {
    m_language=lang;
  }

private:
  SyntagmaticMatrixFilter m_filters;
  MediaId m_language;
};

class LIMA_SYNTACTICANALYSIS_EXPORT SyntagmDefStruct : public AbstractResource
{
  Q_OBJECT
public:
  SyntagmDefStruct();

  virtual ~SyntagmDefStruct();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;

  inline SyntagmaticMatrix& nominal() {return m_nominalMatrix;}
  inline SyntagmaticMatrix& verbal() {return m_verbalMatrix;}
  inline SyntagmaticMatrixRow& nominalBegin() {return m_nomChainBeg;}
  inline SyntagmaticMatrixRow& nominalEnd() {return m_nomChainEnd;}
  inline SyntagmaticMatrixRow& verbalBegin() {return m_verbChainBeg;}
  inline SyntagmaticMatrixRow& verbalEnd() {return m_verbChainEnd;}

  void loadFromFile(const std::string& fileName);

  void deleteMatrices();

  void display() const;

  bool canChainBeginBy(const LinguisticAnalysisStructure::MorphoSyntacticData* filter, Common::MediaticData::ChainsType type) const;
  bool canChainEndBy(const LinguisticAnalysisStructure::MorphoSyntacticData* filter, Common::MediaticData::ChainsType type) const;
  bool belongsToMatrix(const LinguisticAnalysisStructure::MorphoSyntacticData* src, const LinguisticAnalysisStructure::MorphoSyntacticData* dest, Common::MediaticData::ChainsType type) const;

  bool canNominalChainBeginBy(const LinguisticAnalysisStructure::MorphoSyntacticData* cat) const;
  bool canNominalChainEndBy(const LinguisticAnalysisStructure::MorphoSyntacticData* cat) const;
  bool canVerbalChainBeginBy(const LinguisticAnalysisStructure::MorphoSyntacticData* cat) const;
  bool canVerbalChainEndBy(const LinguisticAnalysisStructure::MorphoSyntacticData* cat) const;
  bool belongsToNominalMatrix(const LinguisticAnalysisStructure::MorphoSyntacticData* src, const LinguisticAnalysisStructure::MorphoSyntacticData* dest) const;
  bool belongsToVerbalMatrix(const LinguisticAnalysisStructure::MorphoSyntacticData* src, const LinguisticAnalysisStructure::MorphoSyntacticData* dest) const;

  inline MediaId language() const {return m_language;};

private:
  MediaId m_language;
  const Common::PropertyCode::PropertyAccessor* m_macroAccessor;
  const Common::PropertyCode::PropertyAccessor* m_microAccessor;

  // Definition of the nominal/verbal chains matrices
  SyntagmaticMatrix m_nominalMatrix;
  SyntagmaticMatrix m_verbalMatrix;

  // Definition of the authorized categories to begin/end a nominal/verbal chain
  SyntagmaticMatrixRow m_nomChainBeg;
  SyntagmaticMatrixRow m_nomChainEnd;
  SyntagmaticMatrixRow m_verbChainBeg;
  SyntagmaticMatrixRow m_verbChainEnd;

};



} // namespace SyntacticAnalysis
} // namespace LinguisticProcessing
} // namespace Lima

#endif
