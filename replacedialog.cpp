#include "replacedialog.h"
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextCursor>
#include <QTextDocument>
#include <QMessageBox>

ReplaceDialog::ReplaceDialog(QTextEdit *textEdit, QWidget *parent)
    : QDialog(parent), textEdit(textEdit)
{
    setWindowTitle(tr("替换"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    findLineEdit = new QLineEdit(this);
    findLineEdit->setPlaceholderText(tr("输入要查找的文本..."));

    replaceLineEdit = new QLineEdit(this);
    replaceLineEdit->setPlaceholderText(tr("输入要替换的文本..."));

    caseCheckBox = new QCheckBox(tr("区分大小写"), this);
    wholeWordsCheckBox = new QCheckBox(tr("整词"), this);

    findNextButton = new QPushButton(tr("查找下一个"), this);
    replaceButton = new QPushButton(tr("替换"), this);
    replaceAllButton = new QPushButton(tr("替换所有"), this);
    closeButton = new QPushButton(tr("关闭"), this);

    statusLabel = new QLabel(this);
    statusLabel->setStyleSheet("color: gray;");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    QHBoxLayout *findLayout = new QHBoxLayout;
    findLayout->addWidget(new QLabel(tr("Find:")));
    findLayout->addWidget(findLineEdit);
    mainLayout->addLayout(findLayout);

    QHBoxLayout *replaceLayout = new QHBoxLayout;
    replaceLayout->addWidget(new QLabel(tr("替换为:")));
    replaceLayout->addWidget(replaceLineEdit);
    mainLayout->addLayout(replaceLayout);

    QHBoxLayout *optionsLayout = new QHBoxLayout;
    optionsLayout->addWidget(caseCheckBox);
    optionsLayout->addWidget(wholeWordsCheckBox);
    mainLayout->addLayout(optionsLayout);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(findNextButton);
    buttonLayout->addWidget(replaceButton);
    buttonLayout->addWidget(replaceAllButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(closeButton);
    mainLayout->addLayout(buttonLayout);

    mainLayout->addWidget(statusLabel);

    connect(findNextButton, &QPushButton::clicked, this, &ReplaceDialog::findNext);
    connect(replaceButton, &QPushButton::clicked, this, &ReplaceDialog::replace);
    connect(replaceAllButton, &QPushButton::clicked, this, &ReplaceDialog::replaceAll);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::close);
    connect(findLineEdit, &QLineEdit::returnPressed, this, &ReplaceDialog::findNext);

    resize(500, 200);
}

void ReplaceDialog::findNext()
{
    findText(findLineEdit->text(), caseCheckBox->isChecked(), wholeWordsCheckBox->isChecked());
}

void ReplaceDialog::replace()
{
    if (textEdit->textCursor().hasSelection()) {
        textEdit->textCursor().insertText(replaceLineEdit->text());
        findNext();
    }
}

void ReplaceDialog::replaceAll()
{
    QString findText = findLineEdit->text();
    QString replaceText = replaceLineEdit->text();
    
    if (findText.isEmpty()) {
        statusLabel->setText(tr("请输入要查找的文本。"));
        return;
    }

    QTextDocument::FindFlags flags;
    if (caseCheckBox->isChecked())
        flags |= QTextDocument::FindCaseSensitively;
    if (wholeWordsCheckBox->isChecked())
        flags |= QTextDocument::FindWholeWords;

    int count = 0;
    QTextCursor cursor = textEdit->textCursor();
    cursor.movePosition(QTextCursor::Start);
    textEdit->setTextCursor(cursor);

    while (textEdit->find(findText, flags)) {
        textEdit->textCursor().insertText(replaceText);
        count++;
    }

    statusLabel->setText(tr("替换 %1 次。").arg(count));
}

void ReplaceDialog::findText(const QString &text, bool caseSensitive, bool wholeWords)
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