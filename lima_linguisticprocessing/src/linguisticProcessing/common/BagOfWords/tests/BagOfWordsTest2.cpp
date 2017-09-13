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

#include "BagOfWordsTest2.h"

#include "linguisticProcessing/common/BagOfWords/indexElement.h"
#include <QtCore/QObject>

using namespace Lima::Common::BagOfWords;
using namespace Lima::Common::MediaticData;

void BagOfWordsTest2::initTestCase()
{
}

void BagOfWordsTest2::test_indexElementDefaultConstructor()
{
  qDebug() << "BagOfWordsTest2::test_indexElementDefaultConstructor";
//     IndexElement();
  IndexElement el;
  QVERIFY(el.getId() == 0);
  QVERIFY(el.getType() == BoWType::BOW_NOTYPE);
  QVERIFY(el.getSimpleTerm() == "");
  QVERIFY(el.getCategory() == 0);
  QVERIFY(el.getPosition() == 0);
  QVERIFY(el.getLength() == 0);
  QVERIFY(el.getNamedEntityType() == EntityType());
  QVERIFY(!el.isSimpleTerm());
  QVERIFY(!el.isComposedTerm());
  QVERIFY(!el.isNamedEntity());
  QVERIFY(!el.isPredicate());
  QVERIFY(el.getStructure().empty());
  QVERIFY(el.getRelations().empty());
}

void BagOfWordsTest2::test_indexElementConstructor1()
{
  qDebug() << "BagOfWordsTest2::test_indexElementConstructor1";
//     IndexElement(const uint64_t id,
//                const Lima::Common::BagOfWords::BoWType type,
//                const LimaString& word,
//                const uint64_t cat=0,
//                const uint64_t position=0,
//                const uint64_t length=0,
//                const Common::MediaticData::EntityType neType=Common::MediaticData::EntityType(),
//                const uint64_t reType=0);
  uint64_t id = 1;
  BoWType type = BoWType::BOW_TOKEN;
  QString word = QString::fromUtf8("word");
  uint64_t cat = 0;
  uint64_t position = 0;
  uint64_t length = (uint64_t)word.size();
  EntityType neType = EntityType();

  IndexElement el(id,type,word,cat,position,length,neType);
  QVERIFY(el.getId() == id);
  QVERIFY(el.getType() == BoWType::BOW_TOKEN);
  QVERIFY(el.getSimpleTerm() == word);
  QVERIFY(el.getCategory() == cat);
  QVERIFY(el.getPosition() == position);
  QVERIFY(el.getLength() == length);
  QVERIFY(el.getNamedEntityType() == neType);
  QVERIFY(el.isSimpleTerm());
  QVERIFY(!el.isComposedTerm());
  QVERIFY(!el.isNamedEntity());
  QVERIFY(!el.isPredicate());
  QVERIFY(el.getStructure().empty());
  QVERIFY(el.getRelations().empty());
}

void BagOfWordsTest2::test_indexElementConstructor2()
{
  qDebug() << "BagOfWordsTest2::test_indexElementConstructor2";
//   IndexElement(const uint64_t id,
//                const Lima::Common::BagOfWords::BoWType type,
//                const std::vector<uint64_t>& structure,
//                const std::vector<uint64_t>& relations,
//                const Common::MediaticData::EntityType neType=Common::MediaticData::EntityType(),
//                const uint64_t reType=0);
  uint64_t id = 2;
  BoWType type = BoWType::BOW_TERM;
  std::vector<uint64_t> structure;
  std::vector<uint64_t> relations;
  EntityType neType = EntityType();

  IndexElement el(id,type,structure,relations,neType);
  QVERIFY(el.getId() == id);
  QVERIFY(el.getType() == BoWType::BOW_TERM);
  QVERIFY(el.getSimpleTerm().isEmpty());
  QVERIFY(el.getCategory() == 0);
  QVERIFY(el.getPosition() == 0);
  QVERIFY(el.getLength() == 0);
  QVERIFY(el.getNamedEntityType() == neType);
  QVERIFY(!el.isSimpleTerm());
  QVERIFY(el.isComposedTerm());
  QVERIFY(!el.isNamedEntity());
  QVERIFY(!el.isPredicate());
  QVERIFY(el.getStructure().empty());
  QVERIFY(el.getRelations().empty());
}

void BagOfWordsTest2::test_indexElementCopyConstructor()
{
  qDebug() << "BagOfWordsTest2::test_indexElementCopyConstructor";
//   IndexElement(const IndexElement& ie);
  uint64_t id = 1;
  BoWType type = BoWType::BOW_TOKEN;
  QString word = QString::fromUtf8("word");
  uint64_t cat = 0;
  uint64_t position = 0;
  uint64_t length = (uint64_t)word.size();
  EntityType neType = EntityType();

  IndexElement* el = new IndexElement(id,type,word,cat,position,length,neType);
  IndexElement el_copy(*el);
  // Test members after copy
  QVERIFY(el->getId() == el_copy.getId());
  QVERIFY(el->getType() == el_copy.getType());
  QVERIFY(el->getSimpleTerm() == el_copy.getSimpleTerm());
  QVERIFY(el->getCategory() == el_copy.getCategory());
  QVERIFY(el->getPosition() == el_copy.getPosition());
  QVERIFY(el->getLength() == el_copy.getLength());
  QVERIFY(el->getNamedEntityType() == el_copy.getNamedEntityType());
  QVERIFY(el->isSimpleTerm() && el_copy.isSimpleTerm());
  QVERIFY(!el->isComposedTerm() && !el_copy.isComposedTerm());
  QVERIFY(!el->isNamedEntity() && !el_copy.isNamedEntity());
  QVERIFY(!el->isPredicate() && !el_copy.isPredicate());
  QVERIFY(el->getStructure().empty() && el_copy.getStructure().empty());
  QVERIFY(el->getRelations().empty() && el_copy.getRelations().empty());


  delete el; el = 0;
  // Test members after deleting original objects
  QVERIFY(el_copy.getId() == 1);
  QVERIFY(el_copy.getType() == BoWType::BOW_TOKEN);
  QVERIFY(el_copy.getSimpleTerm() == "word");
  QVERIFY(el_copy.getCategory() == 0);
  QVERIFY(el_copy.getPosition() == 0);
  QVERIFY(el_copy.getLength() == (uint64_t)word.size());
  QVERIFY(el_copy.getNamedEntityType() == neType);
  QVERIFY(el_copy.isSimpleTerm());
  QVERIFY(!el_copy.isComposedTerm());
  QVERIFY(!el_copy.isNamedEntity());
  QVERIFY(!el_copy.isPredicate());
  QVERIFY(el_copy.getStructure().empty());
  QVERIFY(el_copy.getRelations().empty());
}

void BagOfWordsTest2::test_indexElementOperatorAffect()
{
  qDebug() << "BagOfWordsTest2::test_indexElementCopyConstructor";
//   IndexElement(const IndexElement& ie);
  uint64_t id = 1;
  BoWType type = BoWType::BOW_TOKEN;
  QString word = QString::fromUtf8("word");
  uint64_t cat = 0;
  uint64_t position = 0;
  uint64_t length = (uint64_t)word.size();
  EntityType neType = EntityType();
  IndexElement* el = new IndexElement(id,type,word,cat,position,length,neType);

  uint64_t id2 = 2;
  BoWType type2 = BoWType::BOW_TERM;
  QString word2 = QString::fromUtf8("other");
  uint64_t cat2 = 1;
  uint64_t position2 = 10;
  uint64_t length2 = (uint64_t)word.size();
  EntityType neType2 = EntityType();
  IndexElement el2(id2,type2,word2,cat2,position2,length2,neType2);

  el2 = *el;

  QVERIFY(el->getId() == el2.getId());
  QVERIFY(el->getType() == el2.getType());
  QVERIFY(el->getSimpleTerm() == el2.getSimpleTerm());
  QVERIFY(el->getCategory() == el2.getCategory());
  QVERIFY(el->getPosition() == el2.getPosition());
  QVERIFY(el->getLength() == el2.getLength());
  QVERIFY(el->getNamedEntityType() == el2.getNamedEntityType());
  QVERIFY(el->isSimpleTerm() && el2.isSimpleTerm());
  QVERIFY(!el->isComposedTerm() && !el2.isComposedTerm());
  QVERIFY(!el->isNamedEntity() && !el2.isNamedEntity());
  QVERIFY(!el->isPredicate() && !el2.isPredicate());
  QVERIFY(el->getStructure().empty() && el2.getStructure().empty());
  QVERIFY(el->getRelations().empty() && el2.getRelations().empty());

  delete el; el = 0;
  // Test members after deleting original objects
  QVERIFY(el2.getId() == 1);
  QVERIFY(el2.getType() == BoWType::BOW_TOKEN);
  QVERIFY(el2.getSimpleTerm() == "word");
  QVERIFY(el2.getCategory() == 0);
  QVERIFY(el2.getPosition() == 0);
  QVERIFY(el2.getLength() == (uint64_t)word.size());
  QVERIFY(el2.getNamedEntityType() == neType);
  QVERIFY(el2.isSimpleTerm());
  QVERIFY(!el2.isComposedTerm());
  QVERIFY(!el2.isNamedEntity());
  QVERIFY(!el2.isPredicate());
  QVERIFY(el2.getStructure().empty());
  QVERIFY(el2.getRelations().empty());

}

QTEST_MAIN(BagOfWordsTest2)

