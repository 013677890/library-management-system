#ifndef CELLUSERMANAGEMENT_H
#define CELLUSERMANAGEMENT_H

#include <QWidget>
#include <QStandardItemModel>
namespace Ui {
class CellUserManagement;
}

class CellUserManagement : public QWidget
{
    Q_OBJECT

public:
    explicit CellUserManagement(QWidget *parent = nullptr);
    ~CellUserManagement();
    //初始化数据库已有数据

private slots:
    void on_UserImportBtton_clicked();


    void on_UserDeleteButton_clicked();

    void on_lineEdit_returnPressed();

private:
    Ui::CellUserManagement *ui;
    QStandardItemModel *m_model;
    void InitDatabaseData();


};

#endif // CELLUSERMANAGEMENT_H
