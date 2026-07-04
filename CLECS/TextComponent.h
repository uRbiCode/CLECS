#pragma once
#include "RenderConstants.h"
#include <string>

// Used by RenderSystem to display text.
struct TextComponent
{
	std::string Text;
	std::string FontFilePath;
	float FontPointSize = RenderConstants::DefaultFontSize;
};