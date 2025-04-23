#ifndef SHORTCUTDIALOG_H
#define SHORTCUTDIALOG_H

#include <QDialog>
#include <QMap>
#include <QKeySequence>
#include <QKeyEvent>

namespace Ui {
class ShortcutDialog;
}

class ShortcutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ShortcutDialog(QWidget *parent = nullptr);
    ~ShortcutDialog();

    // 设置当前快捷键
    void setShortcuts(const QMap<QString, QKeySequence>& shortcuts);
    // 获取修改后的快捷键
    QMap<QString, QKeySequence> getShortcuts() const;

protected:
    // 重写键盘事件处理函数
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void onShortcutButtonClicked();
    void onResetButtonClicked();
    void onDefaultButtonClicked();

private:
    Ui::ShortcutDialog *ui;
    QMap<QString, QKeySequence> currentShortcuts;
    QMap<QString, QKeySequence> defaultShortcuts;
    QPushButton* currentButton;

    void setupDefaultShortcuts();
    void updateShortcutButton(QPushButton* button, const QKeySequence& shortcut);
    bool isShortcutConflict(const QKeySequence& shortcut);
};

#endif // SHORTCUTDIALOG_H 