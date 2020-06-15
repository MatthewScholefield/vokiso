#include <thread>

#include "AudioManager.hpp"
#include "CLI11.hpp"
#include "HostConnection.hpp"
#include "NetworkManager.hpp"
#include "PortMapping.hpp"
#include "WordEncoder.hpp"

struct Arguments {
    Arguments() : app("The next generation of voice chat") {
        app.ignore_case();
        app.require_subcommand(1);
        hostCommand = app.add_subcommand("host", "Host");
        joinCommand = app.add_subcommand("join", "Join");
        joinCommand->add_option("server words", joinLocation);
    }

    CLI::App app;
    CLI::App *hostCommand;
    CLI::App *joinCommand;
    std::string joinLocation;
};

int main(int argc, char *argv[]) {
    Arguments args;
    CLI11_PARSE(args.app, argc, argv);
    WordEncoder encoder("data/words.txt");
    srand(time(nullptr));

    if (*args.hostCommand) {
        int lower = 49152;
        int upper = 65535;
        int localPort = lower + rand() % (upper - lower);
        int publicPort = lower + rand() % (upper - lower);
        printf("Forwarding public %d -> local %d\n", publicPort, localPort);
        PortMapping mapping(publicPort, localPort);
        printf("Server address: %s:%d\n", mapping.getExternalAddress(), publicPort);
        auto code = encoder.toPairingCode({mapping.getExternalAddress(), publicPort});
        printf("Server code: %s\n", code.c_str());
        AudioManager audio;
        HostConnection conn(localPort, audio);
        std::thread hostThread([&] {
            conn.run();
        });
        NetworkManager nm(WordEncoder::Address{"127.0.0.1", localPort});
        nm.setAudioManager(&audio);
        audio.onAudio([&](char *buffer, size_t numSamples) {
            nm.writeSamples(buffer, numSamples);
        });
        std::thread nmThread([&] {
            nm.run();
        });
        while (true) {
            audio.update();
            usleep(10000000);
        }
    } else if (*args.joinCommand) {
        auto addr = encoder.fromPairingCode(args.joinLocation);
        printf("Connecting to %s:%d...\n", addr.ip.c_str(), addr.port);
        NetworkManager nm(addr);
        AudioManager audio;
        nm.setAudioManager(&audio);
        audio.onAudio([&](char *buffer, size_t numSamples) {
            nm.writeSamples(buffer, numSamples);
        });
        std::thread t([&] {
            nm.run();
        });
        while (true) {
            audio.update();
            usleep(10000000);
        }
    }
    return 0;
}