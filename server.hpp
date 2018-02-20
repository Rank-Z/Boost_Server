#ifndef SERVER_HPP
#define SERVER_HPP

#include"helper.hpp"
#include"session.hpp"
#include<string>
#include<memory>
#include<boost/asio.hpp>

namespace http_server
{
class server
{
public:
	server(
		const std::string filepath,
		const boost::asio::ip::address& address,
		unsigned short port);


	void run();

	void do_accept();

	void accept_handler(std::shared_ptr < boost::asio::ip::tcp::socket> socket_ptr);



private:
	boost::asio::io_context ioc_;
	boost::asio::ip::tcp::acceptor acceptor_;
	const std::string filepath_;
};

} // namespace http_server

#endif // !SERVER_HPP
