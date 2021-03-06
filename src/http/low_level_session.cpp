// 这个文件是 Poseidon 服务器应用程序框架的一部分。
// Copyleft 2014 - 2016, LH_Mouse. All wrongs reserved.

#include "../precompiled.hpp"
#include "low_level_session.hpp"
#include "exception.hpp"
#include "utilities.hpp"
#include "upgraded_session_base.hpp"
#include "../log.hpp"
#include "../profiler.hpp"
#include "../stream_buffer.hpp"

namespace Poseidon {

namespace Http {
	LowLevelSession::LowLevelSession(UniqueFile socket)
		: TcpSessionBase(STD_MOVE(socket))
	{
	}
	LowLevelSession::~LowLevelSession(){
	}

	void LowLevelSession::on_read_hup() NOEXCEPT {
		PROFILE_ME;

		// epoll 线程读取不需要锁。
		const AUTO(upgraded_session, m_upgraded_session);
		if(upgraded_session){
			upgraded_session->on_read_hup();
		}

		TcpSessionBase::on_read_hup();
	}
	void LowLevelSession::on_close(int err_code) NOEXCEPT {
		PROFILE_ME;

		// epoll 线程读取不需要锁。
		const AUTO(upgraded_session, m_upgraded_session);
		if(upgraded_session){
			upgraded_session->on_close(err_code);
		}

		TcpSessionBase::on_close(err_code);
	}

	void LowLevelSession::on_read_avail(StreamBuffer data){
		PROFILE_ME;

		// epoll 线程读取不需要锁。
		AUTO(upgraded_session, m_upgraded_session);
		if(upgraded_session){
			upgraded_session->on_read_avail(STD_MOVE(data));
			return;
		}

		ServerReader::put_encoded_data(STD_MOVE(data));

		upgraded_session = m_upgraded_session;
		if(upgraded_session){
			StreamBuffer queue;
			queue.swap(ServerReader::get_queue());
			if(!queue.empty()){
				upgraded_session->on_read_avail(STD_MOVE(queue));
			}
		}
	}

	void LowLevelSession::on_request_headers(RequestHeaders request_headers,
		std::string transfer_encoding, boost::uint64_t content_length)
	{
		PROFILE_ME;

		on_low_level_request_headers(STD_MOVE(request_headers), STD_MOVE(transfer_encoding), content_length);
	}
	void LowLevelSession::on_request_entity(boost::uint64_t entity_offset, bool is_chunked, StreamBuffer entity){
		PROFILE_ME;

		on_low_level_request_entity(entity_offset, is_chunked, STD_MOVE(entity));
	}
	bool LowLevelSession::on_request_end(boost::uint64_t content_length, bool is_chunked, OptionalMap headers){
		PROFILE_ME;

		AUTO(upgraded_session, on_low_level_request_end(content_length, is_chunked, STD_MOVE(headers)));
		if(upgraded_session){
			const Mutex::UniqueLock lock(m_upgraded_session_mutex);
			m_upgraded_session = STD_MOVE(upgraded_session);
			return false;
		}
		return true;
	}

	long LowLevelSession::on_encoded_data_avail(StreamBuffer encoded){
		PROFILE_ME;

		return TcpSessionBase::send(STD_MOVE(encoded));
	}

	boost::shared_ptr<UpgradedSessionBase> LowLevelSession::get_upgraded_session() const {
		const Mutex::UniqueLock lock(m_upgraded_session_mutex);
		return m_upgraded_session;
	}

	bool LowLevelSession::send(ResponseHeaders response_headers, StreamBuffer entity){
		PROFILE_ME;

		return ServerWriter::put_response(STD_MOVE(response_headers), STD_MOVE(entity));
	}
	bool LowLevelSession::send(StatusCode status_code, StreamBuffer entity, std::string content_type){
		PROFILE_ME;

		OptionalMap headers;
		if(!entity.empty()){
			headers.set(sslit("Content-Type"), STD_MOVE(content_type));
		}
		return send(status_code, STD_MOVE(headers), STD_MOVE(entity));
	}
	bool LowLevelSession::send(StatusCode status_code, OptionalMap headers, StreamBuffer entity){
		PROFILE_ME;

		ResponseHeaders response_headers;
		response_headers.version = 10001;
		response_headers.status_code = status_code;
		response_headers.reason = get_status_code_desc(status_code).desc_short;
		response_headers.headers = STD_MOVE(headers);
		return send(STD_MOVE(response_headers), STD_MOVE(entity));
	}
	bool LowLevelSession::send_default(StatusCode status_code, OptionalMap headers){
		PROFILE_ME;

		ResponseHeaders response_headers;
		response_headers.version = 10001;
		response_headers.status_code = status_code;
		response_headers.reason = get_status_code_desc(status_code).desc_short;
		response_headers.headers = STD_MOVE(headers);
		return ServerWriter::put_default_response(STD_MOVE(response_headers));
	}
}

}
