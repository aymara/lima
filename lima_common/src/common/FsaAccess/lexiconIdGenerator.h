// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2003 by  CEA                                            *
 *   author Olivier MESNARD olivier.mesnard@cea.fr                         *
 *                                                                         *
 *  lexiconIdGenerator.h  -  description                                   *
 *  LexiconIdGenerator = base class for concrete IdGenerator               *
 *  implements getId()                                                     *
 *  handles composed word and rely on SimpleStringAccess for simple word   *
 *  uses std::map<DepCompound,id> to store compounds.                      *
 *  SimpleStringAccess can be either FsaAccesSpare16 or a map              *
 ***************************************************************************/
#ifndef FSA_LEXICON_ID_GENERATOR_H
#define FSA_LEXICON_ID_GENERATOR_H

#include "AbstractLexiconIdGenerator.h"
#include "lexiconCompoundElement.h"
#include "FsaAccessIOStreamWrapper.h"

#include <map>
#include "common/LimaCommon.h"
// #include "index/common/LimaString.h"
#include "common/Data/LimaString.h"

namespace Lima {
namespace Common {
namespace FsaAccess {


typedef std::map<DepCompound, uint64_t, DepCompoundLtOp> DepCompoundLinksM;
typedef std::map<ExtCompound, uint64_t, ExtCompoundLtOp> ExtCompoundLinksM;

// default functor class usefull to inform client of AbstractLexiconIdGenerator
// of links created by getId(const std::vector<uint64_t>& structure )
class lexiconIdGeneratorInformer {
  int operator() ( uint64_t cmp1, uint64_t cmp2, uint64_t newId);
};

template<typename SimpleStringAccess> 
class LexiconIdGenerator : public AbstractLexiconIdGenerator
{
  public:
    LexiconIdGenerator(AbstractLexiconIdGeneratorInformer* informer);
    virtual ~LexiconIdGenerator() {}

    uint64_t getSize() const override;
    uint64_t getSimpleTermSize() const override;
    void print(std::ostream& os) const;
    // implementation of operation inherited from interface IndexElementIdAccessor
    uint64_t getId(const Lima::LimaString& word) const;
    uint64_t getId(const std::vector<uint64_t>& structure) const;
    // implementation of operation inherited from interface IndexElementIdGenerator
//    uint64_t getId(const LimaString& word);
    uint64_t getId(const Lima::LimaString & word) override;
    uint64_t getId( const std::vector<uint64_t>& structure ) override;
    void write(Lima::Common::FsaAccess::FsaAccessDataOutputHandler& outputHandler);
    // output of compound words data
    void initTransfert();
    uint64_t getNextDepData(char* buff);
    uint64_t getNextExtData(char* buff);
    void closeTransfert();
    void read(FsaAccessDataInputHandler& inputHandler);
    void feedWithNextDepData( const char* buff, uint64_t buffSize );
    void feedWithNextExtData( const char* buff, uint64_t buffSize);

  protected:
    SimpleStringAccess m_simpleWords;
    DepCompoundLinksM m_depCompoundLinks;
    ExtCompoundLinksM m_extCompoundLinks;
  private:
    // for getNextDepData() and getNextExtData() operation
    ExtCompoundLinksM::const_iterator m_extIt;
    DepCompoundLinksM::const_iterator m_depIt;
    //char* m_buffer;
    static const uint64_t BUFFER_SIZE;
};

} // namespace FsaAccess
} // namespace Common
} // namespace Lima

#include "lexiconIdGenerator.tcc"

#endif   //FSA_LEXICON_ID_GENERATOR_H
