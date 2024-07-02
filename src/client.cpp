#include "client.hpp"
#include <iostream>
#include <thread>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctime>

Client::Client(const std::string &ip, int port) : ip(ip), port(port) {}

void Client::run(const std::string &username)
{
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1)
    {
        std::cerr << "Failed to create socket." << std::endl;
        return;
    }

    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &server_address.sin_addr);

    if (connect(client_socket, (sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        std::cerr << "Failed to connect to server." << std::endl;
        return;
    }

    std::cout << "Connected to server." << std::endl;
    send(client_socket, username.c_str(), username.size(), 0);

    std::thread(&Client::receive_messages, this).detach();

    while (true)
    {
        std::string message;
        std::getline(std::cin, message);
        send(client_socket, message.c_str(), message.size(), 0);
    }

    close(client_socket);
}

void Client::receive_messages()
{
    char buffer[1024];
    while (true)
    {
        ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0)
        {
            std::cout << "Server disconnected." << std::endl;
            close(client_socket);
            return;
        }

        buffer[bytes_received] = '\0';
        std::cout << "Received (" << get_timestamp() << "): " << buffer << std::endl;
    }
}

std::string Client::get_timestamp()
{
    time_t now = time(0);
    char buf[80];
    struct tm tstruct;
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
    return buf;
}
