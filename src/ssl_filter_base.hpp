// 这个文件是 Poseidon 服务器应用程序框架的一部分。
// Copyleft 2014 - 2016, LH_Mouse. All wrongs reserved.

#ifndef POSEIDON_SSL_FILTER_BASE_HPP_
#define POSEIDON_SSL_FILTER_BASE_HPP_

#include "cxx_ver.hpp"
#include "cxx_util.hpp"
#include "ssl_raii.hpp"
#include "mutex.hpp"

namespace Poseidon {

class SslFilterBase : NONCOPYABLE {
private:
	mutable Mutex m_mutex;
	const UniqueSsl m_ssl;
	const int m_fd;

public:
	SslFilterBase(Move<UniqueSsl> ssl, int fd);
	virtual ~SslFilterBase();

public:
	::SSL *get_ssl() const {
		return m_ssl.get();
	}

	long read(void *data, unsigned long size);
	long write(const void *data, unsigned long size);
	void shutdown() NOEXCEPT;
};

}

#endif
