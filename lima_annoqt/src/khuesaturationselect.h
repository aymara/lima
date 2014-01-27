/* This file is part of the KDE libraries
   Copyright (C) 1997 Martin Jones (mjones@kde.org)

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

#ifndef KHUESATURATIONSELECT_H_
#define KHUESATURATIONSELECT_H_

#include "kxyselector.h"

#include "kcolorchoosermode.h"

class KHueSaturationSelector : public KXYSelector
{
    Q_OBJECT
public:
    /**
     * Constructs a hue/saturation selection widget.
     */
    explicit KHueSaturationSelector( QWidget *parent = 0 );

    /**
     * Destructor.
     */
    ~KHueSaturationSelector();

    /**
     * Sets the chooser mode. The allowed modes are defined
     * in KColorChooserMode.
     * 
     * @param    The chooser mode as defined in KColorChooserMode
     */
    void setChooserMode ( KColorChooserMode chooserMode );

    /**
     * Returns the chooser mode.
     * 
     * @return   The chooser mode (defined in KColorChooserMode)
     */
    KColorChooserMode chooserMode () const;


    /**
     * Returns the hue value
     * 
     * @return   The hue value (0-360)
     */
    int hue() const;

    /**
     * Sets the hue value (0-360)
     * 
     * @param  hue  The hue value (0-360)
     */
    void setHue( int hue );

    /**
     * Returns the saturation (0-255)
     * 
     * @return The saturation (0-255)
     */
    int saturation() const;

    /**
     * Sets the saturation (0-255)
     * 
     * @param  saturation   The saturation (0-255)
     */
    void setSaturation( int saturation );

    /**
     * Returns the color value (also known as lumniousity, 0-255)
     * 
     * @return  The color value (0-255)
     */
    int colorValue() const;

    /**
     * Sets the color value (0-255)
     * 
     * @param  colorValue  The color value (0-255)
     */
    void setColorValue( int colorValue );


    /**
     * Updates the contents
     */
    void updateContents();

protected:
    /**
     * Draws the contents of the widget on a pixmap,
     * which is used for buffering.
     */
    virtual void drawPalette( QPixmap *pixmap );
    virtual void resizeEvent( QResizeEvent * );

    /**
     * Reimplemented from KXYSelector. This drawing is
     * buffered in a pixmap here. As real drawing
     * routine, drawPalette() is used.
     */
    virtual void drawContents( QPainter *painter );

private:

    class Private;
    friend class Private;
    Private * const d;

    Q_DISABLE_COPY( KHueSaturationSelector )
};

#endif /*KHUESATURATIONSELECT_H_*/
