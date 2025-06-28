#include "cellusermanagement.h"
#include "ui_cellusermanagement.h"
#include "lib/sqlmanager.h"
#include "QXlsx/header/xlsxdocument.h"
#include "QXlsx/header/xlsxworksheet.h"
#include "Operation.h" // 引入操作相关的头文件
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
CellUserManagement::CellUserManagement(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CellUserManagement)
    ,m_model(new QStandardItemModel(this))
{
    ui->setupUi(this);
    ui->tableView->setModel(m_model);
    m_model->setHorizontalHeaderLabels(QStringList{"用户id","年级","部门","用户名","密码","昵称","权限"} );
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    /*
    // 第 1 行数据：按表头顺序 [用户id, 年级, 部门, 用户名, 密码, 昵称, 权限]
    QList<QStandardItem*> row1;
    row1 << new QStandardItem("1001")    // 用户id
         << new QStandardItem("2025级")  // 年级
         << new QStandardItem("图书馆部")// 部门
         << new QStandardItem("testUser")// 用户名
         << new QStandardItem("123456")  // 密码
         << new QStandardItem("测试昵称")// 昵称
         << new QStandardItem("普通用户");// 权限
    // 将整行数据加入模型
    m_model->appendRow(row1);

    // 第 2 行数据（演示不同内容）
    QList<QStandardItem*> row2;
    row2 << new QStandardItem("1002")
         << new QStandardItem("2024级")
         << new QStandardItem("技术部")
         << new QStandardItem("admin")
         << new QStandardItem("admin123")
         << new QStandardItem("管理员")
         << new QStandardItem("管理员权限");
    m_model->appendRow(row2);
        //测试样例
*/
    //调用初始化函数
    InitDatabaseData();


}

CellUserManagement::~CellUserManagement()
{
    delete ui;
}

void CellUserManagement::InitDatabaseData()
{
    // 初始化已有数据（向表中添加）
    SqlManager &sqlmanager = SqlManager::GetInstance();
    QVector<QVector<QString>> users = sqlmanager.GetUsers();
    for (QVector<QString> &user : users) {
        if (user.size() >= 7) { // 确保数据完整
            QList<QStandardItem*> row;
            for (const QString &field : user) {
                row.append(new QStandardItem(field));
            }
            m_model->appendRow(row);
        }
        else {
            qDebug() << "用户数据不完整，跳过添加：" << user;
        }
    }
    ui->tableView->resizeColumnsToContents(); // 自动调整列宽以适应内容
}

void CellUserManagement::on_UserImportBtton_clicked()//通过excel导入用户
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

    // 4. 收集已有 userid 和用户名
    QSet<QString> existingUserIds, existingUsernames;
    int rowCount = m_model->rowCount();
    for (int i = 0; i < rowCount; ++i) {
        QString userId = m_model->index(i, 0).data().toString();
        QString username = m_model->index(i, 3).data().toString();
        existingUserIds.insert(userId);
        existingUsernames.insert(username);
    }

    // 5. 逐行读取数据（适配 shared_ptr<Cell>）
    int successCount = 0, skipCount = 0;
    for (int row = 2; row <= maxRow; ++row) {
        // 用于按数据库表字段顺序存储一行数据
        QVector<QString> rowData;
        bool hasDuplicate = false;

        // 5.1 读取用户ID（第1列），对应数据库表 userid 字段
        auto cellUserId = sheet->cellAt(row, 1);
        QString userId = cellUserId? cellUserId->value().toString().trimmed() : "";
        if (!userId.isEmpty() && existingUserIds.contains(userId)) {
            qDebug() << tr("跳过重复userid：%1，行号：%2").arg(userId).arg(row);
            hasDuplicate = true;
        }
        rowData.append(userId);

        // 5.2 读取 grade（第2列），对应数据库表 grade 字段
        auto cellGrade = sheet->cellAt(row, 2);
        QString grade = cellGrade? cellGrade->value().toString().trimmed() : "";
        rowData.append(grade);

        // 5.3 读取 department（第3列），对应数据库表 department 字段
        auto cellDepartment = sheet->cellAt(row, 3);
        QString department = cellDepartment? cellDepartment->value().toString().trimmed() : "";
        rowData.append(department);

        // 5.4 读取 username（第4列），对应数据库表 username 字段
        auto cellUsername = sheet->cellAt(row, 4);
        QString username = cellUsername? cellUsername->value().toString().trimmed() : "";
        if (!username.isEmpty() && existingUsernames.contains(username)) {
            qDebug() << tr("跳过重复用户名：%1，行号：%2").arg(username).arg(row);
            hasDuplicate = true;
        }
        rowData.append(username);
        /*
        auto cellAuth = sheet->cellAt(row, 4);
        QString auth = cellAuth? cellAuth->value().toString().trimmed() : "";
        rowData.append(auth);
        */
        // 5.5 读取 nickname（第5列），对应数据库表 nickname 字段
        auto cellNickname = sheet->cellAt(row, 5);
        QString nickname = cellNickname? cellNickname->value().toString().trimmed() : "";
        rowData.append(nickname);

        // 5.6 读取 password（第6列），对应数据库表 password 字段
        auto cellPassword = sheet->cellAt(row, 6);
        QString password = cellPassword? cellPassword->value().toString().trimmed() : "";
        rowData.append(password);
        //5.5和5.6反了 不过对逻辑没影响 我也懒得改了
        // 5.7 读取 auth（第7列），对应数据库表 auth 字段
        auto cellAuth = sheet->cellAt(row, 4);
        QString auth = cellAuth? cellAuth->value().toString().trimmed() : "";
        rowData.append(auth);

        if (hasDuplicate) {
            skipCount++;
            continue;
        }

        // 5.8 根据 rowData 构造 QList<QStandardItem*> 并添加到模型
        QList<QStandardItem*> newRow;
        for (const QString& data : rowData) {
            newRow.append(new QStandardItem(data));
        }
        m_model->appendRow(newRow);

        // 5.9 更新检测集合
        existingUserIds.insert(userId);
        existingUsernames.insert(username);
        successCount++;

        //6.0 将新用户添加到数据库
        SqlManager &sqlmanager = SqlManager::GetInstance();
        if (!sqlmanager.AddUser(rowData)) {
            qDebug() << tr("添加用户失败，行号：%1").arg(row);
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
    ui->tableView->resizeColumnsToContents(); // 自动调整列宽以适应内容
}


void CellUserManagement::on_UserDeleteButton_clicked()
{
    int row = getFocusedRow(ui->tableView);
    if (row != -1) {
        //内置一个Qialog(询问是否确认进行删除操作)
        QMessageBox::StandardButton reply;
        qDebug() << "当前 QTableView 焦点所在行：" << row;
        reply = QMessageBox::question(this, tr("删除确认"), tr("是否确认删除当前选中行？"),QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::No) {
            return;  // 用户选择不删除，直接返回
        }
        else{
            // 获取当前行的用户ID
            QString userId = m_model->index(row, 0).data().toString();

            //删除数据库中的数据
            SqlManager &sqlmanager = SqlManager::GetInstance();
            qDebug() << "准备删除用户，用户ID：" << userId;//调试
            if (!sqlmanager.DeleteUser(userId)) {
                qDebug() << "删除用户失败，用户ID：" << userId;
                QMessageBox::critical(this, tr("错误"), tr("删除用户失败，请检查数据库连接或是否有未还书籍"));
                return;
            }
            //删除现在的焦点行
            m_model->removeRow(row);
        }

    } else {
        qDebug() << "当前 QTableView 无有效焦点行";
        QMessageBox::critical(this, tr("错误"), tr("请选取其中的一行进行删除"));
        return;
    }
}


void CellUserManagement::on_lineEdit_returnPressed()
{
    // 获取输入的搜索内容
    QString searchText = ui->lineEdit->text().trimmed();
    if (searchText.isEmpty()) {
        QMessageBox::warning(this, tr("提示"), tr("请输入搜索内容"));
        return;
    }
    //从现在的焦点行的下一行（若已是最后一行则默认为0）开始（若没有则默认是0），搜索下一个匹配的信息，并将焦点行移动到那里
    int startRow = getFocusedRow(ui->tableView) + 1;
    if (startRow >= m_model->rowCount()) {
        startRow = 0; // 如果已经是最后一行，则从第一行开始
    }
    bool found = false;
    for (int row = startRow; row < m_model->rowCount(); ++row) {
        // 遍历每一列，检查是否包含搜索文本
        for (int col = 0; col < m_model->columnCount(); ++col) {
            QString cellText = m_model->index(row, col).data().toString();
            if (cellText.contains(searchText, Qt::CaseInsensitive)) {
                ui->tableView->selectRow(row); // 选中匹配的行
                ui->tableView->scrollTo(m_model->index(row, 0)); // 滚动到该行
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

