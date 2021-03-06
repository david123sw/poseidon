// 这个文件是 Poseidon 服务器应用程序框架的一部分。
// Copyleft 2014 - 2016, LH_Mouse. All wrongs reserved.

#ifndef POSEIDON_MONGODB_CONNECTION_HPP_
#define POSEIDON_MONGODB_CONNECTION_HPP_

#include "../cxx_util.hpp"
#include <string>
#include <cstring>
#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>

namespace Poseidon {

class Uuid;

namespace MongoDb {
	class BsonBuilder;

	class Connection : NONCOPYABLE {
	public:
		static boost::shared_ptr<Connection> create(const char *server_addr, unsigned server_port,
			const char *user_name, const char *password, const char *auth_database, bool use_ssl, const char *database);

		static boost::shared_ptr<Connection> create(const std::string &server_addr, unsigned server_port,
			const std::string &user_name, const std::string &password, const std::string &auth_database, bool use_ssl, const std::string &database)
		{
			return create(server_addr.c_str(), server_port,
				user_name.c_str(), password.c_str(), auth_database.c_str(), use_ssl, database.c_str());
		}

	public:
		virtual ~Connection() = 0;

	public:
		void execute_command(const char *collection, const BsonBuilder &query, boost::uint32_t begin, boost::uint32_t limit);
		void execute_query(const char *collection, const BsonBuilder &query, boost::uint32_t begin, boost::uint32_t limit);
		void execute_insert(const char *collection, const BsonBuilder &doc, bool continue_on_error);
		void execute_update(const char *collection, const BsonBuilder &query, const BsonBuilder &doc, bool upsert, bool update_all);
		void execute_delete(const char *collection, const BsonBuilder &query, bool delete_all);
		void discard_result() NOEXCEPT;

		bool fetch_next();

		bool get_boolean(const char *name) const;
		boost::int64_t get_signed(const char *name) const;
		boost::uint64_t get_unsigned(const char *name) const;
		double get_double(const char *name) const;
		std::string get_string(const char *name) const;
		boost::uint64_t get_datetime(const char *name) const;
		Uuid get_uuid(const char *name) const;
		std::string get_blob(const char *name) const;
	};
}

}

#endif
