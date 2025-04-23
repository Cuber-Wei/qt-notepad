#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <QDialog>
#include <QTextEdit>

class QLineEdit;
class QPushButton;
class QCheckBox;
class QLabel;

class FindDialog : public QDialog
{
    Q_OBJECT

public:
    FindDialog(QTextEdit *textEdit, QWidget *parent = nullptr);
    void findNext();
    void findPrevious();

private slots:
    void findText(const QString &text, bool caseSensitive, bool wholeWords);

private:
    QTextEdit *textEdit;
    QLineEdit *findLineEdit;
    QCheckBox *caseCheckBox;
    QCheckBox *wholeWordsCheckBox;
    QPushButton *findNextButton;
    QPushButton *findPreviousButton;
    QPushButton *closeButton;
    QLabel *statusLabel;
};

#endif // FINDDIALOG_H 