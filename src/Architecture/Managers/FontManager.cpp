#include "FontManager.hpp"

//LIB
#include <Engine/Renderer.h>

FontManager::FontManager(ASGE::Renderer* renderer, std::string font_path)
	: renderer(renderer)
	, font_path(font_path)
{}

void FontManager::addFont(const std::string& path, const std::string& name, FontSize size)
{
	if (fonts.count(name))
	{
		throw "Font already exists";
	}

	fonts[name] = renderer->loadFont(std::string(font_path + path).c_str(), size);

	if (fonts[name] < 0)
	{
		fonts.erase(name);
		throw "Could not load font";
	}
}

void FontManager::setFont(const std::string& name)
{
	renderer->setFont(fonts[name]);
}
