/**
 * @file mynotepad.h
 * @brief 主窗口类的头文件
 * 
 * 定义了记事本应用程序的主窗口类，包含所有用户界面元素和功能实现。
 */

#ifndef MYNOTEPAD_H
#define MYNOTEPAD_H

#include <QMainWindow>
#include <QTextEdit>
#include <QLabel>
#include <QTextCodec>
#include <QMap>
#include <QKeySequence>
#include "syntaxhighlighter.h"
#include "tabmanager.h"
#include "autosavemanager.h"

class FindDialog;
class ReplaceDialog;
class ShortcutDialog;

namespace Ui {
class MyNotepad;
}

/**
 * @class MyNotepad
 * @brief 记事本应用程序的主窗口类
 * 
 * 实现了记事本的所有主要功能，包括：
 * - 文件操作（新建、打开、保存等）
 * - 编辑操作（剪切、复制、粘贴等）
 * - 格式设置（字体、颜色等）
 * - 查找和替换
 * - 语法高亮
 * - 多标签页管理
 * - 自动保存
 * - 快捷键自定义
 */
class MyNotepad : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父窗口指针
     * 
     * 初始化主窗口，创建所有必要的组件和连接信号槽。
     */
    explicit MyNotepad(QWidget *parent = nullptr);

    /**
     * @brief 析构函数
     * 
     * 清理所有资源，保存设置。
     */
    ~MyNotepad();

protected:
    /**
     * @brief 窗口关闭事件处理
     * @param event 关闭事件对象
     * 
     * 在窗口关闭前检查是否需要保存文件。
     */
    void closeEvent(QCloseEvent *event) override;

private slots:
    // 文件操作
    /**
     * @brief 新建文件
     * 
     * 创建新的空白文档。
     */
    void newFile();

    /**
     * @brief 打开文件
     * 
     * 显示文件选择对话框并打开选中的文件。
     */
    void openFile();

    /**
     * @brief 保存文件
     * 
     * 保存当前文档到文件。
     */
    void saveFile();

    /**
     * @brief 另存为
     * 
     * 将当前文档保存到新文件。
     */
    void saveAsFile();

    /**
     * @brief 保存文件到指定路径
     * @param fileName 文件路径
     * @return 是否保存成功
     */
    bool saveFileAs(const QString &fileName);

    /**
     * @brief 打印文件
     * 
     * 显示打印对话框并打印当前文档。
     */
    void printFile();

    // 编辑操作
    /**
     * @brief 撤销
     * 
     * 撤销上一步操作。
     */
    void undo();

    /**
     * @brief 重做
     * 
     * 重做上一步撤销的操作。
     */
    void redo();

    /**
     * @brief 剪切
     * 
     * 剪切选中的文本到剪贴板。
     */
    void cut();

    /**
     * @brief 复制
     * 
     * 复制选中的文本到剪贴板。
     */
    void copy();

    /**
     * @brief 粘贴
     * 
     * 从剪贴板粘贴文本。
     */
    void paste();

    /**
     * @brief 全选
     * 
     * 选中所有文本。
     */
    void selectAll();

    // 格式操作
    /**
     * @brief 设置字体
     * 
     * 显示字体选择对话框并应用选中的字体。
     */
    void setFont();

    /**
     * @brief 设置字体颜色
     * 
     * 显示颜色选择对话框并应用选中的颜色。
     */
    void setFontColor();

    // 查找和替换
    /**
     * @brief 查找
     * 
     * 显示查找对话框。
     */
    void find();

    /**
     * @brief 替换
     * 
     * 显示替换对话框。
     */
    void replace();

    /**
     * @brief 查找下一个
     * 
     * 查找下一个匹配项。
     */
    void findNext();

    /**
     * @brief 查找上一个
     * 
     * 查找上一个匹配项。
     */
    void findPrevious();

    // 状态更新
    /**
     * @brief 更新光标位置
     * 
     * 在状态栏显示当前光标位置。
     */
    void updateCursorPosition();

    /**
     * @brief 文档修改状态更新
     * 
     * 处理文档修改状态的改变。
     */
    void documentModified();

    // 关于对话框
    /**
     * @brief 显示关于对话框
     * 
     * 显示应用程序的关于信息。
     */
    void about();

    // 语言选择
    /**
     * @brief 设置Java语法高亮
     * 
     * 为当前文档启用Java语法高亮。
     */
    void setLanguageJava();

    /**
     * @brief 设置C++语法高亮
     * 
     * 为当前文档启用C++语法高亮。
     */
    void setLanguageCpp();

    /**
     * @brief 设置Python语法高亮
     * 
     * 为当前文档启用Python语法高亮。
     */
    void setLanguagePython();

    /**
     * @brief 禁用语法高亮
     * 
     * 为当前文档禁用语法高亮。
     */
    void setLanguageNone();

    // 标签页管理
    /**
     * @brief 标签页改变处理
     * 
     * 处理当前标签页改变事件。
     */
    void onTabChanged();

    /**
     * @brief 文本改变处理
     * 
     * 处理文本内容改变事件。
     */
    void onTextChanged();

    // 自动保存设置
    /**
     * @brief 设置自动保存间隔
     * 
     * 显示对话框设置自动保存的时间间隔。
     */
    void setAutoSaveInterval();

    /**
     * @brief 切换自动保存
     * 
     * 启用或禁用自动保存功能。
     */
    void toggleAutoSave();

    // 快捷键设置
    /**
     * @brief 显示快捷键设置
     * 
     * 显示快捷键设置对话框。
     */
    void showShortcutSettings();

private:
    Ui::MyNotepad *ui;                    ///< UI对象
    TabManager *tabManager;               ///< 标签页管理器
    AutoSaveManager *autoSaveManager;     ///< 自动保存管理器
    QLabel *statusLabel;                  ///< 状态栏标签
    QLabel *encodingLabel;                ///< 编码标签
    FindDialog *findDialog;               ///< 查找对话框
    ReplaceDialog *replaceDialog;         ///< 替换对话框
    ShortcutDialog *shortcutDialog;       ///< 快捷键设置对话框
    QMap<QString, QKeySequence> shortcuts;///< 快捷键映射
    QMap<QString, QAction*> actionMap;    ///< 菜单动作映射
    
    /**
     * @brief 创建菜单
     * 
     * 创建主窗口的所有菜单和动作。
     */
    void createMenus();

    /**
     * @brief 创建工具栏
     * 
     * 创建主窗口的工具栏。
     */
    void createToolBars();

    /**
     * @brief 创建状态栏
     * 
     * 创建主窗口的状态栏。
     */
    void createStatusBar();

    /**
     * @brief 设置信号连接
     * 
     * 连接所有必要的信号和槽。
     */
    void setupConnections();

    /**
     * @brief 加载文件
     * @param fileName 文件路径
     * 
     * 从文件加载内容到当前标签页。
     */
    void loadFile(const QString &fileName);

    /**
     * @brief 设置当前文件
     * @param fileName 文件路径
     * 
     * 更新当前标签页的文件路径。
     */
    void setCurrentFile(const QString &fileName);

    /**
     * @brief 获取文件名
     * @param fullFileName 完整文件路径
     * @return 文件名
     * 
     * 从完整路径中提取文件名。
     */
    QString strippedName(const QString &fullFileName);

    /**
     * @brief 更新窗口标题
     * 
     * 根据当前文件更新窗口标题。
     */
    void updateWindowTitle();

    /**
     * @brief 检查是否需要保存
     * @return 是否继续操作
     * 
     * 检查当前文档是否需要保存。
     */
    bool maybeSave();

    /**
     * @brief 更新语言菜单
     * 
     * 更新语言菜单的选中状态。
     */
    void updateLanguageMenu();

    /**
     * @brief 加载快捷键设置
     * 
     * 从设置文件加载快捷键配置。
     */
    void loadShortcuts();

    /**
     * @brief 保存快捷键设置
     * 
     * 将快捷键配置保存到设置文件。
     */
    void saveShortcuts();

    /**
     * @brief 应用快捷键设置
     * 
     * 将快捷键配置应用到菜单动作。
     */
    void applyShortcuts();
};

#endif // MYNOTEPAD_H
