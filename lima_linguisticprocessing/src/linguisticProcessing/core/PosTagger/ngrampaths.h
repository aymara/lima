// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/**              Creation of vectors of structures of Trigrams and Bigrams  *
  * @file        ngrampaths.h
  * @author      Nasredine Semmar <Nasredine.Semmar@cea.fr>
  *              Copyright (c) 2003 by CEA
  * @date        Created on  Oct, 27 2003
  * @version     $Id$
  *
  */

#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"

#ifndef LIMA_POSTAGGER_NGRAMPATHS_H
#define LIMA_POSTAGGER_NGRAMPATHS_H

namespace Lima
{
namespace LinguisticProcessing {
namespace PosTagger
{

/**
   * @brief      Structure contenant les trois catégories grammaticales
   *             ainsi que le noeud adjacent au noeud donné
   * @author     Nasredine Semmar
   */
struct TriGramPaths
{
    uint64_t                    m_firstTriGramMicroCategory;
    uint64_t                    m_secondTriGramMicroCategory;
    uint64_t                    m_thirdTriGramMicroCategory;
    LinguisticGraphVertex              m_vertexTriGramPath;

    struct TriGramPaths& operator = (const struct TriGramPaths& src)
    {
        m_firstTriGramMicroCategory = src.m_firstTriGramMicroCategory;
        m_secondTriGramMicroCategory = src.m_secondTriGramMicroCategory;
        m_thirdTriGramMicroCategory = src.m_thirdTriGramMicroCategory;
        m_vertexTriGramPath = src.m_vertexTriGramPath;

        return *this;
    }
};

/**
   * @brief      Structure contenant les deux catégories grammaticales
   *             ainsi que le noeud adjacent au noeud donné
   * @author     Nasredine Semmar
   */
struct BiGramPaths
{
    uint64_t                    m_firstBiGramMicroCategory;
    uint64_t                    m_secondBiGramMicroCategory;
    LinguisticGraphVertex              m_vertexBiGramPath;


    struct BiGramPaths& operator = (const struct BiGramPaths& src)
    {
        m_firstBiGramMicroCategory = src.m_firstBiGramMicroCategory;
        m_secondBiGramMicroCategory = src.m_secondBiGramMicroCategory;
        m_vertexBiGramPath = src.m_vertexBiGramPath;

        return *this;
    }
};

/**
  * @brief      Retourne un vecteur de structures de type TriGramPaths
  *             (Le quatrième champ de la structure est le noeud adjacent au noeud donné)
  * @author     Nasredine Semmar
  */
void buildThreeVerticesPaths(LinguisticGraphVertex& v, const LinguisticGraph& g,
                             std::vector<TriGramPaths>& trigramPathsVector);

/**
  * @brief      Retourne un vecteur de structures de type TriGramPaths
  *             (Le quatrième champ de la structure est le noeud adjacent au noeud adjacent au noeud donné)
  * @author     Nasredine Semmar
  */
void buildParticularCaseThreeVerticesPaths(LinguisticGraphVertex& v, const LinguisticGraph& g,
                             std::vector<TriGramPaths>& trigramPathsVector);

/**
  * @brief      Retourne un vecteur de structures de type BiGramPaths
  *             (Le troisième champ de la structure est le noeud adjacent au noeud donné)
  * @author     Nasredine Semmar
  */
void buildTwoVerticesPaths(LinguisticGraphVertex& v, const LinguisticGraph& g,
                           std::vector<BiGramPaths>& bigramPathsVector);

} // closing namespace PosTagger
} // closing namespace LinguisticProcessing
} // cmosing namespace Lima

#endif // LIMA_POSTAGGER_NGRAMPATHS_H
