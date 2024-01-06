#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "freedesktoptheme.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
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

public slots:
    void darkModeChanged(const bool checked);

private:
    FreedesktopTheme m_theme;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
