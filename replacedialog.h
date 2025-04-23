#ifndef REPLACEDIALOG_H
#define REPLACEDIALOG_H

#include <QDialog>
#include <QTextEdit>

class QLineEdit;
class QPushButton;
class QCheckBox;
class QLabel;

class ReplaceDialog : public QDialog
{
    Q_OBJECT

public:
    ReplaceDialog(QTextEdit *textEdit, QWidget *parent = nullptr);

private slots:
    void findNext();
    void replace();
    void replaceAll();
    void findText(const QString &text, bool caseSensitive, bool wholeWords);

private:
    QTextEdit *textEdit;
    QLineEdit *findLineEdit;
    QLineEdit *replaceLineEdit;
    QCheckBox *caseCheckBox;
    QCheckBox *wholeWordsCheckBox;
    QPushButton *findNextButton;
    QPushButton *replaceButton;
    QPushButton *replaceAllButton;
    QPushButton *closeButton;
    QLabel *statusLabel;
};

#endif // REPLACEDIALOG_H 