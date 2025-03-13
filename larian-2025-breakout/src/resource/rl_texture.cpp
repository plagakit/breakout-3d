#include "rl_texture.h"

RLTexture::RLTexture(const std::string& path)
{
	Image img = LoadImage(path.c_str());
	m_texture = LoadTextureFromImage(img);
	UnloadImage(img);
}

RLTexture::RLTexture(const std::string& path, float width, float height) :
	RLTexture(path)
{
	m_texture.width = width;
	m_texture.height = height;
}

RLTexture::~RLTexture()
{
	UnloadTexture(m_texture);
}

Texture2D RLTexture::GetTexture()
{
	return m_texture;
}
