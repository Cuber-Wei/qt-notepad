#ifndef TABMANAGER_H
#define TABMANAGER_H

#include <QTabWidget>
#include <QTextEdit>
#include <QString>
#include "syntaxhighlighter.h"

class TabManager : public QTabWidget
{
    Q_OBJECT

public:
    explicit TabManager(QWidget *parent = nullptr);
    ~TabManager();

    QTextEdit* currentEditor() const;
    QTextEdit* editorAt(int index) const;
    int addNewTab(const QString &title = tr("Untitled"));
    void closeTab(int index);
    void setCurrentTabTitle(const QString &title);
    QString currentTabTitle() const;
    void setCurrentTabModified(bool modified);
    bool isCurrentTabModified() const;
    void setCurrentTabPath(const QString &path);
    QString currentTabPath() const;
    void setCurrentTabLanguage(SyntaxHighlighter::Language language);
    SyntaxHighlighter::Language currentTabLanguage() const;

private slots:
    void onTabCloseRequested(int index);
    void onCurrentChanged(int index);
    void onTextChanged();

signals:
    void currentTabChanged();
    void tabClosed(int index);
    void textChanged();

private:
    struct TabInfo {
        QTextEdit *editor;
        QString path;
        SyntaxHighlighter *highlighter;
        SyntaxHighlighter::Language language;
    };

    QVector<TabInfo> tabs;
    void setupTab(int index);
    void updateTabTitle(int index);
};

#endif // TABMANAGER_H 