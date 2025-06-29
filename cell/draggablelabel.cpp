#include "draggablelabel.h"
#include <QLabel>
#include <QMouseEvent>
#include <QWidget> // 新增，用于访问父窗口

DraggableLabel::DraggableLabel(QWidget *parent) : QLabel(parent) {
    setMouseTracking(true);
}

void DraggableLabel::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_dragStartPosition = event->globalPos(); // 保存全局坐标
        m_windowStartPosition = parentWidget()->pos(); // 保存窗口起始位置
    }
    QLabel::mousePressEvent(event);
}

void DraggableLabel::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        // 计算鼠标移动距离（全局坐标）
        QPoint delta = event->globalPos() - m_dragStartPosition;
        // 移动父窗口（主窗口）
        parentWidget()->move(m_windowStartPosition + delta);
    }
    QLabel::mouseMoveEvent(event);
}
