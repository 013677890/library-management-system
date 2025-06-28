#include "cell_main.h"
#include "dlg_login.h"
#include <QApplication>
#include "lib/sqlmanager.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    SqlManager &sqlmanager=SqlManager::GetInstance();
    sqlmanager.Print();
       //test实例
    bool ok=sqlmanager.InitDatabase();
    if(!ok){
        exit(0);
        return 0;
    }
    Dlg_Login d;
    d.show();
    int ret=d.exec();
    if(1==ret){
        cell_main w;
        w.show();
        return a.exec();
    }
    if(0==ret){
        exit(0);
        return 0;
    }
    return 0;
}
