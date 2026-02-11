#include "FontManager.h"
#include "SDL3_ttf/SDL_ttf.h"
#include <filesystem>

namespace
{
    constexpr const char* FontsDirectory = "../Assets/Fonts/";
    constexpr int DefaultFontSize = 16;
}

FontManager::~FontManager()
{
    UnloadAll();
}

void FontManager::Initialize()
{
    if (!TTF_Init())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "FontManager::Initialize -> Failed to initialize SDL_ttf: %s", SDL_GetError());
        return;
    }

    LoadFontsFromAssetsDirectory();
}

TTF_Font* FontManager::LoadFont(const std::string& FilePath, int PointSize)
{
    const std::string FontKey = MakeFontKey(FilePath, PointSize);
    
    const auto It = FontCache.find(FontKey);
    if (It != FontCache.end())
        return It->second;

    const auto NewFont = TTF_OpenFont(FilePath.c_str(), static_cast<float>(PointSize));
    if (NewFont == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "FontManager::LoadFont -> Failed to load font: %s at size %d. SDL Error: %s", FilePath.c_str(), PointSize, SDL_GetError());
        return nullptr;
    }

    FontCache[FontKey] = NewFont;
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "FontManager::LoadFont -> Loaded font: %s at size %d", FilePath.c_str(), PointSize);

    return NewFont;
}

TTF_Font* FontManager::GetFont(const std::string& FilePath, int PointSize) const
{
    const std::string FontKey = MakeFontKey(FilePath, PointSize);
    
    const auto It = FontCache.find(FontKey);
    if (It != FontCache.end())
        return It->second;

    return nullptr;
}

bool FontManager::HasFont(const std::string& FilePath, int PointSize) const
{
    const std::string FontKey = MakeFontKey(FilePath, PointSize);
    return FontCache.find(FontKey) != FontCache.end();
}

void FontManager::UnloadFont(const std::string& FilePath, int PointSize)
{
    const std::string FontKey = MakeFontKey(FilePath, PointSize);
    
    const auto It = FontCache.find(FontKey);
    if (It == FontCache.end())
        return;

    TTF_CloseFont(It->second);
    FontCache.erase(It);
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "FontManager::UnloadFont -> Unloaded font: %s at size %d", FilePath.c_str(), PointSize);
}

void FontManager::UnloadAll()
{
    for (const auto& [Key, Font] : FontCache)
    {
        TTF_CloseFont(Font);
    }
    FontCache.clear();
    TTF_Quit();
}

std::string FontManager::MakeFontKey(const std::string& FilePath, int PointSize) const
{
    return FilePath + "_" + std::to_string(PointSize);
}

void FontManager::LoadFontsFromAssetsDirectory()
{
    if (!std::filesystem::exists(FontsDirectory))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "FontManager::LoadFontsFromAssetsDirectory -> Directory does not exist: %s", FontsDirectory);
        return;
    }

    if (!std::filesystem::is_directory(FontsDirectory))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "FontManager::LoadFontsFromAssetsDirectory -> Path is not a directory: %s", FontsDirectory);
        return;
    }

    for (const auto& Entry : std::filesystem::directory_iterator(FontsDirectory))
    {
        if (!Entry.is_regular_file())
            continue;

        const auto FilePath = Entry.path().string();
        const auto Extension = Entry.path().extension().string();
        
        if (Extension == ".ttf")
        {
            LoadFont(FilePath, DefaultFontSize);
        }
    }
}