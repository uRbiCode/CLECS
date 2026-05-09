#pragma once

struct SystemContext;

/* Responsible for displaying remaining player lives. These hearts at the bottom left.
 * Reacts to events if to display the health or not and updates number of hearts.
 */
namespace HealthIndicatorSystem
{
	void Initialize(const SystemContext& Context);
}