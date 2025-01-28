#include "vulkwindow.h"
#include "vulkanwindowglfw.h"
#include "vulkcanvas.h"
#include "paths.h"

#include "graphics.h"




//#include <print>

using namespace std;
using namespace gjh;

std::string findVulkanShader(const std::string &filename)
{
    return Paths::findAsset("vulkan\\" + filename);
}


unique_ptr<VulkCanvas> createCanvas(VulkSurfaceRenderManager &renderManager)
{
    return make_unique<VulkCanvas>(renderManager);
}

class VulkBackendWindow : public VulkanGraphicsWindow<VulkanWindowGLFW, VulkCanvas>
{
public:
    VulkBackendWindow(std::string title, int width, int height)
        : VulkanGraphicsWindow<VulkanWindowGLFW, VulkCanvas>(title, width, height, ::createCanvas)
    {
    }
protected:
    void beginDrawing(bool wasResized) override;
    void endDrawing() override;
};

void VulkBackendWindow::beginDrawing(bool wasResized)
{
    VulkanGraphicsWindow<VulkanWindowGLFW, VulkCanvas>::beginDrawing(wasResized);
}

void VulkBackendWindow::endDrawing()
{
    VulkanGraphicsWindow<VulkanWindowGLFW, VulkCanvas>::endDrawing();
}

class VulkanGraphicsBackend : public GraphicsBackend {
public:
    VulkBackendWindow* vulkWindow{};
public:
    VulkanGraphicsBackend(std::string title, int width, int height)
    {
        vulkWindow = new VulkBackendWindow(title, width, height);
        window = vulkWindow;
        window->update(true);
    }
    virtual ~VulkanGraphicsBackend()
    {
        delete vulkWindow;
    }
    void postConfigure() override;

    // GraphicsBackend interface
public:
    void setCursor(CoreWindowCursor cursor) override;
};

void VulkanGraphicsBackend::setCursor(CoreWindowCursor cursor)
{
    window->setCursor(cursor);
}

void VulkanGraphicsBackend::postConfigure()
{
    canvas = vulkWindow->getCanvas();
}

VulkBackendWindow* getVulk(GraphicsBackend* backend)
{
    return static_cast<VulkanGraphicsBackend*>(backend)->vulkWindow;
}

mssm::Graphics::Graphics(std::string title, int width, int height)
{
    backend = new VulkanGraphicsBackend(title, width, height);
   // backend->window->update(true);
    backend->postConfigure();
}

mssm::Graphics::~Graphics()
{
    delete backend;
}

std::shared_ptr<mssm::ImageInternal> mssm::Graphics::loadImg(std::string filename)
{
    return getVulk(backend)->loadImg(Paths::findAsset(filename));
}


std::chrono::milliseconds::rep mssm::Graphics::time()
{
    return getVulk(backend)->timeMs();
}

double mssm::Graphics::elapsedMs() const
{
    return getVulk(backend)->elapsedMs();
}

Vec2d  mssm::Graphics::mousePos() const
{
    auto* v = getVulk(backend);
    return {v->mousePosX(), v->mousePosY()};
}

void mssm::Graphics::setBackground(Color c)
{
    throw logic_error("setBackground Not Implemented");
}

bool mssm::Graphics::isClipped(Vec2d pos) const
{
    return backend->getCanvas()->isClipped(pos);
}

void mssm::Graphics::setClip(int x, int y, int w, int h)
{
    backend->getCanvas()->setClip(x, y, w, h);
}

void mssm::Graphics::resetClip()
{
    backend->getCanvas()->resetClip();
}

void mssm::Graphics::pushClip(int x, int y, int w, int h, bool replace)
{
    backend->getCanvas()->pushClip(x, y, w, h, replace);
}

void mssm::Graphics::popClip()
{
    backend->getCanvas()->popClip();
}

void mssm::Graphics::setViewport(int x, int y, int w, int h)
{
    backend->getCanvas()->setViewport(x, y, w, h);
}

void mssm::Graphics::resetViewport()
{
    backend->getCanvas()->resetViewport();
}

void mssm::Graphics::play(Sound sound, double volume)
{
    sound.snd.play();
}
  // volume 0 to 1
void mssm::Graphics::music(const std::string& filename, double volume)
{
    auto musicFile = Paths::findAsset(filename);
    stopMusic();
    musicHandle = getSoundPlayer().create(musicFile);
    musicHandle.play();
}

void mssm::Graphics::stopMusic()
{
    if (musicHandle) {
        musicHandle = SoundHandle{};
    }
}

void   mssm::Graphics::setMusicVolume(double volume){
}

bool   mssm::Graphics::draw()
{
    std::deque<std::string> lines;
    drawFromStream(cout, lines, { 10, 20 }, PURPLE);
    drawFromStream(cerr, cerrLines, { width()/2, 20 }, RED);

    return getVulk(backend)->update(true);
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

void   mssm::Graphics::drawFromStream(std::stringstream& ss, std::deque<std::string>& lines, Vec2d start, Color c)
{
    string tmp = ss.str();
    if (!tmp.empty()) {
        splitRows(lines, tmp, (height()-40)/25);
        ss.str("");
    }

    double y = start.y;
    for (const auto& row : lines) {
        text({start.x, y}, 20, row, c);
        y += 25;
    }
}

bool   mssm::Graphics::isClosed() const
{
    throw logic_error("isClosed Not Implemented");
}

double mssm::Graphics::timeMs() const
{
    return getVulk(backend)->timeMs();
}

int mssm::Graphics::getDigitPressed()
{
    switch (getKeyPressed()) {
    case '0': return 0;
    case '1': return 1;
    case '2': return 2;
    case '3': return 3;
    case '4': return 4;
    case '5': return 5;
    case '6': return 6;
    case '7': return 7;
    case '8': return 8;
    case '9': return 9;
    }

    return -1;
}

mssm::Sound::Sound(mssm::Graphics& g, const string &filename)
{
    auto fpath = Paths::findAsset(filename);
    if (!fpath.empty()) {
        snd = g.getSoundPlayer().create(fpath);
    }
    else
    {
        g.cerr << "Error loading sound " << filename << endl;
    }
}
