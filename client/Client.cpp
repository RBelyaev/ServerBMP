#include "Client.h"

void Client::check_file(std::ifstream& file, std::string &path)
{
    file.open(path, std::ios::binary);
    if (!file.is_open())
        throw std::runtime_error("Не удается открыть файл для отправки");
    
    char header[54];
    file.read(header, 54); 
    if (header[0] != 'B' || header[1] != 'M')
        throw std::runtime_error("Это не BMP файл");
    
    if(int(header[28]) != 24)
        throw std::runtime_error("Глубина не 24 бит/пиксель");
}


void Client::send_file(boost::asio::ip::tcp::socket &socket, std::ifstream& file)
{

    // Получаем размер файла
    file.seekg(0, std::ios::end);
    int file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    // Отправляем размер файла
    boost::system::error_code error;
    boost::asio::write(socket, boost::asio::buffer(&file_size, sizeof(file_size)), error);

    if (error)
        throw boost::system::system_error(error); // Обработка ошибок при отправке размера

    // Читаем содержимое файла в буфер и отправляем
    std::vector<char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    boost::asio::write(socket, boost::asio::buffer(buffer), error);

    if (error)
        throw boost::system::system_error(error); // Обработка ошибок при отправке данных
}


void Client::take_comands(boost::asio::ip::tcp::socket &socket)
{
    boost::system::error_code error;
    
    int n;
    std::cout << "Доступные команды: \n1. Поворот налево \n";
    std::cout << "2. Поворот направо \n3. Перевернуть \n4. Наложить фильтр Гауса\n";
    std::cout << "Введите количество команд: ";
    std::cin >> n;

    std::vector<int> comands(n);

    for(int i = 0; i < n; i++)
        std::cin >> comands[i];

    boost::asio::const_buffer num = boost::asio::buffer(&n, sizeof(n));
    boost::asio::write(socket, num, error);
    boost::asio::write(socket, boost::asio::buffer(comands), error);

}


void Client::receive_file(boost::asio::ip::tcp::socket &socket, const std::string &path)
{
    std::ofstream output(path, std::ios::binary);
    if (!output.is_open())
        throw std::runtime_error("Не удается открыть файл для записи");

    boost::system::error_code error;

    int file_size = 0; // размер получаемого файла
    boost::asio::read(socket, boost::asio::buffer(&file_size, sizeof(file_size)), error);
    if (error && error != boost::asio::error::eof)
        throw boost::system::system_error(error); // Обработка ошибок при чтении размера


    std::vector<char> buffer(file_size);

    int len;
    int total_bytes_received = 0; // счетчик числа считанных байт
    // Цикл для получения данных от сервера
    while (total_bytes_received < file_size)
    {
        len = socket.read_some(boost::asio::buffer(buffer), error);

        if (len <= 0) break; // Прекращаем, если нет данных

        output.write(buffer.data(), len); // Записываем только фактически прочитанные данные
        total_bytes_received += len;      // Увеличиваем общее количество полученных байт

        if (error == boost::asio::error::eof) break; // Конец передачи
        else if (error)
            throw boost::system::system_error(error); // Обработка других ошибок
    }

    output.close(); // Закрываем файл
}


void Client::run()
{
    try
    {
        boost::asio::io_context io_context;
        boost::asio::ip::tcp::socket socket(io_context);
        boost::asio::ip::tcp::resolver resolver(io_context);


        std::string path;
        std::cout << "Введите путь: ";
        std::cin >> path;
        std::ifstream file;
        check_file(file, path);


        auto endpoints = resolver.resolve("192.168.43.73", "7000");
        boost::asio::connect(socket, endpoints);

        send_file(socket, file);
        std::cout << "Файл успешно отправлен!" << std::endl;


        take_comands(socket);


        std::cout << "Введите путь для сохранения: ";
        std::cin >> path;

        receive_file(socket, path);
        std::cout << "Файл успешно получен и сохранен как: " << path << std::endl;

        // Корректное завершение работы с сокетом
        socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
        socket.close();
    }
    catch (std::exception &e)
    {
        std::cerr << "Ошибка: " << e.what() << std::endl;
    }
}