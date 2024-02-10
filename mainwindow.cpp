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
    // QElapsedTimer timer;
    // timer.start();
    statusBar()->clearMessage();
    deleteAllButtons();
    QBrush brush = palette().window();
    QList<QString> iconNames = m_theme.contextIcons(ui->cboContext->currentText());
    for (int i = 0; i < iconNames.count(); ++i) {
        QString name = iconNames[i];
        QListWidgetItem *item = new QListWidgetItem(ui->buttonsWidget);
        item->setToolTip(name);
        item->setIcon(m_theme.loadIcon(name));
        item->setStatusTip(name);
        item->setBackground(brush);
    }
    //qDebug() << Q_FUNC_INFO << "elapsed time:" << timer.elapsed();
}

void MainWindow::styleChanged(const QString name)
{
    qApp->setStyle(name);
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
    qApp->setPalette(checked ? dark : light);
    QBrush brush = checked ? dark.window() : light.window();
    for (int row = 0; row < ui->buttonsWidget->count(); row++) {
        ui->buttonsWidget->item(row)->setBackground(brush);
    }
}
