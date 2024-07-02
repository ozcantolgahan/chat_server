#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <mutex>

class Server
{
public:
    Server(int port);
    void run();

private:
    int server_socket;
    int port;
    std::vector<int> clients;
    std::mutex clients_mutex;

    void handle_client(int client_socket);
};

#endif // SERVER_HPP
