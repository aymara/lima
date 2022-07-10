// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2003 by  CEA                                            *
 *   author Olivier MESNARD olivier.mesnard@cea.fr                         *
 *                                                                         *
 *  lexiconIdAccessor.h  -  description                                   *
 *  LexiconIdAccessor = base class for concrete IdGenerator               *
 *  implements 
 *  handle composed word and rely on concrete
 *  based on FsaAccesSpare16 for simple words and                          *
 *  std::map<pair<id1,id2>,id3> for compounds.                             *
 ***************************************************************************/
#ifndef LEXICON_ID_ACCESSOR_H
#define LEXICON_ID_ACCESSOR_H

#include "AbstractLexiconIdAccessor.h"
#include "lexiconCompoundElement.h"
// #include "AbstractFsaAccessIStreamWrapper.h"
#include "FsaAccessIOStreamWrapper.h"

#include "common/LimaCommon.h"
namespace Lima {
namespace Common {
namespace FsaAccess {

typedef std::vector<DepCompoundLink> DepCompoundLinksV;
typedef std::vector<ExtCompoundLink> ExtCompoundLinksV;

class CompoundsDataIterator;

template<typename SimpleStringAccess> 
class LexiconIdAccessor : public AbstractLexiconIdAccessor 
{
  friend class CompoundsDataIterator;
  public:
    LexiconIdAccessor() {}
    virtual ~LexiconIdAccessor() {}
     
    // implementation of operation inherited from interface AbstractLexiconIdAccessor
    uint64_t getId(const LimaString& word) override;
    uint64_t getId(const std::vector<uint64_t>& structure) override;
//    CompoundsDataIterator getDataIterator();
    void read(FsaAccessDataInputHandler& handler);
//     void read(AbstractFsaAccessIStreamWrapper& handler);
    void initTransfert();
    void feedWithNextDepData(const char* buff, uint64_t );
    void feedWithNextExtData(const char* buff, uint64_t );
    void closeTransfert();
    uint64_t getSize() const override;
    uint64_t getSimpleTermSize() const override;
    void print(std::ostream& os) const;
    
  private:
    SimpleStringAccess m_simpleWords;
    DepCompoundLinksV m_depCompoundLinks;
    ExtCompoundLinksV m_extCompoundLinks;
    // for getNextDepData() and getNextExtData() operation
    ExtCompoundLinksV::const_iterator m_extIt;
    DepCompoundLinksV::const_iterator m_depIt;
    static const uint64_t BUFFER_SIZE;
};

/*
template<typename SimpleStringAccess, typename ForwardIDepter> 
class CompoundsDataIterator {
  public:
    CompoundsDataIterator( typename LexiconIdAccessor<SimpleStringAccess>& accessor );
  private:
    uint8_t* m_data;
    LexiconIdAccessor<SimpleStringAccess>& m_accessor;
    ForwardDepIter
};
*/


} // namespace FsaAccess
} // namespace Common
} // namespace Lima
#include "lexiconIdAccessor.tcc"

#endif   //FSA_LEXICON_ID_ACCESSOR_H
