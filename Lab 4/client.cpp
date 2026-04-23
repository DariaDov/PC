#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <iostream>
#include "my_network.h"
#include "data_packet.h"

int matrix_size = 10;
int matrix_size_elem = matrix_size * matrix_size;
int matrix_size_bytes = matrix_size_elem * sizeof(int);
int *matrix;
int *response_m;
const int max_num = 100;
const int thread_num = 3;

void printMatrix(int* matrix)
{
    for (int i = 0; i < matrix_size; i++) {
        for (int j = 0; j < matrix_size; j++) {
            std::cout << matrix[j + i * matrix_size] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void fillRandom() {
    matrix = new int[matrix_size_elem];

    for (int i = 0; i < matrix_size_elem; ++i) {
            matrix[i] = rand() % max_num;
    }
}

DataPacket* get_data_packet(int* packet_size) {
    *packet_size = data_packet_size() + matrix_size_bytes;
    DataPacket* packet = (DataPacket*) malloc(*packet_size);

    packet->matrix_size = matrix_size;
    packet->thread_num = thread_num;

    memcpy(packet->matrix, matrix, matrix_size_bytes);

    return packet;
}

int main() {
    fillRandom();
    printMatrix(matrix);

    int packet_size;

    DataPacket* packet = get_data_packet(&packet_size);;

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Помилка виклику socket");
        exit(1);
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
        perror("Помилка виклику connect");
        exit(1);
    }

    if (send(sock, COMMAND_POST, sizeof(COMMAND_POST), 0) == -1) {
        perror("Помилка виклику send");
        exit(1);
    }

    std::cout << "Command: POST" << std::endl;

    if (send(sock, packet, packet_size, 0) == -1) {
        perror("Помилка виклику send");
        exit(1);
    }

    char response_buff[100]{0};
    if (read(sock, response_buff, sizeof(response_buff)) == -1) {
        perror("Помилка виклику read");
        exit(1);
    }
    if (strcmp(RESPONSE_ACCEPTED, response_buff) != 0) {
        perror("Помилка серверу");
        exit(1);
    }

    std::cout << "Response: ACCEPTE" << std::endl;

    // START
    if (send(sock, COMMAND_START, sizeof(COMMAND_START), 0) == -1) {
        perror("Помилка виклику send");
        exit(1);
    }

    std::cout << "Command: START" << std::endl;

    if (read(sock, response_buff, sizeof(response_buff))== -1) {
        perror("Помилка виклику read");
        exit(1);
    }

    if (strcmp(RESPONSE_STARTED, response_buff) != 0) {
        perror("Помилка серверу");
        exit(1);
    }

    std::cout << "Response: STARTED" << std::endl;

    std::cout << std::endl;
    std::cout <<"--Started--"<<std::endl;

    // GET_STATUS
    while (true) {
        if (send(sock, COMMAND_GET_STATUS, sizeof(COMMAND_GET_STATUS), 0) == -1) {
            perror("Помилка виклику send");
            exit(1);
        }
      
        std::cout << "Command: GET STATUS" << std::endl;

        if (read(sock, response_buff, sizeof(response_buff))== -1) {
            perror("Помилка виклику read");
            exit(1);
        }

        if (strcmp(RESPONSE_COMPLETED, response_buff) == 0) {
            std::cout << "Response: COMPLETED" << std::endl;
            break;
        }

        if (strcmp(RESPONSE_FAILED, response_buff) == 0) {
            std::cout << "Response: FAILED" << std::endl;
            perror("Помилка серверу");
            exit(1);
        }
        
        std::cout << "Оброблено: " << response_buff << "%" << std::endl;

        sleep(1);
    }

    // GET_RESULT
    if (send(sock, COMMAND_GET_RESULT, sizeof(COMMAND_GET_RESULT), 0) == -1) {
        perror("Помилка виклику send");
        exit(1);
    }
    response_m = (int*) malloc(matrix_size_bytes);

    std::cout << "Command: GET RESULT" << std::endl;
    
    if (read(sock, response_m, matrix_size_bytes) == -1) {
        perror("Помилка виклику read");
        exit(1);
    }

    std::cout << "Результат:" << std::endl;
    printMatrix(response_m);
    
    close(sock);
    return 0;
}