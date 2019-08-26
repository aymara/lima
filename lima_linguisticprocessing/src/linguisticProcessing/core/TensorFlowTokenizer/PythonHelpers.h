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

#ifndef LIMA_LINGUISTICPROCESSING_TensorFlowTokenizer_PythonHelpers_H
#define LIMA_LINGUISTICPROCESSING_TensorFlowTokenizer_PythonHelpers_H

#include <Python.h>

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

std::vector<PyObject*> pyListOrTupleToVector(PyObject* incoming);

} // namespace TensorFlowTokenizer
} // namespace LinguisticProcessing
} // namespace Lima

#endif
