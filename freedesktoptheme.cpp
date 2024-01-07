#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QList>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QSettings>
#include <QString>

#include "freedesktoptheme.h"

FreedesktopTheme::FreedesktopTheme(QObject *parent)
    : QObject{parent}
{
    loadThemes();
    loadTheme();
    loadThemeIcons();
    //dumpTheme();
}

void FreedesktopTheme::loadThemes()
{
    QElapsedTimer timer;
    timer.start();
    m_themes.clear();
    m_themeNames.clear();
    foreach (const auto searchPath, QIcon::themeSearchPaths()) {
        QDir searchDir(searchPath);
        searchDir.setFilter(QDir::AllDirs | QDir::Drives | QDir::NoDotAndDotDot | QDir::Readable);
        searchDir.setSorting(QDir::NoSort);
        foreach (const auto entry, searchDir.entryInfoList()) {
            if (QFile::exists(
                    QDir(entry.canonicalFilePath()).absoluteFilePath(QStringLiteral("index.theme"))))
                m_themes.insert(entry.fileName(), entry.filePath());
        }
    }
    m_themeNames = m_themes.keys();
    m_themeNames.sort();
    m_themeNames.removeDuplicates();
    qDebug() << Q_FUNC_INFO << "elapsed time:" << timer.elapsed();
}

void FreedesktopTheme::loadTheme()
{
    using namespace Qt::Literals::StringLiterals;
    QElapsedTimer timer;
    timer.start();
    QFileInfo themeIndex;
    const QList<QString> iconDirs = QIcon::themeSearchPaths();
    foreach (const auto dirName, iconDirs) {
        QDir themeDir(dirName);
        themeDir.cd(currentTheme());
        themeIndex.setFile(themeDir, "index.theme"_L1);
        if (themeIndex.exists()) {
            break;
        }
    }
    if (themeIndex.exists()) {
        const QSettings indexReader(themeIndex.absoluteFilePath(), QSettings::IniFormat);
        const QList<QString> keys = indexReader.allKeys();
        foreach (const auto &key, keys) {
            if (key.endsWith("/Context"_L1)) {
                auto context = indexReader.value(key).toString().toLower();
                m_themeContexts.append(context);
                m_contextDirs[context].insert(key.left(key.indexOf('/')));
            }
        }
        m_parents = indexReader.value("Icon Theme/Inherits"_L1).toStringList();
        m_parents.sort();
        m_parents.removeAll(QString());
        m_themeContexts.sort();
        m_themeContexts.removeDuplicates();
    }
    qDebug() << Q_FUNC_INFO << "elapsed time:" << timer.elapsed();
}

void FreedesktopTheme::loadThemeIcons()
{
    QElapsedTimer timer;
    timer.start();
    const auto themePath = m_themes[currentTheme()];
    QRegularExpression rex(themePath + "/(.*)/.*");
    QDir iconDir(themePath);
    iconDir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::Readable);
    iconDir.setSorting(QDir::Name);
    QDirIterator iterator(iconDir, QDirIterator::FollowSymlinks | QDirIterator::Subdirectories);
    while (iterator.hasNext()) {
        iterator.next();
        const auto iconName = iterator.fileInfo().baseName();
        QString dir, context;
        QRegularExpressionMatch match = rex.match(iterator.fileInfo().path());
        if (match.hasMatch()) {
            dir = match.captured(1);
            context = dirContext(dir);
        }
        if (!context.isEmpty() && QIcon::hasThemeIcon(iconName)) {
            if (m_iconNames[context].contains(iconName))
                continue;
            m_iconNames[context].insert(iconName);
        }
    }
    qDebug() << Q_FUNC_INFO << "elapsed time:" << timer.elapsed();
}

void FreedesktopTheme::changeTheme(const QString themeName)
{
    if (m_themes.contains(themeName) && themeName != currentTheme()) {
        m_iconNames.clear();
        m_themeContexts.clear();
        m_contextDirs.clear();
        QIcon::setThemeName(themeName);
        loadTheme();
        loadThemeIcons();
        //dumpTheme();
    }
}

/*void FreedesktopTheme::dumpTheme()
{
    qDebug() << Q_FUNC_INFO;
    qDebug() << "available icon themes:" << themeNames();
    qDebug() << "system theme name:" << systemTheme();
    qDebug() << "current theme name:" << currentTheme();
    qDebug() << "theme contexts:" << themeContexts();
    qDebug() << "theme icons:" << iconNames();
    qDebug() << "context dirs:" << m_contextDirs["applications"];
    qDebug() << "application icons:" << contextIcons("applications");
}*/

QIcon FreedesktopTheme::loadIcon(const QString &iconName) const
{
    return QIcon::fromTheme(iconName);
}

QString FreedesktopTheme::currentTheme() const
{
    return QIcon::themeName();
}

QMap<QString, QSet<QString>> FreedesktopTheme::iconNames() const
{
    return m_iconNames;
}

QMap<QString, QString> FreedesktopTheme::themes() const
{
    return m_themes;
}

QString FreedesktopTheme::systemTheme() const
{
    return m_systemTheme;
}

QStringList FreedesktopTheme::themeNames() const
{
    return m_themeNames;
}

QList<QString> FreedesktopTheme::themeContexts() const
{
    return m_themeContexts;
}

QList<QString> FreedesktopTheme::contextIcons(const QString &context) const
{
    const QSet<QString> &set = m_iconNames[context];
    QList<QString> tmp(set.begin(), set.end());
    tmp.sort();
    return tmp;
}

QString FreedesktopTheme::dirContext(const QString &dirName) const
{
    foreach (const auto &key, m_contextDirs.keys()) {
        const QSet<QString> &set = m_contextDirs[key];
        if (set.contains(dirName)) {
            return key;
        }
    }
    return QString();
}
