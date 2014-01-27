/* This file is part of the KDE project
 * Copyright (C) 2007 Matthew Woehlke <mw_triad@users.sourceforge.net>
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

/*
 * If you use KColorSpaces in your own KDE code, please drop me a line at
 * mw_triad@users.sourceforge.net, as I would like to track if people find it
 * useful. Thanks!
 */

#ifndef KCOLORSPACES_H
#define KCOLORSPACES_H

#include <QtGui/QColor>

namespace KColorSpaces
{

class KHSL
{
public:
    explicit KHSL(const QColor&);
    explicit KHSL(qreal h_, qreal s_, qreal l_, qreal a_ = 1.0);
    QColor qColor() const;
    qreal h, s, l, a;
private:
};

class KHCY
{
public:
    explicit KHCY(const QColor&);
    explicit KHCY(qreal h_, qreal c_, qreal y_, qreal a_ = 1.0);
    QColor qColor() const;
    qreal h, c, y, a;
    static qreal luma(const QColor&);
private:
    static qreal gamma(qreal);
    static qreal igamma(qreal);
    static qreal lumag(qreal, qreal, qreal);
};

}

#endif // KCOLORUTILS_H
// kate: space-indent on; indent-width 4; replace-tabs on; auto-insert-doxygen on;
