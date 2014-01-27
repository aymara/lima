/* This file is part of the KDE libraries
   Copyright (C) 2007 Timo A. Hummel (timo@timohummel.com)

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
#ifndef KCOLORCHOOSERMODE_H_
#define KCOLORCHOOSERMODE_H_

enum KColorChooserMode
{
	ChooserClassic	 = 0x0000,
	ChooserHue       = 0x0001,
	ChooserSaturation= 0x0002,
	ChooserValue     = 0x0003,
	ChooserRed       = 0x0004,
	ChooserGreen     = 0x0005,
	ChooserBlue      = 0x0006
};

#endif /* KCOLORCHOOSERMODE_H_ */
