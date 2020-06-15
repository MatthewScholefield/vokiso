#pragma once
#include "blockingconcurrentqueue.h"
#include "kissnet.hpp"
#include <functional>
#include <mutex>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

class AudioManager;

class HostConnection {
    struct Block {
        char *data;
        size_t n;
    };

  public:
    HostConnection(int port, AudioManager &audioManager);

    void run();

  private:
    AudioManager &audioManager;
    kissnet::socket<kissnet::protocol::tcp> server;
    std::vector<kissnet::socket<kissnet::protocol::tcp>> clients;
    moodycamel::BlockingConcurrentQueue<Block> messages;
    std::mutex clientsMut;
};