#pragma once
#include <SDL3/SDL.h>
#include <unordered_map>
#include <string>

class TextureManager
{
public:
    TextureManager() = default;
    ~TextureManager();

    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;

    TextureManager(TextureManager&&) noexcept = default;
    TextureManager& operator=(TextureManager&&) noexcept = default;

    void Initialize(SDL_Renderer& Renderer);
    
    SDL_Texture* LoadTexture(const std::string& FilePath);
    
    SDL_Texture* GetTexture(const std::string& FilePath) const;
    
    bool HasTexture(const std::string& FilePath) const;
    
    void UnloadTexture(const std::string& FilePath);
    
    void UnloadAll();

private:
    void LoadTexturesFromAssetsDirectory();

    SDL_Renderer* Renderer = nullptr;
    std::unordered_map<std::string, SDL_Texture*> TextureCache;
};