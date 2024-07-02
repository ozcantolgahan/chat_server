#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client
{
public:
    Client(const std::string &ip, int port);
    void run(const std::string &username);

private:
    int client_socket;
    std::string ip;
    int port;

    void receive_messages();
    std::string get_timestamp();
};

#endif