# Chat server

This project is a simple network-based chat application. Users can connect to a server and exchange messages with other users in real-time. The project is written in C++ and includes features like user authentication, timestamps, private messaging, and user lists.

## Features

- **User Authentication:** Users provide a username to join the chat room.
- **Timestamps:** Messages are displayed with the time they were sent.
- **Private Messaging:** Users can send private messages to specific users.
- **User List:** Users can see a list of connected users.
- **Logging:** The server logs all messages and events to a log file.

## Requirements

To run this project, you will need:

- CMake 3.10 or higher
- A compiler that supports C++11 or higher (e.g., g++)

## Installation

1. Clone the project:
    ```sh
    https://github.com/ozcantolgahan/chat_server.git
    cd network-chat-app
    ```

2. Build and run:
    ```sh
    mkdir build
    cd build
    cmake ..
    make
    ```

## Usage

### Starting the Server

1. Start the server by running the `server_main` application:
    ```sh
    ./server_main
    ```

2. The server will start listening on the specified port (default port: 8080).

### Starting the Client

1. Start the client by running the `client_main` application:
    ```sh
    ./client_main
    ```

2. Enter the server's IP address and port number (default: 127.0.0.1:8080).

3. Enter your username and start chatting with other users.

### Commands

- **Private Message:** To send a private message to a specific user, use the `/msg <username> <message>` command.
    ```sh
    /msg username Hello, how are you?
    ```

- **User List:** To see a list of connected users, use the `/users` command.
    ```sh
    /users
    ```

## Contributing

If you want to contribute, please open an issue to discuss your changes before submitting a pull request.


## Contact

If you have any questions or suggestions, feel free to contact us at tolgahanozcan@hotmail.com
