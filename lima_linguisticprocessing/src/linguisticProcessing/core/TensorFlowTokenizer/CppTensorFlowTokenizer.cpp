/*
    Copyright 2002-2019 CEA LIST

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

#include "CppTensorFlowTokenizer.h"
#include "tokUtils.h"

#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/common/linguisticData/LimaStringText.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "common/misc/Exceptions.h"
#include "common/Data/strwstrtools.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/tools/FileUtils.h"
#include "common/MediaticData/mediaticData.h"
#include "common/time/timeUtilsController.h"

#include <QtCore/QTemporaryFile>

#include <string>

using namespace std;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::Common::Misc;
using namespace tensorflow;

#define EIGEN_DONT_VECTORIZE

namespace Lima
{
namespace LinguisticProcessing
{
namespace TensorFlowTokenizer
{

static SimpleFactory<MediaProcessUnit,CppTensorFlowTokenizer> cpptokenizerFactory(CPPTENSORFLOWTOKENIZER_CLASSID); // clazy:exclude=non-pod-global-static

class CppTokenizerPrivate
{
public:
  CppTokenizerPrivate();
  virtual ~CppTokenizerPrivate();

  void computeDefaultStatus(LinguisticAnalysisStructure::TStatus& curSettings);

  MediaId m_language;
  FsaStringsPool* m_stringsPool;
  LinguisticGraphVertex m_currentVx;

  std::map<QString,int> m_vocabWords;
  std::map<QChar,int> m_vocabChars;
  std::map<unsigned int,QString> m_vocabTags;
  Session* m_session;
  std::shared_ptr<Status> m_status;
  GraphDef m_graphDef;
  int m_batchSizeMax;
  std::string m_graph;
};

CppTokenizerPrivate::CppTokenizerPrivate() :
    m_stringsPool(nullptr),
    m_currentVx(0)
{
}

CppTokenizerPrivate::~CppTokenizerPrivate()
{
}

CppTensorFlowTokenizer::CppTensorFlowTokenizer() : m_d(new CppTokenizerPrivate())
{}

CppTensorFlowTokenizer::~CppTensorFlowTokenizer()
{
  delete m_d;
}

// auto python_error = []()
// {
//   TOKENIZERLOGINIT;
//   LERROR << __FILE__ << __LINE__ << ": Python error";
//   PyErr_Print();
//   Py_Exit(1);
// };
//
// auto failed_to_allocate_memory = []()
// {
//   TOKENIZERLOGINIT;
//   LERROR << __FILE__ << __LINE__ << ": Failed to allocate memory";
//   PyErr_Print();
//   Py_Exit(1);
// };
//
// auto failed_to_import_the_sys_module = []()
// {
//   TOKENIZERLOGINIT;
//   LERROR << "Failed to import the sys module";
//   PyErr_Print();
//   Py_Exit(1);
// };
//
// auto cannot_instantiate_the_tokenizer_python_class = []()
// {
//   TOKENIZERLOGINIT;
//   LERROR << "Cannot instantiate the CppTensorFlowTokenizer python class";
//   PyErr_Print();
//   Py_Exit(1);
// };

void CppTensorFlowTokenizer::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
#ifdef DEBUG_LP
  TOKENIZERLOGINIT;
  LDEBUG << "CppTensorFlowTokenizer::init";
#endif
  m_d->m_language = manager->getInitializationParameters().media;
  m_d->m_stringsPool = &Common::MediaticData::MediaticData::changeable().stringsPool(m_d->m_language);

  QString embeddingsPath; // The path to the LIMA python tensorflow-based tokenizer
  try
  {
    embeddingsPath = QString::fromUtf8(unitConfiguration.getParamsValueAtKey("embeddings_path").c_str());
  }
  catch (NoSuchParam& )
  {
    TOKENIZERLOGINIT;
    LERROR << "no param 'embeddings_path' in PythonTensorFlowTokenizer group configuration";
    throw InvalidConfiguration();
  }

  QString modelPath; // The path to the LIMA python tensorflow-based tokenizer
  try
  {
    modelPath = QString::fromUtf8(unitConfiguration.getParamsValueAtKey("model_path").c_str());
  }
  catch (NoSuchParam& )
  {
    TOKENIZERLOGINIT;
    LERROR << "no param 'model_path' in PythonTensorFlowTokenizer group configuration";
    throw InvalidConfiguration();
  }

  int windowSize = -1; // The window size used with the tensorflow-based tokenizer
  try
  {
    bool success;
    windowSize = QString::fromUtf8(unitConfiguration.getParamsValueAtKey("window_size").c_str()).toInt(&success);
    if (!success)
    {
      TOKENIZERLOGINIT;
      LERROR << "Param 'window_size' in PythonTensorFlowTokenizer group configuration is not an integer";
      throw InvalidConfiguration();
    }
  }
  catch (NoSuchParam& )
  {
    TOKENIZERLOGINIT;
    LERROR << "no param 'model_path' in PythonTensorFlowTokenizer group configuration";
    throw InvalidConfiguration();
  }

  try
  {
    m_d->m_graph=
    Common::Misc::findFileInPaths(
      Common::MediaticData::MediaticData::single().getResourcesPath().c_str(),
      unitConfiguration.getParamsValueAtKey("graphOutputFile").c_str()).toStdString();
  }
  catch (NoSuchParam& )
  {
    LERROR << "no param 'graphOutputFile' in TensorflowSpecificEntities group for m_d->m_language " << (int) m_d->m_language;
    throw InvalidConfiguration();
  }

  QString fileChars,fileWords,fileTags;
  try
  {
    fileChars=Common::Misc::findFileInPaths(
      Common::MediaticData::MediaticData::single().getResourcesPath().c_str(),
      unitConfiguration.getParamsValueAtKey("charValuesFile").c_str());
  }
  catch (NoSuchParam& )
  {
    LERROR << "no param 'charValuesFile' in TensorflowSpecificEntities group for m_d->m_language " << (int) m_d->m_language;
    throw InvalidConfiguration();
  }

  try
  {
      fileWords=
      Common::Misc::findFileInPaths(
      Common::MediaticData::MediaticData::single().getResourcesPath().c_str(),
      unitConfiguration.getParamsValueAtKey("wordValuesFile").c_str());
  }
  catch (NoSuchParam& )
  {
    LERROR << "no param 'wordValuesFile' in TensorflowSpecificEntities group for m_d->m_language " << (int) m_d->m_language;
    throw InvalidConfiguration();
  }

  try
  {
      fileTags=Common::Misc::findFileInPaths(
      Common::MediaticData::MediaticData::single().getResourcesPath().c_str(),
      unitConfiguration.getParamsValueAtKey("tagValuesFile").c_str());
  }
  catch (NoSuchParam& )
  {
    LERROR << "no param 'tagValuesFile' in TensorflowSpecificEntities group for m_d->m_language " << (int) m_d->m_language;
    throw InvalidConfiguration();
  }

  //Minibatching (group of max 20 sentences of different size) is used in order to amortize the cost of loading the network weights from CPU/GPU memory across many inputs.
    //and to take advantage from parallelism.
//     std::string::size_type sz;
  try
  {
    std::string::size_type sz;
    m_d->m_batchSizeMax = std::stoi(unitConfiguration.getParamsValueAtKey("batchSizeMax"),&sz);
  }
  catch (NoSuchParam& )
  {
    LERROR << "no param 'batchSizeMax' in TensorflowSpecificEntities group for m_d->m_language " << (int) m_d->m_language;
    throw InvalidConfiguration();
  }

  try
  {
    m_d->m_vocabWords= loadFileWords(fileWords);
    if(m_d->m_vocabWords.empty()){
      throw LimaException();
    }
    m_d->m_vocabChars= loadFileChars(fileChars);
    if(m_d->m_vocabChars.empty()){
      throw LimaException();
    }
    m_d->m_vocabTags = loadFileTags(fileTags);
    if(m_d->m_vocabTags.empty()){
      throw LimaException();
    }
  }
  catch(const BadFileException& e){
    TFSELOGINIT;
    LERROR<<e.what();
    throw LimaException();
  }

  // Initialize a tensorflow session
  m_d->m_status.reset(new Status(NewSession(SessionOptions(), &m_d->m_session)));
  if (!m_d->m_status->ok()) {
    TFSELOGINIT;
    LERROR << m_d->m_status->ToString();
    throw LimaException();
  }

  // Read in the protobuf graph we have exported
  *m_d->m_status = ReadBinaryProto(Env::Default(), m_d->m_graph, &m_d->m_graphDef);
  if (!m_d->m_status->ok()) {
    TFSELOGINIT;
    LERROR << m_d->m_status->ToString();
    throw LimaException();
  }

  // Add the graph to the session
  *m_d->m_status = m_d->m_session->Create(m_d->m_graphDef);
  if (!m_d->m_status->ok()) {
    TFSELOGINIT;
    LERROR << m_d->m_status->ToString();
    throw LimaException();
  }
}


LimaStatusCode CppTensorFlowTokenizer::process(AnalysisContent& analysis) const
{
  TimeUtilsController TensorFlowTokenizerProcessTime("TensorFlowTokenizer");
  TOKENIZERLOGINIT;
  LINFO << "start tokenizer process";
  auto anagraph = new AnalysisGraph("AnalysisGraph",m_d->m_language,true,true);
  analysis.setData("AnalysisGraph",anagraph);
  LinguisticGraph* graph=anagraph->getGraph();
  m_d->m_currentVx = anagraph->firstVertex();
  // Get text from analysis
  LimaStringText* originalText=static_cast<LimaStringText*>(analysis.getData("Text"));
  std::vector< std::vector< std::pair<QString,int> > > sentencesTokens;
  // Insert the tokens in the graph and create sentence limits
  for (const auto& sentence: sentencesTokens)
  {
    for (const auto& token: sentence)
    {
      const auto& str = token.first;
#ifdef DEBUG_LP
      LDEBUG << "      Adding token '" << str << "'";
#endif
      StringsPoolIndex form=(*m_d->m_stringsPool)[str];
      Token *tToken = new Token(form,str, token.second, token.first.size());
      if (tToken == 0) throw MemoryErrorException();
      LinguisticAnalysisStructure::TStatus curSettings;
      // @todo: set tokenization status

//   _curSettings.setAlphaCapital(alphaCapital);
//   switch (alphaCapital)
//   {
//     case T_CAPITAL:
//       _curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_capital"));
//       break;
//     case T_SMALL:
//       _curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_small"));
//       break;
//     case T_CAPITAL_1ST:
//       _curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_capital_1st"));
//       break;
//     case T_ACRONYM:
//       _curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_acronym"));
//       break;
//     case T_CAPITAL_SMALL:
//       _curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_capital_small"));
//       break;
//     case T_ABBREV:
//       _curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_abbrev"));
//       break;
//     default:
//     _curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_small"));
//   _curSettings.setAlphaRoman(alphaRoman);
//   switch (alphaRoman)
//   {
//     case T_CARDINAL_ROMAN:
//       _curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_cardinal_roman"));
//       break;
//     case T_ORDINAL_ROMAN:
//       _curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_ordinal_roman"));
//       break;
//     case T_NOT_ROMAN:
//       _curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_not_roman"));
//   _curSettings.setAlphaPossessive(isAlphaPossessive);
//  _curSettings.setAlphaConcatAbbrev(isConcatAbbreviation);
//   if (isConcatAbbreviation> 0)
//     _curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_alpha_concat_abbrev"));
//   _curSettings.setTwitter(isTwitter);
//   if (isTwitter> 0)
//     _curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_twitter"));
//   _curSettings.setNumeric(numeric);
//   switch (numeric)
//   {
//     case T_INTEGER:
//       _curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_integer"));
//       break;
//     case T_COMMA_NUMBER:
//       _curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_comma_number"));
//       break;
//     case T_DOT_NUMBER:
//       _curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_dot_number"));
//       break;
//     case T_FRACTION:
//       _curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_fraction"));
//       break;
//     case T_ORDINAL_INTEGER:
//       _curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_ordinal_integer"));
//   _curSettings.setStatus(status);
//   switch (status)
//   {
//     case T_ALPHA:
//       if (previousStatus != T_ALPHA)
//         _curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_alphanumeric"));
//       break;
//     case T_NUMERIC:
//       _curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_alphanumeric"));
//       break;
//     case T_ALPHANUMERIC:
//       _curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_alphanumeric"));
//       break;
//     case T_PATTERN:
//       _curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_pattern"));
//       break;
//     case T_WORD_BRK:
//       _curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_word_brk"));
//       break;
//     case T_SENTENCE_BRK:
//       _curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_sentence_brk"));
//       break;
//     case T_PARAGRAPH_BRK:
//       _curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_paragraph_brk"));


      tToken->setStatus(curSettings);
#ifdef DEBUG_LP
      //   LDEBUG << "      _curSettings is " << _curSettings.toString();
      LDEBUG << "      status is " << tToken->status().toString();
#endif
      // Adds on the path
      LinguisticGraphVertex newVx = add_vertex(*graph);
      put(vertex_token, *graph, newVx, tToken);
      put(vertex_data, *graph, newVx, new MorphoSyntacticData());
      add_edge(m_d->m_currentVx, newVx, *graph);
      m_d->m_currentVx = newVx;
    }
  }

  add_edge(m_d->m_currentVx,anagraph->lastVertex(),*graph);
  remove_edge(anagraph->firstVertex(),
              anagraph->lastVertex(),
              *graph);
  return SUCCESS_ID;
}

// set default key in status according to other elements in status
void CppTokenizerPrivate::computeDefaultStatus(LinguisticAnalysisStructure::TStatus& curSettings)
{
  std::string defaultKey;
  switch (curSettings.getStatus()) {
    case T_ALPHA : {
      switch (curSettings.getAlphaCapital()) {
        case T_CAPITAL       : defaultKey = "t_capital"      ; break;
        case T_SMALL         : defaultKey = "t_small"        ; break;
        case T_CAPITAL_1ST   : defaultKey = "t_capital_1st"  ; break;
        case T_ACRONYM       : defaultKey = "t_acronym"      ; break;
        case T_CAPITAL_SMALL : defaultKey = "t_capital_small"; break;
        case T_ABBREV       : defaultKey = "t_abbrev"      ; break;
        default : break;
      }
      switch (curSettings.getAlphaRoman()) { // Roman supersedes Cardinal
        case T_CARDINAL_ROMAN : defaultKey = "t_cardinal_roman"; break;
        case T_ORDINAL_ROMAN  : defaultKey = "t_ordinal_roman" ; break;
        case T_NOT_ROMAN      : defaultKey = "t_not_roman"     ; break;
        default : break;
      }
      if (curSettings.isAlphaHyphen()) {
        //no change
        //defaultKey = "t_alpha_hyphen";
      }
      if (curSettings.isAlphaPossessive()) {
        defaultKey = "t_alpha_possessive";
      }
      break;
    } // end T_ALPHA
    case T_NUMERIC : {
      switch (curSettings.getNumeric()) {
        case T_INTEGER         : defaultKey = "t_integer"       ; break;
        case T_COMMA_NUMBER    : defaultKey = "t_comma_number"  ; break;
        case T_DOT_NUMBER      : defaultKey = "t_dot_number"    ; break;
        case T_FRACTION        : defaultKey = "t_fraction"      ; break;
        case T_ORDINAL_INTEGER : defaultKey = "t_ordinal_integer"; break;
        default: break;
      }
      break;
    }
    case T_ALPHANUMERIC : defaultKey = "t_alphanumeric" ; break;
    case T_PATTERN      : defaultKey = "t_pattern"      ; break;
    case T_WORD_BRK     : defaultKey = "t_word_brk"     ; break;
    case T_SENTENCE_BRK : defaultKey = "t_sentence_brk" ; break;
    case T_PARAGRAPH_BRK: defaultKey = "t_paragraph_brk" ; break;
    default: defaultKey = "t_fallback";
  }
#ifdef DEBUG_LP
  TOKENIZERLOGINIT;
  LDEBUG << "Text::computeDefaultKey " << defaultKey;
#endif
  curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring(defaultKey));
}

} //namespace TensorFlowTokenizer
} // namespace LinguisticProcessing
} // namespace Lima
