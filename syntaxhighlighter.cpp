#include "syntaxhighlighter.h"
#include <QTextDocument>

SyntaxHighlighter::SyntaxHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent), currentLanguage(None)
{
    setupCommonRules();
}

void SyntaxHighlighter::setLanguage(Language language)
{
    currentLanguage = language;
    highlightingRules.clear();
    setupCommonRules();

    switch (language) {
        case Java:
            setupJavaRules();
            break;
        case Cpp:
            setupCppRules();
            break;
        case Python:
            setupPythonRules();
            break;
        default:
            break;
    }

    rehighlight();
}

void SyntaxHighlighter::highlightBlock(const QString &text)
{
    // Apply highlighting rules
    for (const HighlightingRule &rule : highlightingRules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    // Handle multi-line comments
    if (currentLanguage != Python) {
        setCurrentBlockState(0);
        int startIndex = 0;
        if (previousBlockState() != 1)
            startIndex = text.indexOf(commentStartExpression);

        while (startIndex >= 0) {
            QRegularExpressionMatch match = commentEndExpression.match(text, startIndex);
            int endIndex = match.capturedStart();
            int commentLength;
            if (endIndex == -1) {
                setCurrentBlockState(1);
                commentLength = text.length() - startIndex;
            } else {
                commentLength = endIndex - startIndex + match.capturedLength();
            }
            setFormat(startIndex, commentLength, multiLineCommentFormat);
            startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
        }
    }
}

void SyntaxHighlighter::setupCommonRules()
{
    // Common keywords for all languages
    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(QColor(255, 140, 0));  // Dark orange color
    keywordFormat.setFontWeight(QFont::Bold);

    QTextCharFormat classFormat;
    classFormat.setForeground(Qt::darkMagenta);
    classFormat.setFontWeight(QFont::Bold);

    QTextCharFormat singleLineCommentFormat;
    singleLineCommentFormat.setForeground(Qt::red);

    QTextCharFormat multiLineCommentFormat;
    multiLineCommentFormat.setForeground(Qt::red);

    QTextCharFormat quotationFormat;
    quotationFormat.setForeground(Qt::darkGreen);

    QTextCharFormat functionFormat;
    functionFormat.setForeground(Qt::blue);

    // Common rules
    HighlightingRule rule;

    // Strings
    rule.pattern = QRegularExpression("\".*\"");
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    // Single-line comments
    if (currentLanguage == Python) {
        rule.pattern = QRegularExpression("#[^\n]*");
    } else {
        rule.pattern = QRegularExpression("//[^\n]*");
    }
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    // Multi-line comments
    if (currentLanguage != Python) {
        commentStartExpression = QRegularExpression("/\\*");
        commentEndExpression = QRegularExpression("\\*/");
        this->multiLineCommentFormat = multiLineCommentFormat;
    }
}

void SyntaxHighlighter::setupJavaRules()
{
    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(QColor(255, 140, 0));  // Dark orange color
    keywordFormat.setFontWeight(QFont::Bold);

    QTextCharFormat functionFormat;
    functionFormat.setForeground(Qt::blue);
    functionFormat.setFontWeight(QFont::Bold);

    // Java keywords
    QStringList javaKeywords;
    javaKeywords << "\\babstract\\b" << "\\bassert\\b" << "\\bboolean\\b"
                 << "\\bbreak\\b" << "\\bbyte\\b" << "\\bcase\\b" << "\\bcatch\\b"
                 << "\\bchar\\b" << "\\bclass\\b" << "\\bconst\\b" << "\\bcontinue\\b"
                 << "\\bdefault\\b" << "\\bdo\\b" << "\\bdouble\\b" << "\\belse\\b"
                 << "\\benum\\b" << "\\bextends\\b" << "\\bfinal\\b" << "\\bfinally\\b"
                 << "\\bfloat\\b" << "\\bfor\\b" << "\\bgoto\\b" << "\\bif\\b"
                 << "\\bimplements\\b" << "\\bimport\\b" << "\\binstanceof\\b"
                 << "\\bint\\b" << "\\binterface\\b" << "\\blong\\b" << "\\bnative\\b"
                 << "\\bnew\\b" << "\\bpackage\\b" << "\\bprivate\\b" << "\\bprotected\\b"
                 << "\\bpublic\\b" << "\\breturn\\b" << "\\bshort\\b" << "\\bstatic\\b"
                 << "\\bstrictfp\\b" << "\\bsuper\\b" << "\\bswitch\\b" << "\\bsynchronized\\b"
                 << "\\bthis\\b" << "\\bthrow\\b" << "\\bthrows\\b" << "\\btransient\\b"
                 << "\\btry\\b" << "\\bvoid\\b" << "\\bvolatile\\b" << "\\bwhile\\b";

    // Java common functions
    QStringList javaFunctions;
    javaFunctions << "\\bSystem\\.out\\.println\\b" << "\\bSystem\\.out\\.print\\b"
                  << "\\bSystem\\.out\\.printf\\b" << "\\bSystem\\.out\\.format\\b"
                  << "\\bString\\.format\\b" << "\\bString\\.valueOf\\b"
                  << "\\bInteger\\.parseInt\\b" << "\\bDouble\\.parseDouble\\b"
                  << "\\bMath\\.abs\\b" << "\\bMath\\.sqrt\\b" << "\\bMath\\.pow\\b"
                  << "\\bMath\\.sin\\b" << "\\bMath\\.cos\\b" << "\\bMath\\.tan\\b"
                  << "\\bArrays\\.sort\\b" << "\\bArrays\\.toString\\b"
                  << "\\bCollections\\.sort\\b" << "\\bCollections\\.reverse\\b"
                  << "\\bFile\\.exists\\b" << "\\bFile\\.delete\\b"
                  << "\\bBufferedReader\\.readLine\\b" << "\\bBufferedWriter\\.write\\b"
                  << "\\bScanner\\.nextLine\\b" << "\\bScanner\\.nextInt\\b"
                  << "\\bScanner\\.nextDouble\\b" << "\\bScanner\\.hasNext\\b";

    for (const QString &pattern : javaKeywords) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    for (const QString &pattern : javaFunctions) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = functionFormat;
        highlightingRules.append(rule);
    }
}

void SyntaxHighlighter::setupCppRules()
{
    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(QColor(255, 140, 0));  // Dark orange color
    keywordFormat.setFontWeight(QFont::Bold);

    QTextCharFormat functionFormat;
    functionFormat.setForeground(Qt::blue);
    functionFormat.setFontWeight(QFont::Bold);

    // C++ keywords
    QStringList cppKeywords;
    cppKeywords << "\\bauto\\b" << "\\bbreak\\b" << "\\bcase\\b" << "\\bcatch\\b"
                << "\\bchar\\b" << "\\bclass\\b" << "\\bconst\\b" << "\\bcontinue\\b"
                << "\\bdefault\\b" << "\\bdelete\\b" << "\\bdo\\b" << "\\bdouble\\b"
                << "\\belse\\b" << "\\benum\\b" << "\\bexplicit\\b" << "\\bexport\\b"
                << "\\bextern\\b" << "\\bfalse\\b" << "\\bfloat\\b" << "\\bfor\\b"
                << "\\bfriend\\b" << "\\bgoto\\b" << "\\bif\\b" << "\\binclude\\b"
                << "\\binline\\b"<< "\\bint\\b" << "\\blong\\b" << "\\bmutable\\b"
                << "\\bnamespace\\b" << "\\bnew\\b" << "\\boperator\\b" << "\\bprivate\\b"
                << "\\bprotected\\b" << "\\bpublic\\b" << "\\bregister\\b" << "\\breinterpret_cast\\b"
                << "\\breturn\\b" << "\\bshort\\b" << "\\bsigned\\b" << "\\bsizeof\\b"
                << "\\bstatic\\b" << "\\bstatic_cast\\b" << "\\bstruct\\b" << "\\bswitch\\b"
                << "\\btemplate\\b" << "\\bthis\\b" << "\\bthrow\\b" << "\\btrue\\b"
                << "\\btry\\b" << "\\btypedef\\b" << "\\btypeid\\b" << "\\btypename\\b"
                << "\\bunion\\b" << "\\bunsigned\\b" << "\\busing\\b" << "\\bvirtual\\b"
                << "\\bvoid\\b" << "\\bvolatile\\b" << "\\bwhile\\b";

    // C++ common functions
    QStringList cppFunctions;
    cppFunctions << "\\bstd::cout\\b" << "\\bstd::cin\\b" << "\\bstd::endl\\b"
                 << "\\bstd::string\\b" << "\\bstd::vector\\b" << "\\bstd::map\\b"
                 << "\\bstd::sort\\b" << "\\bstd::find\\b" << "\\bstd::max\\b"
                 << "\\bstd::min\\b" << "\\bstd::swap\\b" << "\\bstd::reverse\\b"
                 << "\\bstd::unique\\b" << "\\bstd::count\\b" << "\\bstd::accumulate\\b"
                 << "\\bstd::transform\\b" << "\\bstd::copy\\b" << "\\bstd::move\\b"
                 << "\\bstd::fill\\b" << "\\bstd::generate\\b" << "\\bstd::iota\\b"
                 << "\\bstd::random_shuffle\\b" << "\\bstd::shuffle\\b"
                 << "\\bstd::next_permutation\\b" << "\\bstd::prev_permutation\\b"
                 << "\\bstd::is_sorted\\b" << "\\bstd::binary_search\\b"
                 << "\\bstd::lower_bound\\b" << "\\bstd::upper_bound\\b"
                 << "\\bstd::equal_range\\b" << "\\bstd::merge\\b"
                 << "\\bstd::inplace_merge\\b" << "\\bstd::includes\\b"
                 << "\\bstd::set_union\\b" << "\\bstd::set_intersection\\b"
                 << "\\bstd::set_difference\\b" << "\\bstd::set_symmetric_difference\\b";

    for (const QString &pattern : cppKeywords) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    for (const QString &pattern : cppFunctions) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = functionFormat;
        highlightingRules.append(rule);
    }
}

void SyntaxHighlighter::setupPythonRules()
{
    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(QColor(255, 140, 0));  // Dark orange color
    keywordFormat.setFontWeight(QFont::Bold);

    QTextCharFormat functionFormat;
    functionFormat.setForeground(Qt::blue);
    functionFormat.setFontWeight(QFont::Bold);

    // Python keywords
    QStringList pythonKeywords;
    pythonKeywords << "\\band\\b" << "\\bas\\b" << "\\bassert\\b" << "\\bbreak\\b"
                   << "\\bclass\\b" << "\\bcontinue\\b" << "\\bdef\\b" << "\\bdel\\b"
                   << "\\belif\\b" << "\\belse\\b" << "\\bexcept\\b" << "\\bFalse\\b"
                   << "\\bfinally\\b" << "\\bfor\\b" << "\\bfrom\\b" << "\\bglobal\\b"
                   << "\\bif\\b" << "\\bimport\\b" << "\\bin\\b" << "\\bis\\b"
                   << "\\blambda\\b" << "\\bNone\\b" << "\\bnonlocal\\b" << "\\bnot\\b"
                   << "\\bor\\b" << "\\bpass\\b" << "\\braise\\b" << "\\breturn\\b"
                   << "\\bTrue\\b" << "\\btry\\b" << "\\bwhile\\b" << "\\bwith\\b"
                   << "\\byield\\b";

    // Python common functions
    QStringList pythonFunctions;
    pythonFunctions << "\\bprint\\b" << "\\binput\\b" << "\\blen\\b" << "\\brange\\b"
                    << "\\bstr\\b" << "\\bint\\b" << "\\bfloat\\b" << "\\bbool\\b"
                    << "\\blist\\b" << "\\bdict\\b" << "\\bset\\b" << "\\btuple\\b"
                    << "\\bsorted\\b" << "\\breversed\\b" << "\\benumerate\\b"
                    << "\\bzip\\b" << "\\bmap\\b" << "\\bfilter\\b" << "\\breduce\\b"
                    << "\\bmax\\b" << "\\bmin\\b" << "\\bsum\\b" << "\\ball\\b"
                    << "\\bany\\b" << "\\babs\\b" << "\\bround\\b" << "\\bpow\\b"
                    << "\\bdivmod\\b" << "\\bchr\\b" << "\\bord\\b" << "\\bhex\\b"
                    << "\\boct\\b" << "\\bbin\\b" << "\\bformat\\b" << "\\bsplit\\b"
                    << "\\bjoin\\b" << "\\bstrip\\b" << "\\breplace\\b" << "\\bfind\\b"
                    << "\\bindex\\b" << "\\bcount\\b" << "\\bupper\\b" << "\\blower\\b"
                    << "\\bcapitalize\\b" << "\\btitle\\b" << "\\bswapcase\\b"
                    << "\\bstartswith\\b" << "\\bendswith\\b" << "\\bisalpha\\b"
                    << "\\bisdigit\\b" << "\\bisalnum\\b" << "\\bisspace\\b"
                    << "\\bljust\\b" << "\\brjust\\b" << "\\bcenter\\b" << "\\bzfill\\b"
                    << "\\bopen\\b" << "\\bread\\b" << "\\bwrite\\b" << "\\bclose\\b"
                    << "\\bseek\\b" << "\\btell\\b" << "\\bflush\\b" << "\\btruncate\\b"
                    << "\\bjson\\.dumps\\b" << "\\bjson\\.loads\\b" << "\\bjson\\.dump\\b"
                    << "\\bjson\\.load\\b" << "\\brandom\\.random\\b" << "\\brandom\\.randint\\b"
                    << "\\brandom\\.choice\\b" << "\\brandom\\.shuffle\\b" << "\\brandom\\.sample\\b"
                    << "\\bdatetime\\.datetime\\.now\\b" << "\\bdatetime\\.datetime\\.strptime\\b"
                    << "\\bdatetime\\.datetime\\.strftime\\b" << "\\bdatetime\\.timedelta\\b"
                    << "\\bos\\.path\\.exists\\b" << "\\bos\\.path\\.isfile\\b"
                    << "\\bos\\.path\\.isdir\\b" << "\\bos\\.path\\.join\\b"
                    << "\\bos\\.path\\.split\\b" << "\\bos\\.path\\.splitext\\b"
                    << "\\bos\\.path\\.basename\\b" << "\\bos\\.path\\.dirname\\b"
                    << "\\bsys\\.argv\\b" << "\\bsys\\.exit\\b" << "\\bsys\\.path\\b"
                    << "\\bre\\.search\\b" << "\\bre\\.match\\b" << "\\bre\\.findall\\b"
                    << "\\bre\\.sub\\b" << "\\bre\\.split\\b" << "\\bre\\.compile\\b";

    for (const QString &pattern : pythonKeywords) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    for (const QString &pattern : pythonFunctions) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = functionFormat;
        highlightingRules.append(rule);
    }
} 