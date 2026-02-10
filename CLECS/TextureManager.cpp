#include "TextureManager.h"
#include "SDL3_image/SDL_image.h"
#include <filesystem>

namespace
{
	constexpr const char* TexturesDirectory = "../Assets/Textures/";
}

TextureManager::~TextureManager()
{
    UnloadAll();
}

void TextureManager::Initialize(SDL_Renderer& InRenderer)
{
    Renderer = &InRenderer;
	LoadTexturesFromAssetsDirectory();
}

SDL_Texture* TextureManager::LoadTexture(const std::string& FilePath)
{
    auto It = TextureCache.find(FilePath);
    if (It != TextureCache.end())
        return It->second;

    if (!Renderer)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TextureManager: Renderer not initialized");
        return nullptr;
    }

    auto NewTexture = IMG_LoadTexture(Renderer, FilePath.c_str());
    if (!NewTexture)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load texture: %s. SDL Error: %s", 
                     FilePath.c_str(), SDL_GetError());
        return nullptr;
    }

    TextureCache[FilePath] = NewTexture;
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Loaded texture: %s", FilePath.c_str());

    return NewTexture;
}

SDL_Texture* TextureManager::GetTexture(const std::string& FilePath) const
{
    auto It = TextureCache.find(FilePath);
    if (It != TextureCache.end())
        return It->second;

    return nullptr;
}

bool TextureManager::HasTexture(const std::string& FilePath) const
{
    return TextureCache.find(FilePath) != TextureCache.end();
}

void TextureManager::UnloadTexture(const std::string& FilePath)
{
    auto It = TextureCache.find(FilePath);
    if (It != TextureCache.end())
    {
        SDL_DestroyTexture(It->second);
        TextureCache.erase(It);
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Unloaded texture: %s", FilePath.c_str());
    }
}

void TextureManager::UnloadAll()
{
    for (auto& [Path, Texture] : TextureCache)
    {
        SDL_DestroyTexture(Texture);
    }
    TextureCache.clear();
}

void TextureManager::LoadTexturesFromAssetsDirectory()
{
    auto current_dir = std::filesystem::current_path();
    if (!Renderer)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TextureManager: Renderer not initialized");
        return;
    }

    if (!std::filesystem::exists(TexturesDirectory))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Directory does not exist: %s", TexturesDirectory);
        return;
    }

    if (!std::filesystem::is_directory(TexturesDirectory))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Path is not a directory: %s", TexturesDirectory);
        return;
    }

	for (const auto& Entry : std::filesystem::directory_iterator(TexturesDirectory))
	{
		if (!Entry.is_regular_file())
			continue;

		const std::string FilePath = Entry.path().string();
		LoadTexture(FilePath);
	}
}