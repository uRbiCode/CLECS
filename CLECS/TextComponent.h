#pragma once
#include "RenderConstants.h"
#include <string>

#pragma warning(push)
#pragma warning(disable : 4820)
// Used by RenderSystem to display text.
struct TextComponent
{
	std::string Text;
	std::string FontFilePath;
	float FontPointSize = RenderConstants::DefaultFontSize;
};
#pragma warning(pop)