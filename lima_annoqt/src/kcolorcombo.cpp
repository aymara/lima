/* This file is part of the KDE libraries
    Copyright (C) 1997 Martin Jones (mjones@kde.org)
    Copyright (C) 2007 Pino Toscano (pino@kde.org)
    Copyright (c) 2007 David Jarvie (software@astrojar.org.uk)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/
//-----------------------------------------------------------------------------
// KDE color selection dialog.
//
// 1999-09-27 Espen Sand <espensa@online.no>
// KColorDialog is now subclassed from KDialog. I have also extended
// KColorDialog::getColor() so that in contains a parent argument. This
// improves centering capability.
//
// layout management added Oct 1997 by Mario Weilguni
// <mweilguni@sime.com>
//

#include "kcolorcombo.h"

#include <QtCore/QVector>
#include <QtGui/QAbstractItemDelegate>
#include <QtGui/QStylePainter>

#include <kglobal.h>
#include <klocale.h>

#include "kcolordialog.h"

// This is repeated from the KColorDlg, but I didn't
// want to make it public BL.
// We define it out when compiling with --enable-final in which case
// we use the version defined in KColorDlg

class KColorComboDelegate : public QAbstractItemDelegate
{
    public:
        static const int ColorRole = Qt::UserRole + 1;

        KColorComboDelegate(QObject *parent = 0);
        virtual ~KColorComboDelegate();

        virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
        virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

static const int colorframe_delta = 3;

static QBrush k_colorcombodelegate_brush(const QModelIndex &index, int role)
{
    QBrush brush;
    QVariant v = index.data(role);
    if (v.type() == QVariant::Brush) {
        brush = v.value<QBrush>();
    } else if (v.type() == QVariant::Color) {
        brush = QBrush(v.value<QColor>());
    }
    return brush;
}

KColorComboDelegate::KColorComboDelegate(QObject *parent)
    : QAbstractItemDelegate(parent)
{
}

KColorComboDelegate::~KColorComboDelegate()
{
}

void KColorComboDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // background
    QBrush backbrush = k_colorcombodelegate_brush(index, Qt::BackgroundRole);
    QColor innercolor(Qt::white);
    bool isSelected = (option.state & QStyle::State_Selected);
    bool paletteBrush = false;
    if (backbrush.style() == Qt::NoBrush) {
        paletteBrush = true;
        if (isSelected) {
            backbrush = option.palette.brush(QPalette::Highlight);
        } else {
            backbrush = option.palette.brush(QPalette::Base);
        }
    }
    if (isSelected) {
        innercolor = option.palette.color(QPalette::Highlight);
    } else {
        innercolor = option.palette.color(QPalette::Base);
    }
    painter->fillRect(option.rect, backbrush);
    QRect innerrect = option.rect.adjusted(colorframe_delta, colorframe_delta, -colorframe_delta, -colorframe_delta);
    // inner color
    QVariant cv = index.data(ColorRole);
    if (cv.type() == QVariant::Color) {
        QColor tmpcolor = cv.value<QColor>();
        if (tmpcolor.isValid()) {
            innercolor = tmpcolor;
            paletteBrush = false;
            painter->setPen(Qt::black);
            painter->setBrush(innercolor);
            painter->drawRect(innerrect);
            painter->setBrush(Qt::NoBrush);
        }
    }
    // text
    QVariant tv = index.data(Qt::DisplayRole);
    if (tv.type() == QVariant::String) {
        QString text = tv.toString();
        QColor textColor;
        if (paletteBrush) {
            if (isSelected) {
                textColor = option.palette.color(QPalette::HighlightedText);
            } else {
                textColor = option.palette.color(QPalette::Text);
            }
        } else {
            int unused, v;
            innercolor.getHsv(&unused, &unused, &v);
            if (v > 128) {
                textColor = Qt::black;
            } else {
                textColor = Qt::white;
            }
        }
        painter->setPen(textColor);
        painter->drawText(innerrect.adjusted(1, 1, -1, -1), text);
    }
}

QSize KColorComboDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index)

    // the width does not matter, as the view will always use the maximum width available
    return QSize(100, option.fontMetrics.height() + 2 * colorframe_delta);
}


#define STANDARD_PAL_SIZE 17

K_GLOBAL_STATIC_WITH_ARGS(QVector<QColor>, standardPalette, (STANDARD_PAL_SIZE))
static void createStandardPalette()
{
    if ( standardPalette->isEmpty() )
        return;

    int i = 0;

    (*standardPalette)[i++] = Qt::red;
    (*standardPalette)[i++] = Qt::green;
    (*standardPalette)[i++] = Qt::blue;
    (*standardPalette)[i++] = Qt::cyan;
    (*standardPalette)[i++] = Qt::magenta;
    (*standardPalette)[i++] = Qt::yellow;
    (*standardPalette)[i++] = Qt::darkRed;
    (*standardPalette)[i++] = Qt::darkGreen;
    (*standardPalette)[i++] = Qt::darkBlue;
    (*standardPalette)[i++] = Qt::darkCyan;
    (*standardPalette)[i++] = Qt::darkMagenta;
    (*standardPalette)[i++] = Qt::darkYellow;
    (*standardPalette)[i++] = Qt::white;
    (*standardPalette)[i++] = Qt::lightGray;
    (*standardPalette)[i++] = Qt::gray;
    (*standardPalette)[i++] = Qt::darkGray;
    (*standardPalette)[i++] = Qt::black;
}

class KColorComboPrivate
{
    public:
        KColorComboPrivate(KColorCombo *qq);

        void addColors();
        void setCustomColor(const QColor &color, bool lookupInPresets = true);

        // slots
        void _k_slotActivated(int index);
        void _k_slotHighlighted(int index);

        KColorCombo *q;
	QList<QColor> colorList;
	QColor customColor;
	QColor internalcolor;
};

KColorComboPrivate::KColorComboPrivate(KColorCombo *qq)
    : q(qq), customColor(Qt::white)
{
}

void KColorComboPrivate::setCustomColor(const QColor &color, bool lookupInPresets)
{
    if (lookupInPresets) {
        if (colorList.isEmpty()) {
            for (int i = 0; i < STANDARD_PAL_SIZE; ++i) {
                if (standardPalette->at(i) == color) {
                    q->setCurrentIndex(i + 1);
                    internalcolor = color;
                    return;
                }
            }
        } else {
            int i = colorList.indexOf(color);
            if (i >= 0) {
                q->setCurrentIndex(i + 1);
                internalcolor = color;
                return;
            }
        }
    }

    internalcolor = color;
    customColor = color;
    q->setItemData(0, customColor, KColorComboDelegate::ColorRole);
}


KColorCombo::KColorCombo( QWidget *parent )
    : QComboBox(parent), d(new KColorComboPrivate(this))
{
	createStandardPalette();

    setItemDelegate(new KColorComboDelegate(this));
    d->addColors();

    connect(this, SIGNAL(activated(int)), SLOT(_k_slotActivated(int)));
    connect(this, SIGNAL(highlighted(int)), SLOT(_k_slotHighlighted(int)));

    // select the white color
    setCurrentIndex(13);
    d->_k_slotActivated(13);
}


KColorCombo::~KColorCombo()
{
	delete d;
}

void KColorCombo::setColors( const QList<QColor> &colors )
{
    clear();
    d->colorList = colors;
    d->addColors();
}

QList<QColor> KColorCombo::colors() const
{
    if (d->colorList.isEmpty()) {
        QList<QColor> list;
        for (int i = 0; i < STANDARD_PAL_SIZE; ++i) {
            list += standardPalette->at(i);
	}
        return list;
    } else {
        return d->colorList;
    }
}

/**
   Sets the current color
 */
void KColorCombo::setColor( const QColor &col )
{
    if (!col.isValid()) {
        return;
    }

    if (count() == 0) {
        d->addColors();
    }

    d->setCustomColor(col, true);
}


/**
   Returns the currently selected color
 */
QColor KColorCombo::color() const {
  return d->internalcolor;
}

bool KColorCombo::isCustomColor() const
{
    return d->internalcolor == d->customColor;
}

void KColorCombo::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QStylePainter painter(this);
    painter.setPen(palette().color(QPalette::Text));

    QStyleOptionComboBox opt;
    initStyleOption(&opt);
    painter.drawComplexControl(QStyle::CC_ComboBox, opt);

    QRect frame = style()->subControlRect(QStyle::CC_ComboBox, &opt, QStyle::SC_ComboBoxEditField, this);
    painter.fillRect(frame.adjusted(1, 1, -1, -1), QBrush(d->internalcolor));
}

/**
   Show an empty list, till the next color is set with setColor
 */
void KColorCombo::showEmptyList()
{
    clear();
}

void KColorComboPrivate::_k_slotActivated(int index)
{
    if (index == 0) {
        if (KColorDialog::getColor(customColor, q) == QDialog::Accepted) {
            setCustomColor(customColor, false);
        }
    } else if (colorList.isEmpty()) {
        internalcolor = standardPalette->at(index - 1);
    } else {
        internalcolor = colorList[index - 1];
    }

    emit q->activated(internalcolor);
}

void KColorComboPrivate::_k_slotHighlighted(int index)
{
    if (index == 0) {
        internalcolor = customColor;
    } else if (colorList.isEmpty()) {
        internalcolor = standardPalette->at(index - 1);
    } else {
        internalcolor = colorList[index - 1];
    }

    emit q->highlighted(internalcolor);
}

void KColorComboPrivate::addColors()
{
    q->addItem(i18nc("Custom color", "Custom..."));

    if (colorList.isEmpty()) {
        for (int i = 0; i < STANDARD_PAL_SIZE; ++i) {
            q->addItem(QString());
            q->setItemData(i + 1, standardPalette->at(i), KColorComboDelegate::ColorRole);
        }
    } else {
        for (int i = 0, count = colorList.count(); i < count; ++i) {
            q->addItem(QString());
            q->setItemData(i + 1, colorList[i], KColorComboDelegate::ColorRole);
        }
    }
}

#include "kcolorcombo.moc"
