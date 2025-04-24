# 基于QT实现的记事本应用程序实验报告

## 1. 项目概述

本项目是一个基于Qt框架开发的现代化记事本应用程序，具有丰富的文本编辑功能和用户友好的界面。该应用程序不仅支持基本的文本编辑功能，还包含了**语法高亮**、**多标签页管理**、**自动保存**、**快捷键自定义**等高级特性。

## 2. 核心功能模块

### 2.1 项目架构图

#### 项目结构
```mermaid
graph TD
    A[MyNotepad] --> B[UI模块]
    A --> C[业务逻辑模块（编辑）]
    A --> D[文件操作模块（保存）]
    A --> E[全局异常处理]

    B --> B1[mynotepad]
    B --> B2[shortcutdialog]

    C --> C1[TabManager]
    C --> C2[AutoSaveManager]
    C --> C3[SyntaxHighlighter]

    D --> D1[FindDialog]
    D --> D2[ReplaceDialog]
    D --> D3[ShortcutDialog]

    E --> E1[main.cpp]
```

#### 类关系图
```mermaid
classDiagram
    class MyNotepad {
        -Ui::MyNotepad *ui
        -TabManager *tabManager
        -AutoSaveManager *autoSaveManager
        -SyntaxHighlighter *highlighter
        +newFile()
        +openFile()
        +saveFile()
        +find()
        +replace()
    }

    class TabManager {
        -QList<QTextEdit*> tabs
        +addTab()
        +removeTab()
        +switchTab()
    }

    class AutoSaveManager {
        -QTimer *timer
        +startAutoSave()
        +stopAutoSave()
        +setInterval()
    }

    class SyntaxHighlighter {
        -QMap<QString, QTextCharFormat> formats
        +setLanguage()
        +highlightBlock()
    }

    class FindDialog {
        +findNext()
        +findPrevious()
    }

    class ReplaceDialog {
        +replace()
        +replaceAll()
    }

    class ShortcutDialog {
        +loadShortcuts()
        +saveShortcuts()
    }

    MyNotepad --> TabManager
    MyNotepad --> AutoSaveManager
    MyNotepad --> SyntaxHighlighter
    MyNotepad --> FindDialog
    MyNotepad --> ReplaceDialog
    MyNotepad --> ShortcutDialog
```

### 2.2 主窗口模块 (MyNotepad)
主窗口类是整个应用程序的核心，负责协调各个功能模块的工作。主要功能包括：

- 文件操作：新建、打开、保存、另存为等
- 编辑操作：撤销、重做、剪切、复制、粘贴等
- 格式设置：字体、颜色等
- 查找和替换功能
- 多标签页管理
- 自动保存功能
- 快捷键自定义

### 2.3 标签页管理模块 (TabManager)
负责管理多个文本编辑标签页，主要功能：
- 创建新标签页
- 切换标签页
- 关闭标签页
- 保存标签页状态

### 2.4 语法高亮模块 (SyntaxHighlighter)
支持多种编程语言的语法高亮（目前支持`Java`、`C++`和`Python`）。
### 2.5 自动保存模块 (AutoSaveManager)
提供自动保存功能。
- 可配置自动保存间隔
- 自动保存文件到临时目录
- 程序启动时恢复未保存的内容

## 3. 技术实现要点

### 3.1 用户界面设计
```cpp
void MyNotepad::createMenus()
{
    // 文件菜单
    QMenu *fileMenu = menuBar()->addMenu(tr("文件(&F)"));
    QAction *newAction = fileMenu->addAction(tr("新建(&N)"), this, &MyNotepad::newFile);
    // ... 其他菜单项
    // 存储所有动作的映射
    actionMap["新建"] = newAction;
    // ... 其他操作项
}
```
- 使用Qt的菜单系统创建标准菜单栏
- 支持快捷键操作，将操作映射到`actionMap`上实现快捷键编辑与菜单动作解耦
- 提供工具栏快速访问常用功能

### 3.2 文件操作实现
```cpp
void MyNotepad::openFile()
{
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this,
            tr("打开文件"), QString(),
            tr("文本文件 (*.txt);;所有文件 (*)"));
        // ... 文件打开逻辑
    }
}
```
- 使用Qt的文件对话框进行文件选择
- 支持多种文件格式
- 实现文件编码自动识别

### 3.3 多标签页管理
```cpp
void MyNotepad::setupConnections()
{
    connect(tabManager, &TabManager::currentTabChanged, this, &MyNotepad::onTabChanged);
    connect(tabManager, &TabManager::textChanged, this, &MyNotepad::onTextChanged);
}
```
- 使用信号槽机制实现标签页状态同步
- 支持标签页之间的切换
- 维护每个标签页的独立状态

### 3.4 自动保存机制
```cpp
void MyNotepad::toggleAutoSave()
{
    autoSaveManager->setAutoSaveEnabled(!autoSaveManager->isAutoSaveEnabled());
}
```
- 使用定时器实现定期保存
- 保存到临时文件
- 程序启动时自动恢复

### 3.4 全局异常捕获

```cpp
void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(context);  // 标记未使用的参数

    QString txt;
    switch (type) {
    case QtDebugMsg:
        txt = QString("Debug: %1").arg(msg);
        break;
    case QtInfoMsg:
        txt = QString("Info: %1").arg(msg);
        break;
    case QtWarningMsg:
        txt = QString("Warning: %1").arg(msg);
        break;
    case QtCriticalMsg:
        txt = QString("Critical: %1").arg(msg);
        break;
    case QtFatalMsg:
        txt = QString("Fatal: %1").arg(msg);
        break;
    }

    // 创建日志目录
    QDir dir;
    if (!dir.exists("logs")) {
        dir.mkdir("logs");
    }

    // 写入日志文件
    QFile outFile("logs/crash.log");
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << " - " << txt << "\n";
    outFile.close();

    // 如果是致命错误，显示错误对话框
    if (type == QtFatalMsg) {
        QMessageBox::critical(nullptr, "程序错误",
            QString("程序发生严重错误：\n%1\n\n"
                   "错误已记录到日志文件：logs/crash.log\n"
                   "请将日志文件发送给开发人员以帮助解决问题。").arg(msg));
    }
}
```

- 注册全局异常处理函数并通过消息处理器将异常处理函数绑定到程序。

## 4. 项目特色

1. **模块化设计**：各个功能模块独立封装，便于维护和扩展
2. **用户友好**：提供丰富的快捷键和工具栏
3. **智能保存**：自动保存功能防止意外丢失
4. **多语言支持**：支持多种编程语言的语法高亮
5. **可定制性**：支持自定义快捷键和界面布局

## 5. 技术难点及解决方案

1. **文件编码处理**
   - 使用QTextCodec自动识别文件编码
   - 支持多种编码格式的读写

2. **多标签页管理**
   - 使用TabManager类统一管理标签页
   - 实现标签页状态的保存和恢复

3. **自动保存机制**
   - 使用定时器实现定期保存
   - 采用临时文件存储机制

4. **语法高亮实现**
   - 使用正则表达式匹配语法规则
   - 支持多种编程语言的语法规则

## 6. 运行效果图

![image-20250424103502386](assets/image-20250424103502386.png)

<center>初始化窗口</center>

![image-20250424103833275](assets/image-20250424103833275.png)

<center>多标签页编辑</center>

![image-20250424104454254](assets/image-20250424104454254.png)

![image-20250424104402567](assets/image-20250424104402567.png)

![image-20250424104256844](assets/image-20250424104256844.png)

<center>字体格式设置</center>

![image-20250424104731112](assets/image-20250424104731112.png)

![image-20250424104829209](assets/image-20250424104829209.png)

![image-20250424104837437](assets/image-20250424104837437.png)

<center>新建文件、保存</center>

![image-20250424110700066](assets/image-20250424110700066.png)

![image-20250424110716905](assets/image-20250424110716905.png)

![image-20250424110825189](assets/image-20250424110825189.png)

![image-20250424110904541](assets/image-20250424110904541.png)

![image-20250424110920616](assets/image-20250424110920616.png)

![image-20250424110932394](assets/image-20250424110932394.png)

![image-20250424110951275](assets/image-20250424110951275.png)

![image-20250424111001043](assets/image-20250424111001043.png)

<center>常用编辑操作</center>

![image-20250424104947866](assets/image-20250424104947866.png)

![image-20250424105739702](assets/image-20250424105739702.png)

![image-20250424105753427](assets/image-20250424105753427.png)

<center>代码高亮</center>

![image-20250424110208194](assets/image-20250424110208194.png)

![image-20250424110302819](assets/image-20250424110302819.png)

![image-20250424110311559](assets/image-20250424110311559.png)

<center>查找&替换</center>

![image-20250424110412801](assets/image-20250424110412801.png)

<center>自动保存</center>

![image-20250424110452901](assets/image-20250424110452901.png)

<center>快捷键自定义</center>

![image-20250424110531220](assets/image-20250424110531220.png)

<center>About窗口</center>

![image-20250424111053084](assets/image-20250424111053084.png)

<center>警告弹窗</center>

## 7. 未来改进方向

1. 添加更多编程语言的语法高亮支持
2. 实现文件比较功能
3. 添加代码折叠功能
4. 支持插件系统
5. 优化大文件处理性能

## 8. 总结

本项目实现了一个功能完善的现代化记事本应用程序，通过模块化设计和Qt框架的强大功能，实现了丰富的文本编辑功能。项目代码结构清晰，易于维护和扩展，为用户提供了良好的使用体验。 