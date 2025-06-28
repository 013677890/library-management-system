#include "cellborrowingrecord.h"
#include "ui_cellborrowingrecord.h"
#include "lib/sqlmanager.h"
#include "Operation.h" // 引入操作相关的头文件
#include <QMessageBox>

CellBorrowingRecord::CellBorrowingRecord(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CellBorrowingRecord)
    , m_model(new QStandardItemModel(this)) // 初始化模型
{
    ui->setupUi(this);
    // 设置模型到视图
    ui->tableView_2->setModel(m_model);
    // 设置表头标签
    m_model->setHorizontalHeaderLabels(QStringList{"借阅记录ID", "用户ID", "图书ID", "借阅时间", "归还时间"});
    // 设置表格属性
    ui->tableView_2->setEditTriggers(QAbstractItemView::NoEditTriggers); // 禁止编辑
    ui->tableView_2->setSelectionBehavior(QAbstractItemView::SelectRows); // 行选择
    // 调用初始化函数
    //InitDatabaseData();不在这初始化了 要求在切换到这个页面的时候才初始化

}

CellBorrowingRecord::~CellBorrowingRecord()
{
    delete ui;
}

void CellBorrowingRecord::on_lineEdit_returnPressed()
{
    // 获取输入的搜索内容
    QString searchText = ui->lineEdit->text().trimmed();
    if (searchText.isEmpty()) {
        QMessageBox::warning(this, tr("提示"), tr("请输入搜索内容"));
        return;
    }
    //从现在的焦点行的下一行（若已是最后一行则默认为0）开始（若没有则默认是0），搜索下一个匹配的信息，并将焦点行移动到那里
    int startRow = getFocusedRow(ui->tableView_2) + 1;
    if (startRow >= m_model->rowCount()) {
        startRow = 0; // 如果已经是最后一行，则从第一行开始
    }
    bool found = false;
    for (int row = startRow; row < m_model->rowCount(); ++row) {
        // 遍历每一列，检查是否包含搜索文本
        for (int col = 0; col < m_model->columnCount(); ++col) {
            QString cellText = m_model->index(row, col).data().toString();
            if (cellText.contains(searchText, Qt::CaseInsensitive)) {
                ui->tableView_2->selectRow(row); // 选中匹配的行
                ui->tableView_2->scrollTo(m_model->index(row, 0)); // 滚动到该行
                found = true;
                break; // 找到后跳出内层循环
            }
        }
        if (found) break; // 找到后跳出外层循环
    }
    if (!found) {
        QMessageBox::information(this, tr("提示"), tr("未找到匹配的用户信息"));
        ui->lineEdit->clear(); // 清空搜索框
        ui->lineEdit->setFocus(); // 设置焦点到搜索框
    } else {
        qDebug() << "搜索成功，焦点行已移动到匹配行";
    }
}


void CellBorrowingRecord::on_BookReturnButton_clicked()
{
    // 获取焦点行
    int row = getFocusedRow(ui->tableView_2);
    if (row < 0) {
        QMessageBox::critical(this, tr("错误"), tr("请选取其中的一行进行归还"));
        return;
    }
    QString bookId = m_model->index(row, 2).data().toString(); // 获取图书ID
    qDebug() << "准备归还图书，图书ID：" << bookId; // 调试信息
    QString userId = m_model->index(row, 1).data().toString(); // 获取用户ID
    qDebug() << "准备归还图书，用户ID：" << userId; // 调试信息
    SqlManager &sqlmanager = SqlManager::GetInstance();
    if (!sqlmanager.ReturnBook(bookId, userId)) {
        QMessageBox::critical(this, tr("错误"), tr("归还图书失败，请检查日志"));
        return;
    }
    QMessageBox::information(this, tr("成功"), tr("图书归还成功！"));
    // 更新当前行的归还时间
    QString returnTime = getCurrentTime(); // 获取当前时间作为归还时间
    m_model->setData(m_model->index(row, 4), returnTime); // 更新归还时间列
    // 更新表格
    ui->tableView_2->resizeColumnsToContents(); // 自动调整列宽以适应内容
    emit bookReturned(bookId);
    qDebug() << "图书归还信号已发送，图书ID：" << bookId; // 调试信息
}

void CellBorrowingRecord::InitDatabaseData()
{
    //清除之前表中的数据（保留表头）
    m_model->clear();
    m_model->setHorizontalHeaderLabels(QStringList{"借阅记录ID", "用户ID", "图书ID", "借阅时间", "归还时间"});
    // 设置表格属性
    ui->tableView_2->setEditTriggers(QAbstractItemView::NoEditTriggers); // 禁止编辑
    ui->tableView_2->setSelectionBehavior(QAbstractItemView::SelectRows); // 行选择
    // 初始化已有数据（向表中添加）
    SqlManager &sqlmanager = SqlManager::GetInstance();
    QVector<QVector<QString>> records = sqlmanager.GetBorrowRecords();
    for (QVector<QString> &record : records) {
        if (record.size() >= 5) { // 确保数据完整
            QList<QStandardItem*> row;
            for (const QString &field : record) {
                row.append(new QStandardItem(field));
            }
            m_model->appendRow(row);
        }
    }
    ui->tableView_2->resizeColumnsToContents(); // 自动调整列宽以适应内容
}


void CellBorrowingRecord::on_ClearButton_clicked()
{
    //检验是否有归还时间不存在的（即未归还）
    //检查tableview
    for (int row = 0; row < m_model->rowCount(); ++row) {
        QString endTime = m_model->index(row, 4).data().toString();
        if (endTime.isEmpty()) {
            QMessageBox::warning(this, tr("警告"), tr("存在未归还的图书，无法清空记录"));
            return;
        }
    }
    // 清空记录
    SqlManager &sqlmanager = SqlManager::GetInstance();
    if (!sqlmanager.ClearBorrowRecords()) {
        QMessageBox::critical(this, tr("错误"), tr("清空借阅记录失败，请检查日志"));
        return;
    }
    m_model->clear(); // 清空模型数据
    m_model->setHorizontalHeaderLabels(QStringList{"借阅记录ID", "用户ID", "图书ID", "借阅时间", "归还时间"}); // 重设表头
    QMessageBox::information(this, tr("成功"), tr("借阅记录已清空！"));
    ui->tableView_2->resizeColumnsToContents(); // 自动调整列宽以适应内容
}

