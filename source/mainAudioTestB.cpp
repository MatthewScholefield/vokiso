#include <thread>

#include "AudioManager.hpp"
#include "CLI11.hpp"
#include "HostConnection.hpp"

struct Arguments {
    Arguments() : app("Vokiso audio test b") {}

    CLI::App app;
};

struct AudioBuffer {
    char *data;
    size_t numSamples;
};

/**
 * Tests the audio manager by echoing audio in multithreaded environment
 */
int main(int argc, char *argv[]) {
    Arguments args;
    CLI11_PARSE(args.app, argc, argv);

    AudioManager audio;
    AudioManager::PersonId self = audio.createPerson();
    moodycamel::BlockingConcurrentQueue<AudioBuffer> audioChunks;

    std::thread t([&] {
        while (true) {
            AudioBuffer buf;
            audioChunks.wait_dequeue(buf);
            audio.writeAudio(self, buf.data, buf.numSamples);
            delete buf.data;
        }
    });

    audio.onAudio([&](char *buffer, size_t numSamples) {
        AudioBuffer audioBuffer{new char[numSamples * AudioManager::sampWidth], numSamples};
        memcpy(audioBuffer.data, buffer, numSamples * AudioManager::sampWidth);
        audioChunks.enqueue(audioBuffer);
    });
    while (true) {
        audio.update();
    }
    return 0;
}