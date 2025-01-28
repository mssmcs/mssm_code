#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include "soundengine.h"
#include <iostream>

#include <algorithm>
#include <iostream>
#include <memory>
#include <mutex>
#include <vector>

using namespace std;

SoundHandle::SoundHandle(std::shared_ptr<SoundEngine::Sound> sound)
    : sound{sound}
{}

void SoundHandle::play()
{
    sound->engine.play(sound);
}


bool SoundHandle::isPlaying()
{
    return sound->playingCount > 0;
}

void soundEndCallback(void *enginePtr, ma_sound *sound)
{
 //   cout << "Callback was called" << endl;
    SoundEngine &engine = *reinterpret_cast<SoundEngine *>(enginePtr);
    engine.onSoundEnd(sound);
}

SoundEngine::Sound::Sound(SoundEngine &engine, string filename)
    : engine{engine}
    // , loops{loop}
{
    ma_sound *ms = createInstance();
 //   cout << "New sound from file" << ms << " " << filename << endl;
    ma_result result = ma_sound_init_from_file(engine.engine, filename.c_str(), 0, NULL, NULL, ms);
    if (result == MA_SUCCESS) {
        ma_sound_set_end_callback(ms, soundEndCallback, &engine);
    }
    else {
        if (result == MA_DOES_NOT_EXIST) {
            cout << "SoundEngine: File not found: " << filename << endl;
        }
        cout << "SoundEngine: Some kinda error" << endl;
        instances.clear();
    }
}

SoundEngine::Sound::~Sound()
{
   // cout << "Sound destructed" << endl;
}

void SoundEngine::Sound::play()
{
    // if (loops && playingCount > 0) {
    //     cout << "Attempt to replay looping sound!" << endl;
    //     return;
    // }

    if (disconnected()) {
        cout << "Can't play disconnected sound" << endl;
        return;
    }
    SoundInstance &snd = getPlayableSound();
  //  cout << "Playing: " << &snd.ms << endl;
    ma_sound_seek_to_pcm_frame(snd.ms, 0);
    auto result = ma_sound_start(snd.ms);
    if (result != MA_SUCCESS) {
        cout << "SoundEngine: Some kinda error playing" << result << endl;
    }
    snd.isPlaying = true;
    playingCount++;
    //cout << "Playing count is now: " << playingCount << endl;
}

void SoundEngine::Sound::stop()
{
    //cout << "Stopping instances for a sound" << endl;
    for (auto &inst : instances) {
        inst->stop();
    }
}

void SoundEngine::Sound::disconnect()
{
    stop();
    instances.clear();
}

SoundEngine::SoundInstance *SoundEngine::Sound::findInstance(ma_sound *ms)
{
    for (auto &snd : instances) {
        if (snd->ms == ms) {
            return snd.get();
        }
    }
    return nullptr;
}

SoundEngine::SoundInstance::SoundInstance()
{
    ms = new ma_sound{};
}

SoundEngine::SoundInstance::~SoundInstance()
{
    //cout << "Releasing instance" << endl;
    ma_sound_uninit(ms);

    delete ms;
}

void SoundEngine::SoundInstance::stop()
{
    if (isPlaying) {
        //cout << "Calling stop on a sound" << endl;
        ma_sound_stop(ms);
    }
}

ma_sound *SoundEngine::Sound::createInstance()
{
    instances.push_back(std::make_unique<SoundInstance>());
    ma_sound *ms = instances.back()->ms;
    return ms;
}

SoundEngine::SoundInstance &SoundEngine::Sound::duplicate()
{
    ma_sound *ms = createInstance();
    //cout << "Duplicating " << &instances.front()->ms << " to make sound: " << ms << endl;
    auto result = ma_sound_init_copy(engine.engine, instances.front()->ms, 0, nullptr, ms);
    if (result != MA_SUCCESS) {
        cout << "SoundEngine: Some kinda error copying" << result << endl;
    }
    ma_sound_set_end_callback(ms, soundEndCallback, &engine);
    return *instances.back();
}

SoundEngine::SoundInstance &SoundEngine::Sound::getPlayableSound()
{
    if (instances.empty()) {
        throw logic_error("getPlayableSound called on disconnected SoundInstance");
    }
    for (auto &snd : instances) {
        if (!snd->isPlaying) {
            return *snd;
        }
    }
    return duplicate();
}

SoundEngine::SoundEngine()
{
    engine = new ma_engine{}; // this allows header to only contain pointers,
//    so no need to include miniaudio.h there

    ma_engine_config engineConfig = ma_engine_config_init();

    ma_result result = ma_engine_init(&engineConfig, engine);
    if (result != MA_SUCCESS) {
    }
}

SoundEngine::~SoundEngine()
{
    //cout << "Stopping..." << endl;
    {
        std::lock_guard<std::mutex> guard(sound_mutex); //HEY

        ignoreCallback = true;

        for (auto &snd : playing) {
            snd->stop();
        }
    }

    disconnectSounds();

   // cout << "Clearing..." << endl;

    {
        std::lock_guard<std::mutex> guard(sound_mutex);
        playing.clear();

        //cout << "Closing Engine" << endl;
        ma_engine_uninit(engine);
    }

    delete engine;
}

void SoundEngine::onSoundEnd(ma_sound *sound)
{
    std::lock_guard<std::mutex> guard(sound_mutex);

    if (ignoreCallback) {
        cout << "Callback Ignored" << endl;
        return;
    }

    //cout << "Sound Ended: " << sound << endl;
    SoundInstance *snd;
    auto i = std::find_if(playing.begin(), playing.end(), [sound, &snd](auto &s) {
        snd = s->findInstance(sound);
        return snd != nullptr;
    });

    if (i != playing.end()) {
        auto s = *i;
        // if (s->loops) {
        //     ma_sound_seek_to_pcm_frame(&snd->ms, 0);
        //     ma_sound_start(&snd->ms);
        // }
        // else {
            snd->isPlaying = false;
            s->playingCount--;
            //cout << "_ Playing count is now: " << s->playingCount << endl;
        // }
    } else {
        cout << "playing sound not found" << endl;
    }
}

SoundHandle SoundEngine::create(string filename/*, bool looping*/)
{
    cleanup();

    std::lock_guard<std::mutex> guard(sound_mutex);
    auto snd = std::make_shared<Sound>(*this, filename/*, looping*/);
    allSounds.push_back(snd);
    return snd;
}

void SoundEngine::cleanup()
{
    std::lock_guard<std::mutex> guard(sound_mutex);

    // remove any sounds from playing list that aren't actually playing
    playing.erase(std::remove_if(playing.begin(), playing.end(), [](std::shared_ptr<Sound> sound) -> bool {
                      return sound->playingCount == 0;
                  }), playing.end());

    allSounds.erase(std::remove_if(allSounds.begin(),
                                   allSounds.end(),
                                   [](std::weak_ptr<Sound> sound) -> bool {
                                       return sound.expired();
                                   }),
                    allSounds.end());
}

void SoundEngine::disconnectSounds()
{
    cleanup();

    std::lock_guard<std::mutex> guard(sound_mutex);

    for (auto &s : allSounds) {
        if (!s.expired()) {
            auto sp = s.lock();
            sp->disconnect();
        }
    }
}

void SoundEngine::play(shared_ptr<Sound> &sound)
{
    cleanup();

    std::lock_guard<std::mutex> guard(sound_mutex);

    if (sound->playingCount == 0) {
        if (playing.empty()) {
            // hasResources.acquire();
        }
        playing.push_back(sound);
    }

    sound->play();
}
