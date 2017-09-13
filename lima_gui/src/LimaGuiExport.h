/*
    Copyright 2017 CEA LIST

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
