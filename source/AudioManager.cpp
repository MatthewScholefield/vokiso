
#include "AudioManager.hpp"
#include "NetworkManager.hpp"
#include "utils.hpp"

AudioManager::AudioManager()
    : audioDevice(nullptr), audioContext(nullptr), inputDevice(nullptr) {
    audioDevice = alcOpenDevice(nullptr); // Request default audio device
    handleAlcError(audioDevice, "opening output device");
    audioContext = alcCreateContext(audioDevice, nullptr);
    alcMakeContextCurrent(audioContext);
    handleAlcError(audioDevice, "setting context");

    inputDevice = alcCaptureOpenDevice(nullptr, sampRate, AL_FORMAT_MONO16, sampRate / 2);
    handleAlcError(inputDevice, "opening input device");
    alcCaptureStart(inputDevice);
    handleAlcError(inputDevice, "starting capture");

    alListener3f(AL_POSITION, 0.f, 0.f, 0.f);
    handleAlError("creating source");
}

AudioManager::~AudioManager() {
    alcCaptureStop(inputDevice);
    alcCaptureCloseDevice(inputDevice);

    for (auto i : people) {
        alSourceStopv(1, &i);
        alSourcei(i, AL_BUFFER, 0);
        alDeleteSources(1, &i);
    }

    alDeleteBuffers(buffers.size(), buffers.data());
    handleAlError("deleting sources/buffers", false);
    alcMakeContextCurrent(NULL);
    handleAlError("deleting context", false);
    alcDestroyContext(audioContext);
    alcCloseDevice(audioDevice);
}

void AudioManager::update() {
    std::lock_guard<std::mutex> lg(mut);
    for (auto person : people) { // Recover stale buffers
        ALint availBuffers = 0;
        alGetSourcei(person, AL_BUFFERS_PROCESSED, &availBuffers);
        if (availBuffers > 0) {
            ALuint staleBuffers[16];
            alSourceUnqueueBuffers(person, availBuffers, staleBuffers);
            for (int i = 0; i < availBuffers; ++i) {
                bufferQueue.enqueue(staleBuffers[i]);
            }
        }
    }

    ALCint samplesIn = 0; // How many samples have been captured
    alcGetIntegerv(inputDevice, ALC_CAPTURE_SAMPLES, 1, &samplesIn);
    if (samplesIn > capSize) {
        alcCaptureSamples(inputDevice, buffer, capSize);
        audioHandler(buffer, capSize);
    }
}

void AudioManager::onAudio(const AudioHandler &audioHandler) {
    this->audioHandler = audioHandler;
}

AudioManager::PersonId AudioManager::createPerson() {
    std::lock_guard<std::mutex> lg(mut);
    ALuint src;
    alGenSources(1, &src); // Create a sound source
    alSource3f(src, AL_POSITION, 0.f, 0.f, 0.f);
    people.emplace(src);
    return src;
}

void AudioManager::deletePerson(PersonId person) {
    std::lock_guard<std::mutex> lg(mut);
    people.erase(people.find(person));
    alDeleteSources(1, &person);
}

void AudioManager::writeAudio(PersonId person, char *audio, size_t numSamples) {
    printf("Audio for %d...\n", person);
    std::lock_guard<std::mutex> lg(mut);
    ALuint bufId;
    if (!bufferQueue.try_dequeue(bufId)) {
        buffers.resize(buffers.size() + 16);
        alGenBuffers(16, buffers.data() + buffers.size() - 16);
        handleAlError("creating buffer");

        for (size_t ii = buffers.size() - 16; ii < buffers.size(); ++ii) {
            bufferQueue.enqueue(buffers[ii]);
        }
        bufferQueue.wait_dequeue(bufId);
    }
    alBufferData(bufId, AL_FORMAT_MONO16, audio, numSamples * sampWidth, sampRate);
    alSourceQueueBuffers(person, 1, &bufId);
    ALint sState = 0;
    alGetSourcei(person, AL_SOURCE_STATE, &sState);
    if (sState != AL_PLAYING) {
        alSourcePlay(person);
    }
}

void AudioManager::setPos(PersonId person, int x, int y) {
    alSource3f(person, AL_POSITION, roomSx * x / 255.f, roomSy * y / 255.f, 0.f);
}

void AudioManager::setSelfPos(int x, int y) {
    alListener3f(AL_POSITION, roomSx * x / 255.f, roomSy * y / 255.f, 0.f);
}
