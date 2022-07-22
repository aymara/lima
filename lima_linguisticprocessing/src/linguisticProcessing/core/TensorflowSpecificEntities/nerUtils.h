// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DATA_UTILS_H
#define DATA_UTILS_H

// includes system
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <QString>
#include <QTextStream>
#include <QFile>
#include <stdexcept>
#include "tensorflow/core/public/session.h"
#include "tensorflow/core/platform/env.h"
#include "tensorflow/cc/client/client_session.h"

#include "linguisticProcessing/core/TensorflowSpecificEntities/TensorflowSpecificEntitiesExport.h"

/** @brief Indicates if the code is running well
 */
enum class NERStatusCode : unsigned int {SUCCESS,MISSING_DATA,EMPTY_FILE};

/**
 * @class UnknownWordClassException
 * @brief Class exception
 */
class UnknownWordClassException : public std::runtime_error {
public:
    UnknownWordClassException(const std::string& s= "") :
        std::runtime_error(s){}
    virtual ~UnknownWordClassException() throw() {}
};

/**
 * @class BadFileException
 * @brief Class exception
 */
class BadFileException : public std::logic_error {
public:
    BadFileException(const std::string& s= "") :
        std::logic_error(s){}
    virtual ~BadFileException() throw() {}
};

/** @brief Loads the vocabulary from a file into a std::map
 * @param[in] filepath the file to load, e.g. data/IOB1/words.txt
 * @return a map which associates to each word from the file an identifier
*/
LIMA_TENSORFLOWSPECIFICENTITIES_EXPORT std::map<QString,int> loadFileWords(const QString& filepath);

/** @brief Loads the vocabulary from a file into a std::map
 * @param[in] filepath the file to load, e.g. data/IOB1/chars.txt
 * @return a map which associates to each character from the file an identifier
*/
LIMA_TENSORFLOWSPECIFICENTITIES_EXPORT std::map< QChar, int > loadFileChars(const QString&  filepath);

/** @brief Loads the vocabulary from a file into a std::map
 * @param[in] filepath the file to load, e.g. data/IOB1/tags.txt
 * @return a map which associates to each tag from the file an identifier
*/
LIMA_TENSORFLOWSPECIFICENTITIES_EXPORT std::map<unsigned int,QString> loadFileTags(const QString& filepath);

/** @brief Opens the text to evaluate
 * @param[in] filepath the file to load, e.g. data/test_to_evaluate.txt
 * @return NERStatusCode::SUCCESS if succeeded else NERStatusCode::EMPTY_FILE
*/
LIMA_TENSORFLOWSPECIFICENTITIES_EXPORT NERStatusCode loadTextToEvaluate(
  QTextStream& qtIn,
  QFile& file,
  const std::string& filepath);

/** @brief Associates to each character of the @ref wordOriginal["word"] its identifier according to the @ref vocabChars and to the word itself according to the @ref vocabWords
 *
 * @details
 * Two parameters @ref lowercase and @ref allowUnk have to be considered.
 * If @ref lowercase is set to true, the word sent is set in lowercase.
 * If @ref allowUnk is set to true, words whose are not in the vocabulary are associated to the string "UNK"' s identifier. If it is not, an exception is thrown.
 *
 * @note This step is mandatory to feed the inputs of the net @ref getFeedDict. Each input is a matrix or a vector of int.
 *
 * @param[in] wordOriginal the word to convert
 * @param[in] vocabWords the base vocabulary list of words
 * @param[in] vocabChars the base vocabulary list of chars
 * @param[in] lowercase
 * @param[in] allowUnk
 *
 * @return NERStatusCode::SUCCESS if succeeded else NERStatusCode::EMPTY_FILE
 *
 * @throw NerException if @ref allowUnk is set to false and word is unkonw from the vocabulary.
*/
LIMA_TENSORFLOWSPECIFICENTITIES_EXPORT std::pair<std::vector<int>,int> getProcessingWord(
  const QString& wordOriginal,
  const std::map<QString,int>& vocabWords,
  const std::map<QChar,int>& vocabChars,
  bool lowercase=false, bool allowUnk=true);

/** @brief predicts the tags for each word in a sentence
 *
 * @details
 * 1. initializes the input tensors calling @ref getFeedDict function
 * 2. run the session to fetch the logits and transition matrix through the net
 * 3. calls @ref viterbiDecode function to choose the correct tag which
 * returns the result in @ref result matrix
 *
 * @note the learning graph is supposed to be already loaded into the session
 *
 * @param[out] status success or failure of a call in Tensorflow.
 * @param[out]session the environment in which the architecture is executed
 * @param[in] batchSize size of the current batch
 * @param[in,out] charIds list of identifiers of each character from each word in each sentence from the batch, maybe updated with padding
 * @param[in,out] wordIds list of identifiers of each word in each sentence from the batch
 * @param[in,out] result
 *
 * @return
 * NERStatusCode::SUCCESS if success
 * else NERStatusCode::MISSING_DATA if some data are missing
*/
LIMA_TENSORFLOWSPECIFICENTITIES_EXPORT NERStatusCode predictBatch(
  const std::shared_ptr<tensorflow::Status>& status,
  tensorflow::Session* session,
  int batchSize,
  std::vector<std::vector< std::vector< int > >>& charIds,
  std::vector<std::vector< int >>& wordIds,
  std::vector<Eigen::MatrixXi>& result);

/** @brief Feeds the input tensors
 *
 * @param[out] inputs dict {input_name,value}
 * @param[in] charIds list of identifiers of each character from each word in each sentence from the batch, maybe updated with padding
 * @param[in] wordIds wordIds list of identifiers of each word in each sentence from the batch
 * @param[in,out] wordIdsT input tensor storing wordIds list
 * @param[in,out] sequenceLengthsT input tensor storing size of sentences
 * @param[in,out] charIdsT input tensor storing @ref charIds list
 * @param[in,out] wordLengthsT input tensor storing size of each word from each sentence from the current batch
 * @param[in,out] dropoutT dropout tensor set to 1 for development
 * @param[in] batchSize size of the current batch
 * @param[in] maxSizeSentence of a words' list
 * @param[in] maxSizeWord of a characters' list
 *
 * @return NERStatusCode::SUCCESS if succeeded else NERStatusCode::MISSING_DATA if data is missing
*/
LIMA_TENSORFLOWSPECIFICENTITIES_EXPORT NERStatusCode getFeedDict(
  std::vector< std::pair< std::string, tensorflow::Tensor > >& inputs,
  std::vector<std::vector< std::vector< int > >>& charIds,
  std::vector<std::vector< int >>& wordIds,
  tensorflow::Tensor& wordIdsT,
  tensorflow::Tensor& sequenceLengthsT,
  tensorflow::Tensor& charIdsT,
  tensorflow::Tensor& wordLengthsT,
  tensorflow::Tensor& dropoutT,
  int batchSize,
  int maxSizeSentence, int maxSizeWord);

/** @brief Apply viterbi algorithm
 *
 * @details
 * This algorithm is inspired by the viterbiDecode method from tensorflow.contribute.crf.python.crf
 * Goal : Decode the highest scoring sequence of tags
 *
 * @note I choose to use Eigen::Matrix instead of Eigen::Tensor for two reasons :
 * Tensor is in the unspported report
 * and for example method like argmax() returns a new object Tuple which is difficult to store in a matrix and no convertible to int.
 * In order to perform better the algorithm, I choose to use Eigen objects (used also by tensorflow api c++) (src :<https://eigen.tuxfamily.org/dox/index.html>)
 *
 * @param[in] score a [seq_len, num_tags] matrix of logits.
 * @param[in] transitionParams a [num_tags, num_tags] matrix of binary potentials.
 *
 * @return
 * viterbi: A [seq_len] list of integers containing the highest scoring tag indices.
 * viterbi_score: A float containing the score for the Viterbi sequence.
*/
LIMA_TENSORFLOWSPECIFICENTITIES_EXPORT Eigen::MatrixXi viterbiDecode(
  const Eigen::MatrixXf& score,
  const Eigen::MatrixXf& transitionParams);

#endif // DATA_UTILS_H
