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

#include "PythonTensorFlowTokenizer.h"

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

#include <Python.h>

#include <QtCore/QTemporaryFile>

#include <string>

using namespace std;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::Common::Misc;

namespace Lima
{
namespace LinguisticProcessing
{
namespace TensorFlowTokenizer
{

static SimpleFactory<MediaProcessUnit,PythonTensorFlowTokenizer> pythontokenizerFactory(PYTHONTENSORFLOWTOKENIZER_CLASSID); // clazy:exclude=non-pod-global-static

class PythonTokenizerPrivate
{
public:
  PythonTokenizerPrivate();
  virtual ~PythonTokenizerPrivate();

  void computeDefaultStatus(LinguisticAnalysisStructure::TStatus& curSettings);

  MediaId m_language;
  PyObject* m_instance;
  FsaStringsPool* m_stringsPool;
  LinguisticGraphVertex m_currentVx;

};

PythonTokenizerPrivate::PythonTokenizerPrivate() :
    m_stringsPool(nullptr),
    m_currentVx(0)
{
}

PythonTokenizerPrivate::~PythonTokenizerPrivate()
{
}

PythonTensorFlowTokenizer::PythonTensorFlowTokenizer() : m_d(new PythonTokenizerPrivate())
{}

PythonTensorFlowTokenizer::~PythonTensorFlowTokenizer()
{
  Py_DECREF(m_d->m_instance);

  // Finish the Python Interpreter
  Py_Finalize();
  delete m_d;
}

auto python_error = []()
{
  TOKENIZERLOGINIT;
  LERROR << __FILE__ << __LINE__ << ": Python error";
  PyErr_Print();
  Py_Exit(1);
};

auto failed_to_allocate_memory = []()
{
  TOKENIZERLOGINIT;
  LERROR << __FILE__ << __LINE__ << ": Failed to allocate memory";
  PyErr_Print();
  Py_Exit(1);
};

auto failed_to_import_the_sys_module = []()
{
  TOKENIZERLOGINIT;
  LERROR << "Failed to import the sys module";
  PyErr_Print();
  Py_Exit(1);
};

auto cannot_instantiate_the_tokenizer_python_class = []()
{
  TOKENIZERLOGINIT;
  LERROR << "Cannot instantiate the PythonTensorFlowTokenizer python class";
  PyErr_Print();
  Py_Exit(1);
};

void PythonTensorFlowTokenizer::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
#ifdef DEBUG_LP
  TOKENIZERLOGINIT;
  LDEBUG << "PythonTensorFlowTokenizer::init";
#endif
  m_d->m_language = manager->getInitializationParameters().media;
  m_d->m_stringsPool = &Common::MediaticData::MediaticData::changeable().stringsPool(m_d->m_language);

  QString path; // The path to the LIMA python tensorflow-based tokenizer
  try
  {
    path = QString::fromUtf8(unitConfiguration.getParamsValueAtKey("path").c_str());
  }
  catch (NoSuchParam& )
  {
    TOKENIZERLOGINIT;
    LERROR << "no param 'path' in TensorFlowTokenizer group configuration";
    throw InvalidConfiguration();
  }


  // Initialize the python SRL system
  /*
   * Find the first python executable in the path and use it as the program name.
   *
   * This allows to find the modules set up in an activated virtualenv
   */
  QString str_program_name;
  QString pathEnv = QString::fromUtf8(qgetenv("PATH").constData());
  QStringList paths = pathEnv.split(QRegExp("[;:]"));
  for (auto it = paths.begin(); it != paths.end(); ++it)
  {
    const auto & path = *it;
    if (QFile::exists(path + "/python" ))
    {
      str_program_name = path + "/python";
      break;
    }
  }
#ifndef WIN32
  Py_SetProgramName(const_cast<wchar_t*>( str_program_name.toStdWString().c_str()));
#else
  Py_SetProgramName( (wchar_t*)str_program_name.unicode() );
#endif


  Py_Initialize();

  PyObject* main_module = PyImport_ImportModule("__main__");
  PyObject* main_dict = PyModule_GetDict(main_module);
  PyObject* sys_module = PyImport_ImportModule("sys");
  if (sys_module == nullptr)
  {
    failed_to_import_the_sys_module();
    Py_Exit(1);
  }

  PyDict_SetItemString(main_dict, "sys", sys_module);

  // Add the path to the knowledgesrl pachkage to putho path
  PyObject* pythonpath = PySys_GetObject("path");
  if (PyList_Append(pythonpath, PyUnicode_DecodeFSDefault(path.toUtf8().constData())) ==  -1)
  {
    TOKENIZERLOGINIT;
    LERROR << "Failed to append to python path";
    PyErr_Print();
    Py_Exit(1);
  }

  // Import the tokenizer module
  PyObject* tokenizer_module = PyImport_ImportModule("tfudpipe-predict");
  if (tokenizer_module == NULL)
  {
    TOKENIZERLOGINIT;
    LERROR << "PythonTensorFlowTokenizer::init"<< __FILE__ << __LINE__
            << ": Failed to import tokenizer module";
    PyErr_Print();
    Py_Exit(1);
  }

  // pDict is a borrowed reference
  auto pDict = PyModule_GetDict(tokenizer_module);

  // Build the name of a callable class
  auto pClass = PyDict_GetItemString(pDict, "PythonTensorFlowTokenizer");
  // Create an instance of the class
  if (PyCallable_Check(pClass))
  {
    //     corpus, embeddings_path, model_path, window_size
    auto corpus = PyUnicode_FromString("en");
    if (corpus == NULL)
    {
      failed_to_allocate_memory();
    }
    auto embeddings_path = PyUnicode_FromString("/home/gael/Projets/Lima/TFUDPipe/log");
    if (embeddings_path == NULL)
    {
      failed_to_allocate_memory();
    }
    auto model_path = PyUnicode_FromString("/home/gael/Projets/Lima/TFUDPipe/udmodel");
    if (model_path == NULL)
    {
      failed_to_allocate_memory();
    }
    auto window_size = PyLong_FromLong(20);
    if (window_size == NULL)
    {
      failed_to_allocate_memory();
    }

    auto pArgs = PyTuple_New(4);
    if (pArgs == NULL)
    {
      failed_to_allocate_memory();
    }
    if (PyTuple_SetItem(pArgs, 0, corpus) != 0
        || PyTuple_SetItem(pArgs, 1, embeddings_path) != 0
        || PyTuple_SetItem(pArgs, 2, model_path) != 0
        || PyTuple_SetItem(pArgs, 3, window_size) != 0)
    {
      python_error();
    }

    m_d->m_instance = PyObject_CallObject(pClass, pArgs);
    if (m_d->m_instance == NULL)
    {
      cannot_instantiate_the_tokenizer_python_class();
    }

    Py_DECREF(corpus);
    Py_DECREF(embeddings_path);
    Py_DECREF(model_path);
    Py_DECREF(window_size);
  }
}

// PyObject -> Vector
std::vector<PyObject*> pyListOrTupleToVector(PyObject* incoming)
{
  std::vector<PyObject*> data;
  if (PyTuple_Check(incoming))
  {
    for(Py_ssize_t i = 0; i < PyTuple_Size(incoming); i++)
    {
      PyObject *value = PyTuple_GetItem(incoming, i);
      data.push_back( value );
    }
  }
  else
  {
    if (PyList_Check(incoming))
    {
      for(Py_ssize_t i = 0; i < PyList_Size(incoming); i++)
      {
        PyObject *value = PyList_GetItem(incoming, i);
        data.push_back( value );
      }
    }
    else
    {
      throw std::logic_error("Passed PyObject pointer was not a list or tuple!");
    }
  }
  return data;
}

LimaStatusCode PythonTensorFlowTokenizer::process(AnalysisContent& analysis) const
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
  // Convert text to PyObject
  auto pyText = PyUnicode_FromString(originalText->toUtf8().constData());
  if (pyText == NULL)
  {
    failed_to_allocate_memory();
  }
  auto pyTokenize = PyUnicode_FromString("tokenize");
  if (pyTokenize == NULL)
  {
    failed_to_allocate_memory();
  }
  // Call python tokenizer object tokenize method with the text
  auto pySentences = PyObject_CallMethodObjArgs(m_d->m_instance,
                                                 pyTokenize,
                                                 pyText,
                                                 NULL);

  // Convert resulting python list of list of tuples (pairs) to Qt or std
  // objects
  auto sentencesPyTokens = pyListOrTupleToVector(pySentences);
  std::vector< std::vector< std::pair<QString, int> > > sentencesTokens;
  for (auto& sentencePyTokens: sentencesPyTokens)
  {
    std::vector< std::pair<QString, int> > sentence;
    auto sentenceTokensPyPair = pyListOrTupleToVector(sentencePyTokens);
    for (auto& pyPair: sentenceTokensPyPair)
    {
      // pyPair is a PyObject which is a PyTuple with two elements: a PyUnicode and a PyLong
      auto pyToken = pyListOrTupleToVector(pyPair);
      // pyToken is a std::vector with two elements: a PyUnicode and a PyLong
      auto pyStr = PyUnicode_AsEncodedString(pyToken[0], "utf-8", "strict");
      if (pyStr == NULL)
      {
        python_error();
      }
      auto strExcType =  PyBytes_AsString(pyStr);
      if (strExcType == NULL)
      {
        python_error();
      }
      auto tokenString = QString::fromUtf8(strExcType);
      if (tokenString.isEmpty())
      {
        continue;
      }
      Py_XDECREF(pyStr);

      auto offset = PyLong_AsLong(pyToken[1]);
      if (offset == -1 and PyErr_Occurred())
      {
        python_error();
      }
      auto token = std::make_pair(tokenString, offset);

      sentence.push_back(token);
    }

    sentencesTokens.push_back(sentence);
  }
  Py_DECREF(pyText);
  Py_DECREF(pyTokenize);

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
void PythonTokenizerPrivate::computeDefaultStatus(LinguisticAnalysisStructure::TStatus& curSettings)
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
