// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/**              Creation of vectors of structures of Trigrams and Bigrams  *
  * @file        ngrampaths.cpp
  * @author      Nasredine Semmar <Nasredine.Semmar@cea.fr>
  *              Copyright (c) 2003 by CEA
  * @date        Created on  Oct, 27 2003
  * @version     $Id$
  *
  */

#include "PosTagger/ngrampaths.h"

using namespace boost;

namespace Lima
{
namespace LinguisticProcessing {
namespace PosTagger
{

/**
  * @brief      Retourne un vecteur de structures de type TriGramPaths
  *             (Le quatrième champ de la structure est le noeud adjacent au noeud donné)
  * @author     Nasredine Semmar
  */
void buildThreeVerticesPaths(PhoenixGraphVertex& v, const PhoenixLingGraph& g,
                             std::vector<TriGramPaths>& trigramPathsVector)
{
    PTLOGINIT;

    LDEBUG << "====> Call of buildThreeVerticesPaths(" << v << "):";

    // Pile contenant les noeuds adjacents au noeud v
    std::stack<PhoenixGraphVertex> verticesStack;

    // Objet temporaire du type TriGramPaths
    TriGramPaths    tempTrigramPaths;

    // Pile Contenant les categories grammaticales des noeuds adjacents au noeud v
    std::stack<uint64_t> lingInfoStack;
    graph_traits<PhoenixLingGraph>::adjacency_iterator first_ai, first_a_end;

    // Property Map entre les noeuds du graphe et leurs informations linguistiques
    CVertexWordPropertyMap    wordMap = get
                                            (vertex_word, g);

    // Boucle pour visiter tous les noeuds adjacents au noeud v
    for (boost::tie(first_ai, first_a_end) = adjacent_vertices(v, g); first_ai != first_a_end; first_ai++)
    {
        // Pile contenant les noeuds adjacents au noeud v
        verticesStack.push(*first_ai);

        // Pile Contenant les catégories grammaticales des noeuds adjacents (suivants) au noeud v
        lingInfoStack.push(wordMap[*first_ai].microCategory());
    }

    // Boucle pour visiter tous les noeuds adjacents aux noeuds adjacents au noeud v
    graph_traits<PhoenixLingGraph>::adjacency_iterator second_ai, second_a_end;

    while (!verticesStack.empty() && !lingInfoStack.empty())
    {
        for (boost::tie(second_ai, second_a_end) = adjacent_vertices(verticesStack.top(), g); second_ai !=
                second_a_end; second_ai++)
        {
            // Catégorie grammaticale du noeud v
            tempTrigramPaths.m_firstTriGramMicroCategory = wordMap[v].microCategory();

            // Catégorie grammaticale du noeud adjacent au noeud v
            tempTrigramPaths.m_secondTriGramMicroCategory = lingInfoStack.top();

            // Catégorie grammaticale du noeud adjacent du noeud adjacent au noeud v
            tempTrigramPaths.m_thirdTriGramMicroCategory = wordMap[*second_ai].microCategory();

            // Noeud adjacent au noeud v
            tempTrigramPaths.m_vertexTriGramPath = verticesStack.top();

            // Vecteur de structures en Trigrams pour le noeud v
            trigramPathsVector.push_back(tempTrigramPaths);
        }

        // Pile contenant les noeuds adjacents au noeud v
        verticesStack.pop();

        // Pile Contenant les catégories grammaticales des noeuds adjacents au noeud v
        lingInfoStack.pop();
    }
}

/**
  * @brief      Retourne un vecteur de structures de type TriGramPaths
  *             (Le quatrième champ de la structure est le noeud adjacent au noeud adjacent au noeud donné)
  * @author     Nasredine Semmar
  */
void buildParticularCaseThreeVerticesPaths(PhoenixGraphVertex& v, const PhoenixLingGraph& g,
        std::vector<TriGramPaths>& trigramPathsVector)
{
    PTLOGINIT;

    LDEBUG << "====> Call of buildParticularCaseThreeVerticesPaths(" << v << "):";

    // Pile contenant les noeuds adjacents au noeud v
    std::stack<PhoenixGraphVertex> verticesStack;

    // Objet temporaire du type TriGramPaths
    TriGramPaths    tempTrigramPaths;

    // Pile Contenant les catégories grammaticales des noeuds adjacents au noeud v
    std::stack<uint64_t> lingInfoStack;
    graph_traits<PhoenixLingGraph>::adjacency_iterator first_ai, first_a_end;

    // Property Map entre les noeuds du graphe et leurs informations linguistiques
    CVertexWordPropertyMap    wordMap = get
                                            (vertex_word, g);

    // Boucle pour visiter tous les noeuds adjacents (suivants) au noeud v
    for (boost::tie(first_ai, first_a_end) = adjacent_vertices(v, g); first_ai != first_a_end; first_ai++)
    {
        // Pile contenant les noeuds adjacents (suivants) au noeud v
        verticesStack.push(*first_ai);

        // Pile Contenant les catégories grammaticales des noeuds adjacents (suivants) au noeud v
        lingInfoStack.push(wordMap[*first_ai].microCategory());
    }

    // Boucle pour visiter tous les noeuds adjacents aux noeuds adjacents au noeud v
    graph_traits<PhoenixLingGraph>::adjacency_iterator second_ai, second_a_end;

    while (!verticesStack.empty() && !lingInfoStack.empty())
    {
        for (boost::tie(second_ai, second_a_end) = adjacent_vertices(verticesStack.top(), g); second_ai !=
                second_a_end; second_ai++)
        {
            // Catégorie grammaticale du noeud v
            tempTrigramPaths.m_firstTriGramMicroCategory = wordMap[v].microCategory();

            // Catégorie grammaticale du noeud adjacent au noeud v
            tempTrigramPaths.m_secondTriGramMicroCategory = lingInfoStack.top();

            // Catégorie grammaticale du noeud adjacent du noeud adjacent au noeud v
            tempTrigramPaths.m_thirdTriGramMicroCategory = wordMap[*second_ai].microCategory();

            // Cas particulier: Noeud adjacent du noeud adjacent du noeud v
            tempTrigramPaths.m_vertexTriGramPath = *second_ai;

            // Vecteur de structures en Trigrams pour le noeud v
            trigramPathsVector.push_back(tempTrigramPaths);
        }

        // Pile contenant les noeuds adjacents au noeud v
        verticesStack.pop();

        // Pile Contenant les catégories grammaticales des noeuds adjacents au noeud v
        lingInfoStack.pop();
    }
}

/**
  * @brief      Retourne un vecteur de structures de type BiGramPaths
  *             (Le troisième champ de la structure est le noeud adjacent au noeud donné)
  * @author     Nasredine Semmar
  */
void buildTwoVerticesPaths(PhoenixGraphVertex& v,
                           const PhoenixLingGraph& g, std::vector<BiGramPaths>& bigramPathsVector)
{
    PTLOGINIT;

    LDEBUG << "====> Call of buildTwoVerticesPaths(" << v << "):";

    // Objet temporaire du type TriGramPaths
    BiGramPaths    tempBiGramPaths;
    graph_traits<PhoenixLingGraph>::adjacency_iterator ai, a_end;

    // Property Map entre les noeuds du graphe et leurs informarions linguistiques
    CVertexWordPropertyMap    wordMap = get
                                            (vertex_word, g);

    // Boucle pour visiter tous les noeuds adjacents (suivants) au noeud v
    for (boost::tie(ai, a_end) = adjacent_vertices(v, g); ai != a_end; ai++)
    {
        // Catégorie grammaticale du noeud v
        tempBiGramPaths.m_firstBiGramMicroCategory = wordMap[v].microCategory();

        // Catégorie grammaticale du noeud adjacent au noeud v
        tempBiGramPaths.m_secondBiGramMicroCategory = wordMap[*ai].microCategory();

        // Noeud adjacent au noeud v
        tempBiGramPaths.m_vertexBiGramPath = *ai;

        // Vecteur de structures en Bigrams pour le noeud v
        bigramPathsVector.push_back(tempBiGramPaths);
    }
}

} // closing namespace PosTagger
} // closing namespace LinguisticProcessing
} // cmosing namespace Lima
