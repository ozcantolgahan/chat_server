#include "client.hpp"
#include <iostream>
#include <thread>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctime>

Client::Client(const std::string &ip, int port) : ip(ip), port(port), ssl(nullptr)
{
    initialize_ssl();
}

Client::~Client()
{
    SSL_shutdown(ssl);
    SSL_free(ssl);
}

void Client::initialize_ssl()
{
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
    SSL_library_init();
    ssl = SSL_new(SSL_CTX_new(SSLv23_client_method()));

    if (!ssl)
    {
        std::cerr << "SSL initialization failed." << std::endl;
        exit(EXIT_FAILURE);
    }

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1)
    {
        std::cerr << "Failed to create socket." << std::endl;
        exit(EXIT_FAILURE);
    }

    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &server_address.sin_addr);

    if (connect(client_socket, (sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        std::cerr << "Failed to connect to server." << std::
