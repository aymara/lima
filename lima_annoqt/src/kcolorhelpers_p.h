// Copyright (C) 2007 Matthew Woehlke <mw_triad@users.sourceforge.net>
// Copyright (C) 2007 Olaf Schmidt <ojschmidt@kde.org>
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef KDEUI_COLORS_KCOLORHELPERS_P_H
#define KDEUI_COLORS_KCOLORHELPERS_P_H

// normalize: like qBound(a, 0.0, 1.0) but without needing the args and with
// "safer" behavior on NaN (isnan(a) -> return 0.0)
static inline qreal normalize(qreal a)
{
    return (a < 1.0 ? (a > 0.0 ? a : 0.0) : 1.0);
}

#endif // KDEUI_COLORS_KCOLORHELPERS_P_H
