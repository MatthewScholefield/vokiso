#include "NetworkManager.hpp"
#include "AudioManager.hpp"
#include "kissnet.hpp"

#include <algorithm>
#include <chrono>
#include <climits>
#include <functional>
#include <random>
#include <thread>
#include <vector>

namespace kn = kissnet;

NetworkManager::NetworkManager(const WordEncoder::Address &addr) : userId({}), sock(kn::endpoint(addr.ip, addr.port)), audioManager(nullptr) {
    std::independent_bits_engine<std::default_random_engine, CHAR_BIT, unsigned char> rbe;
    std::generate((unsigned char *)userId.data(), (unsigned char *)(userId.data() + userId.size()), std::ref(rbe));
    sock.connect();
}

void NetworkManager::setAudioManager(AudioManager *audioManager) {
    this->audioManager = audioManager;
}

void NetworkManager::run() {
    std::thread t([&] {
        while (true) {
            {
                auto msgTypeId = (std::byte)MsgType::HEARTBEAT;
                std::lock_guard<std::mutex> lg(sockWrMut);
                sendAll(sock, &msgTypeId, 1);
                sendAll(sock, userId);
            }
            std::this_thread::sleep_for(std::chrono::seconds(heartbeatSeconds));
            {
                std::lock_guard<std::mutex> lg(hbMut);
                auto curTime = time(nullptr);
                for (auto it = lastHeartbeat.begin(); it != lastHeartbeat.end();) {
                    if (curTime - it->second > 3 * heartbeatSeconds) {
                        std::lock_guard<std::mutex> lg2(ppMut);
                        auto it2 = people.find(it->first);
                        if (it2 != people.end()) {
                            audioManager->deletePerson(it2->second);
                            people.erase(it2);
                        }
                        it = lastHeartbeat.erase(it);
                    } else {
                        ++it;
                    }
                }
            }
        }
    });
    t.detach();
    while (true) {
        std::byte msgTypeId;
        recvAll(sock, &msgTypeId, 1);
        switch ((MsgType)msgTypeId) {
        case MsgType::AUDIO: {

            kn::buffer<userIdBytes> speakerId;
            kn::buffer<AudioManager::capSize * AudioManager::sampWidth> buffer;
            recvAll(sock, speakerId);
            recvAll(sock, buffer);

            if (speakerId != userId) {
                auto it = people.find(speakerId);
                if (it == people.end()) {
                    it = people.emplace(speakerId, audioManager->createPerson()).first;
                }
                audioManager->writeAudio(it->second, (char *)buffer.data(), AudioManager::capSize);
            }
        } break;
        case MsgType::LEAVE: { // TODO: Generate this
            kn::buffer<userIdBytes> speakerId;
            recvAll(sock, speakerId);

            auto it = people.find(speakerId);
            if (it == people.end()) {
                throw std::runtime_error("Nonexistent speaker left");
            }
            audioManager->deletePerson(it->second);
            people.erase(it);
        } break;
        case MsgType::POS: {
            kn::buffer<userIdBytes> speakerId;
            kn::buffer<2> pos;
            recvAll(sock, speakerId);
            recvAll(sock, pos);

            auto it = people.find(speakerId);
            if (it == people.end()) {
                it = people.emplace(speakerId, audioManager->createPerson()).first;
            }
            audioManager->setPos(it->second, (int)pos[0], (int)pos[1]);
        } break;
        case MsgType::HEARTBEAT: {

            kn::buffer<userIdBytes> speakerId;
            recvAll(sock, speakerId);

            auto it = people.find(speakerId);
            if (it != people.end()) {
                printf("Heart beat: %d\n", it->second);
            } else {
                printf("Heart beat: self\n");
            }

            std::lock_guard<std::mutex> lg(hbMut);
            lastHeartbeat[speakerId] = time(nullptr);
        } break;
        }
    }
}

void NetworkManager::writeSamples(char *data, size_t numSamples) {
    auto msgTypeId = (std::byte)MsgType::AUDIO;
    std::lock_guard<std::mutex> lg(sockWrMut);
    recvAll(sock, &msgTypeId, 1);
    recvAll(sock, userId);
    recvAll(sock, (std::byte *)data, numSamples * AudioManager::sampWidth);
}
