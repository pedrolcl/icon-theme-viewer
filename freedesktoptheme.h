#ifndef FREEDESKTOPTHEME_H
#define FREEDESKTOPTHEME_H

#include <QIcon>
#include <QMap>
#include <QObject>
#include <QSet>
#include <QString>

class FreedesktopTheme : public QObject
{
    Q_OBJECT
public:
    explicit FreedesktopTheme(QObject *parent = nullptr);
    void changeTheme(const QString themeName);

    QList<QString> themeNames() const;
    QList<QString> themeContexts() const;
    QList<QString> contextIcons(const QString &context) const;
    QString systemTheme() const;
    QString currentTheme() const;
    QMap<QString, QSet<QString>> iconNames() const;
    QMap<QString, QString> themes() const;
    QIcon loadIcon(const QString &iconName) const;
    // void dumpTheme();

protected:
    void loadThemes();
    void loadTheme();
    void loadThemeIcons();

private:
    QList<QString> m_themeNames;
    QList<QString> m_themeContexts;
    QMap<QString, QString> m_themes; // [key=name]->path
    QMap<QString, QSet<QString>> m_iconNames; //[key=context]->{icon_name, ...}
    QList<QString> m_parents;
    const QString m_systemTheme = QIcon::themeName();
};

#endif // FREEDESKTOPTHEME_H
