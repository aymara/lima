// Copyright (C) 1997 Martin Jones (mjones@kde.org)
// Copyright (c) 2007 David Jarvie (software@astrojar.org.uk)
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

//-----------------------------------------------------------------------------
// KDE color selection combo box

// layout management added Oct 1997 by Mario Weilguni
// <mweilguni@sime.com>


#ifndef KCOLORCOMBO_H
#define KCOLORCOMBO_H

#include <QtWidgets/QComboBox>
#include <QtCore/QList>

#include <kdeui_export.h>

class KColorComboPrivate;

/**
 * Combobox for colors.
 *
 * The combobox provides some preset colors to be selected, and an entry to
 * select a custom color using a color dialog.
 */
class KDEUI_EXPORT KColorCombo : public QComboBox
{
    Q_OBJECT
    Q_PROPERTY( QColor color READ color WRITE setColor )
    Q_PROPERTY( QList<QColor> colors READ colors WRITE setColors )

public:
    /**
     * Constructs a color combo box.
     */
    explicit KColorCombo(QWidget *parent = 0);
    ~KColorCombo();

    /**
     * Selects the color @p col.
     */
    void setColor( const QColor &col );
    /**
     * Returns the currently selected color.
     **/
    QColor color() const;

    /**
     * Find whether the currently selected color is a custom color selected
     * using a color dialog.
     **/
    bool isCustomColor() const;

    /**
     * Set a custom list of colors to choose from, in place of the standard
     * list.
     * @param cols list of colors. If empty, the selection list reverts to
     *             the standard list.
     **/
    void setColors(const QList<QColor> &colors );

    /**
     * Return the list of colors available for selection.
     * @return list of colors
     **/
    QList<QColor> colors() const;

    /**
     * Clear the color list and don't show it, till the next setColor() call
     **/
     void showEmptyList();

Q_SIGNALS:
    /**
     * Emitted when a new color box has been selected.
     */
    void activated( const QColor &col );
    /**
     * Emitted when a new item has been highlighted.
     */
    void highlighted( const QColor &col );

protected:
        virtual void paintEvent(QPaintEvent *event);

private:
        friend class KColorComboPrivate;
        KColorComboPrivate *const d;
        
        Q_DISABLE_COPY(KColorCombo)

        Q_PRIVATE_SLOT(d, void _k_slotActivated(int))
        Q_PRIVATE_SLOT(d, void _k_slotHighlighted(int))
};

#endif	// KCOLORCOMBO_H
