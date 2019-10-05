/**************************************************************************************************
---------------------------------------------------------------------------------------------------
	Copyright (C) 2017  Jonathan Bagg
	This file is part of QtZeroConf.

	QtZeroConf is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	QtZeroConf is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License
	along with QtZeroConf.  If not, see <http://www.gnu.org/licenses/>.
---------------------------------------------------------------------------------------------------
   Project name : QtZeroConf
   File name    : qzeroconfglobal.h
   Created      : 31 Oct 2017
   Author(s)    : Jonathan Bagg
---------------------------------------------------------------------------------------------------
   Sets up Q_DECL_EXPORT / IMPORT for QZeroConf and QZeroConfService classes
---------------------------------------------------------------------------------------------------
**************************************************************************************************/
#ifndef QZEROCONFGLOBAL_H
#define QZEROCONFGLOBAL_H

#include <QtCore/QtGlobal>

#if (!defined(QT_STATIC) && !defined(QZEROCONF_STATIC))
#	ifdef QT_BUILD_ZEROCONF_LIB
#		define Q_ZEROCONF_EXPORT Q_DECL_EXPORT
#	else
#		define Q_ZEROCONF_EXPORT Q_DECL_IMPORT
#	endif
#else
#	define Q_ZEROCONF_EXPORT
#endif

#endif // QZEROCONFGLOBAL_H
