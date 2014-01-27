/* This file is part of the KDE libraries
 * 
 * Copyright (C) 1997 Martin Jones (mjones@kde.org)
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

#ifndef KCOLORVALUESELECTOR_H
#define KCOLORVALUESELECTOR_H

#include "kselector.h"
#include "kcolorchoosermode.h"
#include <QtGui/QPixmap>

class KColorValueSelector : public KSelector
{
    Q_OBJECT
public:
  /**
   * Constructs a widget for color selection.
   */
  explicit KColorValueSelector( QWidget *parent=0 );
  /**
   * Constructs a widget for color selection with a given orientation
   */
  explicit KColorValueSelector( Qt::Orientation o, QWidget *parent = 0 );

  ~KColorValueSelector();
  
  /**
   * Updates the widget's contents.
   */
  void updateContents();

  /**
   * Returns the current hue value.
   * 
   * @return               The hue value (0-255)
   */
  int hue() const;
  
  /**
   * Sets the hue value. Doesn't automatically update the widget;
   * you have to call updateContents manually.
   * 
   * @param		hue			Sets the hue value (0-255)
   */
  void setHue( int hue );

  /**
   * Returns the current saturation value.
   * 
   * @return				The saturation value (0-255)
   */  
  int saturation() const;
  
  /**
   * Sets the saturation value. Doesn't automatically update the widget;
   * you have to call updateContents manually.
   * 
   * @param		saturation		Sets the saturation value (0-255)
   */  
  void setSaturation( int saturation );
  
  /**
   * Returns the current color value.
   * 
   * @return				The color value (0-255)
   */    
  int colorValue() const;
  
  /**
   * Sets the color value. Doesn't automatically update the widget;
   * you have to call updateContents manually.
   * 
   * @param		colorValue		Sets the color value (0-255)
   */    
  void setColorValue( int colorValue );

  /**
   * Sets the chooser mode. Doesn't automatically update the widget;
   * you have to call updateContents manually.
   * 
   * @param		chooserMode		Sets the chooser mode (one of the KColorChooserMode constants)
   */    
  void setChooserMode (KColorChooserMode chooserMode);

  /**
   * Returns the current chooser mode.
   * 
   * @return				The chooser mode (one of the KColorChooserMode constants)
   */   
  KColorChooserMode chooserMode () const;
	
protected:
  /**
   * Draws the contents of the widget on a pixmap,
   * which is used for buffering.
   */
  virtual void drawPalette( QPixmap *pixmap );
  virtual void resizeEvent( QResizeEvent * );

  /**
   * Reimplemented from KSelector. The drawing is
   * buffered in a pixmap here. As real drawing
   * routine, drawPalette() is used.
   */
  virtual void drawContents( QPainter *painter );

private:
  class Private;
  friend class Private;
  
  Private *const d;
  
  Q_DISABLE_COPY(KColorValueSelector)
};

#endif /* KCOLORVALUESELECTOR_H */

