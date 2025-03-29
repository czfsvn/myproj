#include "MySqlConn.h"

MysqlConn::MysqlConn(const Config& config) {
    if (!m_conn.connect(
        config.database.c_str(),
        config.host.c_str(),
        config.user.c_str(),
        config.password.c_str(),
        config.port)) 
    {
        //throw MySQLException("Connection failed: " + m_conn.error());
    }
    //m_conn.set_charset(config.charset);
}

MysqlConn::~MysqlConn() 
{
    if (m_inTransaction) {
        try { rollback(); }
        catch (...) {}
    }
    m_conn.disconnect();
}

void MysqlConn::checkConnection() 
{
    if (!m_conn.connected()) {
        throw MySQLException("Connection lost");
    }
}

template <typename T>
std::vector<T> MysqlConn::query(const std::string& sql, const mysqlpp::SQLTypeAdapter& params)
{
    checkConnection();
    try {
        mysqlpp::Query query = m_conn.query(sql);
        if (!params.is_null()) {
            query << params;
        }

        mysqlpp::StoreQueryResult res = query.store();
        std::vector<T> result;

        for (const auto& row : res) {
            T item;
            // 假设 T 支持从 mysqlpp::Row 构造或转换
            item = row; // 示例：需要用户自定义类型 T 的转换逻辑
            result.push_back(item);
        }
        return result;
    }
    catch (const mysqlpp::BadQuery& e) {
        handleSQLError(e, sql);
        return {}; // 避免编译器警告
    }
}

#if 0
// 特化：返回原始数据（std::vector<std::string>）
template <>
std::vector<std::vector<std::string>> MySQLConnection::query(
    const std::string& sql,
    const mysqlpp::SQLTypeAdapter& params)
{
    // 实现类似，将每行转换为 std::vector<std::string>
    // ...
}
#endif

size_t MysqlConn::execute(const std::string& sql, const mysqlpp::SQLTypeAdapter& params)
{
    checkConnection();
    try {
        mysqlpp::Query query = m_conn.query(sql);
        if (!params.is_null()) {
            query << params;
        }
        return query.execute().rows();
    }
    catch (const mysqlpp::BadQuery& e) {
        handleSQLError(e, sql);
        return 0;
    }
}

void MysqlConn::beginTransaction() 
{
    checkConnection();
    m_conn.query("START TRANSACTION").execute();
    m_inTransaction = true;
}

void MysqlConn::commit() 
{
    checkConnection();
    m_conn.query("COMMIT").execute();
    m_inTransaction = false;
}

void MysqlConn::rollback() 
{
    checkConnection();
    m_conn.query("ROLLBACK").execute();
    m_inTransaction = false;
}

void MysqlConn::handleSQLError(const mysqlpp::BadQuery& e, const std::string& sql) 
{
    throw MySQLException(
        std::string("SQL Error: ") + e.what() + " (Code: " +
        std::to_string(e.errnum()) + ")",
        sql
    );
}