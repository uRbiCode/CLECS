#pragma once

/* Encapsulates all window and renderer configuration. 
 * It is defined by the Game class and passed to the World during initialization.
 */
struct RendererInitializationData
{
	const char* WindowTitle = "CLECS Game";
	int WindowWidth = 1280;
	int WindowHeight = 720;
};