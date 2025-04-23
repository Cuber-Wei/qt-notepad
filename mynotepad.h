#ifndef MYNOTEPAD_H
#define MYNOTEPAD_H

#include <QMainWindow>
#include <QTextEdit>
#include <QLabel>
#include <QTextCodec>

class FindDialog;
class ReplaceDialog;

namespace Ui {
class MyNotepad;
}

class MyNotepad : public QMainWindow
{
    Q_OBJECT

public:
    explicit MyNotepad(QWidget *parent = nullptr);
    ~MyNotepad();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    // File operations
    void newFile();
    void openFile();
    void saveFile();
    void saveAsFile();
    void printFile();

    // Edit operations
    void undo();
    void redo();
    void cut();
    void copy();
    void paste();
    void selectAll();

    // Format operations
    void setFont();
    void setFontColor();

    // Find and replace
    void find();
    void replace();
    void findNext();
    void findPrevious();

    // Status updates
    void updateCursorPosition();
    void documentModified();

    // About dialog
    void about();

private:
    Ui::MyNotepad *ui;
    QString currentFile;
    QTextEdit *textEdit;
    QLabel *statusLabel;
    QLabel *encodingLabel;
    FindDialog *findDialog;
    ReplaceDialog *replaceDialog;
    
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void setupConnections();
    void loadFile(const QString &fileName);
    bool saveFileAs(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);
    void updateWindowTitle();
    bool maybeSave();
};

#endif // MYNOTEPAD_H
