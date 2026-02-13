#pragma once

/* Sent whenever run stage ends.
 * RunControllerSystem listens to this event to either progress the run or end it.
 */
struct StageEndEvent
{
	bool Victory = true;
};