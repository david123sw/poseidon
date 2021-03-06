// 这个文件是 Poseidon 服务器应用程序框架的一部分。
// Copyleft 2014 - 2016, LH_Mouse. All wrongs reserved.

#ifndef POSEIDON_TCP_CLIENT_BASE_HPP_
#define POSEIDON_TCP_CLIENT_BASE_HPP_

#include "tcp_session_base.hpp"
#include <string>
#include <boost/scoped_ptr.hpp>
#include "sock_addr.hpp"

namespace Poseidon {

class ClientSslFactory;
class IpPort;

class TcpClientBase : protected SockAddr, public TcpSessionBase {
private:
	boost::scoped_ptr<ClientSslFactory> m_ssl_factory;

protected:
	TcpClientBase(const SockAddr &addr, bool use_ssl, bool accept_invalid_cert = false);
	TcpClientBase(const IpPort &addr, bool use_ssl, bool accept_invalid_cert = false);
	~TcpClientBase();

private:
	void real_connect(bool use_ssl, bool accept_invalid_cert);

protected:
	void on_read_avail(StreamBuffer data) OVERRIDE = 0;

public:
	void go_resident();
};

}

#endif
