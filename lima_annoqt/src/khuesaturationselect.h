// Copyright (C) 1997 Martin Jones (mjones@kde.org)
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
    virtual void resizeEvent( QResizeEvent * ) override;

    /**
     * Reimplemented from KXYSelector. This drawing is
     * buffered in a pixmap here. As real drawing
     * routine, drawPalette() is used.
     */
    virtual void drawContents( QPainter *painter ) override;

private:

    class Private;
    friend class Private;
    Private * const d;

    Q_DISABLE_COPY( KHueSaturationSelector )
};

#endif /*KHUESATURATIONSELECT_H_*/
