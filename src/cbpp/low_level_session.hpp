// 这个文件是 Poseidon 服务器应用程序框架的一部分。
// Copyleft 2014 - 2016, LH_Mouse. All wrongs reserved.

#ifndef POSEIDON_CBPP_LOW_LEVEL_SESSION_HPP_
#define POSEIDON_CBPP_LOW_LEVEL_SESSION_HPP_

#include "../tcp_session_base.hpp"
#include "reader.hpp"
#include "writer.hpp"
#include "control_codes.hpp"
#include "status_codes.hpp"

namespace Poseidon {

namespace Cbpp {
	class LowLevelSession : public TcpSessionBase, private Reader, private Writer {
	public:
		explicit LowLevelSession(UniqueFile socket);
		~LowLevelSession();

	protected:
		// TcpSessionBase
		void on_read_avail(StreamBuffer data) OVERRIDE;

		// Reader
		void on_data_message_header(boost::uint16_t message_id, boost::uint64_t payload_size) OVERRIDE;
		void on_data_message_payload(boost::uint64_t payload_offset, StreamBuffer payload) OVERRIDE;
		bool on_data_message_end(boost::uint64_t payload_size) OVERRIDE;

		bool on_control_message(ControlCode control_code, boost::int64_t vint_param, std::string string_param) OVERRIDE;

		// Writer
		long on_encoded_data_avail(StreamBuffer encoded) OVERRIDE;

		// 可覆写。
		virtual void on_low_level_data_message_header(boost::uint16_t message_id, boost::uint64_t payload_size) = 0;
		virtual void on_low_level_data_message_payload(boost::uint64_t payload_offset, StreamBuffer payload) = 0;
		virtual bool on_low_level_data_message_end(boost::uint64_t payload_size) = 0;

		virtual bool on_low_level_control_message(ControlCode control_code, boost::int64_t vint_param, std::string string_param) = 0;

	public:
		bool send(boost::uint16_t message_id, StreamBuffer payload);
		bool send_error(boost::uint16_t message_id, StatusCode status_code, std::string reason);
	};
}

}

#endif
