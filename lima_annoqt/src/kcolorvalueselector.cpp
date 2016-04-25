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

#include "kcolorvalueselector.h"
#include <QPainter>
#include <iostream>

class KColorValueSelector::Private
{
public:
  Private(KColorValueSelector *q): q(q), _hue(0), _sat(0), _colorValue(0), _mode(ChooserClassic) {}

    KColorValueSelector *q;
    int _hue;
    int _sat;
    int _colorValue;
    KColorChooserMode _mode;
  QPixmap pixmap;
};

KColorValueSelector::KColorValueSelector( QWidget *parent )
        : KSelector( Qt::Vertical, parent ), d( new Private( this ) )
{
    setRange( 0, 255 );
}

KColorValueSelector::KColorValueSelector( Qt::Orientation o, QWidget *parent )
        : KSelector( o, parent ), d( new Private( this ) )
{
    setRange( 0, 255 );
}

KColorValueSelector::~KColorValueSelector()
{
    delete d;
}

int KColorValueSelector::hue() const
{
    return d->_hue;
}

void KColorValueSelector::setHue( int hue )
{
    d->_hue = hue;
}

int KColorValueSelector::saturation() const
{
    return d->_sat;
}

void KColorValueSelector::setSaturation( int saturation )
{
    d->_sat = saturation;
}

int KColorValueSelector::colorValue () const
{
    return d->_colorValue;
}

void KColorValueSelector::setColorValue ( int colorValue )
{
    d->_colorValue = colorValue;
}



void KColorValueSelector::updateContents()
{
    drawPalette( &d->pixmap );
}

void KColorValueSelector::resizeEvent( QResizeEvent * )
{
    updateContents();
}

void KColorValueSelector::drawContents( QPainter *painter )
{
    painter->drawPixmap( contentsRect().x(), contentsRect().y(), d->pixmap );
}

void KColorValueSelector::setChooserMode( KColorChooserMode c )
{
    if ( c == ChooserHue ) {
        setRange( 0, 360 );
    } else {
        setRange( 0, 255 );
    }
    d->_mode = c;

    //really needed?
    //Q_EMIT modeChanged();
}

KColorChooserMode KColorValueSelector::chooserMode () const
{
    return d->_mode;
}

void KColorValueSelector::drawPalette( QPixmap *pixmap )
{
    int xSize = contentsRect().width(), ySize = contentsRect().height();
    QImage image( QSize( xSize, ySize ), QImage::Format_RGB32 );
    QColor col;
    uint *p;
    QRgb rgb;
    int _r, _g, _b;

    col.setHsv( hue(), saturation(), colorValue() );
    col.getRgb( &_r, &_g, &_b );

	if ( orientation() == Qt::Horizontal )
	{
		for ( int v = 0; v < ySize; v++ )
		{
            p = ( uint * ) image.scanLine( ySize - v - 1 );

			for( int x = 0; x < xSize; x++ )
			{

                switch ( chooserMode() ) {
                case ChooserClassic:
                default:
                    col.setHsv( hue(), saturation(), 255 * x / ( ( xSize == 1 ) ? 1 : xSize - 1 ) );
                    break;
                case ChooserRed:
                    col.setRgb( 255 * x / ( ( xSize == 1 ) ? 1 : xSize - 1 ), _g, _b );
                    break;
                case ChooserGreen:
                    col.setRgb( _r, 255 * x / ( ( xSize == 1 ) ? 1 : xSize - 1 ), _b );
                    break;
                case ChooserBlue:
                    col.setRgb( _r, _g, 255 * x / ( ( xSize == 1 ) ? 1 : xSize - 1 ) );
                    break;
                case ChooserHue:
                    col.setHsv( 360 * x / ( ( xSize == 1 ) ? 1 : xSize - 1 ), 255, 255 );
                    break;
                case ChooserSaturation:
                    col.setHsv( hue(), 255 * x / ( ( xSize == 1 ) ? 1 : xSize - 1 ), colorValue() );
                    break;
                case ChooserValue:
                    col.setHsv( hue(), saturation(), 255 * x / ( ( xSize == 1 ) ? 1 : xSize - 1 ) );
                    break;
                }

                rgb = col.rgb();
                *p++ = rgb;
            }
        }
    }

	if( orientation() == Qt::Vertical )
	{
		for ( int v = 0; v < ySize; v++ )
		{
            p = ( uint * ) image.scanLine( ySize - v - 1 );

            switch ( chooserMode() ) {
            case ChooserClassic:
            default:
                col.setHsv( hue(), saturation(), 255 * v / ( ( ySize == 1 ) ? 1 : ySize - 1 ) );
                break;
            case ChooserRed:
                col.setRgb( 255 * v / ( ( ySize == 1 ) ? 1 : ySize - 1 ), _g, _b );
                break;
            case ChooserGreen:
                col.setRgb( _r, 255 * v / ( ( ySize == 1 ) ? 1 : ySize - 1 ), _b );
                break;
            case ChooserBlue:
                col.setRgb( _r, _g, 255 * v / ( ( ySize == 1 ) ? 1 : ySize - 1 ) );
                break;
            case ChooserHue:
                col.setHsv( 360 * v / ( ( ySize == 1 ) ? 1 : ySize - 1 ), 255, 255 );
                break;
            case ChooserSaturation:
                col.setHsv( hue(), 255 * v / ( ( ySize == 1 ) ? 1 : ySize - 1 ), colorValue() );
                break;
            case ChooserValue:
                col.setHsv( hue(), saturation(), 255 * v / ( ( ySize == 1 ) ? 1 : ySize - 1 ) );
                break;
            }

            rgb = col.rgb();
            for ( int i = 0; i < xSize; i++ )
                *p++ = rgb;
        }
    }

    /*
    if ( pixmap->depth() <= 8 )
    {
        extern QVector<QColor> kdeui_standardPalette();
        const QVector<QColor> standardPalette = kdeui_standardPalette();
        KImageEffect::dither( image, standardPalette.data(), standardPalette.size() );
    }
    */
    *pixmap = QPixmap::fromImage( image );
}


// #include "kcolorvalueselector.moc"
