#ifndef CLIENT_H
#define CLIENT_H

#include "boost/asio.hpp"
#include <boost/system/error_code.hpp>
#include <fstream>
#include <iostream>
#include <string>


class Client
{
private:
    void check_file(std::ifstream& file, std::string &path);
    void send_file(boost::asio::ip::tcp::socket& socket, std::ifstream& file);
    void take_comands(boost::asio::ip::tcp::socket& socket);
    void receive_file(boost::asio::ip::tcp::socket &socket, const std::string &path);

public:
    void run();
};


#endif