#ifndef DLG_LOGIN_H
#define DLG_LOGIN_H

#include <QDialog>
#include <QPainter>
#include <QStyleOption>


namespace Ui {
class Dlg_Login;
}

class Dlg_Login : public QDialog
{
    Q_OBJECT

public:
    explicit Dlg_Login(QWidget *parent = nullptr);
    ~Dlg_Login();
    void paintEvent(QPaintEvent *event);

private slots:
    void on_LoginButton_clicked();

    void on_ToMinButton_clicked();

    void on_CloseButton_clicked();

private:
    Ui::Dlg_Login *ui;


};

#endif // DLG_LOGIN_H
