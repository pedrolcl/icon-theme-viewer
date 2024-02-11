// Copyright (c) 2023-2024, Pedro López-Cabanillas
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDir>
#include <QGridLayout>
#include <QLabel>
#include <QMap>
#include <QMenu>
#include <QMessageBox>
#include <QMetaEnum>
#include <QScrollArea>
#include <QString>
#include <QStyle>
#include <QStyleFactory>
#include <QToolButton>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : FramelessWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(QApplication::applicationDisplayName());

    QAction *framelessAction = new QAction(tr("Frameless Window"), this);
    framelessAction->setCheckable(true);
    framelessAction->setChecked(true);
    connect(framelessAction, &QAction::triggered, this, &MainWindow::framelessModeChanged);

    m_minimizeActrion = new QAction(tr("Minimize"), this);
    m_minimizeActrion->setIcon(QIcon::fromTheme("window-minimize"));
    connect(m_minimizeActrion, &QAction::triggered, this, [=] {
        setWindowState(Qt::WindowMinimized);
    });

    m_exitAction = new QAction(tr("Quit"), this);
    m_exitAction->setIcon(QIcon::fromTheme("window-close"));
    connect(m_exitAction, &QAction::triggered, this, &FramelessWindow::close);

    QAction *aboutAction = new QAction(tr("About..."), this);
    connect(aboutAction, &QAction::triggered, this, &MainWindow::showAboutBox);

    QAction *aboutQtAction = new QAction(tr("About Qt..."), this);
    connect(aboutQtAction, &QAction::triggered, qApp, &QApplication::aboutQt);

    QMenu *popupMenu = new QMenu(this);
    popupMenu->addAction(framelessAction);
    popupMenu->addAction(aboutAction);
    popupMenu->addAction(aboutQtAction);
    popupMenu->addSeparator();
    popupMenu->addAction(m_minimizeActrion);
    popupMenu->addAction(m_exitAction);

    m_appmenu = new QToolButton(this);
    m_appmenu->setAutoRaise(true);
    m_appmenu->setIcon(QIcon::fromTheme("application-menu"));
    m_appmenu->setPopupMode(QToolButton::InstantPopup);
    m_appmenu->setMenu(popupMenu);

    QLabel *titleLabel = new QLabel(QApplication::applicationDisplayName(), this);
    titleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    titleLabel->setAlignment(Qt::AlignCenter);

    ui->toolBar->addWidget(m_appmenu);
    m_titleAction = ui->toolBar->addWidget(titleLabel);
    ui->toolBar->addAction(m_minimizeActrion);
    ui->toolBar->addAction(m_exitAction);
    setPseudoCaption(ui->toolBar);

    QStringList styleNames = QStyleFactory::keys();
    foreach (const auto &n, styleNames) {
        ui->cboStyle->addItem(n.toLower());
    }
    QString currentStyle = qApp->style()->objectName();
    ui->cboStyle->setCurrentText(currentStyle.toLower());
    ui->cboTheme->addItems(m_theme.themeNames());
    ui->cboContext->addItems(m_theme.themeContexts());
    ui->cboTheme->setCurrentText(m_theme.currentTheme());
    ui->cboContext->setCurrentText(m_theme.themeContexts().constFirst());
    ui->chkDarkMode->setChecked(palette().color(QPalette::WindowText).lightness()
                                > palette().color(QPalette::Window).lightness());
    connect(ui->chkDarkMode, &QCheckBox::toggled, this, &MainWindow::darkModeChanged);
    connect(ui->cboStyle, &QComboBox::currentTextChanged, this, &MainWindow::styleChanged);
    connect(ui->cboTheme, &QComboBox::currentTextChanged, this, &MainWindow::themeChanged);
    connect(ui->cboContext, &QComboBox::currentTextChanged, this, &MainWindow::contextChanged);
    refreshIcons();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::refreshIcons()
{
    // QElapsedTimer timer;
    // timer.start();
    statusBar()->clearMessage();
    deleteAllButtons();
    QList<QString> iconNames = m_theme.contextIcons(ui->cboContext->currentText());
    for (int i = 0; i < iconNames.count(); ++i) {
        QString name = iconNames[i];
        QListWidgetItem *item = new QListWidgetItem(ui->buttonsWidget);
        item->setToolTip(name);
        item->setIcon(m_theme.loadIcon(name));
        item->setStatusTip(name);
        item->setBackground(palette().window());
    }
    //qDebug() << Q_FUNC_INFO << "elapsed time:" << timer.elapsed();
}

void MainWindow::styleChanged(const QString name)
{
    qApp->setStyle(name);
}

void MainWindow::updateAppIcons()
{
    m_appmenu->setIcon(QIcon::fromTheme("application-menu"));
    m_minimizeActrion->setIcon(QIcon::fromTheme("window-minimize"));
    m_exitAction->setIcon(QIcon::fromTheme("window-close"));
}

void MainWindow::themeChanged(const QString name)
{
    m_theme.changeTheme(name);
    ui->cboContext->clear();
    ui->cboContext->addItems(m_theme.themeContexts());
    if (m_theme.themeContexts().count() > 0) {
        ui->cboContext->setCurrentText(m_theme.themeContexts().constFirst());
    }
    refreshIcons();
    updateAppIcons();
}

void MainWindow::contextChanged(const QString name)
{
    refreshIcons();
}

void MainWindow::deleteAllButtons()
{
    // QElapsedTimer timer;
    // timer.start();
    ui->buttonsWidget->clear();
    //qDebug() << Q_FUNC_INFO << "elapsed time:" << timer.elapsed();
}

void MainWindow::darkModeChanged(const bool checked)
{
    static const QPalette dark(QColor(0x30, 0x30, 0x30));
    static const QPalette light(QColor(0xc0, 0xc0, 0xc0));
    const QBrush &brush = checked ? dark.window() : light.window();
    qApp->setPalette(checked ? dark : light);
    for (int row = 0; row < ui->buttonsWidget->count(); ++row) {
        ui->buttonsWidget->item(row)->setBackground(brush);
    }
    updateAppIcons();
}

void MainWindow::framelessModeChanged(const bool checked)
{
    setWindowFlag(Qt::FramelessWindowHint, checked);
    setPseudoCaption(checked ? ui->toolBar : nullptr);
    m_titleAction->setVisible(checked);
    m_minimizeActrion->setVisible(checked);
    m_exitAction->setVisible(checked);
    show();
}

void MainWindow::showAboutBox()
{
    using namespace Qt::Literals::StringLiterals;
    QStringList developers{u"Pedro López-Cabanillas"_s};
    const auto text
        = tr("<h1>%1</h1>"
             "<p>Version %2</p>"
             "<p><a "
             "href='http://github.com/pedrolcl/%3/'>http://github.com/pedrolcl/%3/</a></p>"
             "<p>This program is free software; you can redistribute it and/or modify it under "
             "the terms of "
             "the GNU General Public License as published by the Free Software Foundation; "
             "either version "
             "3 of the License, or (at your option) any later version.</p>"
             "<p>%1 is distributed in the hope that it will be useful, but "
             "WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or "
             "FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more "
             "details.</p>"
             "<p>You should have received a copy of the GNU General Public License along with "
             "%1. If not, see <a "
             "href='https://www.gnu.org/licenses/'>https://www.gnu.org/licenses/</a></p>"
             "<p>Copyright &copy; 2023-2024 %4</p>")
              .arg(QApplication::applicationDisplayName(),
                   QApplication::applicationVersion(),
                   QApplication::applicationName(),
                   developers.join(QLatin1String(", ")));
    QMessageBox::about(this, tr("About"), text);
}
