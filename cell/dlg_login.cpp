#include "dlg_login.h"
#include "ui_dlg_login.h"
#include "lib/sqlmanager.h"
#include <QWidget>
#include <QPainter>
#include <QStyleOption>
#include <QStyle>
#include <QLabel>
#include <QPixmap>
#include <QImage>
#include <QPainterPath>
#include <QIcon>
#include <QMouseEvent>
Dlg_Login::Dlg_Login(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dlg_Login)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint);
    ui->setupUi(this);
    // ========== 标签统一优化 ==========
    // 标题文字（图书借阅管理系统）
    ui->LibraryLabel->setStyleSheet(R"(
    color: #444444;
    font-family: "华文行楷", "方正静蕾简体", serif;
    font-size: 34px;
    font-weight: bold;
    letter-spacing: 2px;
    padding-bottom: 12px;
    border-bottom: 1px dashed #E0E0E0; /* 虚线下划线，更文艺 */
)");

    // 表单标签（用户名、密码）
    QString labelStyle = R"(
    color: #555;
    font-size: 14px;
    padding-right: 8px;
    font-family: "Microsoft YaHei"; /* 统一字体，适配系统 */
)";
    ui->UserLabel->setStyleSheet(labelStyle);
    ui->PasswordLabel->setStyleSheet(labelStyle);
    ui->RegisterLabel->setStyleSheet(labelStyle); // 注册区域的提示文字


    // ========== 输入框优化 ==========
    QString lineEditStyle = R"(
QLineEdit {
    background-color: rgba(250, 245, 235, 0.8); /* 白色半透，贴合插画浅色背景 */
    border: none;
    border-bottom: 1px solid #ccc; /* 下划线替代边框，更轻盈 */
    border-radius: 0; /* 取消圆角，匹配下划线风格 */
    padding: 8px 10px;
    color: #333;
    font-size: 14px;
}
QLineEdit:focus {
    border-bottom: 2px solid #0078d7; /* 聚焦时下划线加粗变色 */
    background-color: rgba(250, 245, 235, 0.95);
    outline: none; /* 清除系统默认聚焦框 */
}
)";
    ui->PasswordEdit->setStyleSheet(lineEditStyle);
    ui->UserEdit->setStyleSheet(lineEditStyle);


    // ========== 按钮优化（登录 + 注册） ==========
    QString buttonStyle = R"(
QPushButton {
    background-color: transparent;
    border: 1px solid #ccc;
    color: #666;
    font-size: 14px;
    font-weight: bold;
    padding: 8px 24px; /* 增大点击区域 */
    margin: 4px;
    border-radius: 20px; /* 圆润胶囊形状，更柔和 */
    transition: all 0.3s ease;
}
QPushButton:hover {
    border-color: #0078d7;
    color: #0078d7;
    text-decoration: underline;
    box-shadow: 0 2px 6px rgba(0, 0, 120, 0.1); /* 悬浮时微阴影，呼应插画层次 */
}
QPushButton:pressed {
    border-color: #005A9E;
    color: #005A9E;
    text-decoration: none;
    background-color: rgba(0, 120, 215, 0.05); /* 按下时浅背景，强化反馈 */
}
)";
    ui->LoginButton->setStyleSheet(buttonStyle);
    setWindowIcon(QIcon(":/new/prefix1/qrcpicture/logo.ico"));

}

Dlg_Login::~Dlg_Login()
{
    delete ui;
}
void Dlg_Login::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing); // 启用抗锯齿，让圆角更平滑
    // 创建圆角矩形路径
    QPainterPath path;
    path.addRoundedRect(rect(), 62, 62); // 使用60px圆角，与样式表保持一致
    // 绘制白色背景
    painter.fillPath(path, QColor(250, 245, 235));
    // 绘制白色边框（1px宽）
    QPen pen(QColor(250, 245, 235), 1);
    painter.setPen(pen);
    painter.drawPath(path);


}

void Dlg_Login::on_LoginButton_clicked()
{
    QString username = ui->UserEdit->text();
    QString password = ui->PasswordEdit->text();
    SqlManager &sqlmanager=SqlManager::GetInstance();
    bool ok=sqlmanager.Login(username, password);
    if (!ok)
    {
        ui->UserEdit->clear();
        ui->PasswordEdit->clear();
        ui->UserEdit->setFocus();
        return;
    }
    else{
    setResult(1);
    hide();
    }
}


void Dlg_Login::on_ToMinButton_clicked()
{
    //窗口最小化
    //QWidget *parentWidget = parentWidget()；不能这么写 强制类型转换
    QWidget *parentWidget = qobject_cast<QWidget *>(parent());
    if (parentWidget) {
        parentWidget->showMinimized(); // 最小化父窗口
    } else {
        showMinimized(); // 如果没有父窗口，则最小化当前窗口
    }
}


void Dlg_Login::on_CloseButton_clicked()
{
    //关闭程序
    QWidget *parentWidget = qobject_cast<QWidget *>(parent());
    if (parentWidget) {
        parentWidget->close(); // 关闭父窗口
    } else {
        close(); // 如果没有父窗口，则关闭当前窗口
    }
}

