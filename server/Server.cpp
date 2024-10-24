#include "Server.h"

void Server::accept_file(boost::asio::ip::tcp::socket &socket, std::string &path)
{
    std::ofstream output(path, std::ios::binary);
    if (!output.is_open())
        throw std::runtime_error("Не удается открыть файл для записи"); // проверяем, открылся ли файл

    boost::system::error_code error;
    int file_size = 0; // размер получаемого файла

    // Сначала читаем размер файла
    boost::asio::read(socket, boost::asio::buffer(&file_size, sizeof(file_size)), error);

    if (error && error != boost::asio::error::eof)
        throw boost::system::system_error(error); // Обработка ошибок при чтении размера

    std::vector<char> buffer(file_size);

    int len;
    int total_bytes_received = 0; // счетчик числа считанных байт
    while (total_bytes_received < file_size)
    {
        len = socket.read_some(boost::asio::buffer(buffer), error);

        if (len <= 0) break; // Прекращаем, если нет данных


        output.write(buffer.data(), len); // Записываем только фактически прочитанные данные
        total_bytes_received += len;      // Увеличиваем общее количество полученных байт

        if (error == boost::asio::error::eof) break; // Конец передачи
        else if (error) throw boost::system::system_error(error); // Обработка других ошибок
    }

    output.close();
}

void Server::send_file(boost::asio::ip::tcp::socket &socket, const std::string &file_path)
{
    // Открываем файл и отправляем данные
    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open())
        throw std::runtime_error("Не удается открыть файл для отправки");

    boost::system::error_code error;
    // Получаем размер файла
    file.seekg(0, std::ios::end);
    int file_size = file.tellg();
    file.seekg(0, std::ios::beg);
    boost::asio::write(socket, boost::asio::buffer(&file_size, sizeof(file_size)), error);
    if (error)
        throw boost::system::system_error(error); // Обработка ошибок при отправке размера


    std::vector<char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    boost::asio::write(socket, boost::asio::buffer(buffer), error);

    if (error)
        throw boost::system::system_error(error);
}

std::vector<int> Server::get_comands(boost::asio::ip::tcp::socket &socket)
{
    boost::system::error_code error;
    int n;
    boost::asio::read(socket, boost::asio::buffer(&n, sizeof(n)), error);
    if (error && error != boost::asio::error::eof)
        throw boost::system::system_error(error); // Обработка ошибок при чтении размера

    std::vector<int> comands(n);
    int total_bytes_received = 0; // счетчик числа считанных байт
    int len;
    while (total_bytes_received < n * sizeof(int))
    {
        len = socket.read_some(boost::asio::buffer(comands), error);

        if (len <= 0) break; // Прекращаем, если нет данных
        total_bytes_received += len;      // Увеличиваем общее количество полученных байт

        if (error == boost::asio::error::eof) break; // Конец передачи
        else if (error) throw boost::system::system_error(error); // Обработка других ошибок
    }
    return comands;
}




void Server::changeBMP(std::string &file_path, std::string &save_path, std::vector<int> comands)
{
    ImageBMP img;

    img.ReadBMPfile(file_path);


    for(auto& comand: comands)
    {
        if(comand == 1)
            img.Rotation(ImageBMP::ImageState::LEFT);
        if(comand == 2)
            img.Rotation(ImageBMP::ImageState::RIGHT);
        if(comand == 3)
            img.Rotation(ImageBMP::ImageState::FLIP);
        if(comand == 4)
            img.GaussFilter(2);
    }

    img.SaveImage(save_path);
}

void Server::process_client(boost::asio::ip::tcp::socket &socket)
{
    // Прием данных
    std::stringstream ss;
    ss << std::this_thread::get_id();
    std::string thread_id;
    ss >> thread_id;
    std::string path1 = "../ans/" + thread_id + ".bmp";
    std::string path2 = path1;
    accept_file(socket, path1);

    std::vector<int>comands = get_comands(socket);

    if(comands.size())
        changeBMP(path1, path2, comands);

    send_file(socket, path2);

    std::remove(path1.c_str()); // удаляем файл на сервере
    // Корректное завершение работы с сокетом
    socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
    socket.close();

}

void Server::run()
{
    boost::asio::thread_pool pool(5);
    try
    {
        boost::asio::io_context io_context;
        boost::asio::ip::tcp::acceptor acceptor(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 7000));
        

        while (1)
        {
            boost::asio::ip::tcp::socket socket(io_context);
            // Ожидание подключения клиента
            acceptor.accept(socket);

            boost::asio::post(pool, [this, socket = std::move(socket)]() mutable 
            {
                process_client(socket);  // Вызов функции обработки клиента
            });
        }
    }
    catch (std::exception &e)
    {
        std::cerr << "Ошибка: " << e.what() << std::endl;
    }
    pool.join();
}