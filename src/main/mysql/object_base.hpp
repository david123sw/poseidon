#ifndef POSEIDON_MYSQL_OBJECT_BASE_HPP_
#define POSEIDON_MYSQL_OBJECT_BASE_HPP_

#include "../virtual_shared_from_this.hpp"
#include <string>
#include <boost/thread/shared_mutex.hpp>

namespace sql {

class Connection;

}

namespace Poseidon {

class MySqlObjectBase
	: public virtual VirtualSharedFromThis
{
protected:
	mutable boost::shared_mutex m_mutex;

public:
	bool shouldSyncSaveNow() const;
	virtual void syncSave(sql::Connection *conn) const = 0;
	virtual bool syncLoad(sql::Connection *conn, const char *filter) = 0;

	void asyncSave() const;
	void asyncLoad(std::string filter);
};

}

#endif