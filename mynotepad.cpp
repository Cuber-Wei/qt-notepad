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
    
    // Create text edit
    textEdit = new QTextEdit(this);
    setCentralWidget(textEdit);
    
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
    textEdit->setFont(font);
}

MyNotepad::~MyNotepad()
{
    delete ui;
    delete findDialog;
    delete replaceDialog;
}

void MyNotepad::createMenus()
{
    // File menu
    QMenu *fileMenu = menuBar()->addMenu(tr("&文件"));
    fileMenu->addAction(tr("&新建"), this, &MyNotepad::newFile, QKeySequence::New);
    fileMenu->addAction(tr("&打开..."), this, &MyNotepad::openFile, QKeySequence::Open);
    fileMenu->addAction(tr("&保存"), this, &MyNotepad::saveFile, QKeySequence::Save);
    fileMenu->addAction(tr("另存为..."), this, &MyNotepad::saveAsFile, QKeySequence::SaveAs);
    fileMenu->addSeparator();
    fileMenu->addAction(tr("&退出"), this, &QWidget::close, QKeySequence::Quit);

    // Edit menu
    QMenu *editMenu = menuBar()->addMenu(tr("&编辑"));
    editMenu->addAction(tr("&撤销"), this, &MyNotepad::undo, QKeySequence::Undo);
    editMenu->addAction(tr("&重做"), this, &MyNotepad::redo, QKeySequence::Redo);
    editMenu->addSeparator();
    editMenu->addAction(tr("&剪切"), this, &MyNotepad::cut, QKeySequence::Cut);
    editMenu->addAction(tr("&复制"), this, &MyNotepad::copy, QKeySequence::Copy);
    editMenu->addAction(tr("&粘贴"), this, &MyNotepad::paste, QKeySequence::Paste);
    editMenu->addAction(tr("&全选"), this, &MyNotepad::selectAll, QKeySequence::SelectAll);

    // Format menu
    QMenu *formatMenu = menuBar()->addMenu(tr("&格式"));
    formatMenu->addAction(tr("&字体..."), this, &MyNotepad::setFont);
    formatMenu->addAction(tr("&字体颜色..."), this, &MyNotepad::setFontColor);

    // Find menu
    QMenu *findMenu = menuBar()->addMenu(tr("&查找"));
    findMenu->addAction(tr("&查找..."), this, &MyNotepad::find, QKeySequence::Find);
    findMenu->addAction(tr("查找 下一个"), this, &MyNotepad::findNext, QKeySequence::FindNext);
    findMenu->addAction(tr("查找 上一个"), this, &MyNotepad::findPrevious, QKeySequence::FindPrevious);
    findMenu->addAction(tr("&替换..."), this, &MyNotepad::replace, QKeySequence::Replace);

    // Help menu
    QMenu *helpMenu = menuBar()->addMenu(tr("&帮助"));
    helpMenu->addAction(tr("&关于"), this, &MyNotepad::about);
}

void MyNotepad::createToolBars()
{
    QToolBar *fileToolBar = addToolBar(tr("文件"));
    fileToolBar->addAction(tr("新建"), this, &MyNotepad::newFile);
    fileToolBar->addAction(tr("打开"), this, &MyNotepad::openFile);
    fileToolBar->addAction(tr("保存"), this, &MyNotepad::saveFile);

    QToolBar *editToolBar = addToolBar(tr("编辑"));
    editToolBar->addAction(tr("剪切"), this, &MyNotepad::cut);
    editToolBar->addAction(tr("复制"), this, &MyNotepad::copy);
    editToolBar->addAction(tr("粘贴"), this, &MyNotepad::paste);
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
    connect(textEdit->document(), &QTextDocument::modificationChanged,
            this, &MyNotepad::documentModified);
    connect(textEdit, &QTextEdit::cursorPositionChanged,
            this, &MyNotepad::updateCursorPosition);
}

void MyNotepad::newFile()
{
    if (maybeSave()) {
        textEdit->clear();
        setCurrentFile(QString());
    }
}

void MyNotepad::openFile()
{
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this,
            tr("打开文件"), QString(),
            tr("文本文件 (*.txt);;所有文件 (*)"));
            
        if (!fileName.isEmpty())
            loadFile(fileName);
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

    QTextStream in(&file);
    QTextCodec *codec = QTextCodec::codecForUtfText(file.peek(1024), QTextCodec::codecForName("UTF-8"));
    in.setCodec(codec);
    textEdit->setPlainText(in.readAll());
    
    setCurrentFile(fileName);
    encodingLabel->setText(codec->name());
}

void MyNotepad::saveFile()
{
    if (currentFile.isEmpty()) {
        QString fileName = QFileDialog::getSaveFileName(this,
            tr("另存为"), QString(),
            tr("文本文件 (*.txt);;所有文件 (*)"));
            
        if (!fileName.isEmpty()) {
            saveFileAs(fileName);
        }
    } else {
        saveFileAs(currentFile);
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
    out << textEdit->toPlainText();
    
    setCurrentFile(fileName);
    return true;
}

void MyNotepad::setCurrentFile(const QString &fileName)
{
    currentFile = fileName;
    textEdit->document()->setModified(false);
    updateWindowTitle();
}

void MyNotepad::updateWindowTitle()
{
    setWindowTitle(tr("%1[*] - MyNotepad")
        .arg(currentFile.isEmpty() ? tr("Untitled") : strippedName(currentFile)));
}

QString MyNotepad::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

bool MyNotepad::maybeSave()
{
    if (!textEdit->document()->isModified())
        return true;

    QMessageBox::StandardButton ret = QMessageBox::warning(this, tr("MyNotepad"),
        tr("文档已修改。\n"
           "是否保存更改？"),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
           
    if (ret == QMessageBox::Save) {
        QString fileName = QFileDialog::getSaveFileName(this,
            tr("另存为"), QString(),
            tr("文本文件 (*.txt);;所有文件 (*)"));
            
        if (!fileName.isEmpty()) {
            return saveFileAs(fileName);
        }
        return false;
    } else if (ret == QMessageBox::Cancel) {
        return false;
    }
    return true;
}

void MyNotepad::documentModified()
{
    updateWindowTitle();
}

void MyNotepad::updateCursorPosition()
{
    QTextCursor cursor = textEdit->textCursor();
    statusLabel->setText(tr("Ln %1, Col %2")
        .arg(cursor.blockNumber() + 1)
        .arg(cursor.columnNumber() + 1));
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
    textEdit->undo();
}

void MyNotepad::redo()
{
    textEdit->redo();
}

void MyNotepad::cut()
{
    textEdit->cut();
}

void MyNotepad::copy()
{
    textEdit->copy();
}

void MyNotepad::paste()
{
    textEdit->paste();
}

void MyNotepad::selectAll()
{
    textEdit->selectAll();
}

// Format operations
void MyNotepad::setFont()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, textEdit->font(), this);
    if (ok) {
        textEdit->setFont(font);
        QSettings settings;
        settings.setValue("font", font);
    }
}

void MyNotepad::setFontColor()
{
    QColor color = QColorDialog::getColor(textEdit->textColor(), this);
    if (color.isValid()) {
        textEdit->setTextColor(color);
    }
}

// Find and replace
void MyNotepad::find()
{
    if (!findDialog) {
        findDialog = new FindDialog(textEdit, this);
    }
    findDialog->show();
    findDialog->raise();
    findDialog->activateWindow();
}

void MyNotepad::replace()
{
    if (!replaceDialog) {
        replaceDialog = new ReplaceDialog(textEdit, this);
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
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog dialog(&printer, this);
    if (dialog.exec() == QDialog::Accepted) {
        textEdit->print(&printer);
    }
#endif
}

// About
void MyNotepad::about()
{
    QMessageBox::about(this, tr("关于 MyNotepad"),
        tr("此项目是基于Qt的简单文本编辑器。\n\n"
           "版本： 1.0\n"
           "Copyright (C) 2025 162210118-魏程浩"));
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
