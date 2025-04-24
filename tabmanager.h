/**
 * @file tabmanager.h
 * @brief 标签页管理器的头文件
 * 
 * 定义了多标签页编辑器的标签页管理类，负责管理多个编辑标签页。
 */

#ifndef TABMANAGER_H
#define TABMANAGER_H

#include <QTabWidget>
#include <QTextEdit>
#include <QString>
#include "syntaxhighlighter.h"

/**
 * @struct TabInfo
 * @brief 标签页信息结构
 * 
 * 存储每个标签页的相关信息，包括编辑器、文件路径、语法高亮器等。
 */
struct TabInfo {
    QTextEdit *editor;                    ///< 文本编辑器
    QString path;                         ///< 文件路径
    SyntaxHighlighter *highlighter;       ///< 语法高亮器
    SyntaxHighlighter::Language language; ///< 当前语言
};

/**
 * @class TabManager
 * @brief 标签页管理类
 * 
 * 管理多个编辑标签页，提供标签页的创建、切换、关闭等功能。
 * 支持文件路径管理、语法高亮设置等特性。
 */
class TabManager : public QTabWidget
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父窗口指针
     * 
     * 初始化标签页管理器，设置标签页可关闭和可移动。
     */
    explicit TabManager(QWidget *parent = nullptr);

    /**
     * @brief 析构函数
     * 
     * 清理所有标签页资源。
     */
    ~TabManager();

    /**
     * @brief 获取当前编辑器
     * @return 当前标签页的编辑器指针
     */
    QTextEdit* currentEditor() const;

    /**
     * @brief 获取指定索引的编辑器
     * @param index 标签页索引
     * @return 对应标签页的编辑器指针
     */
    QTextEdit* editorAt(int index) const;

    /**
     * @brief 添加新标签页
     * @param title 标签页标题
     * @return 新标签页的索引
     */
    int addNewTab(const QString &title = QString());

    /**
     * @brief 关闭标签页
     * @param index 要关闭的标签页索引
     */
    void closeTab(int index);

    /**
     * @brief 设置当前标签页标题
     * @param title 新的标题
     */
    void setCurrentTabTitle(const QString &title);

    /**
     * @brief 获取当前标签页标题
     * @return 当前标签页的标题
     */
    QString currentTabTitle() const;

    /**
     * @brief 设置当前标签页的修改状态
     * @param modified 是否已修改
     */
    void setCurrentTabModified(bool modified);

    /**
     * @brief 检查当前标签页是否已修改
     * @return 如果已修改返回true，否则返回false
     */
    bool isCurrentTabModified() const;

    /**
     * @brief 设置当前标签页的文件路径
     * @param path 文件路径
     */
    void setCurrentTabPath(const QString &path);

    /**
     * @brief 获取当前标签页的文件路径
     * @return 当前标签页的文件路径
     */
    QString currentTabPath() const;

    /**
     * @brief 设置当前标签页的编程语言
     * @param language 编程语言类型
     */
    void setCurrentTabLanguage(SyntaxHighlighter::Language language);

    /**
     * @brief 获取当前标签页的编程语言
     * @return 当前标签页的编程语言类型
     */
    SyntaxHighlighter::Language currentTabLanguage() const;

signals:
    /**
     * @brief 当前标签页改变信号
     * 
     * 当当前标签页改变时发出。
     */
    void currentTabChanged();

    /**
     * @brief 文本改变信号
     * 
     * 当文本内容改变时发出。
     */
    void textChanged();

    /**
     * @brief 标签页关闭信号
     * @param index 被关闭的标签页索引
     * 
     * 当标签页被关闭时发出。
     */
    void tabClosed(int index);

private slots:
    /**
     * @brief 处理标签页关闭请求
     * @param index 要关闭的标签页索引
     */
    void onTabCloseRequested(int index);

    /**
     * @brief 处理当前标签页改变事件
     * @param index 新的当前标签页索引
     */
    void onCurrentChanged(int index);

    /**
     * @brief 处理文本改变事件
     * 
     * 更新标签页标题并发出文本改变信号。
     */
    void onTextChanged();

private:
    QList<TabInfo> tabs;  ///< 标签页列表

    /**
     * @brief 设置标签页
     * @param index 标签页索引
     * 
     * 连接编辑器的修改状态改变信号。
     */
    void setupTab(int index);

    /**
     * @brief 更新标签页标题
     * @param index 标签页索引
     * 
     * 根据文件路径和修改状态更新标签页标题。
     */
    void updateTabTitle(int index);
};

#endif // TABMANAGER_H 