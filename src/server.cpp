#include "server.hpp"
#include <iostream>
#include <algorithm>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>

Server::Server(int port) : port(port) {}

void Server::run()
{
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        std::cerr << "Failed to create socket." << std::endl;
        return;
    }

    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port);

    if (bind(server_socket, (sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        std::cerr << "Failed to bind socket." << std::endl;
        return;
    }

    if (listen(server_socket, 5) == -1)
    {
        std::cerr << "Failed to listen on socket." << std::endl;
        return;
    }

    std::cout << "Server listening on port " << port << std::endl;

    while (true)
    {
        sockaddr_in client_address;
        socklen_t client_size = sizeof(client_address);
        int client_socket = accept(server_socket, (sockaddr *)&client_address, &client_size);

        if (client_socket == -1)
        {
            std::cerr << "Failed to accept client." << std::endl;
            continue;
        }

        std::cout << "Client connected." << std::endl;

        std::lock_guard<std::mutex> lock(clients_mutex);
        clients.push_back(client_socket);

        std::thread(&Server::handle_client, this, client_socket).detach();
    }

    close(server_socket);
}

void Server::handle_client(int client_socket)
{
    char buffer[1024];
    while (true)
    {
        ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0)
        {
            std::cout << "Client disconnected." << std::endl;
            close(client_socket);
            std::lock_guard<std::mutex> lock(clients_mutex);
            clients.erase(std::remove(clients.begin(), clients.end(), client_socket), clients.end());
            return;
        }

        buffer[bytes_received] = '\0';
        std::string message(buffer);
        std::cout << "Received: " << message << std::endl;

        std::lock_guard<std::mutex> lock(clients_mutex);
        for (int client : clients)
        {
            if (client != client_socket)
            {
                send(client, message.c_str(), message.size(), 0);
            }
        }
    }
}
