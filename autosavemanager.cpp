#include "autosavemanager.h"
#include <QStandardPaths>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

AutoSaveManager::AutoSaveManager(TabManager *tabManager, QObject *parent)
    : QObject(parent)
    , tabManager(tabManager)
    , autoSaveEnabled(true)
    , saveInterval(5)  // Default 5 minutes
{
    autoSaveTimer = new QTimer(this);
    autoSaveTimer->setSingleShot(false);  // Make it a repeating timer
    connect(autoSaveTimer, &QTimer::timeout, this, &AutoSaveManager::onAutoSaveTimeout);
    connect(tabManager, &TabManager::tabClosed, this, &AutoSaveManager::onTabClosed);
    connect(tabManager, &TabManager::textChanged, this, &AutoSaveManager::onTextChanged);

    setupAutoSaveDir();
    if (autoSaveEnabled) {
        autoSaveTimer->start(saveInterval * 60 * 1000);  // Convert minutes to milliseconds
        qDebug() << "Auto save timer started with interval:" << saveInterval << "minutes";
    }
}

AutoSaveManager::~AutoSaveManager()
{
    saveAllTabs();
    clearAutoSaveFiles();
}

void AutoSaveManager::setAutoSaveInterval(int minutes)
{
    if (minutes > 0) {
        saveInterval = minutes;
        if (autoSaveEnabled) {
            autoSaveTimer->start(saveInterval * 60 * 1000);
            qDebug() << "Auto save timer restarted with new interval:" << saveInterval << "minutes";
        }
    }
}

int AutoSaveManager::autoSaveInterval() const
{
    return autoSaveTimer->interval() / 60000;
}

void AutoSaveManager::setAutoSaveEnabled(bool enabled)
{
    autoSaveEnabled = enabled;
    if (enabled) {
        autoSaveTimer->start(saveInterval * 60 * 1000);
        qDebug() << "Auto save timer started with interval:" << saveInterval << "minutes";
    } else {
        autoSaveTimer->stop();
        qDebug() << "Auto save timer stopped";
    }
}

bool AutoSaveManager::isAutoSaveEnabled() const
{
    return autoSaveTimer->isActive();
}

void AutoSaveManager::saveAllTabs()
{
    qDebug() << "Saving all tabs...";
    for (int i = 0; i < tabManager->count(); ++i) {
        saveTab(i);
    }
}

void AutoSaveManager::restoreTabs()
{
    QFile infoFile(autoSaveDir.filePath("tabs.json"));
    if (!infoFile.exists()) {
        return;
    }

    if (!infoFile.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(nullptr, tr("Auto Save"),
            tr("Cannot read auto save information."));
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(infoFile.readAll());
    if (doc.isNull() || !doc.isArray()) {
        return;
    }

    QJsonArray tabs = doc.array();
    for (const QJsonValue &tabValue : tabs) {
        if (!tabValue.isObject()) continue;

        QJsonObject tab = tabValue.toObject();
        QString path = tab["path"].toString();
        QString content = tab["content"].toString();
        int language = tab["language"].toInt();

        int index = tabManager->addNewTab(path.isEmpty() ? tr("Untitled") : QFileInfo(path).fileName());
        if (QTextEdit *editor = tabManager->editorAt(index)) {
            editor->setPlainText(content);
            tabManager->setCurrentTabPath(path);
            tabManager->setCurrentTabLanguage(static_cast<SyntaxHighlighter::Language>(language));
        }
    }
}

void AutoSaveManager::onAutoSaveTimeout()
{
    qDebug() << "Auto save timeout triggered";
    if (autoSaveEnabled) {
        saveAllTabs();
    }
}

void AutoSaveManager::onTabClosed(int index)
{
    QString autoSavePath = getAutoSavePath(index);
    QFile::remove(autoSavePath);
}

void AutoSaveManager::onTextChanged()
{
    if (autoSaveEnabled) {
        saveTab(tabManager->currentIndex());
    }
}

void AutoSaveManager::setupAutoSaveDir()
{
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    autoSaveDir = QDir(appDataPath + "/autosave");
    if (!autoSaveDir.exists()) {
        autoSaveDir.mkpath(".");
    }
}

QString AutoSaveManager::getAutoSavePath(int tabIndex) const
{
    return autoSaveDir.filePath(QString("tab_%1.txt").arg(tabIndex));
}

void AutoSaveManager::saveTab(int tabIndex)
{
    if (tabIndex < 0 || tabIndex >= tabManager->count()) {
        return;
    }

    QTextEdit *editor = tabManager->editorAt(tabIndex);
    if (!editor) {
        return;
    }

    // Save tab content
    QString autoSavePath = getAutoSavePath(tabIndex);
    QFile file(autoSavePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out.setCodec("UTF-8");
        out << editor->toPlainText();
    }

    // Save tab information
    QJsonObject tabInfo;
    tabInfo["path"] = tabManager->currentTabPath();
    tabInfo["content"] = editor->toPlainText();
    tabInfo["language"] = static_cast<int>(tabManager->currentTabLanguage());

    QFile infoFile(autoSaveDir.filePath("tabs.json"));
    if (infoFile.open(QIODevice::ReadWrite)) {
        QJsonDocument doc;
        if (infoFile.size() > 0) {
            doc = QJsonDocument::fromJson(infoFile.readAll());
        }
        
        QJsonArray tabs;
        if (doc.isArray()) {
            tabs = doc.array();
        }
        
        while (tabs.size() <= tabIndex) {
            tabs.append(QJsonObject());
        }
        tabs[tabIndex] = tabInfo;
        
        infoFile.resize(0);
        infoFile.write(QJsonDocument(tabs).toJson());
    }
}

void AutoSaveManager::loadTab(int tabIndex)
{
    QString autoSavePath = getAutoSavePath(tabIndex);
    QFile file(autoSavePath);
    if (file.exists() && file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        in.setCodec("UTF-8");
        if (QTextEdit *editor = tabManager->editorAt(tabIndex)) {
            editor->setPlainText(in.readAll());
        }
    }
}

void AutoSaveManager::clearAutoSaveFiles()
{
    autoSaveDir.removeRecursively();
    autoSaveDir.mkpath(".");
} 