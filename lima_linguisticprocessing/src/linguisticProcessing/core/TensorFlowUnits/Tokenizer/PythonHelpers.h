// Copyright 2002-2019 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_LINGUISTICPROCESSING_TensorFlowTokenizer_PythonHelpers_H
#define LIMA_LINGUISTICPROCESSING_TensorFlowTokenizer_PythonHelpers_H

#include <Python.h>

namespace Lima
{
namespace LinguisticProcessing
{
namespace TensorFlowUnits
{
namespace Tokenizer
{

void python_error();
void failed_to_allocate_memory();
void failed_to_import_the_sys_module();
void cannot_instantiate_the_tokenizer_python_class();

std::vector<PyObject*> pyListOrTupleToVector(PyObject* incoming);

} // namespace Tokenizer
} // namespace TensorFlowUnits
} // namespace LinguisticProcessing
} // namespace Lima

#endif
