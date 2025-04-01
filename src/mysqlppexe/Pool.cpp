#include "Pool.h"



#if 0

void MyConnectionPool::clear(bool all)
{
	std::lock_guard<std::mutex> lock(mutex_);	// ensure we're not interfered with

	PoolIt it = pool_.begin();
	while (it != pool_.end()) {
		if (all || !it->in_use) {
			remove(it++);
		}
		else {
			++it;
		}
	}
}


//// exchange //////////////////////////////////////////////////////////
// Passed connection is defective, so remove it from the pool and return
// a new one.

MysqlConn* MyConnectionPool::exchange(const MysqlConn* pc)
{
	// Don't grab the mutex first.  remove() and grab() both do.
	// Inefficient, but we'd have to hoist their contents up into this
	// method or extract a mutex-free version of each mechanism for
	// each, both of which are also inefficient.
	remove(pc);
	return grab();
}


//// find_mru //////////////////////////////////////////////////////////
// Find most recently used available connection.  Uses operator< for
// ConnectionInfo to order pool with MRU connection last.  Returns 0 if
// there are no connections not in use.

MysqlConn* MyConnectionPool::find_mru()
{
	PoolIt mru = std::max_element(pool_.begin(), pool_.end());
	if (mru != pool_.end() && !mru->in_use) {
		mru->in_use = true;
		return mru->conn;
	}
	else {
		return 0;
	}
}


//// grab //////////////////////////////////////////////////////////////

MysqlConn* MyConnectionPool::grab()
{
	std::lock_guard<std::mutex> lock(mutex_);	// ensure we're not interfered with
	remove_old_connections();
	if (MysqlConn* mru = find_mru()) {
		return mru;
	}
	else {
		// No free connections, so create and return a new one.
		pool_.push_back(ConnectionInfo(create()));
		return pool_.back().conn;
	}
}


//// release ///////////////////////////////////////////////////////////

void MyConnectionPool::release(const MysqlConn* pc)
{
	std::lock_guard<std::mutex> lock(mutex_);	// ensure we're not interfered with

	for (PoolIt it = pool_.begin(); it != pool_.end(); ++it) {
		if (it->conn == pc) {
			it->in_use = false;
			it->last_used = time(0);
			break;
		}
	}
}


//// remove ////////////////////////////////////////////////////////////
// 2 versions:
//
// First takes a Connection pointer, finds it in the pool, and calls
// the second.  It's public, because Connection pointers are all
// outsiders see of the pool.
//
// Second takes an iterator into the pool, destroys the referenced
// connection and removes it from the pool.  This is only a utility
// function for use by other class internals.

void MyConnectionPool::remove(const MysqlConn* pc)
{
	std::lock_guard<std::mutex> lock(mutex_);	// ensure we're not interfered with

	for (PoolIt it = pool_.begin(); it != pool_.end(); ++it) {
		if (it->conn == pc) {
			remove(it);
			return;
		}
	}
}

void MyConnectionPool::remove(const PoolIt& it)
{
	destroy(it->conn);
	pool_.erase(it);
}


//// remove_old_connections ////////////////////////////////////////////
// Remove connections that were last used too long ago.

void MyConnectionPool::remove_old_connections()
{
	TooOld<ConnectionInfo> too_old(max_idle_time());

	PoolIt it = pool_.begin();
	while ((it = std::find_if(it, pool_.end(), too_old)) != pool_.end()) {
		remove(it++);
	}
}


//// safe_grab /////////////////////////////////////////////////////////

MysqlConn* MyConnectionPool::safe_grab()
{
	MysqlConn* pc;
	while (!(pc = grab())->ping()) {
		remove(pc);
		pc = 0;
	}
	return pc;
}

MysqlConn* MyConnectionPool::create() 
{
	return new MysqlConn(conn_cfg);
}

ScopedMySqlConn::ScopedMySqlConn(MyConnectionPool* pool, bool safe) :
	pool_(pool),
	connection_(safe ? pool->safe_grab() : pool->grab())
{
}

ScopedMySqlConn::~ScopedMySqlConn()
{
	pool_->release(connection_);
}
#endif