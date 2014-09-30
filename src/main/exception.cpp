#include "../precompiled.hpp"
#include "exception.hpp"
#include "log.hpp"
#include "utilities.hpp"
using namespace Poseidon;

Exception::Exception(const char *file, std::size_t line, std::string what)
	: std::runtime_error(STD_MOVE(what)), m_file(file), m_line(line)
{
	LOG_ERROR("Constructing Exception: file = ", m_file,
		", line = ", m_line, ", what = ", this->what());
}

SystemError::SystemError(const char *file, std::size_t line, int code)
	: Exception(file, line, getErrorDescAsString(code)), m_code(code)
{
}

ProtocolException::ProtocolException(const char *file, std::size_t line,
	std::string what, int code)
	: Exception(file, line, STD_MOVE(what)), m_code(code)
{
}