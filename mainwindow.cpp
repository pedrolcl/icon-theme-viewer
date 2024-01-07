#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include <QDir>
#include <QGridLayout>
#include <QLabel>
#include <QMap>
#include <QMetaEnum>
#include <QScrollArea>
#include <QString>
#include <QStyle>
#include <QStyleFactory>
#include <QToolButton>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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
    QElapsedTimer timer;
    timer.start();
    statusBar()->clearMessage();
    deleteAllButtons();
    QList<QString> iconNames = m_theme.contextIcons(ui->cboContext->currentText());
    ui->buttonsWidget->setUpdatesEnabled(false);
    for (int i = 0; i < iconNames.count(); ++i) {
        QToolButton *btn = new QToolButton(ui->buttonsWidget);
        btn->setFixedSize(32, 32);
        btn->setIconSize(QSize(32, 32));
        QString name = iconNames[i];
        btn->setToolTip(name);
        btn->setIcon(m_theme.loadIcon(name));
        connect(btn, &QToolButton::clicked, this, [=] { statusBar()->showMessage(btn->toolTip()); });
        ui->btnsGrid->addWidget(btn, i / 8, i % 8);
    }
    auto rows = iconNames.count() / 8;
    if (rows < 14) {
        ui->btnsGrid->setRowStretch(rows + 1, 1);
    }
    ui->buttonsWidget->setUpdatesEnabled(true);
    ui->buttonsWidget->update();
    qDebug() << Q_FUNC_INFO << "elapsed time:" << timer.elapsed();
}

void MainWindow::styleChanged(const QString name)
{
    qDebug() << Q_FUNC_INFO << name;
    qApp->setStyle(name);
}

void MainWindow::themeChanged(const QString name)
{
    qDebug() << Q_FUNC_INFO << name;
    m_theme.changeTheme(name);
    ui->cboContext->clear();
    ui->cboContext->addItems(m_theme.themeContexts());
    if (m_theme.themeContexts().count() > 0) {
        ui->cboContext->setCurrentText(m_theme.themeContexts().constFirst());
    }
    refreshIcons();
}

void MainWindow::contextChanged(const QString name)
{
    qDebug() << Q_FUNC_INFO << name;
    refreshIcons();
}

void MainWindow::deleteAllButtons()
{
    QElapsedTimer timer;
    timer.start();
    ui->buttonsWidget->setUpdatesEnabled(false);
    qDeleteAll(
        ui->buttonsWidget->findChildren<QToolButton *>(QString(), Qt::FindDirectChildrenOnly));
    ui->buttonsWidget->setUpdatesEnabled(true);
    qDebug() << Q_FUNC_INFO << "elapsed time:" << timer.elapsed();
}

void MainWindow::darkModeChanged(const bool checked)
{
    static const QPalette dark(QColor(0x30, 0x30, 0x30));
    static const QPalette light(QColor(0xc0, 0xc0, 0xc0));
    qDebug() << Q_FUNC_INFO;
    qApp->setPalette(checked ? dark : light);
}
