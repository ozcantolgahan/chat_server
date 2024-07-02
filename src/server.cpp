#include "server.hpp"
#include <iostream>
#include <algorithm>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <fstream>
#include <ctime>

std::ofstream log_file("server_log.txt", std::ios_base::app);

Server::Server(int port) : port(port), ssl_context(nullptr)
{
    initialize_ssl();
}

Server::~Server()
{
    SSL_CTX_free(ssl_context);
}

void Server::initialize_ssl()
{
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
    ssl_context = SSL_CTX_new(SSLv23_server_method());
    if (!ssl_context)
    {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    SSL_CTX_set_ecdh_auto(ssl_context, 1);

    if (SSL_CTX_use_certificate_file(ssl_context, "server-cert.pem", SSL_FILETYPE_PEM) <= 0)
    {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    if (SSL_CTX_use_PrivateKey_file(ssl_context, "server-key.pem", SSL_FILETYPE_PEM) <= 0)
    {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
}

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
    log_message("Server started on port " + std::to_string(port));

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

        SSL *ssl = SSL_new(ssl_context);
        SSL_set_fd(ssl, client_socket);
        if (SSL_accept(ssl) <= 0)
        {
            ERR_print_errors_fp(stderr);
            close(client_socket);
            continue;
        }

        std::cout << "Client connected." << std::endl;
        log_message("Client connected.");

        std::thread(&Server::handle_client, this, client_socket, ssl).detach();
    }

    close(server_socket);
}

void Server::handle_client(int client_socket, SSL *ssl)
{
    char username[1024];
    ssize_t bytes_received = SSL_read(ssl, username, sizeof(username));
    if (bytes_received <= 0)
    {
        SSL_shutdown(ssl);
        close(client_socket);
        return;
    }
    username[bytes_received] = '\0';
    std::string user(username);

    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        usernames[client_socket] = user;
        clients.push_back(client_socket);
    }

    std::string welcome_message = user + " has joined the chat.";
    std::cout << welcome_message << std::endl;
    log_message(welcome_message);

    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        for (int client : clients)
        {
            if (client != client_socket)
            {
                SSL_write(ssl, welcome_message.c_str(), welcome_message.size());
            }
        }
    }

    char buffer[1024];
    while (true)
    {
        ssize_t bytes_received = SSL_read(ssl, buffer, sizeof(buffer));
        if (bytes_received <= 0)
        {
            std::cout << "Client disconnected." << std::endl;
            log_message("Client disconnected.");
            SSL_shutdown(ssl);
            close(client_socket);
            {
                std::lock_guard<std::mutex> lock(clients_mutex);
                clients.erase(std::remove(clients.begin(), clients.end(), client_socket), clients.end());
                usernames.erase(client_socket);
            }
            return;
        }

        buffer[bytes_received] = '\0';
        std::string message(buffer);
        std::string message_with_timestamp = get_timestamp() + " " + message;

        if (message.substr(0, 5) == "/msg ")
        {
            size_t pos = message.find(' ', 5);
            std::string recipient = message.substr(5, pos - 5);
            std::string private_message = message.substr(pos + 1);

            std::lock_guard<std::mutex> lock(clients_mutex);
            for (const auto &pair : usernames)
            {
                if (pair.second == recipient)
                {
                    std::string private_msg_with_sender = "[Private] " + user + ": " + private_message;
                    SSL_write(ssl, private_msg_with_sender.c_str(), private_msg_with_sender.size());
                    break;
                }
            }
            continue;
        }
        if (message == "/users")
        {
            std::lock_guard<std::mutex> lock(clients_mutex);
            std::string user_list = "Connected users:\n";
            for (const auto &pair : usernames)
            {
                user_list += pair.second + "\n";
            }
            SSL_write(ssl, user_list.c_str(), user_list.size());
            continue;
        }

        std::cout << "Received: " << message << std::endl;
        log_message("Received: " + message);

        std::lock_guard<std::mutex> lock(clients_mutex);
        for (int client : clients)
        {
            if (client != client_socket)
            {
                SSL_write(ssl, message_with_timestamp.c_str(), message_with_timestamp.size());
            }
        }
    }
}

void Server::log_message(const std::string &message)
{
    log_file << get_timestamp() << " " << message << std::endl;
}

std::string Server::get_timestamp()
{
    time_t now = time(0);
    char buf[80];
    struct tm tstruct;
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
    return buf;
}
