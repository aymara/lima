// Copyright (C) 2007 Matthew Woehlke <mw_triad@users.sourceforge.net>
// Copyright (C) 2007 Thomas Zander <zander@kde.org>
// Copyright (C) 2007 Zack Rusin <zack@kde.org>
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef KCOLORUTILS_H
#define KCOLORUTILS_H

#include <kdeui_export.h>
#include <QtGui/QPainter>

class QColor;

/**
 * A set of methods used to work with colors.
 */
namespace KColorUtils {
    /**
     * Calculate the luma of a color. Luma is weighted sum of gamma-adjusted
     * R'G'B' components of a color. The result is similar to qGray. The range
     * is from 0.0 (black) to 1.0 (white).
     *
     * KColorUtils::darken(), KColorUtils::lighten() and KColorUtils::shade()
     * operate on the luma of a color.
     *
     * @see http://en.wikipedia.org/wiki/Luma_(video)
     */
    KDEUI_EXPORT qreal luma(const QColor&);

    /**
     * Calculate the contrast ratio between two colors, according to the
     * W3C/WCAG2.0 algorithm, (Lmax + 0.05)/(Lmin + 0.05), where Lmax and Lmin
     * are the luma values of the lighter color and the darker color,
     * respectively.
     *
     * A contrast ration of 5:1 (result == 5.0) is the minimum for "normal"
     * text to be considered readable (large text can go as low as 3:1). The
     * ratio ranges from 1:1 (result == 1.0) to 21:1 (result == 21.0).
     *
     * @see KColorUtils::luma
     */
    KDEUI_EXPORT qreal contrastRatio(const QColor&, const QColor&);

    /**
     * Adjust the luma of a color by changing its distance from white.
     *
     * @li amount == 1.0 gives white
     * @li amount == 0.5 results in a color whose luma is halfway between 1.0
     * and that of the original color
     * @li amount == 0.0 gives the original color
     * @li amount == -1.0 gives a color that is 'twice as far from white' as
     * the original color, that is luma(result) == 1.0 - 2*(1.0 - luma(color))
     *
     * @param amount factor by which to adjust the luma component of the color
     * @param chromaInverseGain (optional) factor by which to adjust the chroma
     * component of the color; 1.0 means no change, 0.0 maximizes chroma
     * @see KColorUtils::shade
     */
    KDEUI_EXPORT QColor lighten(const QColor&, qreal amount = 0.5, qreal chromaInverseGain = 1.0);

    /**
     * Adjust the luma of a color by changing its distance from black.
     *
     * @li amount == 1.0 gives black
     * @li amount == 0.5 results in a color whose luma is halfway between 0.0
     * and that of the original color
     * @li amount == 0.0 gives the original color
     * @li amount == -1.0 gives a color that is 'twice as far from black' as
     * the original color, that is luma(result) == 2*luma(color)
     *
     * @param amount factor by which to adjust the luma component of the color
     * @param chromaGain (optional) factor by which to adjust the chroma
     * component of the color; 1.0 means no change, 0.0 minimizes chroma
     * @see KColorUtils::shade
     */
    KDEUI_EXPORT QColor darken(const QColor&, qreal amount = 0.5, qreal chromaGain = 1.0);

    /**
     * Adjust the luma and chroma components of a color. The amount is added
     * to the corresponding component.
     *
     * @param lumaAmount amount by which to adjust the luma component of the
     * color; 0.0 results in no change, -1.0 turns anything black, 1.0 turns
     * anything white
     * @param chromaAmount (optional) amount by which to adjust the chroma
     * component of the color; 0.0 results in no change, -1.0 minimizes chroma,
     * 1.0 maximizes chroma
     * @see KColorUtils::luma
     */
    KDEUI_EXPORT QColor shade(const QColor&, qreal lumaAmount, qreal chromaAmount = 0.0);

    /**
     * Create a new color by tinting one color with another. This function is
     * meant for creating additional colors withing the same class (background,
     * foreground) from colors in a different class. Therefore when @p amount
     * is low, the luma of @p base is mostly preserved, while the hue and
     * chroma of @p color is mostly inherited.
     *
     * @param base color to be tinted
     * @param color color with which to tint
     * @param amount how strongly to tint the base; 0.0 gives @p base,
     * 1.0 gives @p color
     */
    KDEUI_EXPORT QColor tint(const QColor &base, const QColor &color, qreal amount = 0.3);

    /**
     * Blend two colors into a new color by linear combination.
     * @code
        QColor lighter = KColorUtils::mix(myColor, Qt::white)
     * @endcode
     * @param c1 first color.
     * @param c2 second color.
     * @param bias weight to be used for the mix. @p bias <= 0 gives @p c1,
     * @p bias >= 1 gives @p c2. @p bias == 0.5 gives a 50% blend of @p c1
     * and @p c2.
     */
    KDEUI_EXPORT QColor mix(const QColor &c1, const QColor &c2,
                            qreal bias = 0.5);

    /**
     * Blend two colors into a new color by painting the second color over the
     * first using the specified composition mode.
     * @code
        QColor white(Qt::white);
        white.setAlphaF(0.5);
        QColor lighter = KColorUtils::overlayColors(myColor, white);
       @endcode
     * @param base the base color (alpha channel is ignored).
     * @param paint the color to be overlayed onto the base color.
     * @param comp the CompositionMode used to do the blending.
     */
    KDEUI_EXPORT QColor overlayColors(const QColor &base, const QColor &paint,
                                      QPainter::CompositionMode comp = QPainter::CompositionMode_SourceOver);

}

#endif // KCOLORUTILS_H
// kate: space-indent on; indent-width 4; replace-tabs on; auto-insert-doxygen on;
