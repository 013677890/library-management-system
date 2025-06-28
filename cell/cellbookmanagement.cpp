#include "cellbookmanagement.h"
#include "ui_cellbookmanagement.h"
#include "lib/sqlmanager.h"
#include "QXlsx/header/xlsxdocument.h"
#include "QXlsx/header/xlsxworksheet.h"
#include <QInputDialog>
#include "Operation.h" // 引入操作相关的头文件
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

CellBookManagement::CellBookManagement(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CellBookManagement)
    , m_model(new QStandardItemModel(this)) // 初始化模型
{
    ui->setupUi(this);
    // 设置模型到视图
    ui->tableView_2->setModel(m_model);
    // 设置表头标签
    m_model->setHorizontalHeaderLabels(QStringList{"图书ID", "书名", "价格", "标签1", "标签2", "标签3", "库存数量"});
    // 设置表格属性
    ui->tableView_2->setEditTriggers(QAbstractItemView::NoEditTriggers); // 禁止编辑
    ui->tableView_2->setSelectionBehavior(QAbstractItemView::SelectRows); // 行选择
    //表头为bookid，name，price，type1，type2，type3，cnt（其中type可为空）
    // 调用初始化函数
    InitDatabaseData();

}

CellBookManagement::~CellBookManagement()
{
    delete ui;
}

void CellBookManagement::InitDatabaseData()
{
    // 初始化已有数据（向表中添加）
    SqlManager &sqlmanager = SqlManager::GetInstance();
    QVector<QVector<QString>> books = sqlmanager.GetBooks();
    for (QVector<QString> &book : books) {
        if (book.size() >= 7) { // 确保数据完整
            QList<QStandardItem*> row;
            for (const QString &field : book) {
                row.append(new QStandardItem(field));
            }
            m_model->appendRow(row);
        }
    }
    ui->tableView_2->resizeColumnsToContents(); // 自动调整列宽以适应内容
}

void CellBookManagement::on_BookAddButton_clicked()
{
    // 1. 选择 Excel 文件
    QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("选择Excel文件"),
        "",
        tr("Excel文件 (*.xlsx *.xls)")
        );
    if (filePath.isEmpty()) return;

    // 2. 加载 Excel 文档
    QXlsx::Document xlsx(filePath);
    qDebug() << "选择的文件路径:" << filePath;
    QXlsx::Worksheet *sheet = dynamic_cast<QXlsx::Worksheet*>(xlsx.sheet("Sheet1"));
    if (!sheet) {
        QMessageBox::critical(this, tr("错误"), tr("无法加载Excel文件或无有效工作表"));
        return;
    }

    // 3. 获取数据范围
    QXlsx::CellRange range = sheet->dimension();
    int maxRow = range.rowCount();
    int maxCol = range.columnCount();
    if (maxCol < 7) {
        QMessageBox::critical(this, tr("错误"), tr("Excel表头需至少7列"));
        return;
    }

    // 4. 收集已有 bookid
    QSet<QString> existingBookIds;
    int rowCount = m_model->rowCount();
    for (int i = 0; i < rowCount; ++i) {
        QString bookId = m_model->index(i, 0).data().toString();
        existingBookIds.insert(bookId);
    }

    // 5. 逐行读取数据
    int successCount = 0, skipCount = 0;
    for (int row = 2; row <= maxRow; ++row) {
        // 用于按数据库表字段顺序存储一行数据
        QVector<QString> rowData;
        bool hasDuplicate = false;

        // 5.1 读取图书ID（第1列）
        auto cellBookId = sheet->cellAt(row, 1);
        QString bookId = cellBookId ? cellBookId->value().toString().trimmed() : "";
        if (!bookId.isEmpty() && existingBookIds.contains(bookId)) {
            qDebug() << tr("跳过重复bookid：%1，行号：%2").arg(bookId).arg(row);
            hasDuplicate = true;
            skipCount++;
            continue;
        }
        rowData.append(bookId);

        // 5.2 读取书名（第2列）
        auto cellName = sheet->cellAt(row, 2);
        QString name = cellName ? cellName->value().toString().trimmed() : "";
        rowData.append(name);

        // 5.3 读取价格（第3列）
        auto cellPrice = sheet->cellAt(row, 3);
        QString price = cellPrice ? cellPrice->value().toString().trimmed() : "";
        rowData.append(price);

        // 5.4 读取标签1（第4列）
        auto cellType1 = sheet->cellAt(row, 4);
        QString type1 = cellType1 ? cellType1->value().toString().trimmed() : "";
        rowData.append(type1);

        // 5.5 读取标签2（第5列）
        auto cellType2 = sheet->cellAt(row, 5);
        QString type2 = cellType2 ? cellType2->value().toString().trimmed() : "";
        rowData.append(type2);

        // 5.6 读取标签3（第6列）
        auto cellType3 = sheet->cellAt(row, 6);
        QString type3 = cellType3 ? cellType3->value().toString().trimmed() : "";
        rowData.append(type3);

        // 5.7 读取库存数量（第7列）
        auto cellCnt = sheet->cellAt(row, 7);
        QString cnt = cellCnt ? cellCnt->value().toString().trimmed() : "";
        rowData.append(cnt);

        // 5.8 根据 rowData 构造 QList<QStandardItem*> 并添加到模型
        QList<QStandardItem*> newRow;
        for (const QString& data : rowData) {
            newRow.append(new QStandardItem(data));
        }
        m_model->appendRow(newRow);

        // 5.9 更新检测集合
        existingBookIds.insert(bookId);
        successCount++;

        // 6.0 将新图书添加到数据库
        SqlManager &sqlmanager = SqlManager::GetInstance();
        if (!sqlmanager.AddBook(rowData)) {
            qDebug() << tr("添加图书失败，行号：%1").arg(row);
            continue; // 如果添加失败，跳过此行
        }
    }

    // 6. 提示结果
    QMessageBox::information(
        this,
        tr("导入结果"),
        tr("导入完成！成功导入 %1 条，跳过 %2 条重复数据").arg(successCount).arg(skipCount)
        );

    // 7. 自动调整列宽
    ui->tableView_2->resizeColumnsToContents();
}

void CellBookManagement::on_pushButton_clicked()
{
    /*
     * 这里为什么有个函数没写呢
     * 因为我想把按钮改个名
     * 然后qt不会自动把所有文件中的都改了
     * 导致运行不行了
     * 所以这个函数是留着占位的
     */
}

void CellBookManagement::on_BookDeleteButton_clicked()
{
    //与 CellUserManagement 类似
    int row = getFocusedRow(ui->tableView_2); // 获取当前焦点行
    if (row < 0) {
        QMessageBox::critical(this, tr("错误"), tr("请选取其中的一行进行删除"));
        return;
    }
    // 获取当前行的图书ID
    QString bookId = m_model->index(row, 0).data().toString();
    // 弹出确认对话框
    QMessageBox::StandardButton reply;
    qDebug() << "当前 QTableView 焦点所在行：" << row;
    reply = QMessageBox::question(this, tr("删除确认"), tr("是否确认删除当前选中行？"), QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::No) {
        return;  // 用户选择不删除，直接返回
    }
    // 删除现在的焦点行
    m_model->removeRow(row);
    // 删除数据库中的数据
    SqlManager &sqlmanager = SqlManager::GetInstance();
    qDebug() << "准备删除图书，图书ID：" << bookId; // 调试
    if (!sqlmanager.DeleteBook(bookId)) {
        qDebug() << "删除图书失败，图书ID：" << bookId;
        QMessageBox::critical(this, tr("错误"), tr("删除图书失败，请检查日志"));
        return;
    }
    qDebug() << "删除图书成功，图书ID：" << bookId; // 调试
    QMessageBox::information(this, tr("成功"), tr("图书删除成功！"));
    ui->tableView_2->resizeColumnsToContents(); // 自动调整列宽以适应内容
}


void CellBookManagement::on_BookBorrowButton_clicked()
{
    //被借阅的图书即为当前焦点行的图书
    int row = getFocusedRow(ui->tableView_2); // 获取当前焦点行
    //弹出一个dialog对话框 然后里边用输入框输入用户ID
    if (row < 0) {
        QMessageBox::critical(this, tr("错误"), tr("请选取其中的一行进行借阅"));
        return;
    }
    QString bookId = m_model->index(row, 0).data().toString();
    QString userId = QInputDialog::getText(this, tr("借阅图书"), tr("请输入用户ID："));
    if (userId.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("用户ID不能为空！"));
        return;
    }
    SqlManager &sqlmanager = SqlManager::GetInstance();
    if (!sqlmanager.BorrowBook(bookId, userId)) {
        QMessageBox::critical(this, tr("错误"), tr("借阅图书失败，请检查日志"));
        return;
    }
    QMessageBox::information(this, tr("成功"), tr("图书借阅成功！"));
    //在现在显示的图书里 也减去现在借的这一本
    int currentCount = m_model->index(row, 6).data().toString().toInt();
    m_model->setData(m_model->index(row, 6), currentCount - 1); // 更新库存数量

}


void CellBookManagement::on_lineEdit_returnPressed()
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

void CellBookManagement::onBookReturned(QString bookId)
{
    // 查找匹配的图书ID并增加库存
    for (int row = 0; row < m_model->rowCount(); ++row) {
        if (m_model->index(row, 0).data().toString() == bookId) {
            int currentCount = m_model->index(row, 6).data().toInt();
            m_model->setData(m_model->index(row, 6), currentCount + 1);
            break;
        }
    }
}

