#include "FontManager.h"
#include "RenderConstants.h"
#include "SDL3_ttf/SDL_ttf.h"
#include <filesystem>

namespace FontConstants
{
    constexpr const char* FontsDirectory = "../Assets/Fonts/";
    constexpr const char* DefaultFont = "../Assets/Fonts/pixy_regular.ttf";
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
	SetupDefaultFont();
}

TTF_Font* FontManager::LoadFont(const std::string& FilePath, float PointSize)
{
    const std::string FontKey = MakeFontKey(FilePath, PointSize);
    
    const auto It = FontCache.find(FontKey);
    if (It != FontCache.end())
        return It->second;

    TTF_Font* NewFont = TTF_OpenFont(FilePath.c_str(), PointSize);
    if (NewFont == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "FontManager::LoadFont -> Failed to load font: %s at size %f. SDL Error: %s", FilePath.c_str(), PointSize, SDL_GetError());
        return DefaultFont;
    }

    FontCache[FontKey] = NewFont;
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "FontManager::LoadFont -> Loaded font: %s at size %f", FilePath.c_str(), PointSize);

    return NewFont;
}

TTF_Font* FontManager::GetFont(const std::string& FilePath, float PointSize)
{
    const std::string FontKey = MakeFontKey(FilePath, PointSize);
    
    const auto It = FontCache.find(FontKey);
    if (It != FontCache.end())
        return It->second;

    return LoadFont(FilePath, PointSize);
}

void FontManager::UnloadFont(const std::string& FilePath, float PointSize)
{
    const std::string FontKey = MakeFontKey(FilePath, PointSize);
    
    const auto It = FontCache.find(FontKey);
    if (It == FontCache.end())
        return;

    TTF_CloseFont(It->second);
    FontCache.erase(It);
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "FontManager::UnloadFont -> Unloaded font: %s at size %f", FilePath.c_str(), PointSize);
}

void FontManager::UnloadAll()
{
    for (const auto& [Key, Font] : FontCache)
    {
        TTF_CloseFont(Font);
    }
    FontCache.clear();
	TTF_CloseFont(DefaultFont);
    TTF_Quit();
}

std::string FontManager::MakeFontKey(const std::string& FilePath, float PointSize) const
{
    return FilePath + "_" + std::to_string(PointSize);
}

void FontManager::LoadFontsFromAssetsDirectory()
{
    if (!std::filesystem::exists(FontConstants::FontsDirectory))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "FontManager::LoadFontsFromAssetsDirectory -> Directory does not exist: %s", FontConstants::FontsDirectory);
        return;
    }

    if (!std::filesystem::is_directory(FontConstants::FontsDirectory))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "FontManager::LoadFontsFromAssetsDirectory -> Path is not a directory: %s", FontConstants::FontsDirectory);
        return;
    }

    for (const auto& Entry : std::filesystem::directory_iterator(FontConstants::FontsDirectory))
    {
        if (!Entry.is_regular_file())
            continue;

        const std::string FilePath = Entry.path().string();
        const std::string Extension = Entry.path().extension().string();
        
        if (Extension != ".ttf" && Extension != ".otf")
            continue;

        LoadFont(FilePath, RenderConstants::DefaultFontSize);
    }
}

void FontManager::SetupDefaultFont()
{
    DefaultFont = LoadFont(FontConstants::DefaultFont, RenderConstants::DefaultFontSize);
}