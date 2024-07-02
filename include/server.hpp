#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <mutex>
#include <string>
#include <unordered_map>

class Server
{
public:
    Server(int port);
    void run();

private:
    int server_socket;
    int port;
    std::vector<int> clients;
    std::unordered_map<int, std::string> usernames;
    std::mutex clients_mutex;

    void handle_client(int client_socket);
    void log_message(const std::string &message);
    std::string get_timestamp();
};

#endif
