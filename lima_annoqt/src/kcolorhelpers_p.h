/* This file is part of the KDE project
 * Copyright (C) 2007 Matthew Woehlke <mw_triad@users.sourceforge.net>
 * Copyright (C) 2007 Olaf Schmidt <ojschmidt@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KDEUI_COLORS_KCOLORHELPERS_P_H
#define KDEUI_COLORS_KCOLORHELPERS_P_H

// normalize: like qBound(a, 0.0, 1.0) but without needing the args and with
// "safer" behavior on NaN (isnan(a) -> return 0.0)
static inline qreal normalize(qreal a)
{
    return (a < 1.0 ? (a > 0.0 ? a : 0.0) : 1.0);
}

#endif // KDEUI_COLORS_KCOLORHELPERS_P_H
