#ifndef SQLMANAGER_H
#define SQLMANAGER_H
#include <QString>
#include <QSqlDatabase>
class SqlManager
{
public:
    // 获取单实例对象
    static SqlManager& GetInstance();
    // 打印实例地址
    void Print();
    // 初始化数据库
    bool InitDatabase();
    //登录
    bool Login(const QString &username, const QString &password);
    //获取所有用户
    QVector<QVector<QString>> GetUsers();
    //添加用户
    bool AddUser(QVector<QString>);
    //删除用户
    bool DeleteUser(const QString &strId);
    //获取所有图书
    QVector<QVector<QString>> GetBooks();
    //添加图书
    bool AddBook(QVector<QString>);
    //删除图书
    bool DeleteBook(const QString &strId);
    //图书归还
    bool ReturnBook(const QString &strId, const QString &userId);
    //图书借阅
    bool BorrowBook(const QString &strId, const QString &userId);
    //获取借阅记录
    QVector<QVector<QString>> GetBorrowRecords();
    //清空借阅记录
    bool ClearBorrowRecords();

private:
    QSqlDatabase db;
    // 禁止外部构造
    SqlManager();
    // 禁止外部析构
    ~SqlManager();
    // 禁止外部拷贝构造
    SqlManager(const SqlManager &sqlManager) = delete;
    // 禁止外部赋值操作
    const SqlManager &operator=(const SqlManager &sqlManager) = delete;
};

#endif // SQLMANAGER_H
