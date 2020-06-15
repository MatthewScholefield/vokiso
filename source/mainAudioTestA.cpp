#include <thread>

#include "AudioManager.hpp"
#include "CLI11.hpp"
#include "HostConnection.hpp"
#include "NetworkManager.hpp"
#include "PortMapping.hpp"
#include "WordEncoder.hpp"

struct Arguments {
    Arguments() : app("Vokiso audio test a") {}

    CLI::App app;
};

/**
 * Tests the audio manager by directly echoing audio
 */
int main(int argc, char *argv[]) {
    Arguments args;
    CLI11_PARSE(args.app, argc, argv);

    AudioManager audio;
    auto self = audio.createPerson();
    audio.onAudio([&](char *buffer, size_t numSamples) {
        audio.writeAudio(self, buffer, numSamples);
    });
    while (true) {
        audio.update();
    }
    return 0;
}