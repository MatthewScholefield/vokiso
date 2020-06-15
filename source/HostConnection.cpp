#include "HostConnection.hpp"
#include "AudioManager.hpp"
#include "kissnet.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <unistd.h>

#include "NetworkManager.hpp"

namespace kn = kissnet;

HostConnection::HostConnection(int port, AudioManager &audioManager)
    : audioManager(audioManager), server(kn::endpoint{"0.0.0.0", (kissnet::port_t)port}) {
    server.bind();
    server.listen();
}

void HostConnection::run() {
    std::thread t([&] {
        while (true) {
            Block block;
            messages.wait_dequeue(block);
            {
                std::lock_guard<std::mutex> lg(clientsMut);
                for (auto &i : clients) {
                    sendAll(i, (std::byte *)block.data, block.n);
                }
            }
            delete[] block.data;
        }
    });
    while (true) {
        auto inpConn = server.accept();
        printf("New connection.\n");
        kn::socket<kn::protocol::tcp> *conn;
        {
            std::lock_guard<std::mutex> lg(clientsMut);
            clients.emplace_back(std::move(inpConn));
            conn = &clients.back();
        }
        std::thread t([&] {
            std::byte buffer[NetworkManager::getMaxMessageSize()];
            while (true) {
                recvAll(*conn, buffer, 1);
                size_t msgSize = NetworkManager::getMessageSize((NetworkManager::MsgType)buffer[0]);
                recvAll(*conn, buffer + 1, msgSize);
                Block block = {new char[msgSize + 1], msgSize + 1};
                memcpy(block.data, buffer, block.n);
                messages.enqueue(block);
            }
        });
        t.detach();
    }
}
