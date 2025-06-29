#include "sqlmanager.h"
#include <QDebug>
#include <QCoreApplication>
#include <QSqlQuery>
SqlManager::SqlManager() {}

SqlManager::~SqlManager()
{
    qDebug()<<QCoreApplication::applicationDirPath();
}
SqlManager& SqlManager::GetInstance()
{
    /**
     * 局部静态特性的方式实现单实例。
     * 静态局部变量只在当前函数内有效，其他函数无法访问。
     * 静态局部变量只在第一次被调用的时候初始化，也存储在静态存储区，生命周期从第一次被初始化起至程序结束止。
     */
    static SqlManager sqlManager;
    return sqlManager;
}

void SqlManager::Print()
{
    qDebug() << "我的实例内存地址是:" << this << Qt::endl;
}

bool SqlManager::InitDatabase()
{
    db=QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QCoreApplication::applicationDirPath()+"/db/book.db");
    if (db.open())
    {
        qDebug() << "Database opened successfully！";
        return true;
    }
    else
    {
        qDebug() << "无法打开数据库" ;
        return false;
    }
}

bool SqlManager::Login(const QString &username, const QString &password)
{
    QSqlQuery query;
    query.prepare("SELECT * FROM user WHERE username = :username AND password = :password");
    query.bindValue(":username", username);
    query.bindValue(":password", password);

    if (query.exec() && query.next())
    {
        qDebug() << "登录成功！";
        return true;
    }
    else
    {
        qDebug() << "登录失败！";
        return false;
    }
}

QVector<QVector<QString>> SqlManager::GetUsers()
{
    QVector<QVector<QString>> users;
    QSqlQuery query("SELECT * FROM user");
    while (query.next())
    {
        QVector<QString> user;
        //数据从左到右依次分别为userid,grade,department,auth,nickname,password,username
        user.append(query.value(0).toString()); // userid
        user.append(query.value(1).toString()); // grade
        user.append(query.value(2).toString()); // department
        user.append(query.value(6).toString()); // username
        user.append(query.value(5).toString()); // password
        user.append(query.value(4).toString()); // nickname
        user.append(query.value(3).toString()); // auth
        users.append(user);
    }
    return users;
}

bool SqlManager::AddUser(QVector<QString> userData)
{
    // 检查传入的数据是否至少有7个元素，对应数据库表的7个字段
    if (userData.size() < 7) {
        qWarning() << "传入的用户数据不完整，至少需要7个字段的数据";
        return false;
    }
    //向数据库中添加用户
    //按表头顺序 [用户id, 年级, 部门, 用户名, 密码, 昵称, 权限](为什么有这一行注释，纯粹是搞混了，顺序错了懒得动了)
    QSqlQuery query;
    query.prepare("INSERT INTO user (userid, grade, department, auth, nickname, password, username) "
                  "VALUES (:userid, :grade, :department, :auth, :nickname, :password, :username)");
    query.bindValue(":userid", userData[0]);
    query.bindValue(":grade", userData[1]);
    query.bindValue(":department", userData[2]);
    query.bindValue(":auth", userData[6]);
    query.bindValue(":nickname", userData[5]);
    query.bindValue(":password", userData[4]);
    query.bindValue(":username", userData[3]);
    if (query.exec())
    {
        qDebug() << "添加用户成功！";
        return true;
    }
    else
    {
        qDebug() << "添加用户失败！";
        return false;
    }
}

bool SqlManager::DeleteUser(const QString &strId)
{
    QSqlQuery query;
    //删除用户sql语句
    //如果record表中有该用户的借阅记录（即有用户的id且end为null），则不能删除用户
    query.prepare("DELETE FROM user WHERE userid = :userid AND userid NOT IN (SELECT userid FROM record WHERE end IS NULL)");
    query.bindValue(":userid",strId);
    //如果没有删掉就返回0（借阅的书没还）
    if (query.exec())
    {
        if (query.numRowsAffected() > 0)
        {
            qDebug() << "删除用户成功！";
            return true;
        }
        else
        {
            qDebug() << "删除用户失败，可能是因为该用户有未还书籍！";
            return false;
        }
    }
    else
    {
        qDebug() << "删除用户失败！";
        return false;
    }
}

QVector<QVector<QString> > SqlManager::GetBooks()
{
    QVector<QVector<QString>> books;
    QSqlQuery query("SELECT * FROM book");
    while (query.next())
    {
        QVector<QString> book;
        //表头为bookid，name，price，type1，type2，type3，cnt（其中type可为空）
        book.append(query.value(0).toString()); // bookid
        book.append(query.value(1).toString()); // name
        book.append(query.value(2).toString()); // price
        book.append(query.value(3).toString()); // type1
        book.append(query.value(4).toString()); // type2
        book.append(query.value(5).toString()); // type3
        book.append(query.value(6).toString()); // cnt
        books.append(book);
    }
    return books;
}

bool SqlManager::AddBook(QVector<QString> userData)
{
    //类似AddUser
    QSqlQuery query;
    query.prepare("INSERT INTO book (bookid, name, price, type1, type2, type3, cnt) "
                  "VALUES (:bookid, :name, :price, :type1, :type2, :type3, :cnt)");
    query.bindValue(":bookid", userData[0]);
    query.bindValue(":name", userData[1]);
    query.bindValue(":price", userData[2]);
    query.bindValue(":type1", userData[3]);
    query.bindValue(":type2", userData[4]);
    query.bindValue(":type3", userData[5]);
    query.bindValue(":cnt", userData[6]);
    if (query.exec())
    {
        qDebug() << "添加图书成功！";
        return true;
    }
    else
    {
        qDebug() << "添加图书失败！";
        return false;
    }
}

bool SqlManager::DeleteBook(const QString &strId)
{
    QSqlQuery query;
    //删除图书sql语句
    query.prepare("DELETE FROM book WHERE bookid= :bookid;");
    query.bindValue(":bookid", strId);
    if (query.exec())
    {
        qDebug() << "删除图书成功！";
        return true;
    }
    else
    {
        qDebug() << "删除图书失败！";
        return false;
    }
}

bool SqlManager::ReturnBook(const QString &strId, const QString &userId)
{
    QSqlQuery query;
    //还书sql语句 同时更新库存 同时在record表中加上归还时间
    //还要在book表中更新cnt库存
    query.prepare("UPDATE book SET cnt = cnt + 1 WHERE bookid = :bookid");
    query.bindValue(":bookid", strId);
    if (!query.exec())
    {
        qDebug() << "更新图书库存失败！";
        return false;
    }
    //如果更新成功，则更新借阅记录的end时间
    query.prepare("UPDATE record SET end = datetime(CURRENT_TIMESTAMP, 'localtime') WHERE bookid = :bookid AND userid = :userid AND end IS NULL");
    query.bindValue(":bookid", strId);
    query.bindValue(":userid", userId);
    if (query.exec())
    {
        if (query.numRowsAffected() > 0) {
            qDebug() << "还书成功，更新了" << query.numRowsAffected() << "条记录";
            return true;
        } else {
            qDebug() << "执行成功，但无记录匹配条件（end 可能已非 NULL，或 bookid/userid 不匹配）";
            return false;
        }
    }
    else
    {
        qDebug() << "还书失败！";
        return false;
    }
}

bool SqlManager::BorrowBook(const QString &strId, const QString &userId)
{
    QSqlQuery query;
    //借书sql语句 同时更新库存 同时在record表中加上借阅记录（id，userid，bookid，start，end（这个是还书的时候加上））
    //还要在book表中更新cnt库存 同时检验cnt是否大于0
    query.prepare("UPDATE book SET cnt = cnt - 1 WHERE bookid = :bookid AND cnt > 0");
    query.bindValue(":bookid", strId);
    if (!query.exec())
    {
        qDebug() << "更新图书库存失败！";
        return false;
    }
    //如果更新成功，则插入借阅记录
    query.prepare("INSERT INTO record (bookid, userid, start) VALUES (:bookid, :userid, datetime(CURRENT_TIMESTAMP, 'localtime'))");
    query.bindValue(":bookid", strId);
    query.bindValue(":userid", userId);
    if (query.exec())
    {
        qDebug() << "借书成功！";
        return true;
    }
    else
    {
        qDebug() << "借书失败！";
        return false;
    }
}

QVector<QVector<QString> > SqlManager::GetBorrowRecords()
{
    //获取借阅记录
    QVector<QVector<QString>> records;
    QSqlQuery query("SELECT * FROM record");
    while (query.next())
    {
        QVector<QString> record;
        //表头为id，bookid，userid，start，end
        record.append(query.value(0).toString()); // id
        record.append(query.value(1).toString()); // bookid
        record.append(query.value(2).toString()); // userid
        record.append(query.value(3).toString()); // start
        record.append(query.value(4).toString()); // end
        records.append(record);
    }
    return records;
}

bool SqlManager::ClearBorrowRecords()
{
    //清空record表
    QSqlQuery query;
    query.prepare("DELETE FROM record");
    if (query.exec())
    {
        qDebug() << "清空成功！";
        return true;
    }
    else
    {
        qDebug() << "清空失败！";
        return false;
    }
}
