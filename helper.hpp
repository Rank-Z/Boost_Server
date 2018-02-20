#ifndef HELPER_HPP
#define HELPER_HPP

#include<iostream>
#include<boost\system\error_code.hpp>

namespace http_server
{
inline void _error(const boost::system::error_code& ec,const char* str)
{
	std::cerr << str << " : fail " << std::endl;
	std::cerr << ec.message() << std::endl;
}




} // namespace http_server

#endif // !HELPER_HPP

