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
   File name    : window.h
   Created      : 3 November 2015
   Author(s)    : Jonathan Bagg
---------------------------------------------------------------------------------------------------
   Example app to demonstrate service publishing and service discovery
---------------------------------------------------------------------------------------------------
**************************************************************************************************/
#ifndef WINDOW_H_
#define WINDOW_H_

#include <QMainWindow>
#include <QTableWidget>
#include "qzeroconf.h"

class mainWindow : public QMainWindow
{
	Q_OBJECT

public:
	mainWindow();

private:
	void buildGUI();
	void startPublish();
	QString buildName(void);
	QTableWidget table;
	QZeroConf zeroConf;
	bool publishEnabled;

private slots:
	void appStateChanged(Qt::ApplicationState state);
	void startPublishClicked();
	void stopPublishClicked();
	void addService(QZeroConfService item);
	void removeService(QZeroConfService item);
	void updateService(QZeroConfService zcs);
};

#endif /* WINDOW_H_ */
