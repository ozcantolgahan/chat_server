#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <openssl/ssl.h>
#include <openssl/err.h>

class Client
{
public:
    Client(const std::string &ip, int port);
    ~Client();
    void run(const std::string &username);

private:
    int client_socket;
    SSL *ssl;
    std::string ip;
    int port;

    void initialize_ssl();
    void receive_messages();
    std::string get_timestamp();
};

#endif
