#pragma once

#include <vector>
#include <mysql++.h>

#include "Configs.h"

class MySQLException : public std::runtime_error 
{
public:
    MySQLException(const std::string& msg, const std::string& sql = "")
        : std::runtime_error(msg + " [SQL: " + sql + "]") {}
};

class MysqlConn
{
public:


public:
    bool ping() { return true; }

    // 初始化连接（RAII）
    explicit MysqlConn(const cncpp::MysqlConfig& config);
    ~MysqlConn();

    // 执行查询，返回结果集（泛型支持）
    template <typename T = std::vector<std::string>>
    std::vector<T> query(const std::string& sql, const mysqlpp::SQLTypeAdapter& params = "")
    {
        checkConnection();
        try 
        {
            mysqlpp::Query query = m_conn.query(sql);
            if (!params.is_null()) {
                query << " " <<  params;
            }

            mysqlpp::StoreQueryResult res = query.store();
            std::vector<T> result;

            for (const auto& row : res) 
            {
                result.push_back(row);
            }
            return result;
        }
        catch (const mysqlpp::BadQuery& e) {
            handleSQLError(e, sql);
            return {}; // 避免编译器警告
        }
    }

    // 执行更新（INSERT/UPDATE/DELETE）
    size_t execute(const std::string& sql, const mysqlpp::SQLTypeAdapter& params = {});

    // 事务管理
    void beginTransaction();
    void commit();
    void rollback();

private:
    mysqlpp::Connection m_conn;
    bool m_inTransaction = false;

    void checkConnection();
    void handleSQLError(const mysqlpp::BadQuery& e, const std::string& sql);
};