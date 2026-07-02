#pragma once

struct SystemContext;

/* Translates InputState to Arkanoid-specific actions.
 */
namespace InputSystem
{
	void TranslateRawInput(SystemContext& Context, float DeltaTime);
}