#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <vector>
#include <sys/socket.h>
#include <iostream>
#include <thread>
#include <string>
#include <csignal>
#include "my_network.h"
#include "data_packet.h"
#include "task.h"

int sock = socket(AF_INET, SOCK_STREAM, 0);

void cleanup() {
    close(sock);
}

void handle_client(int socket) {
    DataPacket received_header_data;
    char command_buff[100]{0};

    // waiting for POST
    if (read(socket, command_buff, sizeof(COMMAND_POST)) == -1) {
        perror("Помилка виклику read");
        return;
    }

    if (strcmp(COMMAND_POST, command_buff) != 0) {
        perror("Очікується POST");
        return;
    }

    std::cout << "Recieve: POST. Socket: " << socket << std::endl;

    if (read(socket, &received_header_data, data_packet_size())!= data_packet_size()) {
        perror("Помилка виклику read");
        return;
    }

    int packet_size = data_packet_size() + received_header_data.matrix_size_bytes();
    DataPacket *received_data = (DataPacket*) malloc(packet_size);
    received_data->matrix_size = received_header_data.matrix_size;
    received_data->thread_num = received_header_data.thread_num;

    if (read(socket, &received_data->matrix, received_data->matrix_size_bytes()) == -1) {
        perror("Помилка читання matrix");
        return;
    }

    if (send(socket, RESPONSE_ACCEPTED, sizeof(RESPONSE_ACCEPTED), 0) == -1) {
        perror("Помилка виклику send");
        return;
    }

    // waiting for START
    if (read(socket, command_buff, sizeof(COMMAND_START)) == -1) {
        perror("Помилка виклику read");
        return;
    }

    if (strcmp(COMMAND_START, command_buff) != 0) {
        perror("Очікується START");
        return;
    }

    std::cout << "Recieve: START. Socket: " << socket << std::endl;

    Task task(received_data->matrix_size, received_data->matrix, received_data->thread_num);
    task.run();

    if (send(socket, RESPONSE_STARTED, sizeof(RESPONSE_STARTED), 0) == -1) {
        perror("Помилка виклику send");
        task.finish();
        return;
    }

    // waiting for GET_STATUS
    while (true) {

        if (read(socket, command_buff, sizeof(COMMAND_GET_STATUS)) == -1) {
            perror("Помилка виклику read");
            task.finish();
            return;
        }

        if (strcmp(COMMAND_GET_STATUS, command_buff) != 0) {
            perror("Очікується GET START");
            task.finish();
            return;
        }
            
        std::cout << "Recieve: GET STATUS. Socket: " << socket << std::endl;

        if (task.is_finished()) {
            if (send(socket, RESPONSE_COMPLETED, sizeof(RESPONSE_COMPLETED), 0) == -1) {
                perror("Помилка виклику send");
                task.finish();
                return;
            }
            break;
        }
        else {
            char progress[100]{0};
            strcpy(progress, std::to_string(task.get_progress()).c_str());

            if (send(socket, progress, sizeof(progress), 0) == -1) {
                perror("Помилка виклику send");
                task.finish();
                return;
            }
        }
    }

    // waiting for GET_RESULT
    if (read(socket, command_buff, sizeof(COMMAND_GET_RESULT)) == -1) {
        perror("Помилка виклику read");
        task.finish();
        return;
    }

    if (strcmp(COMMAND_GET_RESULT, command_buff) != 0) {
        perror("Очікується GET_RESULT");
        task.finish();
        return;
    }

    std::cout << "Recieve: GET RESULT. Socket: " << socket << std::endl;

    if (send(socket, task.matrix, task.matrix_size_bytes, 0) == -1) {
        perror("Помилка виклику send");
        task.finish();
        return;
    }

    std::cout << "--Server stoped working with client--" << std::endl;
    task.finish();
    close(socket);
}

int main() {
    std::atexit(cleanup);
    signal(SIGPIPE, SIG_IGN);

    if (sock == -1) {
        perror("Помилка виклику socket");
        exit(1);
    }
    
    if (bind(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
        perror("Помилка виклику bind");
        exit(1);
    }

    if (listen(sock, 1) == -1) {
        perror("Помилка виклику listen");
        exit(1);
    }

    std::cout << "--Server listening--" << std::endl;

    std::vector<std::thread> threads;
    while (true) {
        int new_socket = accept(sock, NULL, NULL);

        std::cout << "--Server started working with client--" << std::endl;

        threads.push_back(std::thread(handle_client, new_socket));
    }

    return 0;
}