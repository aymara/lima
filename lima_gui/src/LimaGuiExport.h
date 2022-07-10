// Copyright 2017 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/**
 * \file    LimaGuiExport.h
 * \author  Jocelyn Vernay
 * \date    Wed, Sep 06 2017
 * 
 */

#ifndef LIMA_GUI_EXPORT_H
#define LIMA_GUI_EXPORT_H

#ifdef WIN32

#ifdef LIMA_GUI_EXPORTING
   #define LIMA_GUI_EXPORT    __declspec(dllexport)
#else
   #define LIMA_GUI_EXPORT    __declspec(dllimport)
#endif

#else // Not WIN32

#define LIMA_GUI_EXPORT

#endif

#endif
