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

#include <vector>

#include "linguisticProcessing/LinguisticProcessingCommon.h"

#include "PythonHelpers.h"

namespace Lima
{
namespace LinguisticProcessing
{
namespace TensorFlowTokenizer
{

void python_error();
void failed_to_allocate_memory();
void failed_to_import_the_sys_module();
void cannot_instantiate_the_tokenizer_python_class();

void python_error()
{
  TOKENIZERLOGINIT;
  LERROR << __FILE__ << __LINE__ << ": Python error";
  PyErr_Print();
  Py_Exit(1);
}

void failed_to_allocate_memory()
{
  TOKENIZERLOGINIT;
  LERROR << __FILE__ << __LINE__ << ": Failed to allocate memory";
  PyErr_Print();
  Py_Exit(1);
}

void failed_to_import_the_sys_module()
{
  TOKENIZERLOGINIT;
  LERROR << "Failed to import the sys module";
  PyErr_Print();
  Py_Exit(1);
}

void cannot_instantiate_the_tokenizer_python_class()
{
  TOKENIZERLOGINIT;
  LERROR << "Cannot instantiate the PythonTensorFlowTokenizer python class";
  PyErr_Print();
  Py_Exit(1);
}

// PyObject -> Vector
std::vector<PyObject*> pyListOrTupleToVector(PyObject* incoming)
{
  Q_ASSERT(incoming != nullptr);
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

} // namespace TensorFlowTokenizer
} // namespace LinguisticProcessing
} // namespace Lima

