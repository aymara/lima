// Copyright 2002-2020 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 * @file     bowComplexToken.h
 * @author   Besancon Romaric
 * @date     Tue Oct  7 2003
 * copyright Copyright (C) 2003 by CEA LIST
 *
 * @brief
 *
 ***********************************************************************/

#ifndef BOWCOMPLEXTOKEN_H
#define BOWCOMPLEXTOKEN_H

#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "bowToken.h"

#include <boost/tuple/tuple.hpp>
#include <deque>

#include <boost/shared_ptr.hpp>

namespace Lima {
namespace Common {
namespace BagOfWords {



class BoWRelation;

class BoWComplexTokenPrivate;
/**
  * This is a complex token for an index. It is made of a collection of
  * BoWTokens's, one of which is the head of the token.
  *
  * @author Romaric Besancon
  */
class LIMA_BOW_EXPORT BoWComplexToken : public BoWToken
{
  friend class BoWComplexTokenPrivate;
public:

/**
   * This class represents a part of a ComplexToken : it is composed
   * of a pointer on the actual part (a BoWToken), and a boolean that
   * says if the BoWToken is also in the global list (the BoWText)
   * or not.
   */
  class Part : public boost::tuple< boost::shared_ptr< BoWRelation >,
                                    boost::shared_ptr< BoWToken > >
  {
  public:
    Part() :
        boost::tuple< boost::shared_ptr< BoWRelation >,
                      boost::shared_ptr< BoWToken > >(boost::shared_ptr< BoWRelation >(),
                                                      boost::shared_ptr< BoWToken >() )
    {
    }

    Part(boost::shared_ptr< BoWToken > tok):
      boost::tuple< boost::shared_ptr< BoWRelation >,
                    boost::shared_ptr< BoWToken > >(boost::shared_ptr< BoWRelation >(),
                                                    tok)
    {
    }

    Part(boost::shared_ptr< BoWRelation > rel,
         boost::shared_ptr< BoWToken > tok) :
        boost::tuple< boost::shared_ptr< BoWRelation >,
                      boost::shared_ptr< BoWToken >>(rel,tok)
    {
    }

    boost::shared_ptr< BoWRelation > getBoWRelation() const { return get<0>(); }

    boost::shared_ptr< BoWToken > getBoWToken() const { return get<1>(); }

    LimaString getLemma() const { return get<1>()->getLemma(); }

    LinguisticCode getCategory() const { return get<1>()->getCategory(); }
  };

  BoWComplexToken();
//   /**
//    * copy constructor
//    *
//    * @attention the copy of the parts of a complex token is tricky :
//    * if a part of a complex token points to a token that have a proper
//    * existence outside of the part list, the pointer will be copied
//    * (no creation of a new object), otherwise a new object is created.
//    */
//   BoWComplexToken(const BoWComplexToken&);
//
//   /**
//    * specialized copy constructor that takes into account a
//    * pointer-to-pointer map giving the correspondance of tokens for
//    * the component list, in case of a complete BoWText copy.
//    *
//    */
//   BoWComplexToken(const BoWComplexToken&,
//                   const std::map<BoWToken*,BoWToken*>&);

  BoWComplexToken(const Lima::LimaString& lemma,
                  const LinguisticCode category,
                  const uint64_t position,
                  const uint64_t length);

  /**
   * a constructor to build a simple, independent complex token
   * (parts are simple tokens, not listed elsewhere in a BoWText)
   *
   * @param lemma the lemma of the complex token
   * @param category the category of the complex token
   * @param position the position of the complex token
   * @param length the length of the complex token
   * @param parts a deque containing the simple tokens forming
   * the parts of the complex token
   * @param int head the index of the head token in the preceding list
   *
   * @return
   */
  BoWComplexToken(const Lima::LimaString& lemma,
                  const LinguisticCode category,
                  const uint64_t position,
                  const uint64_t length,
                  std::deque< boost::shared_ptr< Lima::Common::BagOfWords::BoWToken > >& parts,
                  const uint64_t head);

  virtual ~BoWComplexToken();

//   virtual BoWComplexToken* clone() const;
//   virtual BoWComplexToken* clone(const std::map<BoWToken*,BoWToken*>&) const;

  virtual void clear() override;

  virtual Lima::LimaString getLemma(void) const override;
  virtual Lima::LimaString getInflectedForm(void) const override;

  BoWComplexToken& operator= (const BoWComplexToken&);
  bool operator== (const BoWComplexToken&);

  /** size is the number of parts in the token */
  uint64_t size(void) const override;

  std::deque< Part >& getParts(void);
  const std::deque< Part >& getParts(void) const;

  std::deque< Part >::iterator getPartsIterator(void);
  const std::deque< Part >::const_iterator getPartsIterator(void) const;

  /**
   * add a part in the list of parts of the complex token. Have to
   * specify if the token forming the part exists as a proper token or
   * only as a part: in the first case, the pointer is added in the
   * part list, in the second case, a pointer on a copy of the token
   * is added in the list.
   *
   * @param tok a pointer on the BoWToken forming the part to add
   * @param isInList a boolean indicating, if true, that the BoWToken
   * pointed has an existence outside of the part list
   * @param isHead a boolean indicating that the BoWToken a
   */
  boost::shared_ptr< BoWToken > addPart(boost::shared_ptr< BoWToken > tok,
                                        const bool isHead=false);

  /**
   * add a part in the list of parts of the complex token. Have to
   * specify if the token forming the part exists as a proper token or
   * only as a part: in the first case, the pointer is added in the
   * part list, in the second case, a pointer on a copy of the token
   * is added in the list.
   *
   * @param rel a pointer on the BoWRelation linking the part to add to the
   * head or the other parts
   * @param tok a pointer on the BoWToken forming the part to add
   * @param isInList a boolean indicating, if true, that the BoWToken
   * pointed has an existence outside of the part list
   * @param isHead a boolean indicating that the BoWToken a
   */
  boost::shared_ptr< BoWToken > addPart(boost::shared_ptr< BoWRelation > rel,
                                        boost::shared_ptr< BoWToken > tok,
                                        const bool isHead=false);

  /**
   * add a part in the list of parts of the complex token. A pointer on a copy of @ref tok
   * is added in the list.
   *
   * @param tok a pointer on the BoWToken forming the part to be copied and added
   * @param isHead a boolean indicating that the BoWToken is the head of the complex token it
   *        is part of
   */
//   BoWToken* addPart(const boost::shared_ptr< BoWToken > tok,
// //                     const bool isInList,
//                     const bool isHead=false);

  /**
   * add a part in the list of parts of the complex token. A pointer on a copy of @ref tok
   * is added in the list.
   *
   * @param rel a pointer on the BoWRelation linking the part to copy and add to the
   * head or the other parts
   * @param tok a pointer on the BoWToken forming the part to be copied and added
   * @param isHead a boolean indicating that the BoWToken is the head of the complex token it
   *        is part of
   */
//   BoWToken* addPart(const boost::shared_ptr< BoWRelation > rel,
//                     const boost::shared_ptr< BoWToken > tok,
// //                     const bool isInList,
//                     const bool isHead=false);



  uint64_t getHead() const;
  void setHead(const uint64_t);

  // this is an abstract class, has no type
  virtual BoWType getType() const override = 0;

  // common function for output of derived classes
  std::string getUTF8StringParts(const Common::PropertyCode::PropertyManager* macroManager = 0) const;
  std::string getstdstringParts() const;

  virtual Common::Misc::PositionLengthList getPositionLengthList() const override;

  /**
   * add the offset to the positions of the token
   * and of each of its parts (recusrively)
   *
   * @param offset the offset to add to all positions
   */
  void addToPosition(const uint64_t offset) override;


  /**
   * returns the vertices of all its parts
   *
   * @return the vertices of all its parts
   */
  virtual std::set< uint64_t > getVertices() const override;

protected:
    BoWComplexToken(BoWComplexTokenPrivate& d);
};


} // namespace BagOfWords
} // namespace Common
} // namespace Lima

#endif
