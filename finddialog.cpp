#include "finddialog.h"
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextCursor>
#include <QTextDocument>
#include <QMessageBox>

FindDialog::FindDialog(QTextEdit *textEdit, QWidget *parent)
    : QDialog(parent), textEdit(textEdit)
{
    setWindowTitle(tr("查找"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    findLineEdit = new QLineEdit(this);
    findLineEdit->setPlaceholderText(tr("输入要查找的文本..."));

    caseCheckBox = new QCheckBox(tr("区分大小写"), this);
    wholeWordsCheckBox = new QCheckBox(tr("整词"), this);

    findNextButton = new QPushButton(tr("查找下一个"), this);
    findPreviousButton = new QPushButton(tr("查找上一个"), this);
    closeButton = new QPushButton(tr("关闭"), this);

    statusLabel = new QLabel(this);
    statusLabel->setStyleSheet("color: gray;");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(findLineEdit);

    QHBoxLayout *optionsLayout = new QHBoxLayout;
    optionsLayout->addWidget(caseCheckBox);
    optionsLayout->addWidget(wholeWordsCheckBox);
    mainLayout->addLayout(optionsLayout);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(findNextButton);
    buttonLayout->addWidget(findPreviousButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(closeButton);
    mainLayout->addLayout(buttonLayout);

    mainLayout->addWidget(statusLabel);

    connect(findNextButton, &QPushButton::clicked, this, &FindDialog::findNext);
    connect(findPreviousButton, &QPushButton::clicked, this, &FindDialog::findPrevious);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::close);
    connect(findLineEdit, &QLineEdit::returnPressed, this, &FindDialog::findNext);

    resize(400, 150);
}

void FindDialog::findNext()
{
    findText(findLineEdit->text(), caseCheckBox->isChecked(), wholeWordsCheckBox->isChecked());
}

void FindDialog::findPrevious()
{
    QTextCursor cursor = textEdit->textCursor();
    cursor.movePosition(QTextCursor::StartOfWord);
    textEdit->setTextCursor(cursor);
    findText(findLineEdit->text(), caseCheckBox->isChecked(), wholeWordsCheckBox->isChecked());
}

void FindDialog::findText(const QString &text, bool caseSensitive, bool wholeWords)
{
    if (text.isEmpty()) {
        statusLabel->setText(tr("请输入要查找的文本。"));
        return;
    }

    QTextDocument::FindFlags flags;
    if (caseSensitive)
        flags |= QTextDocument::FindCaseSensitively;
    if (wholeWords)
        flags |= QTextDocument::FindWholeWords;

    bool found = textEdit->find(text, flags);
    if (found) {
        statusLabel->setText(tr("找到文本。"));
    } else {
        statusLabel->setText(tr("未找到文本。"));
        QTextCursor cursor = textEdit->textCursor();
        cursor.movePosition(QTextCursor::Start);
        textEdit->setTextCursor(cursor);
    }
} 