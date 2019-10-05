/**************************************************************************************************
---------------------------------------------------------------------------------------------------
	Copyright (C) 2015  Jonathan Bagg
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
   Project name : QtZeroConf Example
   File name    : main.cpp
   Created      : 3 November 2015
   Author(s)    : Jonathan Bagg
---------------------------------------------------------------------------------------------------
   Example app to demonstrate service publishing and service discovery
---------------------------------------------------------------------------------------------------
**************************************************************************************************/
#include <QApplication>
#include "window.h"


int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	app.setApplicationName("QtZeroConf Example");
	app.setApplicationVersion("0.1");

	mainWindow window;

	return app.exec();
}
