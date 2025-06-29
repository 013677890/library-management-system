#ifndef CELL_MAIN_H
#define CELL_MAIN_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include "cellbookmanagement.h"
#include "cellborrowingrecord.h"
#include "cellusermanagement.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class cell_main;
}
QT_END_NAMESPACE

class cell_main : public QMainWindow
{
    Q_OBJECT

public:
    cell_main(QWidget *parent = nullptr);
    ~cell_main();

private slots:
    void on_UserManagementButton_clicked();
    void on_BookManagementButton_clicked();
    void on_BorrowingRecordButton_clicked();

private:
    Ui::cell_main *ui;
    QStackedWidget *m_stackedWidget; // 页面栈
    QPushButton *m_currentButton; // 当前选中按钮
    CellBookManagement *cellbookmanagement;
    CellUserManagement *cellusermanagement;
    CellBorrowingRecord *cellborrowingrecord;

    void setupConnections();         // 初始化信号槽连接
    void highlightButton(QPushButton *btn); // 高亮按钮
    void resetButtonsStyle();        // 重置所有按钮样式
};
#endif // CELL_MAIN_H
