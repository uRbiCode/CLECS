#pragma once
#include <string>
#include <unordered_map>

struct SDL_Renderer;
struct TTF_Font;

/* Responsible for managing fonts in the game.
 * Interacts with SDL_ttf to set up font resources so that they can be used by the RenderSystem.
 */
class FontManager
{
public:
    FontManager() = default;
    ~FontManager();

    FontManager(const FontManager&) = delete;
    FontManager& operator=(const FontManager&) = delete;

    FontManager(FontManager&&) noexcept = default;
    FontManager& operator=(FontManager&&) noexcept = default;

    void Initialize();
    
    TTF_Font* LoadFont(const std::string& FilePath, int PointSize);
    
    TTF_Font* GetFont(const std::string& FilePath, int PointSize) const;
    
    bool HasFont(const std::string& FilePath, int PointSize) const;
    
    void UnloadFont(const std::string& FilePath, int PointSize);
    
    void UnloadAll();

private:
    std::string MakeFontKey(const std::string& FilePath, int PointSize) const;
    void LoadFontsFromAssetsDirectory();

    std::unordered_map<std::string, TTF_Font*> FontCache;
};