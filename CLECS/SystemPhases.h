#pragma once
#include <cstdint>

/* Each SystemPhase represents a stage in the game loop.
 * Systems belong to them and are executed in the order of their phase.
 * Phases are ordered by std::less comparator.
 */
enum class SystemPhase : uint8_t
{
	Input,
	EarlyUpdate,
	Update,
	LateUpdate,
	Render
};