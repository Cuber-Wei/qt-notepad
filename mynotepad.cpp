#include "MyNotepad.h"
#include "ui_MyNotepad.h"
#include "finddialog.h"
#include "replacedialog.h"
#include <QMessageBox>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QFont>
#include <QFontDialog>
#include <QColorDialog>
#include <QTextCursor>
#include <QTextCodec>
#include <QMenu>
#include <QToolBar>
#include <QStatusBar>
#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QTextDocument>
#include <QTextBlock>
#include <QTextCharFormat>
#include <QTextDocumentWriter>
#include <QTextCodec>
#include <QSettings>
#include <QCloseEvent>
#include <QInputDialog>

#if defined(QT_PRINTSUPPORT_LIB)
    #include <QtPrintSupport/qtprintsupportglobal.h>
    #if QT_CONFIG(printer)
        #if QT_CONFIG(printdialog)
            #include <QPrintDialog>
        #endif
        #include <QPrinter>
    #endif
#endif

MyNotepad::MyNotepad(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MyNotepad),
    findDialog(nullptr),
    replaceDialog(nullptr)
{
    ui->setupUi(this);
    
    // Create tab manager
    tabManager = new TabManager(this);
    setCentralWidget(tabManager);
    
    // Create auto save manager
    autoSaveManager = new AutoSaveManager(tabManager, this);
    
    // Create UI elements
    createMenus();
    createToolBars();
    createStatusBar();
    setupConnections();
    
    // Set window properties
    setWindowTitle(tr("Untitled - MyNotepad"));
    resize(800, 600);
    
    // Load settings
    QSettings settings;
    QFont font = settings.value("font", QFont("Consolas", 10)).value<QFont>();
    
    // 不再创建初始标签页
    // 当用户第一次打开文件或新建文件时会自动创建标签页

    // Restore auto-saved tabs
    autoSaveManager->restoreTabs();
}

MyNotepad::~MyNotepad()
{
    delete ui;
    delete findDialog;
    delete replaceDialog;
}

void MyNotepad::createMenus()
{
    // 文件菜单
    QMenu *fileMenu = menuBar()->addMenu(tr("文件(&F)"));
    fileMenu->addAction(tr("新建(&N)"), this, &MyNotepad::newFile, QKeySequence::New);
    fileMenu->addAction(tr("打开(&O)..."), this, &MyNotepad::openFile, QKeySequence::Open);
    fileMenu->addAction(tr("保存(&S)"), this, &MyNotepad::saveFile, QKeySequence::Save);
    fileMenu->addAction(tr("另存为(&A)..."), this, &MyNotepad::saveAsFile, QKeySequence::SaveAs);
    fileMenu->addSeparator();
    fileMenu->addAction(tr("退出(&X)"), this, &QWidget::close, QKeySequence::Quit);

    // 编辑菜单
    QMenu *editMenu = menuBar()->addMenu(tr("编辑(&E)"));
    editMenu->addAction(tr("撤销(&U)"), this, &MyNotepad::undo, QKeySequence::Undo);
    editMenu->addAction(tr("重做(&R)"), this, &MyNotepad::redo, QKeySequence::Redo);
    editMenu->addSeparator();
    editMenu->addAction(tr("剪切(&T)"), this, &MyNotepad::cut, QKeySequence::Cut);
    editMenu->addAction(tr("复制(&C)"), this, &MyNotepad::copy, QKeySequence::Copy);
    editMenu->addAction(tr("粘贴(&P)"), this, &MyNotepad::paste, QKeySequence::Paste);
    editMenu->addAction(tr("全选(&A)"), this, &MyNotepad::selectAll, QKeySequence::SelectAll);

    // 格式菜单
    QMenu *formatMenu = menuBar()->addMenu(tr("格式(&O)"));
    formatMenu->addAction(tr("字体(&F)..."), this, &MyNotepad::setFont);
    formatMenu->addAction(tr("字体颜色(&C)..."), this, &MyNotepad::setFontColor);

    // 语言菜单
    QMenu *languageMenu = menuBar()->addMenu(tr("语言(&L)"));
    languageMenu->addAction(tr("无(&N)"), this, &MyNotepad::setLanguageNone);
    languageMenu->addAction(tr("Java(&J)"), this, &MyNotepad::setLanguageJava);
    languageMenu->addAction(tr("C++(&C)"), this, &MyNotepad::setLanguageCpp);
    languageMenu->addAction(tr("Python(&P)"), this, &MyNotepad::setLanguagePython);

    // 查找菜单
    QMenu *findMenu = menuBar()->addMenu(tr("查找(&S)"));
    findMenu->addAction(tr("查找(&F)..."), this, &MyNotepad::find, QKeySequence::Find);
    findMenu->addAction(tr("查找下一个(&N)"), this, &MyNotepad::findNext, QKeySequence::FindNext);
    findMenu->addAction(tr("查找上一个(&P)"), this, &MyNotepad::findPrevious, QKeySequence::FindPrevious);
    findMenu->addAction(tr("替换(&R)..."), this, &MyNotepad::replace, QKeySequence::Replace);

    // 自动保存菜单
    QMenu *autoSaveMenu = menuBar()->addMenu(tr("自动保存(&A)"));
    QAction *enableAutoSaveAction = autoSaveMenu->addAction(tr("启用自动保存(&E)"), this, &MyNotepad::toggleAutoSave);
    enableAutoSaveAction->setCheckable(true);
    enableAutoSaveAction->setChecked(autoSaveManager->isAutoSaveEnabled());
    autoSaveMenu->addAction(tr("设置间隔(&I)..."), this, &MyNotepad::setAutoSaveInterval);

    // 帮助菜单
    QMenu *helpMenu = menuBar()->addMenu(tr("帮助(&H)"));
    helpMenu->addAction(tr("关于(&A)"), this, &MyNotepad::about);
}

void MyNotepad::createToolBars()
{
    QToolBar *fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(tr("New"), this, &MyNotepad::newFile);
    fileToolBar->addAction(tr("Open"), this, &MyNotepad::openFile);
    fileToolBar->addAction(tr("Save"), this, &MyNotepad::saveFile);

    QToolBar *editToolBar = addToolBar(tr("Edit"));
    editToolBar->addAction(tr("Cut"), this, &MyNotepad::cut);
    editToolBar->addAction(tr("Copy"), this, &MyNotepad::copy);
    editToolBar->addAction(tr("Paste"), this, &MyNotepad::paste);
}

void MyNotepad::createStatusBar()
{
    statusLabel = new QLabel(tr("Ln 1, Col 1"));
    statusBar()->addWidget(statusLabel);
    
    encodingLabel = new QLabel(tr("UTF-8"));
    statusBar()->addPermanentWidget(encodingLabel);
}

void MyNotepad::setupConnections()
{
    connect(tabManager, &TabManager::currentTabChanged, this, &MyNotepad::onTabChanged);
    connect(tabManager, &TabManager::textChanged, this, &MyNotepad::onTextChanged);
}

void MyNotepad::newFile()
{
    if (maybeSave()) {
        tabManager->addNewTab();
    }
}

void MyNotepad::openFile()
{
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this,
            tr("打开文件"), QString(),
            tr("文本文件 (*.txt);;所有文件 (*)"));
            
        if (!fileName.isEmpty()) {
            // 创建新标签页并加载文件
            int index = tabManager->addNewTab();
            tabManager->setCurrentIndex(index);
            loadFile(fileName);
        }
    }
}

void MyNotepad::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("MyNotepad"),
            tr("无法读取文件 %1:\n%2.")
            .arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return;
    }

    // 设置文件编码
    QTextStream in(&file);
    QTextCodec *codec = QTextCodec::codecForUtfText(file.peek(1024), QTextCodec::codecForName("UTF-8"));
    in.setCodec(codec);

    // 使用当前标签页，而不是创建新标签页
    if (QTextEdit *editor = tabManager->currentEditor()) {
        editor->setPlainText(in.readAll());
        tabManager->setCurrentTabPath(fileName);
        tabManager->setCurrentTabTitle(QFileInfo(fileName).fileName());
        tabManager->setCurrentTabModified(false);
        encodingLabel->setText(codec->name());
    }
}

void MyNotepad::saveFile()
{
    QString path = tabManager->currentTabPath();
    if (path.isEmpty()) {
        saveAsFile();
    } else {
        saveFileAs(path);
    }
}

void MyNotepad::saveAsFile()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("另存为"), QString(),
        tr("文本文件 (*.txt);;所有文件 (*)"));
        
    if (!fileName.isEmpty()) {
        saveFileAs(fileName);
    }
}

bool MyNotepad::saveFileAs(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("MyNotepad"),
            tr("无法写入文件 %1:\n%2.")
            .arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return false;
    }

    QTextStream out(&file);
    out.setCodec("UTF-8");
    if (QTextEdit *editor = tabManager->currentEditor()) {
        out << editor->toPlainText();
    }
    
    tabManager->setCurrentTabPath(fileName);
    tabManager->setCurrentTabModified(false);
    return true;
}

void MyNotepad::setCurrentFile(const QString &fileName)
{
    tabManager->setCurrentTabPath(fileName);
    updateWindowTitle();
}

void MyNotepad::updateWindowTitle()
{
    QString title = tabManager->currentTabTitle();
    if (tabManager->isCurrentTabModified()) {
        title += " *";
    }
    setWindowTitle(tr("%1 - MyNotepad").arg(title));
}

QString MyNotepad::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

bool MyNotepad::maybeSave()
{
    if (tabManager->isCurrentTabModified()) {
        QMessageBox::StandardButton ret = QMessageBox::warning(this, tr("MyNotepad"),
            tr("文档已被修改。\n"
               "是否保存更改？"),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
               
        if (ret == QMessageBox::Save) {
            QString path = tabManager->currentTabPath();
            if (path.isEmpty()) {
                QString fileName = QFileDialog::getSaveFileName(this,
                    tr("另存为"), QString(),
                    tr("文本文件 (*.txt);;所有文件 (*)"));
                    
                if (!fileName.isEmpty()) {
                    return saveFileAs(fileName);
                }
                return false;
            } else {
                return saveFileAs(path);
            }
        } else if (ret == QMessageBox::Cancel) {
            return false;
        }
    }
    return true;
}

void MyNotepad::documentModified()
{
    updateWindowTitle();
}

void MyNotepad::updateCursorPosition()
{
    if (QTextEdit *editor = tabManager->currentEditor()) {
        QTextCursor cursor = editor->textCursor();
        statusLabel->setText(tr("Ln %1, Col %2")
            .arg(cursor.blockNumber() + 1)
            .arg(cursor.columnNumber() + 1));
    }
}

void MyNotepad::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}

// Edit operations
void MyNotepad::undo()
{
    if (QTextEdit *editor = tabManager->currentEditor()) {
        editor->undo();
    }
}

void MyNotepad::redo()
{
    if (QTextEdit *editor = tabManager->currentEditor()) {
        editor->redo();
    }
}

void MyNotepad::cut()
{
    if (QTextEdit *editor = tabManager->currentEditor()) {
        editor->cut();
    }
}

void MyNotepad::copy()
{
    if (QTextEdit *editor = tabManager->currentEditor()) {
        editor->copy();
    }
}

void MyNotepad::paste()
{
    if (QTextEdit *editor = tabManager->currentEditor()) {
        editor->paste();
    }
}

void MyNotepad::selectAll()
{
    if (QTextEdit *editor = tabManager->currentEditor()) {
        editor->selectAll();
    }
}

// Format operations
void MyNotepad::setFont()
{
    if (QTextEdit *editor = tabManager->currentEditor()) {
        bool ok;
        QFont font = QFontDialog::getFont(&ok, editor->font(), this);
        if (ok) {
            editor->setFont(font);
            QSettings settings;
            settings.setValue("font", font);
        }
    }
}

void MyNotepad::setFontColor()
{
    if (QTextEdit *editor = tabManager->currentEditor()) {
        QColor color = QColorDialog::getColor(editor->textColor(), this);
        if (color.isValid()) {
            editor->setTextColor(color);
        }
    }
}

// Find and replace
void MyNotepad::find()
{
    if (!findDialog) {
        findDialog = new FindDialog(tabManager->currentEditor(), this);
    }
    findDialog->show();
    findDialog->raise();
    findDialog->activateWindow();
}

void MyNotepad::replace()
{
    if (!replaceDialog) {
        replaceDialog = new ReplaceDialog(tabManager->currentEditor(), this);
    }
    replaceDialog->show();
    replaceDialog->raise();
    replaceDialog->activateWindow();
}

void MyNotepad::findNext()
{
    if (findDialog) {
        findDialog->findNext();
    } else {
        find();
    }
}

void MyNotepad::findPrevious()
{
    if (findDialog) {
        findDialog->findPrevious();
    } else {
        find();
    }
}

// Print
void MyNotepad::printFile()
{
#if defined(QT_PRINTSUPPORT_LIB) && QT_CONFIG(printer)
    if (QTextEdit *editor = tabManager->currentEditor()) {
        QPrinter printer(QPrinter::HighResolution);
        QPrintDialog dialog(&printer, this);
        if (dialog.exec() == QDialog::Accepted) {
            editor->print(&printer);
        }
    }
#endif
}

// About
void MyNotepad::about()
{
    QMessageBox::about(this, tr("关于 MyNotepad"),
        tr("MyNotepad 是一个基于 Qt 的简单文本编辑器。\n\n"
           "版本：1.0\n"
           "版权所有 (C) 2025\n"
           "162210118-魏程浩"));
}

// Language selection
void MyNotepad::setLanguageJava()
{
    tabManager->setCurrentTabLanguage(SyntaxHighlighter::Java);
    updateLanguageMenu();
}

void MyNotepad::setLanguageCpp()
{
    tabManager->setCurrentTabLanguage(SyntaxHighlighter::Cpp);
    updateLanguageMenu();
}

void MyNotepad::setLanguagePython()
{
    tabManager->setCurrentTabLanguage(SyntaxHighlighter::Python);
    updateLanguageMenu();
}

void MyNotepad::setLanguageNone()
{
    tabManager->setCurrentTabLanguage(SyntaxHighlighter::None);
    updateLanguageMenu();
}

void MyNotepad::updateLanguageMenu()
{
    QMenu *languageMenu = menuBar()->findChild<QMenu*>("menuLanguage");
    if (languageMenu) {
        for (QAction *action : languageMenu->actions()) {
            action->setChecked(false);
        }
        switch (tabManager->currentTabLanguage()) {
            case SyntaxHighlighter::Java:
                languageMenu->actions()[1]->setChecked(true);
                break;
            case SyntaxHighlighter::Cpp:
                languageMenu->actions()[2]->setChecked(true);
                break;
            case SyntaxHighlighter::Python:
                languageMenu->actions()[3]->setChecked(true);
                break;
            default:
                languageMenu->actions()[0]->setChecked(true);
                break;
        }
    }
}

// Tab management
void MyNotepad::onTabChanged()
{
    updateWindowTitle();
    updateCursorPosition();
    updateLanguageMenu();
}

void MyNotepad::onTextChanged()
{
    updateWindowTitle();
}

void MyNotepad::setAutoSaveInterval()
{
    bool ok;
    int minutes = QInputDialog::getInt(this, tr("自动保存间隔"),
        tr("请输入自动保存间隔（分钟）："),
        autoSaveManager->autoSaveInterval(), 1, 60, 1, &ok);
    
    if (ok) {
        autoSaveManager->setAutoSaveInterval(minutes);
    }
}

void MyNotepad::toggleAutoSave()
{
    QAction *action = qobject_cast<QAction*>(sender());
    if (action) {
        autoSaveManager->setAutoSaveEnabled(action->isChecked());
    }
}
