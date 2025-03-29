#pragma once

#include <list>
#include <assert.h>

#include "MySqlConn.h"
#include "Singleton.h"

class MyConnectionPool : public cncpp::Singleton<MyConnectionPool>
{
public:
	MyConnectionPool() { }

	virtual ~MyConnectionPool() { assert(empty()); }

	bool empty() const { return pool_.empty(); }
	
	virtual MysqlConn* exchange(const MysqlConn* pc);

	virtual MysqlConn* grab();

	virtual void release(const MysqlConn* pc);

	void remove(const MysqlConn* pc);

	virtual MysqlConn* safe_grab();

	void shrink() { clear(false); }

protected:
	void clear(bool all = true);
	
	virtual MysqlConn* create() { return nullptr; };

	virtual void destroy(MysqlConn*){};
	
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
};
