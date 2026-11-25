#pragma once
namespace WallpaperEngine::Application { class WallpaperApplication; }
namespace WallpaperEngine::WebBrowser {
class WebBrowserContext {
public:
    explicit WebBrowserContext(Application::WallpaperApplication& app);
    ~WebBrowserContext();
private:
    Application::WallpaperApplication& m_app;
};
}
