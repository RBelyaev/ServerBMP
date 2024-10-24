#ifndef SIMPLE_SERVER_H
#define SIMPLE_SERVER_H

#include "boost/asio.hpp"
#include <boost/system/error_code.hpp>
#include <fstream>
#include <string>
#include <iostream>
#include <thread>
#include <sstream>
#include <cstdio>
#include "bmp/BMPclass.h"

class Server
{
private:
    void accept_file(boost::asio::ip::tcp::socket &socket, std::string &path);
    void send_file(boost::asio::ip::tcp::socket &socket, const std::string &file_path);
    std::vector<int> get_comands(boost::asio::ip::tcp::socket &socket);
    void changeBMP(std::string &file_path, std::string &save_path, std::vector<int> comands);
    void process_client(boost::asio::ip::tcp::socket &socket);
    
public:
    void run();
};

#endif