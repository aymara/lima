// Copyright (C) 1997 Martin Jones (mjones@kde.org)
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef KCOLORBUTTON_H
#define KCOLORBUTTON_H

#include <kdeui_export.h>

#include <QtWidgets/QPushButton>

class KColorButtonPrivate;
/**
* @short A pushbutton to display or allow user selection of a color.
*
* This widget can be used to display or allow user selection of a color.
*
* @see KColorDialog
*
* \image html kcolorbutton.png "KDE Color Button"
*/
class KDEUI_EXPORT KColorButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY( QColor color READ color WRITE setColor USER true )
    Q_PROPERTY( QColor defaultColor READ defaultColor WRITE setDefaultColor )

public:
    /**
     * Creates a color button.
     */
    explicit KColorButton( QWidget *parent = 0 );

    /**
     * Creates a color button with an initial color @p c.
     */
    explicit KColorButton( const QColor &c, QWidget *parent = 0 );

    /**
     * Creates a color button with an initial color @p c and default color @p defaultColor.
     */
    KColorButton( const QColor &c, const QColor &defaultColor, QWidget *parent = 0 );

    virtual ~KColorButton();

    /**
     * Returns the currently chosen color.
     */
    QColor color() const;

    /**
     * Sets the current color to @p c.
     */
     void setColor( const QColor &c );

    /**
     * Returns the default color or an invalid color
     * if no default color is set.
     */
    QColor defaultColor() const;

    /**
     * Sets the default color to @p c.
     */
    void setDefaultColor( const QColor &c );

    QSize sizeHint() const;
    QSize minimumSizeHint() const;

Q_SIGNALS:
    /**
     * Emitted when the color of the widget
     * is changed, either with setColor() or via user selection.
     */
    void changed( const QColor &newColor );

protected:
    virtual void paintEvent( QPaintEvent *pe );
    virtual void dragEnterEvent( QDragEnterEvent *);
    virtual void dropEvent( QDropEvent *);
    virtual void mousePressEvent( QMouseEvent *e );
    virtual void mouseMoveEvent( QMouseEvent *e);
    virtual void keyPressEvent( QKeyEvent *e );

private:
    class KColorButtonPrivate;
    KColorButtonPrivate * const d;

    Q_PRIVATE_SLOT( d, void _k_chooseColor() )
};

#endif

