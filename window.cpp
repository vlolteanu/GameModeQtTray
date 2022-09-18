/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "window.h"

#ifndef QT_NO_SYSTEMTRAYICON

#include <QAction>
#include <QCheckBox>
#include <QComboBox>
#include <QCoreApplication>
#include <QCloseEvent>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QPushButton>
#include <QSpinBox>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QMessageBox>

#include <gamemode_client.h>

Window::Window()
{
	icons.emplace_back(std::pair<QIcon, QString> { ":/images/applications-all.svg",   tr("GameMode Off") });
	icons.emplace_back(std::pair<QIcon, QString> { ":/images/applications-games.svg", tr("GameMode On") });

	createActions();
	createTrayIcon();

	setIcon(0);

	connect(trayIcon, &QSystemTrayIcon::activated, this, &Window::iconActivated);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	setLayout(mainLayout);

	trayIcon->show();

	setWindowTitle(tr("GameModeQtTray"));
	resize(400, 300);
}

void Window::closeEvent(QCloseEvent *event)
{
#ifdef Q_OS_MACOS
	if (!event->spontaneous() || !isVisible())
	{
		return;
	}
#endif
	if (trayIcon->isVisible())
	{
		hide();
		event->ignore();
	}
}

void Window::setIcon(int index)
{
	QIcon icon = icons[index].first;
	trayIcon->setIcon(icon);
	setWindowIcon(icon);

	trayIcon->setToolTip(icons[index].second);
}

void Window::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
	if (reason != QSystemTrayIcon::Trigger && reason != QSystemTrayIcon::DoubleClick)
		return;

	bool wanted = !enabled;

	if (wanted)
	{
		int rc = gamemode_request_start();
		if (rc)
		{
			QMessageBox::critical(this, tr("GameModeQtTray"),
				tr("Failed to request gamemode start: ") +
				tr(gamemode_error_string()));
			return;
		}
	}
	else
	{
		int rc = gamemode_request_end();
		if (rc)
		{
			QMessageBox::critical(this, tr("GameModeQtTray"),
				tr("Failed to request gamemode end: ") +
				tr(gamemode_error_string()));
			return;
		}
	}

	enabled = wanted;
	setIcon(enabled);
}

void Window::createActions()
{
	quitAction = new QAction(tr("&Quit"), this);
	connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
}

void Window::createTrayIcon()
{
	trayIconMenu = new QMenu(this);
	trayIconMenu->addSeparator();
	trayIconMenu->addAction(quitAction);

	trayIcon = new QSystemTrayIcon(this);
	trayIcon->setContextMenu(trayIconMenu);
}

#endif
