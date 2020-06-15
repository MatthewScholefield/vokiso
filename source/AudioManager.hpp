#pragma once
#include "readerwriterqueue.h"
#include <AL/al.h>
#include <AL/alc.h>
#include <functional>
#include <list>
#include <mutex>
#include <set>
#include <vector>

class AudioManager {
  public:
    using AudioHandler = std::function<void(char *buffer, size_t numSamples)>;
    using PersonId = ALuint;
    static constexpr int sampRate = 16000;
    static constexpr int capSize = 2048; // Size of capture buffer
    static constexpr int sampWidth = 2;  // bytes per sample
    static constexpr float roomSx = 10.f, roomSy = 10.f;

    AudioManager();
    ~AudioManager();

    void update();

    PersonId createPerson();
    void deletePerson(PersonId person);

    void writeAudio(PersonId person, char *audio, size_t numSamples);
    void setPos(PersonId person, int x, int y);
    void setSelfPos(int x, int y);
    void onAudio(const AudioHandler &func);

  private:
    moodycamel::BlockingReaderWriterQueue<ALuint> bufferQueue;

    std::vector<ALuint> buffers;
    std::set<PersonId> people;

    ALCdevice *audioDevice;
    ALCcontext *audioContext;
    ALCdevice *inputDevice;

    char buffer[capSize * sampWidth];
    AudioHandler audioHandler;

    std::mutex mut;
};
