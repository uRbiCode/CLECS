#pragma once

struct SystemContext;

/* Translates InputState to Arkanoid-specific actions.
 */
namespace InputSystem
{
	void UpdateClickables(SystemContext& Context, float DeltaTime);
	void TranslateRawInput(SystemContext& Context, float DeltaTime);
}