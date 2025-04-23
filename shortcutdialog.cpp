#include "shortcutdialog.h"
#include "ui_shortcutdialog.h"
#include <QPushButton>
#include <QKeyEvent>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>

ShortcutDialog::ShortcutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShortcutDialog),
    currentButton(nullptr)
{
    ui->setupUi(this);
    setWindowTitle(tr("快捷键设置"));

    // 设置默认快捷键
    setupDefaultShortcuts();

    // 获取滚动区域的内容布局
    QVBoxLayout* mainLayout = qobject_cast<QVBoxLayout*>(ui->scrollAreaWidgetContents->layout());
    if (!mainLayout) {
        mainLayout = new QVBoxLayout(ui->scrollAreaWidgetContents);
    }

    // 为每个快捷键创建控件
    for (auto it = defaultShortcuts.begin(); it != defaultShortcuts.end(); ++it) {
        QHBoxLayout* rowLayout = new QHBoxLayout();
        
        // 添加快捷键名称标签
        QLabel* nameLabel = new QLabel(it.key(), this);
        nameLabel->setMinimumWidth(100);  // 设置最小宽度
        rowLayout->addWidget(nameLabel);

        // 添加快捷键按钮
        QPushButton* shortcutButton = new QPushButton(it.value().toString(), this);
        shortcutButton->setProperty("shortcutName", it.key());
        shortcutButton->setMinimumWidth(150);  // 设置最小宽度
        connect(shortcutButton, &QPushButton::clicked, this, &ShortcutDialog::onShortcutButtonClicked);
        rowLayout->addWidget(shortcutButton);

        // 添加重置按钮
        QPushButton* resetButton = new QPushButton(tr("重置"), this);
        resetButton->setProperty("shortcutName", it.key());
        resetButton->setMaximumWidth(80);  // 设置最大宽度
        connect(resetButton, &QPushButton::clicked, this, &ShortcutDialog::onResetButtonClicked);
        rowLayout->addWidget(resetButton);

        mainLayout->addLayout(rowLayout);
    }

    // 添加默认按钮
    QPushButton* defaultButton = new QPushButton(tr("恢复默认"), this);
    connect(defaultButton, &QPushButton::clicked, this, &ShortcutDialog::onDefaultButtonClicked);
    mainLayout->addWidget(defaultButton);

    // 设置滚动区域
    ui->scrollArea->setWidgetResizable(true);

    // 连接确定和取消按钮
    connect(ui->okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(ui->cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    // 设置对话框大小
    resize(500, 400);
}

ShortcutDialog::~ShortcutDialog()
{
    delete ui;
}

void ShortcutDialog::setShortcuts(const QMap<QString, QKeySequence>& shortcuts)
{
    currentShortcuts = shortcuts;
    // 更新所有按钮显示
    for (auto it = currentShortcuts.begin(); it != currentShortcuts.end(); ++it) {
        QPushButton* button = findChild<QPushButton*>(QString(), Qt::FindChildrenRecursively);
        if (button && button->property("shortcutName").toString() == it.key()) {
            updateShortcutButton(button, it.value());
        }
    }
}

QMap<QString, QKeySequence> ShortcutDialog::getShortcuts() const
{
    return currentShortcuts;
}

void ShortcutDialog::setupDefaultShortcuts()
{
    defaultShortcuts.clear();
    defaultShortcuts["新建"] = QKeySequence::New;
    defaultShortcuts["打开"] = QKeySequence::Open;
    defaultShortcuts["保存"] = QKeySequence::Save;
    defaultShortcuts["另存为"] = QKeySequence::SaveAs;
    defaultShortcuts["退出"] = QKeySequence::Quit;
    defaultShortcuts["撤销"] = QKeySequence::Undo;
    defaultShortcuts["重做"] = QKeySequence::Redo;
    defaultShortcuts["剪切"] = QKeySequence::Cut;
    defaultShortcuts["复制"] = QKeySequence::Copy;
    defaultShortcuts["粘贴"] = QKeySequence::Paste;
    defaultShortcuts["全选"] = QKeySequence::SelectAll;
    defaultShortcuts["查找"] = QKeySequence::Find;
    defaultShortcuts["查找下一个"] = QKeySequence::FindNext;
    defaultShortcuts["查找上一个"] = QKeySequence::FindPrevious;
    defaultShortcuts["替换"] = QKeySequence::Replace;

    // 初始化当前快捷键
    currentShortcuts = defaultShortcuts;
}

void ShortcutDialog::onShortcutButtonClicked()
{
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (!button) return;

    currentButton = button;
    button->setText(tr("请按下新的快捷键..."));
    button->setFocus();
}

void ShortcutDialog::onResetButtonClicked()
{
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (!button) return;

    QString shortcutName = button->property("shortcutName").toString();
    QKeySequence defaultShortcut = defaultShortcuts[shortcutName];
    currentShortcuts[shortcutName] = defaultShortcut;
    updateShortcutButton(button, defaultShortcut);
}

void ShortcutDialog::onDefaultButtonClicked()
{
    currentShortcuts = defaultShortcuts;
    // 更新所有按钮显示
    for (auto it = currentShortcuts.begin(); it != currentShortcuts.end(); ++it) {
        QPushButton* button = findChild<QPushButton*>(QString(), Qt::FindChildrenRecursively);
        if (button && button->property("shortcutName").toString() == it.key()) {
            updateShortcutButton(button, it.value());
        }
    }
}

void ShortcutDialog::updateShortcutButton(QPushButton* button, const QKeySequence& shortcut)
{
    if (!button) return;
    button->setText(shortcut.toString());
}

bool ShortcutDialog::isShortcutConflict(const QKeySequence& shortcut)
{
    for (auto it = currentShortcuts.begin(); it != currentShortcuts.end(); ++it) {
        if (it.value() == shortcut) {
            return true;
        }
    }
    return false;
}

void ShortcutDialog::keyPressEvent(QKeyEvent* event)
{
    if (!currentButton) {
        QDialog::keyPressEvent(event);
        return;
    }

    int key = event->key();
    Qt::KeyboardModifiers modifiers = event->modifiers();

    // 忽略单独的修饰键
    if (key == Qt::Key_Control || key == Qt::Key_Shift || 
        key == Qt::Key_Alt || key == Qt::Key_Meta) {
        return;
    }

    // 创建新的快捷键
    QKeySequence newShortcut(key | modifiers);
    QString shortcutName = currentButton->property("shortcutName").toString();

    // 检查冲突
    if (isShortcutConflict(newShortcut)) {
        QMessageBox::warning(this, tr("快捷键冲突"),
            tr("该快捷键已被其他功能使用，请选择其他快捷键。"));
        return;
    }

    // 更新快捷键
    currentShortcuts[shortcutName] = newShortcut;
    updateShortcutButton(currentButton, newShortcut);
    currentButton = nullptr;
} 