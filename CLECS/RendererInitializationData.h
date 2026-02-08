#pragma once

// RendererInitializationData encapsulates all window and renderer configuration
struct RendererInitializationData
{
	const char* WindowTitle = "CLECS Game";
	int WindowWidth = 1280;
	int WindowHeight = 720;

	// Add any other renderer-specific settings here as needed
	// For example:
	// bool VSync = true;
	// int MSAASamples = 0;
	// etc.
};