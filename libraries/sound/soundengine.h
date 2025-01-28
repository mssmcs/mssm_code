#ifndef SOUNDENGINE_H
#define SOUNDENGINE_H

//#include "miniaudio.h"
#include <vector>
#include <memory>
#include <string>
#include <mutex>

class SoundHandle;

struct ma_sound;
struct ma_engine;

class SoundEngine
{
    class SoundInstance // a SoundInstance is needed for each concurrently playing instance of a Sound
    {
    public:
        SoundInstance();
       ~SoundInstance();
        ma_sound *ms{};
        bool isPlaying{false};
        void stop();
    };

    class Sound
    {
    private:
        SoundEngine &engine;
        // cache of instances to handle multiple simultaneous copies of a sound
        std::vector<std::unique_ptr<SoundInstance>> instances;
        int playingCount{0};
      //  bool loops{false};
    public:
        Sound(SoundEngine &engine, std::string filename/*, bool loop*/);
        ~Sound();
        bool disconnected() { return instances.empty(); }

    private:
        void play();
        void stop();
        void disconnect();
        SoundInstance *findInstance(ma_sound *ms);
        ma_sound *createInstance();
        SoundInstance &duplicate();
        SoundInstance &getPlayableSound();
        friend class SoundEngine;
        friend class SoundHandle;
    };

    //std::binary_semaphore hasResources{1};
    bool ignoreCallback{false};
    std::mutex sound_mutex;
    ma_engine* engine{};
    std::vector<std::weak_ptr<Sound>> allSounds;
    std::vector<std::shared_ptr<Sound>> playing;

public:
    SoundEngine();
   ~SoundEngine();
   SoundHandle create(std::string filename/*, bool looping*/);

private:
   void cleanup();
   void disconnectSounds();

   void onSoundEnd(ma_sound *sound);
   void play(std::shared_ptr<Sound> &sound);

   friend void soundEndCallback(void *enginePtr, ma_sound *sound);
   friend class Sound;
   friend class SoundHandle;
};

class SoundHandle
{
    std::shared_ptr<SoundEngine::Sound> sound;

public:
    SoundHandle() {}
    SoundHandle(std::shared_ptr<SoundEngine::Sound> sound);
    void play();
    operator bool() const { return sound.get() != nullptr; }
    bool isPlaying();
};


#endif // SOUNDENGINE_H
