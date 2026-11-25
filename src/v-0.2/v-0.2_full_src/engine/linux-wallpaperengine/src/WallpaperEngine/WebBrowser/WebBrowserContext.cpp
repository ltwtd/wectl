#include "WebBrowserContext.h"
#include "WallpaperEngine/Logging/Log.h"
namespace WallpaperEngine::WebBrowser {
WebBrowserContext::WebBrowserContext(Application::WallpaperApplication& app) : m_app(app) { sLog.out("NOCEF mode"); }
WebBrowserContext::~WebBrowserContext() = default;
}
