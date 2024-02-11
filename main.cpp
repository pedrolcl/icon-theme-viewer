// Copyright (c) 2023-2024, Pedro López-Cabanillas
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QApplication>
#include <QObject>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication::setOrganizationDomain(QT_STRINGIFY(APPDOMAIN));
    QApplication::setApplicationName(QT_STRINGIFY(APPNAME));
    QApplication::setApplicationVersion(QT_STRINGIFY(APPVERSION));
    QApplication::setApplicationDisplayName(QObject::tr("Icon Theme Viewer"));
    QApplication app(argc, argv);
    MainWindow win;
    win.show();
    return app.exec();
}
