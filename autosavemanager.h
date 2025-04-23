#ifndef AUTOSAVEMANAGER_H
#define AUTOSAVEMANAGER_H

#include <QObject>
#include <QTimer>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QSettings>
#include "tabmanager.h"

class AutoSaveManager : public QObject
{
    Q_OBJECT

public:
    explicit AutoSaveManager(TabManager *tabManager, QObject *parent = nullptr);
    ~AutoSaveManager();

    void setAutoSaveInterval(int minutes);
    int autoSaveInterval() const;
    void setAutoSaveEnabled(bool enabled);
    bool isAutoSaveEnabled() const;
    void saveAllTabs();
    void restoreTabs();

private slots:
    void onAutoSaveTimeout();
    void onTabClosed(int index);
    void onTextChanged();

private:
    TabManager *tabManager;
    QTimer *autoSaveTimer;
    QDir autoSaveDir;
    bool autoSaveEnabled;
    int saveInterval;

    void setupAutoSaveDir();
    QString getAutoSavePath(int tabIndex) const;
    void saveTab(int tabIndex);
    void loadTab(int tabIndex);
    void clearAutoSaveFiles();
    void startTimer();
};

#endif // AUTOSAVEMANAGER_H 