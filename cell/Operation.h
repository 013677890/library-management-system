#ifndef OPERATION_H
#define OPERATION_H
#include <QTableView>
#include <QModelIndex>
#include <QDateTime>
//判断焦点行函数
inline int getFocusedRow(QTableView *tableView) {
    QModelIndex currentIndex = tableView->currentIndex();
    if (currentIndex.isValid()) {  // 判断索引是否有效，有效才获取行号
        return currentIndex.row();
    }
    return -1;  // 无效时返回 -1 表示无焦点行
}
//获取时间函数 时间格式为2025-06-28 07:09:41
inline QString getCurrentTime() {
    return QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
}

#endif // OPERATION_H
