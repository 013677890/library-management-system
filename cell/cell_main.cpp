#include "cell_main.h"
#include "ui_cell_main.h"
#include <QIcon>
#include <QVBoxLayout>
#include <QDebug>
#include <QButtonGroup>

cell_main::cell_main(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::cell_main)
    , m_stackedWidget(nullptr)
    , m_currentButton(nullptr)
    , cellusermanagement(new CellUserManagement(this))
    , cellbookmanagement(new CellBookManagement(this))
    , cellborrowingrecord(new CellBorrowingRecord(this))
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/new/prefix1/qrcpicture/logo.ico"));
    // 初始化页面索引（假设用户管理页面索引为0，图书管理页面索引为1，借阅记录页面索引为2）
    m_stackedWidget = ui->stackedWidget;
    m_stackedWidget->addWidget(cellusermanagement);
    m_stackedWidget->addWidget(cellbookmanagement);
    m_stackedWidget->addWidget(cellborrowingrecord);
    m_stackedWidget->setCurrentIndex(0); // 默认显示用户管理页面
    QButtonGroup* buttonGroup = new QButtonGroup(this);

    buttonGroup->addButton(ui->UserManagementButton, 0);  // 第二个参数是按钮的ID，对应stackedWidget的索引
    buttonGroup->addButton(ui->BookManagementButton, 1);
    buttonGroup->addButton(ui->BorrowingRecordButton, 2);
    ui->UserManagementButton->setCheckable(true);
    ui->BookManagementButton->setCheckable(true);
    ui->BorrowingRecordButton->setCheckable(true);
    buttonGroup->setExclusive(true);
    ui->UserManagementButton->setChecked(true);

    //在这里连接自定义的信号与槽（图书归还之后，图书数量+1）
    connect(cellborrowingrecord, &CellBorrowingRecord::bookReturned, cellbookmanagement, &CellBookManagement::onBookReturned);
}

cell_main::~cell_main()
{
    delete ui;
}

void cell_main::on_UserManagementButton_clicked()
{
    m_stackedWidget->setCurrentIndex(0); // 切换回用户管理页面
}


void cell_main::on_BookManagementButton_clicked()
{
    m_stackedWidget->setCurrentIndex(1); // 切换到图书管理页面
}


void cell_main::on_BorrowingRecordButton_clicked()
{
    m_stackedWidget->setCurrentIndex(2); // 切换到借阅记录页面
    //从数据库中更新借阅记录信息
    cellborrowingrecord->InitDatabaseData(); // 初始化借阅记录数据
}
