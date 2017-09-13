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
 * \file    LimaGuiCommon.h
 * \author  Gael de Chalendar <gael.de-chalendar@cea.fr>
 * \date    Fri, Sep 08 2017
 * 
 */
#ifndef LIMA_GUI_COMMON_H
#define LIMA_GUI_COMMON_H

#include <common/LimaCommon.h>

#define LIMAGUILOGINIT LOGINIT("Lima::Gui");
#define ANALYSISTHREADLOGINIT LOGINIT("Lima::Gui::AnalysisThread");
#define CONLLLOGINIT LOGINIT("Lima::Gui::Conll");
#define CONFLOGINIT LOGINIT("Lima::Gui::Config")
#define INITTHREADLOGINIT LOGINIT("Lima::Gui::InitializeThread");

#endif
