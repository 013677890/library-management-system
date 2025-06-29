// draggablelabel.h
#ifndef DRAGGABLELABEL_H
#define DRAGGABLELABEL_H

#include <QLabel>

class DraggableLabel : public QLabel
{
    Q_OBJECT
public:
    explicit DraggableLabel(QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    QPoint m_dragStartPosition;     // 鼠标按下位置（全局坐标）
    QPoint m_windowStartPosition;   // 窗口起始位置
};

#endif // DRAGGABLELABEL_H
