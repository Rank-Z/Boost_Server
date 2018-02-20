#include"server.hpp"
#include"session.hpp"

#include<string>

//test on Windows

int main()
{
	std::string path="D:\\temp";
	auto const address = boost::asio::ip::make_address("127.0.0.1");
	http_server::server ser(path , address , 8000);
	ser.run();
}

