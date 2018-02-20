#include"server.hpp"

namespace http_server
{
	server::server(
		const std::string filepath,
		const boost::asio::ip::address& address,
		unsigned short port)
		:ioc_(1), acceptor_(ioc_), filepath_(filepath)
	{
		boost::system::error_code ec;
		boost::asio::ip::tcp::endpoint endpoint(address, port);

		// we do acceptor_ open bind and listen here but no constructor, 
		// to know if some things wrong, which step happed
		acceptor_.open(endpoint.protocol(), ec);
		if (ec)
		{
			_error(ec, "acceptor open");
			return;
		}

		acceptor_.bind(endpoint, ec);
		if (ec)
		{
			_error(ec, "acceptor bind");
			return;
		}

		acceptor_.listen(500000, ec);// max_listen here
		if (ec)
		{
			_error(ec, "acceptor listen");
			return;
		}
	}

	void server::run()
	{
		if (!acceptor_.is_open())
		{
			return;
		}
		do_accept();
		ioc_.run();
	}

	void server::do_accept()
	{
		auto socket_ptr=std::make_shared < boost::asio::ip::tcp::socket>(ioc_);
		acceptor_.async_accept(*socket_ptr,
			[socket_ptr, this] (const boost::system::error_code& ec)
		{
			if (ec)
				_error(ec, "acceptor async_accept");
			else
				accept_handler(socket_ptr);

			do_accept();
		});
	}

	void server::accept_handler(std::shared_ptr < boost::asio::ip::tcp::socket> socket_ptr)
	{
		std::make_shared<session>(socket_ptr, filepath_)->read();
	}

} // namespace http_server