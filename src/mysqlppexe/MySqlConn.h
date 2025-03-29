#pragma once

#include <vector>
#include <mysql++.h>

class MySQLException : public std::runtime_error {
public:
    MySQLException(const std::string& msg, const std::string& sql = "")
        : std::runtime_error(msg + " [SQL: " + sql + "]") {}
};

class MysqlConn
{
public:
    // ��������
    struct Config 
    {
        std::string host;
        std::string user;
        std::string password;
        std::string database;
        unsigned int port = 3306;
        const char* charset = "utf8mb4";
    };

public:
	bool ping() { return true; }

    // ��ʼ�����ӣ�RAII��
    explicit MysqlConn(const Config& config);
    ~MysqlConn();

    // ִ�в�ѯ�����ؽ����������֧�֣�
    template <typename T = std::vector<std::string>>
    std::vector<T> query(const std::string& sql, const mysqlpp::SQLTypeAdapter& params = {});

    // ִ�и��£�INSERT/UPDATE/DELETE��
    size_t execute(const std::string& sql, const mysqlpp::SQLTypeAdapter& params = {});

    // �������
    void beginTransaction();
    void commit();
    void rollback();

private:
    mysqlpp::Connection m_conn;
    bool m_inTransaction = false;

    void checkConnection();
    void handleSQLError(const mysqlpp::BadQuery& e, const std::string& sql);
};