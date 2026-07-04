#pragma once
#include <string>
#include <unordered_map>

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
    
    // Will try to LoadFont if not present
    TTF_Font* GetFont(const std::string& FilePath, float PointSize);
    
    void UnloadAll();

private:
    std::string MakeFontKey(const std::string& FilePath, float PointSize) const;
    TTF_Font* LoadFont(const std::string& FilePath, float PointSize);
    void LoadFontsFromAssetsDirectory();
    void UnloadFont(const std::string& FilePath, float PointSize);

    std::unordered_map<std::string, TTF_Font*> FontCache;
};