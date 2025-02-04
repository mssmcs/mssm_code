
#include "graphicsbase.h"
#include "paths.h"

using namespace mssm;
using namespace std;






// std::chrono::milliseconds::rep mssm::GraphicsBase::time()
// {
//     return eventSource->timeMs();
// }

// double mssm::GraphicsBase::elapsedMs() const
// {
//     return eventSource->elapsedMs();
// }

// Vec2d  mssm::GraphicsBase::mousePos() const
// {
//     auto* v = eventSource;
//     return {v->mousePosX(), v->mousePosY()};
// }

// void mssm::GraphicsBase::setBackground(Color c)
// {
//     throw logic_error("setBackground Not Implemented");
// }

// bool mssm::GraphicsBase::isClipped(Vec2d pos) const
// {
//     return canvas->isClipped(pos);
// }

// void mssm::GraphicsBase::setClip(int x, int y, int w, int h)
// {
//     canvas->setClip(x, y, w, h);
// }

// void mssm::GraphicsBase::resetClip()
// {
//     canvas->resetClip();
// }

// void mssm::GraphicsBase::pushClip(int x, int y, int w, int h, bool replace)
// {
//     canvas->pushClip(x, y, w, h, replace);
// }

// void mssm::GraphicsBase::popClip()
// {
//     canvas->popClip();
// }

// void mssm::GraphicsBase::setViewport(int x, int y, int w, int h)
// {
//     canvas->setViewport(x, y, w, h);
// }

// void mssm::GraphicsBase::resetViewport()
// {
//     canvas->resetViewport();
// }

void mssm::SoundHost::play(Sound sound, double volume)
{
    sound.snd.play();
}
  // volume 0 to 1
void mssm::SoundHost::music(const std::string& filename, double volume)
{
    auto musicFile = Paths::findAsset(filename);
    stopMusic();
    musicHandle = getSoundPlayer().create(musicFile);
    musicHandle.play();
}

void mssm::SoundHost::stopMusic()
{
    if (musicHandle) {
        musicHandle = SoundHandle{};
    }
}

void   mssm::SoundHost::setMusicVolume(double volume){
}



void splitRows(std::deque<std::string>& lines, const std::string& str, size_t keepLast)
{
    const char* ptr = str.c_str();
    const char* eptr = ptr + str.size();
    const char* sptr = ptr;
    while (ptr < eptr) {
        if (*ptr == 10) {
            lines.emplace_back(sptr, ptr-sptr);
            if (keepLast > 0 && lines.size() > keepLast) {
                lines.erase(lines.begin(), lines.begin() + lines.size() - keepLast);
            }
            ptr++;
            if (ptr < eptr && *ptr == 13) {
                ptr++;
            }
            sptr = ptr;
        }
        else if (*ptr == 13) {
            lines.emplace_back(sptr, ptr-sptr);
            if (keepLast > 0 && lines.size() > keepLast) {
                lines.erase(lines.begin(), lines.begin() + lines.size() - keepLast);
            }
            ptr++;
            if (ptr < eptr && *ptr == 10) {
                ptr++;
            }
            sptr = ptr;
        }
        else {
            ptr++;
        }
    }
    if (ptr > sptr) {
        lines.emplace_back(sptr, ptr-sptr);
        if (keepLast > 0 && lines.size() > keepLast) {
            lines.erase(lines.begin(), lines.begin() + lines.size() - keepLast);
        }
    }
}



// bool   mssm::GraphicsBase::isClosed() const
// {
//     throw logic_error("isClosed Not Implemented");
// }

// double mssm::GraphicsBase::timeMs() const
// {
//     return eventSource->timeMs();
// }

// int mssm::GraphicsBase::getDigitPressed()
// {
//     switch (getKeyPressed()) {
//     case '0': return 0;
//     case '1': return 1;
//     case '2': return 2;
//     case '3': return 3;
//     case '4': return 4;
//     case '5': return 5;
//     case '6': return 6;
//     case '7': return 7;
//     case '8': return 8;
//     case '9': return 9;
//     }

//     return -1;
// }

mssm::Sound::Sound(mssm::SoundHost& g, const string &filename)
{
    auto fpath = Paths::findAsset(filename);
    if (!fpath.empty()) {
        snd = g.getSoundPlayer().create(fpath);
    }
    else
    {
        std::cerr << "Error loading sound " << filename << endl;
    }
}



