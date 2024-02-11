// Copyright (c) 2023-2024, Pedro López-Cabanillas
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAction>
#include <QToolButton>

#include "framelesswindow.h"
#include "freedesktoptheme.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public FramelessWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void refreshIcons();
    void styleChanged(const QString name);
    void themeChanged(const QString name);
    void contextChanged(const QString name);
    void deleteAllButtons();
    void updateAppIcons();

public slots:
    void darkModeChanged(const bool checked);
    void framelessModeChanged(const bool checked);
    void showAboutBox();

private:
    FreedesktopTheme m_theme;
    Ui::MainWindow *ui;

    QAction *m_minimizeActrion;
    QAction *m_exitAction;
    QAction *m_titleAction;
    QToolButton *m_appmenu;
};

#endif // MAINWINDOW_H
