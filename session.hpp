#ifndef SESSION_HPP
#define SESSION_HPP

#include"helper.hpp"
#include<memory>
#include<string>
#include<boost/asio.hpp>
#include<boost/beast/http.hpp>
#include<boost/beast/core.hpp>
#include<boost/beast/version.hpp>
namespace http = boost::beast::http;

namespace http_server
{
class session:public std::enable_shared_from_this<session>
{
public:
	session(std::shared_ptr<boost::asio::ip::tcp::socket> socket_ptr ,
		const std::string& filepath);

	void read();

	void read_handler(std::shared_ptr<session> self_ptr);

	void write_handler(boost::system::error_code ec);

	void close_session(bool flag);

private:
	std::shared_ptr<boost::asio::ip::tcp::socket> socket_ptr_;
	boost::beast::flat_buffer buffer_;
	const std::string& filepath_;
	http::request<http::string_body> request_;
	boost::asio::io_service::strand strand_;
};

} // namespace http_server


#endif // !SESSION_HPP
