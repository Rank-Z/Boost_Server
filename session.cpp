#include"session.hpp"


namespace http_server
{
session::session(std::shared_ptr<boost::asio::ip::tcp::socket> socket_ptr ,
	const std::string& filepath)
	:socket_ptr_(socket_ptr),filepath_(filepath),strand_((*socket_ptr_).get_io_context())
{   }

void session::read()
{
	auto shared_ptr=shared_from_this();
	http::async_read(*socket_ptr_ , buffer_ , request_ , strand_.wrap(
		[shared_ptr] (boost::system::error_code ec , std::size_t read_size)
	{
		if (ec)
		{
			_error(ec , "async_read");
			shared_ptr->close_session(false);
			return;
		}
		shared_ptr->read_handler(shared_ptr);
	}
	));
}

void session::read_handler(std::shared_ptr<session> self_ptr)
{
	auto res = std::make_shared<http::response<http::string_body>>
		(http::status::bad_request,request_.version());
	(*res).set(http::field::server , BOOST_BEAST_VERSION_STRING);
	(*res).set(http::field::content_type , "text/html");
	(*res).keep_alive(request_.keep_alive());
	if (request_.method() != http::verb::get) // GET only temporary
	{
		(*res).body() = "Get only!";
	}
	else if(request_.target().empty() ||
		request_.target().front() != '/') // bad request
	{
		(*res).body() = "bad request";
	}
	else
	{
		std::string file(filepath_);
		file.append(request_.target().data() , request_.target().size());

#if _MSC_VER
		for (auto &t : file)
			if (t == '/')
				t = '\\';
#else // Linux
		for (auto&t : file)
			if (t == '\\')
				t = '/';
#endif // #if _MSC_VER

		if (request_.target().back() == '/')
			file.append("index.html");

		boost::system::error_code ec;
		http::file_body::value_type body;
		body.open(file.c_str() , boost::beast::file_mode::scan , ec);

		if (ec)
		{
			(*res).body() = "cannot find this file";
			(*res).result(http::status::not_found);
		}
		else // good request
		{
			res = nullptr;
			auto good_res = std::make_shared<http::response<http::file_body>>(
				std::piecewise_construct ,
				std::make_tuple(std::move(body)) ,
				std::make_tuple(http::status::ok , request_.version())
				);
			(*good_res).set(http::field::server, BOOST_BEAST_VERSION_STRING);
			(*good_res).set(http::field::content_type, "text/html"); // html tempoary
			(*good_res).content_length(body.size());
			(*good_res).keep_alive(request_.keep_alive());
			http::async_write(*socket_ptr_, *good_res, strand_.wrap(
				[self_ptr, good_res] (boost::system::error_code ec, std::size_t write_size)
			{
				self_ptr->write_handler(ec);
			}));
			return;
		}
	}
	(*res).prepare_payload();
	http::async_write(*socket_ptr_,*res,strand_.wrap(
		[self_ptr,res] (boost::system::error_code ec,std::size_t write_size)
	{
		self_ptr->write_handler(ec);
	}));
}

void session::write_handler(boost::system::error_code ec)
{
	if (ec)
		_error(ec , "async_write");
	close_session(true);
}

void session::close_session(bool flag)
{
	boost::system::error_code ec;
	if(flag)
		(*socket_ptr_).shutdown(boost::asio::ip::tcp::socket::shutdown_send , ec);
	else
		(*socket_ptr_).shutdown(boost::asio::ip::tcp::socket::shutdown_both , ec);
}


} // namespace http_server

