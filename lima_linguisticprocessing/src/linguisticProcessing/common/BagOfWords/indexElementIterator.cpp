/*
    Copyright 2002-2015 CEA LIST

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
 * @author     Besancon Romaric (romaric.besancon@cea.fr)
 * @date       Tue Feb  7 2006
 ***********************************************************************/

#include "bowNamedEntity.h"
#include "bowText.h"
#include "indexElement.h"

#include "linguisticProcessing/common/BagOfWords/BoWRelation.h"
#include "linguisticProcessing/common/BagOfWords/bowTerm.h"
#include "linguisticProcessing/common/BagOfWords/BoWPredicate.h"
#include "indexElementIterator.h"
#include "common/FsaAccess/AbstractLexiconIdGenerator.h"
#include "defaultIdGenerator.h"
#include "common/Data/strwstrtools.h"
#include "common/MediaticData/EntityType.h"
#include "common/MediaticData/mediaticData.h"
#include <limits.h>
#include <algorithm>

using namespace std;
using namespace Lima::Common::Misc;

namespace Lima {
namespace Common {
namespace BagOfWords {


class IndexElementIteratorPrivate
{
  friend class IndexElementIterator;
  
  IndexElementIteratorPrivate(const BoWText& bowText,
                       AbstractLexiconIdGenerator* idAccessor=0,
                       const uint64_t maxCompoundSize=0,
                       const uint64_t nbMaxPartialCompounds=1000);
  IndexElementIteratorPrivate(const IndexElementIteratorPrivate& ieip);
  ~IndexElementIteratorPrivate();


  /** */
  void getPositionLengthList(const std::vector<uint64_t>& structure,
                             Misc::PositionLengthList& poslenlist) const;
                             
  /** Add @ref newElement in queue, only if queue size is lower than its maximum.
    * Only used for compound elements 
    * @return true if the element has been added and false otherwise (size of queue would become 
    * greater than max)
    */
  bool addInPartQueue(const IndexElement& newElement);

  /** Calls addPartElementsInQueue to recursively add @ref token parts and itself in the queue
   */
  void storePartsInQueue(boost::shared_ptr< BoWToken > token);

  bool addPartElementsInQueue(boost::shared_ptr< BoWToken > token,
                              std::pair<std::vector<uint64_t>, uint64_t> & ids_rels,
                              const uint64_t rel);

  /** 
    * this function is recursive to build all composed elements that contains
    * the head and all or parts of the extensions, for all possible values (ids) 
    * of head and extensions
    * 
    * @param partIdsRels : the possible ids of each part, plus one relation per part
    * @param head : the position of the head in the parts
    * @param ids : the id list in which new ids are added for combined element
    * @param structure : the current structure
    * @param i : the current part looked at
    * 
    * @return 
    */
  bool addCombinedPartsInQueue(const Lima::Common::BagOfWords::BoWType type,
                               const std::vector<std::pair<std::vector<uint64_t>, uint64_t> >& partIds_Rels,
                               const uint64_t head,
                               const Common::MediaticData::EntityType neType,
                               std::pair<std::vector<uint64_t>, uint64_t>& ids_rels,
                               std::vector<uint64_t>& structure,
                               std::vector<uint64_t>& relations,
                               const uint64_t i);

  typedef std::deque<IndexElement> IndexElementQueue;

  // members
  BoWText::const_iterator m_iterator;
  BoWText::const_iterator m_iteratorEnd;
  IndexElementQueue m_partQueue;
  uint64_t m_maxSizeQueue;
  uint64_t m_maxCompoundSize;
  AbstractLexiconIdGenerator* m_idGenerator;
  QMap<QString,IndexElement> m_alreadyFoundElements;
};

IndexElementIteratorPrivate::IndexElementIteratorPrivate(const BoWText& bowText,
                     AbstractLexiconIdGenerator* idGenerator,
                     const uint64_t maxCompoundSize,
                     const uint64_t nbMaxPartialCompounds):
m_iterator(bowText.begin()),
m_iteratorEnd(bowText.end()),
m_partQueue(),
m_maxSizeQueue(nbMaxPartialCompounds),
m_maxCompoundSize(maxCompoundSize),
m_idGenerator(idGenerator)
{
  if (m_idGenerator==0) {
    m_idGenerator=new DefaultIdGenerator(
       AbstractLexiconIdGeneratorInformer::getInstance() );
  }
  if (m_maxCompoundSize==0) {
    // if 0, no limitation => set it to UINT_MAX to avoid testing 0
    // each time
    m_maxCompoundSize=UINT_MAX;
  }
}

IndexElementIteratorPrivate::IndexElementIteratorPrivate(const IndexElementIteratorPrivate& ieip):
m_iterator(ieip.m_iterator),
m_iteratorEnd(ieip.m_iteratorEnd),
m_partQueue(ieip.m_partQueue),
m_maxSizeQueue(ieip.m_maxSizeQueue),
m_maxCompoundSize(ieip.m_maxCompoundSize)
{
  m_idGenerator=new DefaultIdGenerator(
    AbstractLexiconIdGeneratorInformer::getInstance() );
  *m_idGenerator = *ieip.m_idGenerator;
}

IndexElementIteratorPrivate::~IndexElementIteratorPrivate()
{
  delete m_idGenerator;
}


//***********************************************************************
// constructors and destructors
IndexElementIterator::IndexElementIterator(const BoWText& bowText,
                     AbstractLexiconIdGenerator* idGenerator,
                     const uint64_t maxCompoundSize,
                     const uint64_t nbMaxPartialCompounds):
    m_d(new IndexElementIteratorPrivate(bowText, idGenerator, maxCompoundSize, nbMaxPartialCompounds))
{
}

IndexElementIterator::IndexElementIterator(const IndexElementIterator& iei):
    m_d(new IndexElementIteratorPrivate(*iei.m_d))
{
}

IndexElementIterator::~IndexElementIterator()
{
  delete m_d;
}

//***********************************************************************

bool IndexElementIterator::isAtEnd() const
{
  return (m_d->m_iterator == m_d->m_iteratorEnd);
}

//**********************************************************************
// get current element ("dereference" iterator)
//**********************************************************************
// getting parts is done in this function (rather than in ++ function):
// which means that if a ++ is done before calling a getElement on 
// a complex token, no parts will be explored
IndexElement IndexElementIterator::getElement()
{
#ifdef DEBUG_CD
  BOWLOGINIT;
  LDEBUG  << "IndexElementIterator::getElement empty:" << m_d->m_partQueue.empty();
#endif  
  // If queue is empty
  // - for simple tokens: a new index element is returned 
  // - for complex tokens : it is filled and then its front is returned
  if (m_d->m_partQueue.empty())
  {
    if (m_d->m_iterator==m_d->m_iteratorEnd)
    { // at end
#ifdef DEBUG_CD
      LDEBUG  << "IndexElementIterator::getElement at end: return empty element";
#endif  
      return IndexElement(); // empty element has id 0
    }
    else
    {
      boost::shared_ptr< BoWToken> token = boost::dynamic_pointer_cast<BoWToken>((*m_d->m_iterator));
      boost::shared_ptr< BoWPredicate > predicate;
      
      switch ((*m_d->m_iterator)->getType())
      {
        case BoWType::BOW_TOKEN:
        {
#ifdef DEBUG_CD
          LDEBUG  << "IndexElementIterator::getElement simple token:" << token->getIdUTF8String();
#endif
          if (!m_d->m_alreadyFoundElements.contains(QString::fromUtf8(token->getIdUTF8String().c_str())))
          {
            m_d->m_alreadyFoundElements.insert(QString::fromUtf8(token->getIdUTF8String().c_str()),
                                                IndexElement(m_d->m_idGenerator->getId(token->getString()),
                                                              token->getType(),
                                                              token->getLemma(),
                                                              token->getCategory(),
                                                              token->getPosition(),
                                                              token->getLength()
                                                            ));
          }
          return m_d->m_alreadyFoundElements[QString::fromUtf8(token->getIdUTF8String().c_str())];
        }
        case BoWType::BOW_TERM:
#ifdef DEBUG_CD
          LDEBUG  << "IndexElementIterator::getElement term:" << token->getIdUTF8String();
#endif
          m_d->storePartsInQueue(token);
          if (m_d->m_partQueue.empty()) {
#ifdef DEBUG_CD
            LDEBUG  << "IndexElementIterator::getElement term: part queue is empty" ;
#endif
            (*this)++;
            return getElement();
          }
#ifdef DEBUG_CD
          LDEBUG  << "IndexElementIterator::getElement term after storePartsInQueue front is:" << m_d->m_partQueue.front();
#endif
          m_d->m_alreadyFoundElements.insert(QString::fromUtf8(token->getIdUTF8String().c_str()),m_d->m_partQueue.front());
          return m_d->m_partQueue.front();
          
        case BoWType::BOW_NAMEDENTITY:
#ifdef DEBUG_CD
          LDEBUG  << "IndexElementIterator::getElement named entity:" << boost::dynamic_pointer_cast<BoWNamedEntity>(*m_d->m_iterator)->getIdUTF8String() ;//<< Lima::Common::MediaticData::MediaticData::single().getEntityName(static_cast<BoWNamedEntity*>((*m_d->m_iterator))->getNamedEntityType());
          // element itself will be stored in queue as part
#endif
          m_d->storePartsInQueue(token);
#ifdef DEBUG_CD
          LDEBUG  << "IndexElementIterator::getElement ne after storePartsInQueue front is:" << m_d->m_partQueue.front();
#endif
          m_d->m_alreadyFoundElements.insert(QString::fromUtf8(token->getIdUTF8String().c_str()),m_d->m_partQueue.front());
          return m_d->m_partQueue.front();
          
        // FIXME Change the handling of predicates to take into account their complex structure nature
        case BoWType::BOW_PREDICATE:
        {
          predicate = boost::dynamic_pointer_cast<BoWPredicate>((*m_d->m_iterator));
          uint64_t id=m_d->m_idGenerator->getId(predicate->getString());
          return IndexElement(id,
                              predicate->getType(),
                              predicate->getString(),
                              0,
                              predicate->getPosition(),
                              predicate->getLength(),
                              predicate->getPredicateType()
                            );
        }
        case BoWType::BOW_NOTYPE:
          return IndexElement();
      }
    }
  }
  // Queue was not empty, returning its front
  else {
#ifdef DEBUG_CD
    LDEBUG  << "IndexElementIterator::getElement empty:" << m_d->m_partQueue.empty() << "return part queue front" << m_d->m_partQueue.front();
#endif
    return m_d->m_partQueue.front();
  }
  
  // Unreachable
  return IndexElement(); // empty element has id 0
}

//**********************************************************************
// operator ++
//**********************************************************************
IndexElementIterator& IndexElementIterator::operator++() 
{
#ifdef DEBUG_CD
  BOWLOGINIT;
#endif
  // If queue is empty, try to advance the text iterator to the next BoWToken
  // Otherwose, pop the front element and advance the text iterator if the queue is now empty
  if (m_d->m_partQueue.empty()) {
#ifdef DEBUG_CD
    LDEBUG << "IndexElementIterator::operator++ part queue is empty";
#endif
   if (m_d->m_iterator!=m_d->m_iteratorEnd) {
      m_d->m_iterator++;
      // Jump already found elements
#ifdef DEBUG_CD
      LDEBUG << "IndexElementIterator::operator++ Jump if necessary";
#endif
      while (m_d->m_iterator != m_d->m_iteratorEnd &&
          boost::dynamic_pointer_cast<BoWToken>((*m_d->m_iterator)) &&
          m_d->m_alreadyFoundElements.contains( QString::fromUtf8(boost::dynamic_pointer_cast<BoWToken>((*m_d->m_iterator))->getIdUTF8String().c_str()) ) ) {
        m_d->m_iterator++;
      }
    }
  }
  else {
#ifdef DEBUG_CD
    LDEBUG << "IndexElementIterator::operator++ part queue not empty";
#endif
    m_d->m_partQueue.pop_front();
    if (m_d->m_partQueue.empty()) { // finished for the parts of this token
      m_d->m_iterator++;
      // Jump already found elements
      while (m_d->m_iterator != m_d->m_iteratorEnd &&
          boost::dynamic_pointer_cast<BoWToken>((*m_d->m_iterator)) &&
          m_d->m_alreadyFoundElements.contains( QString::fromUtf8(boost::dynamic_pointer_cast<BoWToken>((*m_d->m_iterator))->getIdUTF8String().c_str()) ) ) {
       m_d->m_iterator++;
      }
    }
  }
  return *this;
}

// postfix ++ operator
IndexElementIterator IndexElementIterator::operator++(int) {
  IndexElementIterator it = *this;
  ++(*this);
  return it;
}

//**********************************************************************
// helper functions for iterator
//**********************************************************************
void IndexElementIteratorPrivate::getPositionLengthList(const std::vector<uint64_t>& structure,
                      PositionLengthList& poslenlist) const
{
  // update position/length list for structure
  // use previous elements in queue
  for (std::vector<uint64_t>::const_iterator it = structure.begin(); it != structure.end(); ++it) {
    
    QMap<QString,IndexElement>::const_iterator found = m_alreadyFoundElements.begin();
    while (found != m_alreadyFoundElements.end() && *it != found.value().getId()) {
      ++found;
    }
    
    if (found != m_alreadyFoundElements.end()) {
      const PositionLengthList& p = found.value().getPositionLengthList();
      poslenlist.insert(poslenlist.end(), p.begin(), p.end());
    }
    else {
      BOWLOGINIT
      LERROR << "getPositionLengthList failure: element id " << *it << " not found";
    }
  }

  // sort positions
  std::sort(poslenlist.begin(),poslenlist.end());
}


bool IndexElementIteratorPrivate::addInPartQueue(const IndexElement& newElement)
{
#ifdef DEBUG_CD
  BOWLOGINIT;
  LDEBUG << "IndexElementIteratorPrivate::addInPartQueue" << newElement;
#endif
  if (m_partQueue.size() >= m_maxSizeQueue) {
    BOWLOGINIT;
    LWARN << "size of queue exceeded"; 
    return false;
  }
  
  m_partQueue.push_back(newElement);
//   BOWLOGINIT;
//   if (logger.isDebugEnabled()) {
//     ostringstream oss;
//     for (vector<uint64_t>::const_iterator it=structure.begin(),
//            it_end=structure.end(); it!=it_end; it++) {
//       oss << *it << ";";
//     }
//     LDEBUG << "add in part queue " << id << ":" 
//            << oss.str()
//            << ";size of queue=" << m_partQueue.size()
//           ;
//   }
  return true;
}


void IndexElementIteratorPrivate::storePartsInQueue(boost::shared_ptr< Lima::Common::BagOfWords::BoWToken > token)
{
#ifdef DEBUG_CD
  BOWLOGINIT;
  LDEBUG << "IndexElementIteratorPrivate::storePartsInQueue" << token->getIdUTF8String();
#endif
  pair<vector<uint64_t>, uint64_t> tokenIds;
  if (!addPartElementsInQueue(token,tokenIds,0)) {
    BOWLOGINIT;
    LWARN << "Token contain too many subparts (some are ignored): " << token->getLemma();
  }
}

bool IndexElementIteratorPrivate::addPartElementsInQueue(boost::shared_ptr< BoWToken > token,
                       pair<vector<uint64_t>, uint64_t>& ids_rel,
                       uint64_t rel) 
{
#ifdef DEBUG_CD
  BOWLOGINIT;
  LDEBUG << "IndexElementIteratorPrivate::addPartElementsInQueue" << token->getIdUTF8String() << rel;
#endif

  Common::MediaticData::EntityType neType;
  bool result = false;
  switch (token->getType())
  {
    case BoWType::BOW_TOKEN:
    {
#ifdef DEBUG_CD
      LDEBUG  << "IndexElementIteratorPrivate::addPartElementsInQueue simple token:" << token->getIdUTF8String();
#endif
     if (!m_alreadyFoundElements.contains(QString::fromUtf8(token->getIdUTF8String().c_str())))
      {
        LimaString lemma=token->getLemma();
        if (lemma.size()==0) {
            lemma=token->getInflectedForm();
        }
        // simple token : get Id and push in parts
        uint64_t id=m_idGenerator->getId(token->getString());

        m_alreadyFoundElements.insert(QString::fromUtf8(token->getIdUTF8String().c_str()),IndexElement(id,
                              token->getType(),
                              lemma,
                              token->getCategory(),
                              token->getPosition(),
                              token->getLength(),
                              neType));
        result = addInPartQueue(m_alreadyFoundElements[QString::fromUtf8(token->getIdUTF8String().c_str())]);
      } else {
        result = true;
      }
      ids_rel=make_pair(vector<uint64_t>(1,m_alreadyFoundElements[QString::fromUtf8(token->getIdUTF8String().c_str())].getId()),rel);
      return result;
    }
    case BoWType::BOW_NAMEDENTITY: 
      neType=boost::dynamic_pointer_cast<BoWNamedEntity>(token)->getNamedEntityType();
      break;
    case BoWType::BOW_TERM:
    case BoWType::BOW_PREDICATE:
    case BoWType::BOW_NOTYPE:
    default:;
  }

  // is a complex token
  boost::shared_ptr< BoWComplexToken > complexToken=
    boost::dynamic_pointer_cast<BoWComplexToken>(token);
  
  if (complexToken==0) {
    BOWLOGINIT;
    LERROR << "failed to convert BoWText element in complex token";
    return false;
  }

  if (complexToken->size() == 1) {
#ifdef DEBUG_CD
    LDEBUG  << "IndexElementIteratorPrivate::addPartElementsInQueue complex token of size one";
#endif
    // only one part, do not get into it
    // (for instance, named entity with one element)
    // push simple token in parts 
    if (!m_alreadyFoundElements.contains(QString::fromUtf8(token->getIdUTF8String().c_str())))
    {
      uint64_t id=m_idGenerator->getId(token->getString());
      ids_rel=make_pair(vector<uint64_t>(1,id),rel);

      LimaString lemma=token->getLemma();
      if (lemma.size()==0) {
          lemma=token->getInflectedForm();
      }
      m_alreadyFoundElements.insert(QString::fromUtf8(token->getIdUTF8String().c_str()), IndexElement(id,
                          token->getType(),
                          lemma,
                          token->getCategory(),
                          token->getPosition(),
                          token->getLength(),
                          neType));
      result = addInPartQueue(m_alreadyFoundElements[QString::fromUtf8(token->getIdUTF8String().c_str())]);
    } else {
      return result = true;
    }
    return result;
  }
  
#ifdef DEBUG_CD
  LDEBUG  << "IndexElementIteratorPrivate::addPartElementsInQueue complex token of size" << complexToken->size();
#endif
  ids_rel=make_pair(vector<uint64_t>(),rel);
  uint64_t nbParts=complexToken->getParts().size();
  uint64_t head=complexToken->getHead();
  vector<pair<vector<uint64_t>, uint64_t> > partIdsRels(nbParts);
  for (uint64_t i=0; i<nbParts; i++) {
#ifdef DEBUG_CD
    LDEBUG  << "IndexElementIteratorPrivate::addPartElementsInQueue on part" << i << "of complex token" << *complexToken;
#endif
    pair<vector<uint64_t>, uint64_t>& thisPartIdsRels=partIdsRels[i];
    uint64_t relType;
    boost::shared_ptr< BoWRelation > relation=(complexToken->getParts()[i]).getBoWRelation();
    if (relation !=0 ) relType=relation->getSynType(); else  relType=0; 
    if (!addPartElementsInQueue(complexToken->getParts()[i].getBoWToken(),thisPartIdsRels,relType)) {
      return false;
    }
    
    if (i==head) {
      // add ids of the head
        ids_rel.first.insert(ids_rel.first.end(),thisPartIdsRels.first.begin(),thisPartIdsRels.first.end());
    }
  }
#ifdef DEBUG_CD
  LDEBUG  << "IndexElementIteratorPrivate::addPartElementsInQueue parts added; combining them";
#endif
  // add ids for combined parts
  vector<uint64_t> structure; //current structure in recursive function
  vector<uint64_t> relations; //current relations in recursive function
  if (!addCombinedPartsInQueue(token->getType(),partIdsRels,head,neType,ids_rel,structure,relations,0)) {
    return false;
  }
  return true;
}

bool IndexElementIteratorPrivate::addCombinedPartsInQueue(
    const Lima::Common::BagOfWords::BoWType type,
    const std::vector<std::pair<std::vector<uint64_t>, uint64_t> >& partIdsRels,
    const uint64_t head,
    const Common::MediaticData::EntityType neType,
    std::pair<std::vector<uint64_t>, uint64_t>& ids_rel,
    std::vector<uint64_t>& structure,
    std::vector<uint64_t>& relations,
    const uint64_t current)
{
#ifdef DEBUG_CD
  BOWLOGINIT;
#endif
  QStringList structureKey;
#ifdef ANTINNO_SPECIFIC
  // Modif NAN pour que ça compile sous Visual 2010
  for (auto itElement=structure.begin(),it_end=structure.end(); itElement!=it_end; itElement++) {
    structureKey << QString::number(*itElement);
#else
  for (auto it = structure.begin(); it != structure.end(); ++it) {
    structureKey << QString::number(*it);
#endif
  }
#ifdef DEBUG_CD
  LDEBUG << "addCombinedPartsInQueue: nb parts=" << partIdsRels.size() 
        << ", head=" << head << ", current=" << current << ", structure=" << structureKey.join(";");
#endif
  bool result = false;
  if (current>=partIdsRels.size()) {
    if (structure.size() == 1) {
      //just the head: is already in queue
#ifdef DEBUG_CD
      LDEBUG << "addCombinedPartsInQueue: just the head: is already in queue";
#endif
      return true;
    }
    // build indexElement before getting the id : allow to have the
    // true size of compound (trick: use PositionLengthList to have
    // the size: number of leaves of the structure), and to avoid
    // compute the id if size is more than maxCompoundSize
    if (!m_alreadyFoundElements.contains(structureKey.join(";")))
    {
      IndexElement compoundElement(0,type,structure,relations,neType);
      getPositionLengthList(structure,compoundElement.getPositionLengthList());
      if (compoundElement.getPositionLengthList().size() > m_maxCompoundSize) {
        // compound larger than allowed, do not add it in parts, but
        // return true anyway (false is reserved for queue overflow)
#ifdef DEBUG_CD
        LDEBUG << "addCombinedPartsInQueue: just the head: max compound size exceeded";
#endif
        return true;
      }
      // at end of parts => add current structure
      
      uint64_t id=m_idGenerator->getId(structure);
#ifdef DEBUG_CD
      LDEBUG << "IndexElementIterator: got id from generator " << id;
#endif
      compoundElement.setId(id);
      m_alreadyFoundElements.insert(structureKey.join(";"),compoundElement);
      if (!addInPartQueue(m_alreadyFoundElements[structureKey.join(";")])) {
#ifdef DEBUG_CD
        LDEBUG << "addCombinedPartsInQueue: queue overflow";
#endif
        return false;
      } else {
        result = true;
      }
    } else {
      result = true;
    }
    ids_rel.first.push_back(m_alreadyFoundElements[structureKey.join(";")].getId());
#ifdef DEBUG_CD
    LDEBUG << "addCombinedPartsInQueue: added to ids_rel.first: " << m_alreadyFoundElements[structureKey.join(";")].getId() << "; return" << result;
#endif
    return result;
  }

  // add possible at end of structure and recursive call
#ifdef ANTINNO_SPECIFIC
  Q_FOREACH (auto it,partIdsRels[current].first) {
#else
  for (auto it = partIdsRels[current].first.begin(); it != partIdsRels[current].first.end(); ++it) {
#endif
    structure.push_back(*it);
    relations.push_back(partIdsRels[current].second);
    if (!addCombinedPartsInQueue(type, partIdsRels,head,neType,ids_rel,structure,relations,current+1)) {
#ifdef DEBUG_CD
      LDEBUG << "addCombinedPartsInQueue: recursive call returned false";
#endif
      return false;
    }
    structure.pop_back();
    relations.pop_back();
  }
  // if head, stop here: current iterator is head, hence always added
  // otherwise, recursive call without current iterator (that is an
  // extension)
  if (current!=head) {
    if (!addCombinedPartsInQueue(type, partIdsRels,head,neType,ids_rel,structure,relations,current+1)) {
#ifdef DEBUG_CD
      LDEBUG << "addCombinedPartsInQueue: second recursive call returned false";
#endif
      return false;
    }
  }
  return true;
}

} // end namespace
} // end namespace
} // end namespace
