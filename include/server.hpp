#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <mutex>
#include <string>
#include <unordered_map>
#include <openssl/ssl.h>
#include <openssl/err.h>

class Server
{
public:
    Server(int port);
    ~Server();
    void run();

private:
    int server_socket;
    int port;
    SSL_CTX *ssl_context;
    std::vector<int> clients;
    std::unordered_map<int, std::string> usernames;
    std::mutex clients_mutex;

    void initialize_ssl();
    void handle_client(int client_socket);
    void log_message(const std::string &message);
    std::string get_timestamp();
};

#endif
