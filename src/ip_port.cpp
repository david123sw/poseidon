// 这个文件是 Poseidon 服务器应用程序框架的一部分。
// Copyleft 2014 - 2016, LH_Mouse. All wrongs reserved.

#include "precompiled.hpp"
#include "ip_port.hpp"
#include <ostream>
#include <sys/types.h>
#include <sys/socket.h>
#include "sock_addr.hpp"
#include "system_exception.hpp"

namespace Poseidon {

std::ostream &operator<<(std::ostream &os, const IpPort &rhs){
	return os <<rhs.ip <<':' <<rhs.port;
}

IpPort get_remote_ip_port_from_fd(int fd){
	::sockaddr sa;
	::socklen_t salen = sizeof(sa);
	if(::getpeername(fd, &sa, &salen) != 0){
		DEBUG_THROW(SystemException);
	}
	return get_ip_port_from_sock_addr(SockAddr(&sa, salen));
}
IpPort get_local_ip_port_from_fd(int fd){
	::sockaddr sa;
	::socklen_t salen = sizeof(sa);
	if(::getsockname(fd, &sa, &salen) != 0){
		DEBUG_THROW(SystemException);
	}
	return get_ip_port_from_sock_addr(SockAddr(&sa, salen));
}

}
