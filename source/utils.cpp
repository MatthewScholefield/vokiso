#include "utils.hpp"
#include <fstream>
#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <unistd.h>

#include <netinet/in.h>
#include <sys/socket.h>

constexpr int MESSAGE_SIZE_BYTES = 8;

void handleAlcError(ALCdevice *device, const char *msg) {
    ALCenum err = alcGetError(device);
    if (err != ALC_NO_ERROR) {
        fprintf(stderr, "Fatal OpenAL device error: %s\n", msg);
        exit(1);
    }
}

void handleAlError(const char *msg, bool exitOnFail) {
    ALCenum err = alGetError();
    if (err != AL_NO_ERROR) {
        fprintf(stderr, "Fatal OpenAL error: %s\n", msg);
        if (exitOnFail) {
            exit(1);
        }
    }
}

std::vector<std::string> readLines(const std::string &filename) {
    std::ifstream infile(filename);
    if (!infile) {
        throw std::runtime_error("No such file: " + filename);
    }
    std::string line;
    std::vector<std::string> lines;
    while (std::getline(infile, line)) {
        lines.push_back(line);
    }
    return lines;
}

bool recvAll(kissnet::socket<kissnet::protocol::tcp> &sock, std::byte *rawBuffer, size_t buffSize) {
    size_t numRead = 0;
    while (numRead < buffSize) {
        const auto [dataSize, statusCode] = sock.recv(rawBuffer + numRead, buffSize - numRead);
        if (statusCode != kissnet::socket_status::valid || dataSize < 1) {
            return false;
        }
        numRead += dataSize;
    }
    return true;
}

bool sendAll(kissnet::socket<kissnet::protocol::tcp> &sock, const std::byte *rawBuffer, size_t buffSize) {
    size_t numSent = 0;
    while (numSent < buffSize) {
        const auto [dataSize, statusCode] = sock.send(rawBuffer + numSent, buffSize - numSent);
        if (statusCode != kissnet::socket_status::valid || dataSize < 1) {
            return false;
        }
        numSent += dataSize;
    }
    return true;
}
