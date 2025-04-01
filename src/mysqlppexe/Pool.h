#pragma once

#include <assert.h>
#include <list>


#include "MySqlConn.h"
#include "Singleton.h"

#include <algorithm>
#include <functional>

#if 0
// 后续改为这种模版类型
template <typename T>
class MyTemplateClass : public Singleton<MyTemplateClass<T>> { // 注意：CRTP 模式
	friend class Singleton<MyTemplateClass<T>>; // 声明友元以访问私有构造函数

private:
	MyTemplateClass() { /* 初始化逻辑（可依赖 T 类型） */ }
	~MyTemplateClass() = default;

public:
	void ProcessData(const T& data) { /* ... */ }
};

#endif

template <typename ConnInfoT>
class TooOld : std::unary_function<ConnInfoT, bool>
{
public:
#if !defined(DOXYGEN_IGNORE)
    TooOld(unsigned int tmax) :
        min_age_(time(0) - tmax)
    {
    }

    bool operator()(const ConnInfoT& conn_info) const
    {
        return !conn_info.in_use && conn_info.last_used <= min_age_;
    }

#endif
private:
    time_t min_age_;
};

#if 0
class MyConnectionPool : public cncpp::Singleton<MyConnectionPool>
{
public:
  MyConnectionPool() {}

  virtual ~MyConnectionPool() {
    clear();
    assert(empty());
  }

  void init(const cncpp::MysqlConfig &config) { conn_cfg = config; }

  bool empty() const { return pool_.empty(); }

  virtual MysqlConn *exchange(const MysqlConn *pc);

  virtual MysqlConn *grab();

  virtual void release(const MysqlConn *pc);

  void remove(const MysqlConn *pc);

  virtual MysqlConn *safe_grab();

  void shrink() { clear(false); }

protected:
  void clear(bool all = true);

  virtual MysqlConn *create();

  virtual void destroy(MysqlConn *conn) {
    if (conn)
      delete conn;
    
    conn = nullptr;
  };

  virtual unsigned int max_idle_time() { return 3; };

  /// \brief Returns the current size of the internal connection pool.
  size_t size() const { return pool_.size(); }

private:
  struct ConnectionInfo {
    MysqlConn *conn;
    time_t last_used;
    bool in_use;

    ConnectionInfo(MysqlConn *c) : conn(c), last_used(time(0)), in_use(true) {}

    bool operator<(const ConnectionInfo &rhs) const {
      const ConnectionInfo &lhs = *this;
      return lhs.in_use == rhs.in_use ? lhs.last_used < rhs.last_used
                                      : lhs.in_use;
    }
  };
  typedef std::list<ConnectionInfo> PoolT;
  typedef PoolT::iterator PoolIt;

  //// Internal support functions
  MysqlConn *find_mru();
  void remove(const PoolIt &it);
  void remove_old_connections();

  //// Internal data
  PoolT pool_;

  cncpp::MysqlConfig conn_cfg = {};

  std::mutex mutex_;
};


class ScopedMySqlConn {
public:
  explicit ScopedMySqlConn(MyConnectionPool *pool = &MyConnectionPool::getMe(),
                           bool safe = false);

#if __cplusplus >= 201103L
  // ScopedConnection objects cannot be copied.  We want them to be
  // tightly scoped to their use point, not put in containers or
  // passed around promiscuously.
  ScopedConnection(ScopedConnection &&) = default;
  ScopedConnection(const ScopedConnection &no_copies) = delete;
  const ScopedConnection &operator=(const ScopedConnection &no_copies) = delete;
#endif

  /// \brief Destructor
  ///
  /// Releases the Connection back to the ConnectionPool.
  ~ScopedMySqlConn();

  /// \brief Access the Connection pointer
  MysqlConn *operator->() const { return connection_; }

  /// \brief Dereference
  MysqlConn &operator*() const { return *connection_; }

  /// \brief Truthiness operator
  operator void *() const { return connection_; }

private:
#if __cplusplus < 201103L
  // Pre C++11 alternative to no-copies ctors above.
  ScopedMySqlConn(const ScopedMySqlConn &no_copies);
  const ScopedMySqlConn &operator=(const ScopedMySqlConn &no_copies);
#endif

  MyConnectionPool *pool_;
  MysqlConn *const connection_;
};
#endif

template<typename ConnT, typename CONFIG>
class MyConnPool : public cncpp::Singleton<MyConnPool<ConnT, CONFIG>>
{
public:
    using CONN = ConnT;

    struct ConnectionInfo
    {
        CONN* conn;
        time_t last_used;
        bool in_use;

        ConnectionInfo(CONN* c) : conn(c), last_used(time(0)), in_use(true) {}

        bool operator<(const ConnectionInfo& rhs) const
        {
            const ConnectionInfo& lhs = *this;
            return lhs.in_use == rhs.in_use ? lhs.last_used < rhs.last_used
                : lhs.in_use;
        }
    };

    typedef std::list<ConnectionInfo> PoolT;
    typedef typename PoolT::iterator PoolIt;

    std::mutex mutex_;
    CONFIG conn_cfg;

    PoolT pool_;

public:
    MyConnPool() {}

    virtual ~MyConnPool() 
    {
        clear();
        assert(empty());
    }

    void init(const CONFIG& config) { conn_cfg = config; }

    bool empty() const { return pool_.empty(); }

    virtual CONN* exchange(const CONN* pc)
    {
        remove(pc);
        return grab();
    }

    virtual CONN* grab()
    {
        std::lock_guard<std::mutex> lock(mutex_);	// ensure we're not interfered with
        remove_old_connections();
        if (CONN* mru = find_mru())
        {
            return mru;
        }
        else 
        {
            pool_.push_back(ConnectionInfo(create()));
            return pool_.back().conn;
        }
    }

    virtual void release(const CONN* pc)
    {
        std::lock_guard<std::mutex> lock(mutex_);	// ensure we're not interfered with

        for (PoolIt it = pool_.begin(); it != pool_.end(); ++it) 
        {
            if (it->conn == pc) 
            {
                it->in_use = false;
                it->last_used = time(0);
                break;
            }
        }
    }

    void remove(const CONN* pc)
    {
        std::lock_guard<std::mutex> lock(mutex_);	// ensure we're not interfered with

        for (PoolIt it = pool_.begin(); it != pool_.end(); ++it) 
        {
            if (it->conn == pc) 
            {
                remove(it);
                return;
            }
        }
    }

    virtual CONN* safe_grab()
    {        
        MysqlConn* pc;
        while (!(pc = grab())->ping()) 
        {
            remove(pc);
            pc = 0;
        }
        return pc;        
    }

    void shrink() 
    { 
        clear(false); 
    }

    void clear(bool all = true)
    {
        std::lock_guard<std::mutex> lock(mutex_);	// ensure we're not interfered with

        PoolIt it = pool_.begin();
        while (it != pool_.end()) 
        {
            if (all || !it->in_use) 
            {
                remove(it++);
            }
            else 
            {
                ++it;
            }
        }
    }

    virtual CONN* create()
    {
        return new CONN(conn_cfg);
    }

    virtual void destroy(CONN* conn) 
    {
        if (conn)
            delete conn;

        conn = nullptr;
    };

    virtual unsigned int max_idle_time() { return 3; };

    /// \brief Returns the current size of the internal connection pool.
    size_t size() const { return pool_.size(); }




    //// Internal support functions
    CONN* find_mru()
    {
        PoolIt mru = std::max_element(pool_.begin(), pool_.end());
        if (mru != pool_.end() && !mru->in_use) 
        {
            mru->in_use = true;
            return mru->conn;
        }
        else 
        {
            return 0;
        }
    }

    void remove(const PoolIt& it)
    {
        destroy(it->conn);
        pool_.erase(it);
    }

    void remove_old_connections()
    {
        TooOld<ConnectionInfo> too_old(max_idle_time());

        PoolIt it = pool_.begin();
        while ((it = std::find_if(it, pool_.end(), too_old)) != pool_.end()) 
        {
            remove(it++);
        }
    }

};

template<typename T, typename CONFIG>
class MyScopedConn 
{
public:
    using CONN = T;

    explicit MyScopedConn(MyConnPool<CONN, CONFIG>* pool = &MyConnPool<CONN, CONFIG>::getMe(),
                          bool safe = false): pool_(pool),
        connection_(safe ? pool->safe_grab() : pool->grab())
    {

    }

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
    ~MyScopedConn()
    {
        pool_->release(connection_);
    }

    /// \brief Access the Connection pointer
    CONN* operator->() const { return connection_; }

    /// \brief Dereference
    CONN& operator*() const { return *connection_; }

    /// \brief Truthiness operator
    operator void* () const { return connection_; }

private:
#if __cplusplus < 201103L
    // Pre C++11 alternative to no-copies ctors above.
    MyScopedConn(const MyScopedConn& no_copies);
    const MyScopedConn& operator=(const MyScopedConn& no_copies);
#endif

    MyConnPool<CONN, CONFIG>* pool_;
    CONN* const connection_;
};

using  MySqlConnectPool = MyConnPool<MysqlConn, cncpp::MysqlConfig>;
using ScopedMySqlConn = MyScopedConn<MysqlConn, cncpp::MysqlConfig>;