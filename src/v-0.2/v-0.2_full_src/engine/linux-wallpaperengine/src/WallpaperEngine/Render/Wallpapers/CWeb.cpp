#include "CWeb.h"
#include "WallpaperEngine/Logging/Log.h"
namespace WallpaperEngine::Render::Wallpapers {
CWeb::CWeb(const Wallpaper& w, RenderContext& c, AudioContext& a, WebBrowser::WebBrowserContext& b, const WallpaperState::TextureUVsScaling& s, const uint32_t& cl) : CWallpaper(w, c, a, s, cl), m_browserContext(b) { sLog.exception("NOCEF mode"); }
CWeb::~CWeb() = default;
void CWeb::renderFrame(const glm::ivec4& viewport) {}
}
