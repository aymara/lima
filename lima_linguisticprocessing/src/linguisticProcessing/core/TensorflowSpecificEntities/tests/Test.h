// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef TEST_H
#define TEST_H
#include <QObject>

/**
 * @class Test
 * @brief Test the different functions from src/data_utils
 */
class Test : public QObject {
  Q_OBJECT
private Q_SLOTS:
  void initTestCase();
  void cleanupTestCase();

  void init();
  void cleanup();

  /**
   * @brief Test the function loadFileTags
   */
  void loadTags();
  /**
   * @brief Test the function loadFileChars
   */
  void loadChars();
  /**
   * @brief Test the function loadFileWords
   */
  void loadWords();
  /**
   * @brief Test the function loadTextToEvaluate
   */
  void loadText();
  /**
   * @brief Test the function getProcessingWord
   */
  void testProcessingWords();
  /**
   * @brief Test the function getFeedDict
   */
  void testGetFeedDict();
  /**
   * @brief Test the function viterbiDecode
   */
  void testViterbiDecodeWithoutInputs();
  /**
   * @brief Test the function testPredictBatch
   */
  void testPredictBatch1();
//   /**
//    * @brief Test with one sentence containing one entity
//    */
//   void testPredictBatch2();
//   /**
//    * @brief Analyze different sentences with or without entities using minibatching (size=2)
//    */
//   void testPredictBatch3();
};

#endif //TEST_H
