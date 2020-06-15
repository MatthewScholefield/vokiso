#pragma once
#include "kissnet.hpp"
#include <AL/al.h>
#include <AL/alc.h>
#include <string>
#include <vector>

void handleAlcError(ALCdevice *device, const char *msg);
void handleAlError(const char *msg, bool exitOnFail = true);

bool recvAll(kissnet::socket<kissnet::protocol::tcp> &sock, std::byte *rawBuffer, size_t buffSize);
bool sendAll(kissnet::socket<kissnet::protocol::tcp> &sock, const std::byte *rawBuffer, size_t buffSize);

template <size_t buffSize>
bool recvAll(kissnet::socket<kissnet::protocol::tcp> &sock, kissnet::buffer<buffSize> &buffer) {
    return recvAll(sock, buffer.data(), buffSize);
}

template <size_t buffSize>
bool sendAll(kissnet::socket<kissnet::protocol::tcp> &sock, const kissnet::buffer<buffSize> &buffer) {
    return sendAll(sock, buffer.data(), buffSize);
}

std::vector<std::string> readLines(const std::string &filename);
