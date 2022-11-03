// Copyright (C) 2007 Matthew Woehlke <mw_triad@users.sourceforge.net>
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
