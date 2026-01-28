#include "pch.h"
#include "txtLog.h"
#include <fstream>

#define CPRINT(txt) std::cout << logger.Append(txt)
#define PRINT(txt)  std::cout << logger.Append(txt.c_str())

DebugLog::~DebugLog() noexcept
{
	try {	//failure is recoverable and should not result in a crash. it would be lovely to know about it though
		std::ofstream file("log.txt", std::ofstream::binary);
		if (file.bad() || ss.bad())	throw std::exception("Could not open log file, or logger buffer corrupt");
		
		file << ss.rdbuf();
		bool bad = file.bad();
		
		file.close();
		if (bad || ss.bad()) throw std::exception("There was an error writing the log file.");

	} catch (std::exception& e) {
		std::cerr << e.what() + '\n';
	}
	std::cout.flush();	//flushing cerr & cout for good measure
	std::cerr.flush();	//runs after main function exit
}
const char* DebugLog::Append(const char* str) noexcept
{
	ss << str;
	return str;
}

void DebugPrint(const char* str, std::optional<char> suffix) noexcept(false)
{
	if (suffix.has_value())
		PRINT((std::string(str) += suffix.value()));
	else CPRINT(str);
}

void DebugPrint(const char** str_arr, const size_t& size, std::optional<const char*> seperator, std::optional<char> suffix) noexcept(false)
{
	if (size < 1) throw std::runtime_error("DebugPrint(arr*) bad parameters.");
	for (size_t i = 1; i < size; ++i) if (*(str_arr + i) == nullptr) throw std::runtime_error("DebugPrint(arr*) bad parameters.");
	std::string temp(*str_arr);

	if (suffix.has_value())
	{
		if (seperator.has_value()) for (size_t i = 1; i < size; ++i)
			temp += (seperator.value() + std::string(*(str_arr + i)));
		else for (size_t i = 1; i < size; ++i)
			temp += *(str_arr + i);
		temp += suffix.value();
	}
	else
	{
		if (seperator.has_value()) for (size_t i = 1; i < size; ++i)
			temp += (seperator.value() + std::string(*(str_arr + i)));
		else for (size_t i = 1; i < size; ++i)
			temp += *(str_arr + i);
	}

	PRINT(temp);
}

void DebugPrint(const std::string& str, std::optional<char> suffix) noexcept
{
	  if (suffix.has_value()) 
		  PRINT((str + suffix.value()));
	  else PRINT(str);
}

void DebugPrint(const std::string* str_arr, const size_t& size, std::optional<const char*> seperator, std::optional<char> suffix) noexcept(false)
{
	if (size < 1 || str_arr == nullptr) throw std::runtime_error("DebugPrint(arr*) bad parameters.");
	std::string temp(*str_arr);
	if (suffix.has_value()) 
	{
		if (seperator.has_value()) for (size_t i = 1; i < size; ++i)
			temp += (seperator.value() + *(str_arr + i));
		else for (size_t i = 1; i < size; ++i)
			temp += *(str_arr + i);
		temp += suffix.value();
	} 
	else 
	{
		if (seperator.has_value()) for (size_t i = 1; i < size; ++i)
			temp += (seperator.value() + *(str_arr + i));
		else for (size_t i = 1; i < size; ++i)
			temp += *(str_arr + i);
	}

	PRINT(temp);
}

#pragma warning(push)
#pragma warning(disable : 6011)		//we are not dereferencing a nullptr it is dumb
#pragma warning(disable : 6387)		//if you read this you are probably dumb

bool CStringsEqual(const char* cstring1, const char* cstring2) noexcept
{
	if (cstring1 == cstring2) return true;	//addres check -> if both are same TRUE!	(true if both NULL)
	if ((cstring1 == nullptr) ^ (cstring2 == nullptr)) return false;  //checks if one is null and the other is not. then continue

	size_t len = std::strlen(cstring2);
	if (std::strlen(cstring1) != len) return false;	//size check
	size_t i = 0;

	//after every quick way of figuring out an answer lets compare every char 1 by 1 and give result (this can be very slow)
	while (i < len) if (cstring1[i] != cstring2[i++]) return false;
	return true;
}