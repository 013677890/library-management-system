#ifndef CELLBOOKMANAGEMENT_H
#define CELLBOOKMANAGEMENT_H

#include <QWidget>
#include <QStandardItemModel>
namespace Ui {
class CellBookManagement;
}

class CellBookManagement : public QWidget
{
    Q_OBJECT

public:
    explicit CellBookManagement(QWidget *parent = nullptr);
    ~CellBookManagement();

private slots:
    void on_BookAddButton_clicked();
    void on_pushButton_clicked(); // 用于处理按钮点击事件

    void on_BookDeleteButton_clicked();

    void on_BookBorrowButton_clicked();

    void on_lineEdit_returnPressed();
public slots:
    void onBookReturned(QString bookId); // 响应图书归还信号
private:
    Ui::CellBookManagement *ui;
    QStandardItemModel *m_model; // 模型用于存储图书数据
    void InitDatabaseData();
};

#endif // CELLBOOKMANAGEMENT_H
