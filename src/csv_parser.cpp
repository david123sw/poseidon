// 这个文件是 Poseidon 服务器应用程序框架的一部分。
// Copyleft 2014 - 2016, LH_Mouse. All wrongs reserved.

#include "precompiled.hpp"
#include "csv_parser.hpp"
#include "exception.hpp"
#include "file.hpp"
#include "log.hpp"

namespace Poseidon {

CsvParser::CsvParser()
	: m_row(static_cast<std::size_t>(-1))
{
}
CsvParser::CsvParser(const char *file)
	: m_row(static_cast<std::size_t>(-1))
{
	load(file);
}

void CsvParser::load(const char *file){
	LOG_POSEIDON_DEBUG("Loading CSV file: ", file);

	StreamBuffer buffer;
	file_get_contents(buffer, file);
	buffer.put('\n');

	std::vector<OptionalMap> data;

	std::vector<std::vector<std::string> > rows;
	{
		std::vector<std::string> row;
		std::string token;
		bool first = true;
		bool in_quote = false;
		do {
			char ch = buffer.get();
			if(ch == '\r'){
				if(buffer.peek() == '\n'){
					buffer.get();
				}
				ch = '\n';
			}

			if(first){
				first = false;

				if(ch == '\"'){
					in_quote = true;
					continue;
				}
			}

			if(ch == '\"'){
				if(in_quote){
					if(buffer.peek() == '\"'){
						buffer.get();
						token.push_back('\"');
					} else {
						in_quote = false;
					}
					continue;
				}
			}

			if(!in_quote){
				if((ch == ',') || (ch == '\n')){
					std::string trimmed;
					const std::size_t begin = token.find_first_not_of(" \t\r\n");
					if(begin != std::string::npos){
						const std::size_t end = token.find_last_not_of(" \t\r\n") + 1;
						trimmed = token.substr(begin, end - begin);
					}
					row.push_back(STD_MOVE(trimmed));
					token.clear();
					first = true;

					if(ch == '\n'){
						rows.push_back(STD_MOVE(row));
						row.clear();
					}
					continue;
				}
			}

			token.push_back(ch);
		} while(!buffer.empty());
	}
	if(rows.empty() || rows.front().empty()){
		LOG_POSEIDON_ERROR("The first line of a CSV file may not be empty.");
		DEBUG_THROW(Exception, sslit("Bad CSV header"));
	}

	const std::size_t column_count = rows.front().size();
	std::vector<SharedNts> keys(column_count);
	for(std::size_t i = 0; i < column_count; ++i){
		AUTO_REF(key, rows.front().at(i));
		for(std::size_t j = 0; j < i; ++j){
			if(keys.at(j) == key){
				LOG_POSEIDON_ERROR("Duplicate key: ", key);
				DEBUG_THROW(Exception, sslit("Duplicate key"));
			}
		}
		keys.at(i).assign(key.c_str());
	}
	for(std::size_t i = 1; i < rows.size(); ++i){
		rows.at(i - 1).swap(rows.at(i));
	}
	rows.pop_back();

	{
		std::size_t line = 1;
		std::size_t i = 0;
		while(i < rows.size()){
			AUTO_REF(row, rows.at(i));
			++line;
			if((row.size() == 1) && row.front().empty()){
				for(std::size_t j = i + 1; j < rows.size(); ++j){
					rows.at(j - 1).swap(rows.at(j));
				}
				rows.pop_back();
				continue;
			}
			if(row.size() != column_count){
				LOG_POSEIDON_ERROR("There are ", row.size(), " column(s) on line ", line,
					" but there are ", column_count, " in the header");
				DEBUG_THROW(Exception, sslit("Inconsistent CSV column numbers"));
			}
			++i;
		}
	}

	data.resize(rows.size());
	for(std::size_t i = 0; i < rows.size(); ++i){
		AUTO_REF(row, rows.at(i));
		AUTO_REF(map, data.at(i));
		for(std::size_t j = 0; j < column_count; ++j){
			map.create(keys.at(j))->second.swap(row.at(j));
		}
	}

	LOG_POSEIDON_DEBUG("Done loading CSV file: ", file);
	m_data.swap(data);
	m_row = static_cast<std::size_t>(-1);
}
bool CsvParser::load_nothrow(const char *file){
	try {
		load(file);
		return true;
	} catch(std::exception &e){
		LOG_POSEIDON_ERROR("Error loading CSV file: ", e.what());
		return false;
	}
}

void CsvParser::clear(){
	m_data.clear();
	m_row = static_cast<std::size_t>(-1);
}

const std::string &CsvParser::get_raw(const char *key) const {
	const AUTO_REF(map, m_data.at(m_row));
	const AUTO(it, map.find(key));
	if(it == map.end()){
		LOG_POSEIDON_WARNING("Column not found: ", key);
		return EMPTY_STRING;
	}
	return it->second;
}

std::size_t CsvParser::seek(std::size_t row){
	if((row != static_cast<std::size_t>(-1)) && (row >= m_data.size())){
		DEBUG_THROW(Exception, sslit("Row index is out of range"));
	}
	const AUTO(old_row, m_row);
	m_row = row;
	return old_row;
}

}
