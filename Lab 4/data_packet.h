#pragma once

#define COMMAND_POST "POST"
#define COMMAND_START "START"
#define COMMAND_GET_STATUS "GET STATUS"
#define COMMAND_GET_RESULT "GET RESULT"

#define RESPONSE_ACCEPTED "ACCEPTED"
#define RESPONSE_FAILED "FAILED"
#define RESPONSE_STARTED "STARTED"
#define RESPONSE_COMPLETED "COMPLETED"
#define RESPONSE_NO_RESULT "NO RESULT"

struct DataPacket {
    int matrix_size;
    int thread_num;
    int matrix[];

    int matrix_size_elem() {
        return matrix_size * matrix_size;
    }

    int matrix_size_bytes() {
        return matrix_size_elem() * sizeof(int);
    }
};

int data_packet_size() {
    return sizeof(DataPacket::matrix_size) + sizeof(DataPacket::thread_num);
}