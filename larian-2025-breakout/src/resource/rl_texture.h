#pragma once

#include <string>
#include <raylib.h>

// Sunday update: i didn't end up using this

// A simple RAII wrapper class for Raylib textures.
// When the RLTexture is deleted, then the texture
// is deleted, regardless of any other Texture2Ds
// with the same VRAM id are circulating around.
class RLTexture
{
public:
	RLTexture(const std::string& path);
	RLTexture(const std::string& path, float width, float height);
	~RLTexture();

	RLTexture(const RLTexture&) = delete;
	RLTexture& operator=(const RLTexture&) = delete;

	Texture2D GetTexture();

private:
	Texture2D m_texture;
};