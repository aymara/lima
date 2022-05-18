// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifdef _DEBUG
  #undef _DEBUG
  #include <Python.h>
  #define _DEBUG
#else
  #include <Python.h>
#endif#include <iostream>
#include <QtCore/QString>

#if PY_MAJOR_VERSION < 3
#error "Python version must be 3 or more"
#endif

int main(int , char **)
{
std::string text = "1	The	the	DET	DT	_	_	2	NMOD	_	_\n"
"2	Convention	Convention	NP	NNP	_	_	4	SUB	_	_\n"
"3	also	also	ADV	RB	_	_	4	VMOD	_	_\n"
"4	established	establish	V	VBD	_	_	_	_	_	_\n"
"5	eleven	eleven	NOMBRE	CD	Numex.NUMBER	_	_	_	_	_\n"
"6	Working	working	ADJ	JJ	_	_	7	NMOD	_	_\n"
"7	Groups	group	NC	NNS	_	_	4	OBJ	_	_\n"
"8	and	and	CONJ	CC	_	_	_	_	_	_\n"
"9	three	three	NOMBRE	CD	Numex.NUMBER	_	_	_	_	_\n"
"10	Discussion	discussion	NC	NN	_	_	11	NMOD	_	_\n"
"11	Circles	circle	NC	NNS	_	_	_	DEP	_	_";

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
    if (QFile::exists(*it + "/python" ))
    {
      str_program_name = *it + "/python";
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
  if (sys_module == NULL)
  {
    std::cerr << "Failed to import the sys module" << std::endl;
    PyErr_Print();
  }
  PyDict_SetItemString(main_dict, "sys", sys_module);

  // Add the path to the knowledgesrl pachkage to putho path
  PyObject* pythonpath = PySys_GetObject("path");
  if (PyList_Append(pythonpath, PyUnicode_DecodeFSDefault("/home/gael/Projets/knowledgesrl/src")) ==  -1)
  {
    std::cerr << "Failed to append to python path" << std::endl;
    PyErr_Print();
    Py_Exit(1);
  }

  // Import the semanticrolelabeler module
  PyObject* semanticrolelabeler_module = PyImport_ImportModule("semanticrolelabeler");
  if (semanticrolelabeler_module == NULL)
  {
    std::cerr << "Failed to import srl semanticrolelabeler module" << std::endl;
    PyErr_Print();
    Py_Exit(1);
  }

  // Create the semantic role labeller instance
  PyObject* instance = PyObject_CallMethod(semanticrolelabeler_module, "SemanticRoleLabeler", "[s]", "--log=debug");
  if (instance == NULL)
  {
    std::cerr << "Cannot instantiate the SemanticRoleLabeler python class" << std::endl;
    PyErr_Print();
    Py_Exit(1);
  }

  // Run the semantic role labeller
  PyObject* callResult = PyObject_CallMethod(instance, "annotate", "s", text.c_str());
  if (callResult == NULL)
  {
    std::cerr << "Failed to call the annotate method" << std::endl;
    PyErr_Print();
    Py_Exit(1);
  }

  // Display the SRL result
  const char* result = PyUnicode_AsUTF8(callResult);
  if (result == NULL)
  {
    std::cerr << "Cannot convert result item to string" << std::endl;
    PyErr_Print();
    Py_Exit(1);
  }
  std::cout << "Python result is:" << std::endl << result;
  Py_Finalize();

  return 0;

}
