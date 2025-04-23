#include "tabmanager.h"
#include <QTextDocument>
#include <QFileInfo>
#include <QMessageBox>

/**
 * @brief TabManager构造函数
 * @param parent 父窗口部件
 * 
 * 初始化标签页管理器，设置标签页可关闭、可移动，并连接相关信号
 */
TabManager::TabManager(QWidget *parent)
    : QTabWidget(parent)
{
    setTabsClosable(true);    // 允许关闭标签页
    setMovable(true);         // 允许移动标签页
    setDocumentMode(true);    // 设置文档模式

    // 连接标签页关闭和切换信号
    connect(this, &QTabWidget::tabCloseRequested, this, &TabManager::onTabCloseRequested);
    connect(this, &QTabWidget::currentChanged, this, &TabManager::onCurrentChanged);
}

/**
 * @brief TabManager析构函数
 * 
 * 清理所有标签页的资源，包括编辑器和语法高亮器
 */
TabManager::~TabManager()
{
    // 断开所有信号连接
    for (const TabInfo &tab : tabs) {
        if (tab.editor) {
            tab.editor->document()->disconnect(this);
        }
    }
    
    // 清理资源
    for (const TabInfo &tab : tabs) {
        if (tab.highlighter) {
            delete tab.highlighter;
        }
        if (tab.editor) {
            delete tab.editor;
        }
    }
    tabs.clear();
}

/**
 * @brief 获取当前标签页的编辑器
 * @return 当前标签页的QTextEdit指针
 */
QTextEdit* TabManager::currentEditor() const
{
    return editorAt(currentIndex());
}

/**
 * @brief 获取指定索引的编辑器
 * @param index 标签页索引
 * @return 对应标签页的QTextEdit指针，如果索引无效则返回nullptr
 */
QTextEdit* TabManager::editorAt(int index) const
{
    if (index >= 0 && index < tabs.size()) {
        return tabs[index].editor;
    }
    return nullptr;
}

/**
 * @brief 添加新标签页
 * @param title 标签页标题
 * @return 新标签页的索引
 * 
 * 创建新的编辑器、语法高亮器，并添加到标签页中
 */
int TabManager::addNewTab(const QString &title)
{
    QTextEdit *editor = new QTextEdit(this);
    SyntaxHighlighter *highlighter = new SyntaxHighlighter(editor->document());
    
    TabInfo tab;
    tab.editor = editor;
    tab.path = QString();
    tab.highlighter = highlighter;
    tab.language = SyntaxHighlighter::None;
    
    int index = tabs.size();
    tabs.append(tab);
    
    addTab(editor, title);
    setupTab(index);
    
    return index;
}

/**
 * @brief 关闭指定标签页
 * @param index 要关闭的标签页索引
 * 
 * 如果文档已修改，会提示用户是否保存
 */
void TabManager::closeTab(int index)
{
    if (index >= 0 && index < tabs.size()) {
        QTextEdit *editor = tabs[index].editor;
        if (editor->document()->isModified()) {
            QMessageBox::StandardButton ret = QMessageBox::warning(this, tr("MyNotepad"),
                tr("The document has been modified.\n"
                   "Do you want to save your changes?"),
                QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
                   
            if (ret == QMessageBox::Save) {
                // TODO: Implement save functionality
                return;
            } else if (ret == QMessageBox::Cancel) {
                return;
            }
        }
        
        // 先断开所有信号连接
        if (editor) {
            editor->document()->disconnect(this);
        }
        
        // 保存要删除的编辑器和高亮器的指针
        QTextEdit *editorToDelete = tabs[index].editor;
        SyntaxHighlighter *highlighterToDelete = tabs[index].highlighter;
        
        // 从列表中移除
        tabs.removeAt(index);
        
        // 移除标签页
        removeTab(index);
        
        // 最后删除资源
        if (highlighterToDelete) {
            delete highlighterToDelete;
        }
        if (editorToDelete) {
            delete editorToDelete;
        }
        
        emit tabClosed(index);
    }
}

/**
 * @brief 设置当前标签页标题
 * @param title 新的标题
 */
void TabManager::setCurrentTabTitle(const QString &title)
{
    setTabText(currentIndex(), title);
}

/**
 * @brief 获取当前标签页标题
 * @return 当前标签页的标题
 */
QString TabManager::currentTabTitle() const
{
    return tabText(currentIndex());
}

/**
 * @brief 设置当前标签页的修改状态
 * @param modified 是否已修改
 */
void TabManager::setCurrentTabModified(bool modified)
{
    if (QTextEdit *editor = currentEditor()) {
        editor->document()->setModified(modified);
        updateTabTitle(currentIndex());
    }
}

/**
 * @brief 检查当前标签页是否已修改
 * @return 如果已修改返回true，否则返回false
 */
bool TabManager::isCurrentTabModified() const
{
    if (QTextEdit *editor = currentEditor()) {
        return editor->document()->isModified();
    }
    return false;
}

/**
 * @brief 设置当前标签页的文件路径
 * @param path 文件路径
 */
void TabManager::setCurrentTabPath(const QString &path)
{
    int index = currentIndex();
    if (index >= 0 && index < tabs.size()) {
        tabs[index].path = path;
        updateTabTitle(index);
    }
}

/**
 * @brief 获取当前标签页的文件路径
 * @return 当前标签页的文件路径
 */
QString TabManager::currentTabPath() const
{
    int index = currentIndex();
    if (index >= 0 && index < tabs.size()) {
        return tabs[index].path;
    }
    return QString();
}

/**
 * @brief 设置当前标签页的编程语言
 * @param language 编程语言类型
 */
void TabManager::setCurrentTabLanguage(SyntaxHighlighter::Language language)
{
    int index = currentIndex();
    if (index >= 0 && index < tabs.size()) {
        tabs[index].language = language;
        tabs[index].highlighter->setLanguage(language);
    }
}

/**
 * @brief 获取当前标签页的编程语言
 * @return 当前标签页的编程语言类型
 */
SyntaxHighlighter::Language TabManager::currentTabLanguage() const
{
    int index = currentIndex();
    if (index >= 0 && index < tabs.size()) {
        return tabs[index].language;
    }
    return SyntaxHighlighter::None;
}

/**
 * @brief 处理标签页关闭请求
 * @param index 要关闭的标签页索引
 */
void TabManager::onTabCloseRequested(int index)
{
    // 确保索引有效
    if (index >= 0 && index < count()) {
        closeTab(index);
    }
}

/**
 * @brief 处理当前标签页改变事件
 * @param index 新的当前标签页索引
 */
void TabManager::onCurrentChanged(int index)
{
    if (index >= 0 && index < tabs.size()) {
        emit currentTabChanged();
    }
}

/**
 * @brief 处理文本改变事件
 * 
 * 更新标签页标题并发出文本改变信号
 */
void TabManager::onTextChanged()
{
    updateTabTitle(currentIndex());
    emit textChanged();
}

/**
 * @brief 设置标签页
 * @param index 标签页索引
 * 
 * 连接编辑器的修改状态改变信号
 */
void TabManager::setupTab(int index)
{
    if (index >= 0 && index < tabs.size()) {
        QTextEdit *editor = tabs[index].editor;
        connect(editor->document(), &QTextDocument::modificationChanged,
                this, &TabManager::onTextChanged);
    }
}

/**
 * @brief 更新标签页标题
 * @param index 标签页索引
 * 
 * 根据文件路径和修改状态更新标签页标题
 */
void TabManager::updateTabTitle(int index)
{
    if (index >= 0 && index < tabs.size()) {
        QString title = tabs[index].path.isEmpty() ? tr("Untitled") : QFileInfo(tabs[index].path).fileName();
        if (tabs[index].editor->document()->isModified()) {
            title += " *";
        }
        setTabText(index, title);
    }
} 