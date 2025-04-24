/**
 * @file autosavemanager.h
 * @brief 自动保存管理器的头文件
 * 
 * 定义了文本编辑器的自动保存功能，支持定期保存和恢复文档。
 */

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

/**
 * @class AutoSaveManager
 * @brief 自动保存管理器类
 * 
 * 管理文本编辑器的自动保存功能，包括：
 * - 定期保存所有打开的标签页
 * - 在程序启动时恢复上次保存的内容
 * - 支持启用/禁用自动保存
 * - 支持自定义保存间隔
 */
class AutoSaveManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param tabManager 标签页管理器指针
     * @param parent 父对象指针
     * 
     * 初始化自动保存管理器，设置默认保存间隔。
     */
    explicit AutoSaveManager(TabManager *tabManager, QObject *parent = nullptr);

    /**
     * @brief 析构函数
     * 
     * 清理自动保存文件。
     */
    ~AutoSaveManager();

    /**
     * @brief 设置自动保存间隔
     * @param minutes 保存间隔（分钟）
     * 
     * 设置自动保存的时间间隔。
     */
    void setAutoSaveInterval(int minutes);

    /**
     * @brief 获取自动保存间隔
     * @return 当前保存间隔（分钟）
     */
    int autoSaveInterval() const;

    /**
     * @brief 设置自动保存启用状态
     * @param enabled 是否启用
     * 
     * 启用或禁用自动保存功能。
     */
    void setAutoSaveEnabled(bool enabled);

    /**
     * @brief 检查自动保存是否启用
     * @return 如果启用返回true，否则返回false
     */
    bool isAutoSaveEnabled() const;

    /**
     * @brief 保存所有标签页
     * 
     * 将当前所有打开的标签页保存到自动保存文件。
     */
    void saveAllTabs();

    /**
     * @brief 恢复标签页
     * 
     * 从自动保存文件恢复上次保存的标签页。
     */
    void restoreTabs();

private slots:
    /**
     * @brief 处理自动保存超时
     * 
     * 当自动保存定时器超时时触发保存操作。
     */
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