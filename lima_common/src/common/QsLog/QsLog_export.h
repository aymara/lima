// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef QSLOG_EXPORT_H
#define QSLOG_EXPORT_H


#ifdef WIN32
// #pragma message("WIN32 defined")
#ifdef LIMA_COMMONQSLOG_EXPORTING
//#pragma message("LIMA_COMMONQSLOG_EXPORTING defined")
#define LIMA_COMMONQSLOG_EXPORT    __declspec(dllexport)
#else
//#pragma message("LIMA_COMMONQSLOG_EXPORTING NOT defined")
#define LIMA_COMMONQSLOG_EXPORT    __declspec(dllimport)
#endif

#else // Not WIN32
// #pragma message("WIN32 NOT defined")

#define LIMA_COMMONQSLOG_EXPORT

#endif


#endif