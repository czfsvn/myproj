#include "MySqlConn.h"
#include "Log.h"



MysqlConn::MysqlConn(const cncpp::MysqlConfig& config) {
    if (!m_conn.connect(
        config.database.c_str(),
        config.host.c_str(),
        config.user.c_str(),
        config.password.c_str(),
        config.port)) 
    {
        CRITICAL("Connection failed: {}", m_conn.error());
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