/**
 * @file syntaxhighlighter.h
 * @brief 语法高亮器的头文件
 * 
 * 定义了文本编辑器的语法高亮功能，支持多种编程语言的语法高亮。
 */

#ifndef SYNTAXHIGHLIGHTER_H
#define SYNTAXHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>

/**
 * @class SyntaxHighlighter
 * @brief 语法高亮器类
 * 
 * 为文本编辑器提供语法高亮功能，支持多种编程语言。
 * 可以高亮显示关键字、函数、注释等语法元素。
 */
class SyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    /**
     * @brief 编程语言枚举
     * 
     * 定义了支持的编程语言类型。
     */
    enum Language {
        None,   ///< 无语法高亮
        Java,   ///< Java语言
        Cpp,    ///< C++语言
        Python  ///< Python语言
    };

    /**
     * @brief 构造函数
     * @param parent 父文档指针
     * 
     * 初始化语法高亮器，设置默认规则。
     */
    explicit SyntaxHighlighter(QTextDocument *parent = nullptr);

    /**
     * @brief 设置编程语言
     * @param language 编程语言类型
     * 
     * 根据指定的语言设置语法高亮规则。
     */
    void setLanguage(Language language);

    Language getLanguage() const { return currentLanguage; }

protected:
    /**
     * @brief 高亮文本块
     * @param text 要高亮的文本
     * 
     * 重写基类方法，实现语法高亮功能。
     */
    void highlightBlock(const QString &text) override;

private:
    /**
     * @struct HighlightingRule
     * @brief 高亮规则结构
     * 
     * 定义了语法高亮的规则，包括正则表达式和格式。
     */
    struct HighlightingRule {
        QRegularExpression pattern;  ///< 匹配模式
        QTextCharFormat format;      ///< 文本格式
    };

    Language currentLanguage;                    ///< 当前语言
    QList<HighlightingRule> highlightingRules;   ///< 高亮规则列表
    QRegularExpression commentStartExpression;   ///< 多行注释开始
    QRegularExpression commentEndExpression;     ///< 多行注释结束
    QTextCharFormat multiLineCommentFormat;      ///< 多行注释格式

    /**
     * @brief 设置通用规则
     * 
     * 设置适用于所有语言的通用高亮规则。
     */
    void setupCommonRules();

    /**
     * @brief 设置Java语言规则
     * 
     * 设置Java语言的特定高亮规则。
     */
    void setupJavaRules();

    /**
     * @brief 设置C++语言规则
     * 
     * 设置C++语言的特定高亮规则。
     */
    void setupCppRules();

    /**
     * @brief 设置Python语言规则
     * 
     * 设置Python语言的特定高亮规则。
     */
    void setupPythonRules();
};

#endif // SYNTAXHIGHLIGHTER_H 