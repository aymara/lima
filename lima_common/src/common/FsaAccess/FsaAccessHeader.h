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
                          compactDict.h  -  description
                             -------------------
    begin                : mer mai 28 2003
    copyright            : (C) 2003 by Olivier Mesnard
    email                : olivier.mesnard@cea.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *  Compact dictionnary based on finite state automata implemented with    *
 *  Boost Graph library.                                                   *
 *  Algorithm is described in article from Daciuk, Mihov, Watson & Watson: *
 *  "Incremental Construction of Minimal Acyclic Finite State Automata"    *
 *  How to use it to compute hash code is explained in  'perfect hashing' *
 *  of document http://odur.let.rug.nl/alfa/fsa_stuff/#PerfHash            *
 *                                                                         *
 ***************************************************************************/

#ifndef FSA_ACCESS_HEADER_H
#define FSA_ACCESS_HEADER_H

// From standard library
#include <string>
#include "common/FsaAccess/FsaExceptions.h"

namespace Lima {
namespace Common {
namespace FsaAccess {

enum char_order_type {FORWARD=0,REVERSE=1};
enum dict_packing_type {BUILDER=0,BUILT=1,SPARE=2} ;

#define MAGIC_NUMBER "limaFsaDict"
#define HEADER_SIZE 1024
// we autorize around 100 millions simple terms
// higher ids are reserved to identify complex terms  (compound words)
#define MAX_SIMPLE_TID 100000000

#define VERTEX_PROPERTY_16 uint64_t
#define MAJOR_VERSION_16 2
#define MINOR_VERSION_16 0

class AbstractFsaAccessIStreamWrapper;
class AbstractFsaAccessOStreamWrapper;

class FsaAccessHeaderPrivate;
/**
 *  FsaDictionaries (FsaDictBuilder and FsaDictSpare) share the same binary file format:
 *
 *  offset	length	content
 *  0 		??	file ::= header body
 *  0 		1024	header ::= magicNumber version Lima::LimaCharype packing charOrder nbVertices nbEdges padData
 *  0		12	magicNumber ::= "limaFsaDict"
 *  12		5	version ::= nn.nn (n ::= any ascii character to code figure between 0 & 9 i.e. '0', '1', ....) 5 bytes
 *  17		1	Lima::LimaCharype ::= { 1 for ASCII, iso8859-1..., 4 for 4bytes wide char } 1 byte integer
 *  18 		1	packing ::= { BUILT = 1, SPARE = 2, BUILDER = 0  } 1 byte integer
 *  19		1	charOrder ::= { forward = 0, reverse = 1 } 1 byte integer
 *  20		10	nbVertices ::= nnnnnnnnnn integer < 2**31-1. 10 bytes
 *  30		10	nbEdges ::= nnnnnnnnnn integer < 2**31-1. 10 bytes
 *  40		972	padData ::= enough bytes to complete header of 1k
 *  1024	4	body ::= vertice* edges* nbvert0
 *  1024	4	vertices ::= vertexProperty (4 bytes uint64_t)
 *  1024+N*4	8+ep	edges ::= vertexId vertexId edgeProperty (4 bytes + 4 bytes +)
 *
 **/
class LIMA_FSAACCESS_EXPORT FsaAccessHeader {
  public:
    FsaAccessHeader(bool trie_direction_fwd);
    virtual ~FsaAccessHeader();
    void read( AbstractFsaAccessIStreamWrapper& iw );
    void write( AbstractFsaAccessOStreamWrapper& ow);
    bool getTrieDirectionForward() const;
    uint64_t getNbVertices() const;
    uint64_t getNbEdges() const;
    void setPackingStatus(uint8_t packingStatus);
    void setNbVertices(uint64_t nbVerts);
    void setNbEdges(uint64_t nbEdges);
//    uint64_t getMaxSimpleId() const;
//    uint64_t  int getCompoundsDataOffset();
//    uint64_t getNbCompounds() const;
//    void setStreamPos(std::ifstream& is);
//    const long& getStreamPos();
protected:
  FsaAccessHeader(const FsaAccessHeader& h);
  FsaAccessHeader& operator=(const FsaAccessHeader& h);
  FsaAccessHeaderPrivate* m_d;
};

} // namespace FsaAccess
} // namespace Common
} // namespace Lima

#endif   //FSA_ACCESS_HEADER_H
