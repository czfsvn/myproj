#pragma once

#include <list>
#include <assert.h>

#include "MySqlConn.h"
#include "Singleton.h"

class ScopedLock {
public:
	// 构造函数，加锁
	ScopedLock(std::mutex& m) : mutex_(m) {
		mutex_.lock();
	}

	// 析构函数，解锁
	~ScopedLock() {
		mutex_.unlock();
	}

	// 禁止拷贝构造和赋值操作
	ScopedLock(const ScopedLock&) = delete;
	ScopedLock& operator=(const ScopedLock&) = delete;

private:
	std::mutex& mutex_;
};

class MyConnectionPool : public cncpp::Singleton<MyConnectionPool>
{
public:
	MyConnectionPool() { }

	virtual ~MyConnectionPool() { clear(); assert(empty()); }

	void init(const cncpp::MysqlConfig& config)
	{
		conn_cfg = config;
	}

	bool empty() const { return pool_.empty(); }
	
	virtual MysqlConn* exchange(const MysqlConn* pc);

	virtual MysqlConn* grab();

	virtual void release(const MysqlConn* pc);

	void remove(const MysqlConn* pc);

	virtual MysqlConn* safe_grab();

	void shrink() { clear(false); }


protected:
	void clear(bool all = true);
	
	virtual MysqlConn* create();

	virtual void destroy(MysqlConn* conn)
	{
		if (conn)
			delete conn;

		conn = nullptr;
	};
	
	virtual unsigned int max_idle_time() { return 3; };
	
	/// \brief Returns the current size of the internal connection pool.
	size_t size() const { return pool_.size(); }

private:
	//// Internal types
	struct ConnectionInfo {
		MysqlConn* conn;
		time_t last_used;
		bool in_use;

		ConnectionInfo(MysqlConn* c) :
			conn(c),
			last_used(time(0)),
			in_use(true)
		{
		}

		// Strict weak ordering for ConnectionInfo objects.
		// 
		// This ordering defines all in-use connections to be "less
		// than" those not in use.  Within each group, connections
		// less recently touched are less than those more recent.
		bool operator<(const ConnectionInfo& rhs) const
		{
			const ConnectionInfo& lhs = *this;
			return lhs.in_use == rhs.in_use ?
				lhs.last_used < rhs.last_used :
				lhs.in_use;
		}
	};
	typedef std::list<ConnectionInfo> PoolT;
	typedef PoolT::iterator PoolIt;

	//// Internal support functions
	MysqlConn* find_mru();
	void remove(const PoolIt& it);
	void remove_old_connections();

	//// Internal data
	PoolT pool_;

	cncpp::MysqlConfig conn_cfg = {};

	std::mutex mutex_;
};


class ScopedMySqlConn
{
public:
	explicit ScopedMySqlConn(MyConnectionPool* pool = &MyConnectionPool::getMe(), bool safe = false);

#if __cplusplus >= 201103L
	// ScopedConnection objects cannot be copied.  We want them to be
	// tightly scoped to their use point, not put in containers or
	// passed around promiscuously.
	ScopedConnection(ScopedConnection&&) = default;
	ScopedConnection(const ScopedConnection& no_copies) = delete;
	const ScopedConnection& operator=(const ScopedConnection& no_copies) = delete;
#endif

	/// \brief Destructor
	///
	/// Releases the Connection back to the ConnectionPool.
	~ScopedMySqlConn();

	/// \brief Access the Connection pointer
	MysqlConn* operator->() const { return connection_; }

	/// \brief Dereference
	MysqlConn& operator*() const { return *connection_; }

	/// \brief Truthiness operator
	operator void* () const { return connection_; }

private:
#if __cplusplus < 201103L
	// Pre C++11 alternative to no-copies ctors above.
	ScopedMySqlConn(const ScopedMySqlConn& no_copies);
	const ScopedMySqlConn& operator=(const ScopedMySqlConn& no_copies);
#endif

	MyConnectionPool* pool_;
	MysqlConn* const connection_;
};

