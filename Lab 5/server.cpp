#include <iostream>
#include <sstream>
#include <string.h>
#include <thread>
#include <fstream>
#include <csignal>
#include <sys/socket.h>
#include "my_network.h"

const std::string html_not_found = "<html><body><h1>404 Not Found</h1></body></html>";
const std::string html_not_implemented = "<html><body><h1>501 Not Implemented. Waiting For GET</h1></body></html>";

const int BUFFER_SIZE = 4096;
int sock = -1;

void signal_handler(int signal) {
    close(sock);
    exit(0);
}

void sendResponse(int socket, const std::string& response) {
    if (send(socket, response.c_str(), response.size(), 0) == -1) {
        perror("Помилка виклику send");
        return;
    }
}

void handle_client(int socket) {
    struct Params {
        int socket;

        ~Params () {
            close(socket);
        }
    } params;

    params.socket = socket;

    char command_buff[BUFFER_SIZE]{0};

    if (read(socket, command_buff, BUFFER_SIZE) == -1) {
        perror("Помилка виклику read");
        return;
    }

    std::istringstream request(command_buff);
    std::string method, path, protocol_version;
    if (!(request >> method >> path >> protocol_version)) {
        perror("Помилка");
        return;
    }

    if (method != "GET") {
        std::string response = "HTTP/1.1 501 Not Implemented\n"
                   "Content-Length: " + std::to_string(html_not_implemented.size()) + "\n"
                   "Content-Type: text/html\n"
                   "Connection: Closed\n\n" + html_not_implemented;
                
        sendResponse(socket, "Очікую метод GET");
        return;
    }

    if (path == "/") {
        path = "/index.html";
    }

    std::ifstream file("./.." + path);
    std::string response;

    if (file.is_open()) {
        std::stringstream content;
        content << file.rdbuf();
        std::string body = content.str();

        response = "HTTP/1.1 200 OK\n"
                   "Content-Length: " + std::to_string(body.size()) + "\n"
                   "Content-Type: text/html\n"
                   "Connection: Closed\n\n" + body;
    } else {
        response = "HTTP/1.1 404 Not Found\n"
                    "Content-Length: " + std::to_string(html_not_found.size()) + "\n" 
                    "Content-Type: text/html\n"
                    "Connection: Closed\n\n" + html_not_found;
    }
    sendResponse(socket, response);
}

int main() {
    std::signal(SIGINT, signal_handler);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Помилка виклику socket");
        exit(1);
    }

    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (bind(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
        perror("Помилка виклику bind");
        exit(1);
    }

    if (listen(sock, 1) == -1) {
        perror("Помилка виклику listen");
        exit(1);
    }

    std::cout << "--Server listening on port " << MY_PORT << "--" << std::endl;

    while (true) {
        int new_socket = accept(sock, NULL, NULL);
        if (new_socket >= 0){
            std::cout << "--Server started working with client--" << new_socket << std::endl;

            std::thread(handle_client, new_socket).detach();
        }
    }

    return 0;
}