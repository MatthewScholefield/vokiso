#pragma once

#include <atomic>
#include <climits>
#include <map>
#include <mutex>
#include <random>

#include "AudioManager.hpp"
#include "WordEncoder.hpp"
#include "kissnet.hpp"

class NetworkManager {
  public:
    enum class MsgType : char {
        AUDIO = 0,
        POS = 1,
        LEAVE = 3,
        HEARTBEAT = 4
    };
    NetworkManager(const WordEncoder::Address &addr);

    void run();
    void writeSamples(char *data, size_t numSamples);
    void setAudioManager(AudioManager *audioManager);

    static constexpr size_t getMessageSize(MsgType type) {
        switch (type) {
        case MsgType::AUDIO:
            return userIdBytes + AudioManager::capSize * AudioManager::sampWidth;
        case MsgType::HEARTBEAT:
            return userIdBytes;
        case MsgType::LEAVE:
            return userIdBytes;
        case MsgType::POS:
            return userIdBytes + 2;
        default:
            return 0;
        }
    }
    static constexpr size_t getMaxMessageSize() {
        return getMessageSize(MsgType::AUDIO);
    }

  private:
    static constexpr int heartbeatSeconds = 5;
    static constexpr size_t userIdBytes = 4;
    kissnet::buffer<4> userId;

    std::map<kissnet::buffer<4>, AudioManager::PersonId> people;
    std::mutex hbMut, ppMut, sockWrMut;
    std::map<kissnet::buffer<4>, time_t> lastHeartbeat;
    kissnet::tcp_socket sock;
    AudioManager *audioManager;
};
