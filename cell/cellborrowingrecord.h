#ifndef CELLBORROWINGRECORD_H
#define CELLBORROWINGRECORD_H

#include <QWidget>
#include <QStandardItemModel>
namespace Ui {
class CellBorrowingRecord;
}

class CellBorrowingRecord : public QWidget
{
    Q_OBJECT

public:
    explicit CellBorrowingRecord(QWidget *parent = nullptr);
    ~CellBorrowingRecord();
    void InitDatabaseData();//其实应该还是private的 但是因为需要在cell_main.cpp中调用 所以放到public了
//信号
signals:
    void bookReturned(const QString &bookId);  // 添加这个信号

private slots:
    void on_lineEdit_returnPressed();

    void on_BookReturnButton_clicked();

    void on_ClearButton_clicked();

private:
    Ui::CellBorrowingRecord *ui;
    QStandardItemModel *m_model; // 模型用于存储借阅记录数据
    // 初始化已有数据


};

#endif // CELLBORROWINGRECORD_H
