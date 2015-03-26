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

#include "khuesaturationselect.h"

#include <QtGui/QPainter>

typedef void (*KdeCleanUpFunction)();

class KCleanUpGlobalStatic
{
public:
KdeCleanUpFunction func;

inline ~KCleanUpGlobalStatic() { func(); }
};
/*
  # define K_GLOBAL_STATIC_STRUCT_NAME(NAME)

#define K_GLOBAL_STATIC(TYPE, NAME) K_GLOBAL_STATIC_WITH_ARGS(TYPE, NAME, ())

#define K_GLOBAL_STATIC_WITH_ARGS(TYPE, NAME, ARGS)                            \
static QBasicAtomicPointer<TYPE > _k_static_##NAME = Q_BASIC_ATOMIC_INITIALIZER(0); \
static bool _k_static_##NAME##_destroyed;                                      \
static struct K_GLOBAL_STATIC_STRUCT_NAME(NAME)                                \
{                                                                              \
bool isDestroyed()                                                         \
{                                                                          \
return _k_static_##NAME##_destroyed;                                   \
}                                                                          \
inline operator TYPE*()                                                    \
{                                                                          \
return operator->();                                                   \
}                                                                          \
inline TYPE *operator->()                                                  \
{                                                                          \
if (!_k_static_##NAME) {                                               \
if (isDestroyed()) {                                               \
qFatal("Fatal Error: Accessed global static '%s *%s()' after destruction. " \
"Defined at %s:%d", #TYPE, #NAME, __FILE__, __LINE__);  \
}                                                                  \
TYPE *x = new TYPE ARGS;                                           \
if (!_k_static_##NAME.testAndSetOrdered(0, x)                      \
&& _k_static_##NAME != x ) {                                   \
delete x;                                                      \
} else {                                                           \
static KCleanUpGlobalStatic cleanUpObject = { destroy };       \
}                                                                  \
    }                                                                      \
return _k_static_##NAME;                                               \
}                                                                          \
inline TYPE &operator*()                                                   \
{                                                                          \
return *operator->();                                                  \
}                                                                          \
static void destroy()                                                      \
{                                                                          \
_k_static_##NAME##_destroyed = true;                                   \
TYPE *x = _k_static_##NAME;                                            \
_k_static_##NAME = 0;                                                  \
delete x;                                                              \
}                                                                          \
} NAME;


K_GLOBAL_STATIC(QVector<QColor>, s_standardPalette)
*/

static QVector<QColor>* s_standardPalette;

// Shared with KColorValueSelector
QVector<QColor> kdeui_standardPalette()
{
    if ( !s_standardPalette ) {

        int i = 0;
#ifndef STANDARD_PAL_SIZE
#define STANDARD_PAL_SIZE 17
#endif
        s_standardPalette->resize( STANDARD_PAL_SIZE );

        ( *s_standardPalette ) [ i++ ] = Qt::red;
        ( *s_standardPalette ) [ i++ ] = Qt::green;
        ( *s_standardPalette ) [ i++ ] = Qt::blue;
        ( *s_standardPalette ) [ i++ ] = Qt::cyan;
        ( *s_standardPalette ) [ i++ ] = Qt::magenta;
        ( *s_standardPalette ) [ i++ ] = Qt::yellow;
        ( *s_standardPalette ) [ i++ ] = Qt::darkRed;
        ( *s_standardPalette ) [ i++ ] = Qt::darkGreen;
        ( *s_standardPalette ) [ i++ ] = Qt::darkBlue;
        ( *s_standardPalette ) [ i++ ] = Qt::darkCyan;
        ( *s_standardPalette ) [ i++ ] = Qt::darkMagenta;
        ( *s_standardPalette ) [ i++ ] = Qt::darkYellow;
        ( *s_standardPalette ) [ i++ ] = Qt::white;
        ( *s_standardPalette ) [ i++ ] = Qt::lightGray;
        ( *s_standardPalette ) [ i++ ] = Qt::gray;
        ( *s_standardPalette ) [ i++ ] = Qt::darkGray;
        ( *s_standardPalette ) [ i++ ] = Qt::black;
    }
    return *s_standardPalette;
}


class KHueSaturationSelector::Private
{
public:
    Private(KHueSaturationSelector *q): q(q) {}

    KHueSaturationSelector *q;
    QPixmap pixmap;

    /**
     * Stores the chooser mode
     */
    KColorChooserMode _mode;

    /**
     * Stores the values for hue, saturation and lumniousity
     */
    int _hue, _sat, _colorValue;
};



KHueSaturationSelector::KHueSaturationSelector( QWidget *parent )
        : KXYSelector( parent ), d( new Private( this ) )
{
    setRange( 0, 0, 359, 255 );
    setChooserMode( ChooserClassic );
}

KColorChooserMode KHueSaturationSelector::chooserMode() const
{
    return d->_mode;
}

void KHueSaturationSelector::setChooserMode( KColorChooserMode chooserMode )
{
    int x;
    int y = 255;

    switch ( chooserMode ) {
    case ChooserSaturation:
    case ChooserValue:
        x = 359;
        break;
    default:
        x = 255;
        break;
    }

    setRange( 0, 0, x, y );
    d->_mode = chooserMode;
}

int KHueSaturationSelector::hue () const
{
    return d->_hue;
}

void KHueSaturationSelector::setHue ( int hue )
{
    d->_hue = hue;
}

int KHueSaturationSelector::saturation () const

{
    return d->_sat;
}

void KHueSaturationSelector::setSaturation( int saturation )
{
    d->_sat = saturation;
}

int KHueSaturationSelector::colorValue() const
{
    return d->_colorValue;
}

void KHueSaturationSelector::setColorValue( int colorValue )
{
    d->_colorValue = colorValue;
}

KHueSaturationSelector::~KHueSaturationSelector()
{
    delete d;
}

void KHueSaturationSelector::updateContents()
{
    drawPalette( &d->pixmap );
}

void KHueSaturationSelector::resizeEvent( QResizeEvent * )
{
    updateContents();
}

void KHueSaturationSelector::drawContents( QPainter *painter )
{
    painter->drawPixmap( contentsRect().x(), contentsRect().y(), d->pixmap );
}

void KHueSaturationSelector::drawPalette( QPixmap *pixmap )
{
    int xSize = contentsRect().width(), ySize = contentsRect().height();
    QImage image( QSize( xSize, ySize ), QImage::Format_RGB32 );
    QColor col;
    int h, s;
    uint *p;

    col.setHsv( hue(), saturation(), colorValue() );

    int _h, _s, _v, _r, _g, _b;
    col.getHsv( &_h, &_s, &_v );
    col.getRgb( &_r, &_g, &_b );

	for ( s = ySize-1; s >= 0; s-- )
	{
        p = ( uint * ) image.scanLine( ySize - s - 1 );
		for( h = 0; h < xSize; h++ )
		{
            switch ( chooserMode() ) {
            case ChooserClassic:
            default:
                col.setHsv( 359 * h / ( xSize - 1 ), 255 * s / ( ( ySize == 1 ) ? 1 : ySize - 1 ), 192 );
                break;
            case ChooserHue:
                col.setHsv( _h, 255 * h / ( xSize - 1 ), 255 * s / ( ( ySize == 1 ) ? 1 : ySize - 1 ) );
                break;
            case ChooserSaturation:
                col.setHsv( 359 * h / ( xSize - 1 ), _s, 255 * s / ( ( ySize == 1 ) ? 1 : ySize - 1 ) );
                break;
            case ChooserValue:
                col.setHsv( 359 * h / ( xSize - 1 ), 255 * s / ( ( ySize == 1 ) ? 1 : ySize - 1 ), _v );
                break;
            case ChooserRed:
                col.setRgb( _r, 255 * h / ( xSize - 1 ), 255 * s / ( ( ySize == 1 ) ? 1 : ySize - 1 ) );
                break;
            case ChooserGreen:
                col.setRgb( 255 * h / ( xSize - 1 ), _g, 255 * s / ( ( ySize == 1 ) ? 1 : ySize - 1 ) );
                break;
            case ChooserBlue:
                col.setRgb( 255 * s / ( ( ySize == 1 ) ? 1 : ySize - 1 ), 255 * h / ( xSize - 1 ), _b );
                break;
            }

            *p = col.rgb();
            p++;
        }
    }

    /*
    if ( pixmap->depth() <= 8 )
    {
        const QVector<QColor> standardPalette = kdeui_standardPalette();
        KImageEffect::dither( image, standardPalette.data(), standardPalette.size() );
    }
    */
    *pixmap = QPixmap::fromImage( image );
}

// #include "khuesaturationselect.moc"
